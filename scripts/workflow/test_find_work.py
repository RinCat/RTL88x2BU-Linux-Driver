#!/usr/bin/env python3
"""Lightweight tests for find_work helpers (no gh network)."""

import sys
import unittest
from pathlib import Path
from unittest import mock

REPO_ROOT = Path(__file__).resolve().parents[2]

import importlib.util

_spec = importlib.util.spec_from_file_location(
    "find_work", REPO_ROOT / "scripts/workflow/find_work.py"
)
assert _spec and _spec.loader
fw = importlib.util.module_from_spec(_spec)
fw.__name__ = "find_work"
sys.modules["find_work"] = fw
_spec.loader.exec_module(fw)


class PrefetchGitRefsTests(unittest.TestCase):
    def setUp(self) -> None:
        fw._GIT_FETCHED_REFS.clear()

    @mock.patch.object(fw, "subprocess")
    def test_batches_large_ref_sets(self, subprocess_mod: mock.Mock) -> None:
        run_mock = subprocess_mod.run
        branches = [f"cursor/branch-{i}" for i in range(95)]
        fw.prefetch_git_refs(branches)
        self.assertGreaterEqual(run_mock.call_count, 2)
        first_args = run_mock.call_args_list[0][0][0]
        self.assertEqual(first_args[0], "git")
        self.assertEqual(first_args[1], "fetch")
        self.assertIn(fw.DEFAULT_BRANCH, first_args)

    @mock.patch.object(fw, "subprocess")
    def test_skips_repeat_prefetch(self, subprocess_mod: mock.Mock) -> None:
        run_mock = subprocess_mod.run
        fw.prefetch_git_refs(["cursor/a"])
        fw.prefetch_git_refs(["cursor/a", "cursor/b"])
        self.assertEqual(run_mock.call_count, 2)


class ClassifyPrepTests(unittest.TestCase):
    def test_changes_requested_needs_prep(self) -> None:
        pr = fw.PullRequest(
            number=1,
            title="[W1-1] test",
            url="http://example",
            is_draft=False,
            base_ref=fw.DEFAULT_BRANCH,
            head_ref="cursor/test",
        )
        pr.enrichment_ok = True
        pr.mergeable = "MERGEABLE"
        pr.merge_state = "CLEAN"
        pr.review_decision = "CHANGES_REQUESTED"
        self.assertEqual(pr.classify_prep(), "needs_prep")


if __name__ == "__main__":
    unittest.main()
