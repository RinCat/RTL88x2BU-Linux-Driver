#!/usr/bin/env python3
"""Deterministic rust-migration work finder for agent workflows.

Encodes path selection (pick-up-work-item), PR classification
(prepare-all-prs-for-merge Phase 1), and ready-issue selection
(select-ready-issue) so agents call one script instead of re-deriving
rules from skills.

Usage:
  python3 scripts/workflow/find_work.py path [--human] [--issue W3-40]
  python3 scripts/workflow/find_work.py prs [--human]
  python3 scripts/workflow/find_work.py issues [--human] [--issue W3-40]

Requires: gh (authenticated), git, jq not required (stdlib json only).
"""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any, Iterable

REPO_ROOT = Path(__file__).resolve().parents[2]
ISSUE_MAP = REPO_ROOT / "docs/rust-migration/issues/ISSUE-MAP.md"
ISSUE_DIR = REPO_ROOT / "docs/rust-migration/issues"
DEFAULT_BRANCH = "master"
SINGLE_LANE_SATURATION = 15
# gh returns newest-first; `--limit 100` dropped older open PRs once the queue
# passed 100, hiding eligible stack tips (e.g. parent merged → needs retarget).
OPEN_PR_LIMIT = 500
_GIT_FETCHED_REFS: set[str] = set()

# statusCheckRollup on 100+ PRs in one GraphQL response can 502; fetch per eligible PR.
PR_LIST_FIELDS = (
    "number,title,isDraft,baseRefName,headRefName,url,mergeable,"
    "mergeStateStatus,reviewDecision"
)

DRAFT_ID_RE = re.compile(r"\[([A-Z][A-Z0-9]*-\d+)\]")
BLOCKED_BY_RE = re.compile(
    r"^\s*-\s*\*\*Blocked by:\*\*\s*(.+)$", re.MULTILINE | re.IGNORECASE
)
ISSUE_NUM_RE = re.compile(r"#(\d+)")
LANE_RE = re.compile(r"(?:core|rust)/((?:rtw_[a-z0-9_]+|aes[a-z0-9_-]*)\.[cr]s?)")
IN_FLIGHT_RE = re.compile(
    r"In-flight:\s*`?([^\s`]+)`?", re.IGNORECASE
)


def gh_json(args: list[str]) -> Any:
    result = subprocess.run(
        ["gh", *args],
        capture_output=True,
        text=True,
        check=False,
        cwd=REPO_ROOT,
    )
    if result.returncode != 0:
        raise RuntimeError(
            f"gh failed ({result.returncode}): {' '.join(args)}\n{result.stderr.strip()}"
        )
    if not result.stdout.strip():
        return []
    return json.loads(result.stdout)


def repo_owner() -> str:
    data = gh_json(["repo", "view", "--json", "nameWithOwner"])
    return data["nameWithOwner"]


def prefetch_git_refs(branches: Iterable[str]) -> None:
    """Batch-fetch PR base branches once per run (avoids N sequential git fetches)."""
    global _GIT_FETCHED_REFS
    need_master = DEFAULT_BRANCH not in _GIT_FETCHED_REFS
    to_fetch = [
        b
        for b in branches
        if b and b != DEFAULT_BRANCH and b not in _GIT_FETCHED_REFS
    ]
    if not need_master and not to_fetch:
        return
    args = ["git", "fetch", "origin", DEFAULT_BRANCH, "--prune"]
    if to_fetch:
        # Keep argv bounded; unique bases are usually << OPEN_PR_LIMIT.
        args.extend(to_fetch[:80])
    subprocess.run(args, capture_output=True, cwd=REPO_ROOT)
    _GIT_FETCHED_REFS.add(DEFAULT_BRANCH)
    _GIT_FETCHED_REFS.update(to_fetch[:80])
    remainder = to_fetch[80:]
    for i in range(0, len(remainder), 80):
        chunk = remainder[i : i + 80]
        subprocess.run(
            ["git", "fetch", "origin", *chunk, "--prune"],
            capture_output=True,
            cwd=REPO_ROOT,
        )
        _GIT_FETCHED_REFS.update(chunk)


def git_ancestor(branch: str) -> bool:
    if branch not in _GIT_FETCHED_REFS:
        prefetch_git_refs([branch])
    proc = subprocess.run(
        ["git", "merge-base", "--is-ancestor", f"origin/{branch}", f"origin/{DEFAULT_BRANCH}"],
        cwd=REPO_ROOT,
        capture_output=True,
    )
    return proc.returncode == 0


def load_issue_map() -> dict[str, int]:
    mapping: dict[str, int] = {}
    if not ISSUE_MAP.is_file():
        return mapping
    for line in ISSUE_MAP.read_text().splitlines():
        m = re.match(r"^\|\s*([A-Za-z0-9-]+)\s*\|\s*#(\d+)\s*\|", line)
        if m:
            mapping[m.group(1)] = int(m.group(2))
    return mapping


def reverse_map(id_to_num: dict[str, int]) -> dict[int, str]:
    return {n: d for d, n in id_to_num.items()}


def parse_draft_id(title: str) -> str | None:
    m = DRAFT_ID_RE.search(title)
    return m.group(1) if m else None


def title_is_epic(title: str) -> bool:
    return "[Epic]" in title


def parse_blocked_by(body: str, id_to_num: dict[str, int]) -> list[int]:
    """Return GitHub issue numbers from the Blocked by: tracking line only."""
    m = BLOCKED_BY_RE.search(body or "")
    if not m:
        return []
    line = m.group(1)
    nums: list[int] = []
    for num in ISSUE_NUM_RE.findall(line):
        nums.append(int(num))
    # Also resolve draft IDs in backticks, e.g. `W3-38`
    for draft in re.findall(r"`([A-Z][A-Z0-9]*-\d+)`", line):
        if draft in id_to_num:
            n = id_to_num[draft]
            if n not in nums:
                nums.append(n)
    return nums


def local_spec_path(draft_id: str) -> str | None:
    if not ISSUE_DIR.is_dir():
        return None
    if draft_id.startswith("W"):
        glob = f"wave{draft_id[1:].lower()}-*.md"
    elif draft_id.startswith("T"):
        glob = f"test-{draft_id[1:].zfill(2)}-*.md" if draft_id[1:].isdigit() else f"test-{draft_id[1:].lower()}-*.md"
    elif draft_id.startswith("A"):
        glob = f"arch-{draft_id[1:].zfill(2)}-*.md" if draft_id[1:].isdigit() else f"arch-{draft_id[1:].lower()}-*.md"
    else:
        glob = f"*{draft_id.lower()}*.md"
    matches = sorted(ISSUE_DIR.glob(glob))
    if matches:
        return str(matches[0].relative_to(REPO_ROOT))
    # fallback: scan ISSUE-MAP-linked files by id in frontmatter
    for path in ISSUE_DIR.glob("*.md"):
        text = path.read_text(errors="replace")
        if re.search(rf"^id:\s*{re.escape(draft_id)}\s*$", text, re.MULTILINE):
            return str(path.relative_to(REPO_ROOT))
    return None


def extract_lane(text: str) -> str | None:
    m = LANE_RE.search(text or "")
    if not m:
        return None
    return m.group(1).rsplit(".", 1)[0]


def normalize_branch_token(s: str) -> str:
    return re.sub(r"[^a-z0-9]+", "", s.lower())


@dataclass
class PullRequest:
    number: int
    title: str
    url: str
    is_draft: bool
    base_ref: str
    head_ref: str
    mergeable: str | None = None
    merge_state: str | None = None
    review_decision: str | None = None
    status_checks: list[dict[str, Any]] = field(default_factory=list)
    reviews: list[dict[str, Any]] = field(default_factory=list)
    draft_ids: set[str] = field(default_factory=set)
    issue_numbers: set[int] = field(default_factory=set)
    enrichment_ok: bool = False

    @classmethod
    def from_summary(cls, data: dict[str, Any]) -> PullRequest:
        pr = cls(
            number=data["number"],
            title=data["title"],
            url=data["url"],
            is_draft=data.get("isDraft", False),
            base_ref=data.get("baseRefName", DEFAULT_BRANCH),
            head_ref=data.get("headRefName", ""),
        )
        did = parse_draft_id(pr.title)
        if did:
            pr.draft_ids.add(did)
        return pr

    def enrich(self, detail: dict[str, Any], body: str = "") -> None:
        self.enrichment_ok = True
        self.is_draft = detail.get("isDraft", self.is_draft)
        self.base_ref = detail.get("baseRefName", self.base_ref)
        self.mergeable = detail.get("mergeable")
        self.merge_state = detail.get("mergeStateStatus")
        self.review_decision = detail.get("reviewDecision")
        self.status_checks = detail.get("statusCheckRollup") or []
        self.reviews = detail.get("reviews") or []
        text = f"{self.title}\n{body}"
        for did in DRAFT_ID_RE.findall(text):
            self.draft_ids.add(did)
        for num in ISSUE_NUM_RE.findall(text):
            self.issue_numbers.add(int(num))
        token = normalize_branch_token(self.head_ref)
        for part in re.split(r"[/\-_]", self.head_ref.lower()):
            if re.match(r"^[wtar]\d", part):
                self.draft_ids.add(part.upper().replace("W", "W").replace("T", "T"))
        # headRefName like cursor/w3-128-bf-entry-...
        m = re.search(r"/([wtar])(\d+)-", self.head_ref.lower())
        if m:
            prefix = {"w": "W", "t": "T", "a": "A", "r": "R"}[m.group(1)]
            # find full id from title if possible
            if not self.draft_ids:
                did = parse_draft_id(self.title)
                if did:
                    self.draft_ids.add(did)

    def implements_issue(self, issue_number: int, draft_id: str | None) -> bool:
        if issue_number in self.issue_numbers:
            return True
        if draft_id and draft_id in self.draft_ids:
            return True
        if draft_id:
            token = normalize_branch_token(draft_id)
            if token and token in normalize_branch_token(self.head_ref):
                return True
            if f"[{draft_id}]" in self.title:
                return True
        return False

    def review_in_progress(self) -> bool:
        for rev in self.reviews:
            if rev.get("state") == "PENDING":
                return True
        return False

    def checks_ok(self) -> bool:
        if not self.status_checks:
            return True
        for chk in self.status_checks:
            status = (chk.get("state") or chk.get("conclusion") or "").upper()
            if status in ("", "SUCCESS", "NEUTRAL", "SKIPPED"):
                continue
            if status in ("PENDING", "QUEUED", "IN_PROGRESS", "WAITING"):
                return False
            if status in ("FAILURE", "ERROR", "CANCELLED", "TIMED_OUT", "ACTION_REQUIRED"):
                return False
        return True

    def classify_prep(self) -> str:
        """Return needs_prep or merge_ready (eligible PRs only)."""
        if not self.enrichment_ok:
            return "needs_prep"
        if self.is_draft:
            return "needs_prep"
        if self.base_ref != DEFAULT_BRANCH:
            return "needs_prep"
        if self.mergeable is None or self.merge_state is None:
            return "needs_prep"
        if self.mergeable == "CONFLICTING":
            return "needs_prep"
        if self.merge_state in ("BEHIND", "DIRTY", "BLOCKED"):
            return "needs_prep"
        if self.review_decision == "CHANGES_REQUESTED":
            return "needs_prep"
        if self.review_in_progress():
            return "needs_prep"
        if not self.checks_ok():
            return "needs_prep"
        if self.mergeable == "MERGEABLE" and self.merge_state in (
            "CLEAN",
            "HAS_HOOKS",
            "UNSTABLE",
            "UNKNOWN",
        ):
            return "merge_ready"
        if self.mergeable == "UNKNOWN":
            return "needs_prep"
        return "needs_prep"


def base_branch_merged(base_ref: str, owner: str, cache: dict[str, bool]) -> bool:
    if base_ref == DEFAULT_BRANCH:
        return True
    if base_ref in cache:
        return cache[base_ref]
    merged = False
    for query in (
        ["pr", "list", "--state", "merged", "--head", base_ref, "--limit", "1"],
        ["pr", "list", "--state", "merged", "--head", f"{owner}:{base_ref}", "--limit", "1"],
    ):
        try:
            rows = gh_json(query + ["--json", "number"])
            if rows:
                merged = True
                break
        except RuntimeError:
            pass
    if not merged:
        try:
            merged = git_ancestor(base_ref)
        except Exception:
            merged = False
    cache[base_ref] = merged
    return merged


def enrich_pr_from_view(pr: PullRequest) -> None:
    """Load checks, reviews, and body (issue links) for merge prep classification."""
    try:
        detail = gh_json(
            [
                "pr",
                "view",
                str(pr.number),
                "--json",
                "isDraft,baseRefName,mergeable,mergeStateStatus,reviewDecision,"
                "statusCheckRollup,reviews,body",
            ]
        )
    except RuntimeError as exc:
        print(
            f"warning: failed to enrich PR #{pr.number}: {exc}",
            file=sys.stderr,
        )
        return
    body = detail.get("body") or ""
    pr.enrich(detail, body)


def fetch_open_prs(owner: str) -> list[PullRequest]:
    rows = gh_json(
        [
            "pr",
            "list",
            "--state",
            "open",
            "--limit",
            str(OPEN_PR_LIMIT),
            "--json",
            PR_LIST_FIELDS,
        ]
    )
    prs: list[PullRequest] = []
    for row in rows:
        pr = PullRequest.from_summary(row)
        detail = {
            "isDraft": row.get("isDraft"),
            "baseRefName": row.get("baseRefName"),
            "mergeable": row.get("mergeable"),
            "mergeStateStatus": row.get("mergeStateStatus"),
            "reviewDecision": row.get("reviewDecision"),
        }
        pr.enrich(detail, "")
        prs.append(pr)
    if len(rows) >= OPEN_PR_LIMIT:
        print(
            f"warning: open PR count may exceed OPEN_PR_LIMIT={OPEN_PR_LIMIT}; "
            "raise the cap in find_work.py",
            file=sys.stderr,
        )
    return prs


def classify_prs(prs: list[PullRequest], owner: str) -> dict[str, Any]:
    merged_cache: dict[str, bool] = {}
    unique_bases = {pr.base_ref for pr in prs if pr.base_ref != DEFAULT_BRANCH}
    prefetch_git_refs(unique_bases)
    eligible: list[dict[str, Any]] = []
    skipped: list[dict[str, Any]] = []
    for pr in prs:
        entry = {
            "number": pr.number,
            "title": pr.title,
            "url": pr.url,
            "baseRefName": pr.base_ref,
            "headRefName": pr.head_ref,
            "isDraft": pr.is_draft,
        }
        if pr.base_ref == DEFAULT_BRANCH or base_branch_merged(pr.base_ref, owner, merged_cache):
            enrich_pr_from_view(pr)
            entry["isDraft"] = pr.is_draft
            entry["prep"] = pr.classify_prep()
            eligible.append(entry)
        else:
            entry["blockingParent"] = pr.base_ref
            skipped.append(entry)
    needs_prep = sorted(
        [p for p in eligible if p["prep"] == "needs_prep"],
        key=lambda p: p["number"],
    )
    merge_ready = sorted(
        [p for p in eligible if p["prep"] == "merge_ready"],
        key=lambda p: p["number"],
    )
    return {
        "total": len(prs),
        "eligible": eligible,
        "skipped": skipped,
        "needs_prep": needs_prep,
        "merge_ready": merge_ready,
    }


@dataclass
class Issue:
    number: int
    title: str
    state: str
    body: str
    draft_id: str | None
    blocked_by: list[int]
    lane: str | None
    spec_path: str | None
    in_flight_branch: str | None = None

    @property
    def is_epic(self) -> bool:
        return title_is_epic(self.title)


def fetch_issues(id_to_num: dict[str, int]) -> list[Issue]:
    rows = gh_json(
        [
            "issue",
            "list",
            "--label",
            "rust-migration",
            "--state",
            "open",
            "--limit",
            "200",
            "--json",
            "number,title,body,state",
        ]
    )
    issues: list[Issue] = []
    for row in rows:
        title = row["title"]
        body = row.get("body") or ""
        draft_id = parse_draft_id(title)
        blocked = parse_blocked_by(body, id_to_num)
        lane = extract_lane(body)
        spec = local_spec_path(draft_id) if draft_id else None
        if not lane and spec:
            try:
                lane = extract_lane((REPO_ROOT / spec).read_text(errors="replace"))
            except OSError:
                pass
        inflight = None
        m = IN_FLIGHT_RE.search(body)
        if m:
            inflight = m.group(1)
        issues.append(
            Issue(
                number=row["number"],
                title=title,
                state=row.get("state", "OPEN"),
                body=body,
                draft_id=draft_id,
                blocked_by=blocked,
                lane=lane,
                spec_path=spec,
                in_flight_branch=inflight,
            )
        )
    return issues


def dep_satisfaction(
    dep_num: int,
    issues_by_num: dict[int, Issue],
    prs: list[PullRequest],
    num_to_id: dict[int, str],
) -> dict[str, Any]:
    draft_id = num_to_id.get(dep_num)
    dep = issues_by_num.get(dep_num)
    if dep is None:
        # Open-issue fetch only returns still-open children; absent deps are closed.
        return {
            "number": dep_num,
            "draftId": draft_id,
            "satisfied": True,
            "via": "closed",
        }
    if dep.state.upper() == "CLOSED":
        return {"number": dep_num, "draftId": draft_id, "satisfied": True, "via": "closed"}
    implementing = [
        p
        for p in prs
        if p.implements_issue(dep_num, draft_id)
    ]
    if implementing:
        # tip of chain: prefer highest PR number (latest stack layer)
        tip = max(implementing, key=lambda p: p.number)
        return {
            "number": dep_num,
            "draftId": draft_id,
            "satisfied": True,
            "via": "open_pr",
            "pr": {
                "number": tip.number,
                "url": tip.url,
                "headRefName": tip.head_ref,
            },
        }
    return {
        "number": dep_num,
        "draftId": draft_id,
        "satisfied": False,
        "via": "blocked",
        "reason": "open issue with no implementing PR",
    }


def issue_has_open_pr(issue: Issue, prs: list[PullRequest]) -> PullRequest | None:
    matches = [p for p in prs if p.implements_issue(issue.number, issue.draft_id)]
    if matches:
        return max(matches, key=lambda p: p.number)
    if issue.in_flight_branch:
        for p in prs:
            if issue.in_flight_branch in p.head_ref:
                return p
    return None


def stack_base_from_deps(deps: list[dict[str, Any]]) -> str:
    pr_deps = [
        d
        for d in deps
        if d.get("satisfied") and d.get("via") == "open_pr" and d.get("pr")
    ]
    if not pr_deps:
        return DEFAULT_BRANCH
    tip = max(pr_deps, key=lambda d: wave_sort_key(d.get("draftId")))
    return tip["pr"]["headRefName"]


def frontier_sort_key(item: dict[str, Any]) -> tuple:
    return (*wave_sort_key(item.get("draftId")), item["number"])


def wave_sort_key(draft_id: str | None) -> tuple:
    if not draft_id:
        return (9, 0, draft_id or "")
    prefix = draft_id.split("-", 1)[0]
    num = 0
    if "-" in draft_id:
        try:
            num = int(draft_id.split("-", 1)[1])
        except ValueError:
            pass
    order = {"T": 0, "A": 1, "W": 2, "R": 3}.get(prefix[0], 4)
    return (order, num, draft_id)


def count_dependents(issues: list[Issue]) -> dict[int, int]:
    counts: dict[int, int] = {i.number: 0 for i in issues}
    for issue in issues:
        for dep in issue.blocked_by:
            counts[dep] = counts.get(dep, 0) + 1
    return counts


def lane_in_flight(lane: str | None, prs: list[PullRequest], issues_by_num: dict[int, Issue]) -> bool:
    if not lane:
        return False
    for pr in prs:
        for did in pr.draft_ids:
            iss = next((i for i in issues_by_num.values() if i.draft_id == did), None)
            if iss and iss.lane == lane:
                return True
        token = normalize_branch_token(lane)
        if token and token in normalize_branch_token(pr.head_ref):
            return True
    return False


def analyze_issues(
    issues: list[Issue],
    prs: list[PullRequest],
    num_to_id: dict[int, str],
    override_draft: str | None = None,
) -> dict[str, Any]:
    children = [i for i in issues if not i.is_epic]
    issues_by_num = {i.number: i for i in children}
    dependents = count_dependents(children)

    candidates: list[dict[str, Any]] = []
    blocked_issues: list[dict[str, Any]] = []
    in_flight_heads: list[dict[str, Any]] = []

    for issue in children:
        dep_info = [
            dep_satisfaction(n, issues_by_num, prs, num_to_id) for n in issue.blocked_by
        ]
        unsatisfied = [d for d in dep_info if not d["satisfied"]]
        open_pr = issue_has_open_pr(issue, prs)

        if unsatisfied:
            blocked_issues.append(
                {
                    "number": issue.number,
                    "draftId": issue.draft_id,
                    "title": issue.title,
                    "unsatisfiedDeps": unsatisfied,
                }
            )
            continue

        if open_pr:
            in_flight_heads.append(
                {
                    "number": issue.number,
                    "draftId": issue.draft_id,
                    "title": issue.title,
                    "pr": {
                        "number": open_pr.number,
                        "url": open_pr.url,
                        "headRefName": open_pr.head_ref,
                    },
                }
            )
            continue

        stack_base = stack_base_from_deps(dep_info)
        candidates.append(
            {
                "number": issue.number,
                "draftId": issue.draft_id,
                "title": issue.title,
                "lane": issue.lane,
                "specPath": issue.spec_path,
                "stackBase": stack_base,
                "deps": dep_info,
                "parallelLane": not lane_in_flight(issue.lane, prs, issues_by_num),
                "dependentCount": dependents.get(issue.number, 0),
                "sortKey": wave_sort_key(issue.draft_id),
            }
        )

    # Prioritize
    def rank(c: dict[str, Any]) -> tuple:
        override = 0
        if override_draft and c.get("draftId") == override_draft:
            override = -1
        return (
            override,
            0 if c["parallelLane"] else 1,
            -c["dependentCount"],
            c["sortKey"],
        )

    candidates.sort(key=rank)
    selected = candidates[0] if candidates else None

    override_warning: str | None = None
    if override_draft:
        if selected and selected.get("draftId") != override_draft:
            override_warning = (
                f"override {override_draft} ignored — not ready; "
                f"selected {selected.get('draftId')}"
            )
        elif not selected:
            override_warning = f"override {override_draft} ignored — not ready"

    # Frontier diagnosis (lowest draft ID, not lowest GitHub number)
    blocked_issues.sort(key=frontier_sort_key)
    chain_head_blocked = blocked_issues[0] if blocked_issues else None

    in_flight_heads.sort(key=frontier_sort_key)
    chain_head_in_flight = in_flight_heads[0] if in_flight_heads else None

    # Per-lane saturation: ≥15 open children behind the same single blocker
    lanes_with_children = {i.lane or "__unknown__" for i in children}
    saturated_lanes: set[str] = set()
    saturation: dict[str, Any] | None = None
    lane_behind_counts: dict[tuple[str, int], int] = {}

    for item in blocked_issues:
        if len(item["unsatisfiedDeps"]) != 1:
            continue
        blocker = item["unsatisfiedDeps"][0]["number"]
        issue = issues_by_num.get(item["number"])
        lane = (issue.lane if issue else None) or "__unknown__"
        key = (lane, blocker)
        lane_behind_counts[key] = lane_behind_counts.get(key, 0) + 1

    for (lane, blocker), count in lane_behind_counts.items():
        if count >= SINGLE_LANE_SATURATION:
            saturated_lanes.add(lane)
            if saturation is None:
                saturation = {
                    "lane": None if lane == "__unknown__" else lane,
                    "chainHeadBlocker": blocker,
                    "openChildrenBehind": count,
                    "threshold": SINGLE_LANE_SATURATION,
                }

    whole_wave_saturated = bool(
        lanes_with_children
        and lanes_with_children.issubset(saturated_lanes)
    )

    # Path C only when one lane is saturated (≥15 behind same blocker) and other
    # parallel lanes remain draftable. A blocked chain head with <15 filed children
    # yields stop, not Path C (pick-up-work-item).
    path_c_gap = False
    path_c_reason: str | None = None
    if not selected and not whole_wave_saturated:
        other_lanes_draftable = bool(lanes_with_children - saturated_lanes)
        if saturation and other_lanes_draftable:
            path_c_gap = True
            path_c_reason = "single_lane_saturated"

    return {
        "openChildren": len(children),
        "readyCount": len(candidates),
        "selected": selected,
        "readyCandidates": candidates[:10],
        "chainHeadBlocked": chain_head_blocked,
        "chainHeadInFlight": chain_head_in_flight,
        "saturation": saturation,
        "wholeWaveSaturated": whole_wave_saturated,
        "pathCGap": path_c_gap,
        "pathCReason": path_c_reason,
        "overrideWarning": override_warning,
    }


def decide_path(
    pr_report: dict[str, Any],
    issue_report: dict[str, Any],
) -> dict[str, Any]:
    needs_prep = pr_report["needs_prep"]
    if needs_prep:
        return {
            "path": "A",
            "reason": f"{len(needs_prep)} eligible PR(s) need prep",
            "action": "prepare-all-prs-for-merge",
        }
    if issue_report.get("selected"):
        sel = issue_report["selected"]
        result = {
            "path": "B",
            "reason": f"ready issue {sel.get('draftId') or sel['number']}",
            "action": "triage → plan-stacked-prs → implement-stacked-prs",
            "selected": sel,
        }
        if issue_report.get("overrideWarning"):
            result["overrideWarning"] = issue_report["overrideWarning"]
        return result
    if issue_report.get("wholeWaveSaturated"):
        return {
            "path": "stop",
            "reason": "whole-wave saturated — implement/merge instead of drafting",
            "action": "report saturation counts",
        }
    if issue_report.get("chainHeadInFlight"):
        inf = issue_report["chainHeadInFlight"]
        return {
            "path": "stop",
            "reason": "chain head in-flight only",
            "action": "report frontier PR; prep if needs_prep on next run",
            "chainHeadInFlight": inf,
        }
    if issue_report.get("pathCGap"):
        return {
            "path": "C",
            "reason": issue_report.get("pathCReason") or "no ready issue",
            "action": "draft-migration-issues",
        }
    if issue_report.get("chainHeadBlocked"):
        blk = issue_report["chainHeadBlocked"]
        return {
            "path": "stop",
            "reason": "chain head blocked — no accessible dependency code",
            "action": "report blocker",
            "chainHeadBlocked": blk,
        }
    return {
        "path": "stop",
        "reason": "no work identified",
        "action": "report status",
    }


def format_human(report: dict[str, Any]) -> str:
    lines: list[str] = []
    prs = report.get("prs")
    if prs is not None:
        lines.append("## PR queue")
        lines.append(
            f"total={prs['total']} eligible={len(prs['eligible'])} "
            f"needs_prep={len(prs['needs_prep'])} merge_ready={len(prs['merge_ready'])} "
            f"skipped={len(prs['skipped'])}"
        )
        for p in prs["needs_prep"]:
            lines.append(f"  needs_prep: #{p['number']} {p['title']}")
        for p in prs["merge_ready"]:
            lines.append(f"  merge_ready: #{p['number']} {p['title']}")
        for p in prs["skipped"]:
            lines.append(
                f"  skipped: #{p['number']} (base {p['baseRefName']} not on master)"
            )
        lines.append("")

    path = report.get("pathDecision", {})
    if path:
        lines.append(f"## Path: {path.get('path')} — {path.get('reason')}")
        lines.append(f"Action: {path.get('action')}")

    issues = report.get("issues")
    if issues:
        sel = issues.get("selected")
        if sel:
            lines.append("")
            lines.append(
                f"**Selected:** {sel.get('draftId')} / #{sel['number']} — {sel['title']}"
            )
            lines.append(f"**Stack base:** `{sel.get('stackBase')}`")
            if sel.get("specPath"):
                lines.append(f"**Spec:** {sel['specPath']}")
        elif issues.get("chainHeadBlocked"):
            blk = issues["chainHeadBlocked"]
            dep = blk["unsatisfiedDeps"][0]
            lines.append("")
            lines.append(
                f"**Chain head blocked:** {blk.get('draftId')} / #{blk['number']} "
                f"(blocker #{dep['number']} {dep.get('draftId')})"
            )
        elif issues.get("chainHeadInFlight"):
            inf = issues["chainHeadInFlight"]
            lines.append("")
            lines.append(
                f"**Chain head in-flight:** {inf.get('draftId')} / #{inf['number']} "
                f"PR #{inf['pr']['number']}"
            )
        lines.append(f"Ready candidates: {issues.get('readyCount', 0)}")
        if issues.get("overrideWarning"):
            lines.append(f"Warning: {issues['overrideWarning']}")
        if issues.get("wholeWaveSaturated"):
            lines.append("Whole-wave saturated: yes")
    return "\n".join(lines)


def build_report(override_issue: str | None = None) -> dict[str, Any]:
    owner = repo_owner()
    id_to_num = load_issue_map()
    num_to_id = reverse_map(id_to_num)
    prs = fetch_open_prs(owner)
    pr_report = classify_prs(prs, owner)
    issues = fetch_issues(id_to_num)
    issue_report = analyze_issues(issues, prs, num_to_id, override_issue)
    path_decision = decide_path(pr_report, issue_report)
    return {
        "prs": pr_report,
        "issues": issue_report,
        "pathDecision": path_decision,
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "command",
        choices=["path", "prs", "issues"],
        help="path=full pick-up decision; prs=PR classification; issues=ready selection",
    )
    parser.add_argument("--human", action="store_true", help="Human-readable output")
    parser.add_argument(
        "--issue",
        dest="override_issue",
        help="User override draft ID (e.g. W3-40) for issue prioritization",
    )
    args = parser.parse_args()

    try:
        report = build_report(args.override_issue)
    except RuntimeError as exc:
        print(str(exc), file=sys.stderr)
        return 1

    if args.command == "prs":
        out = report["prs"]
    elif args.command == "issues":
        out = report["issues"]
    else:
        out = report

    if args.human:
        if args.command == "prs":
            print(format_human({"prs": out}))
        elif args.command == "issues":
            print(format_human({"issues": out}))
        else:
            print(format_human(out))
    else:
        print(json.dumps(out, indent=2))
    return 0


if __name__ == "__main__":
    sys.exit(main())
