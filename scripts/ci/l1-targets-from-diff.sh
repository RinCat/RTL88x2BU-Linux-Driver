#!/bin/bash
# Map changed paths to rust-check-symbols-* make targets (T7 PR3).
# Prints a space-separated target list on stdout; full suite on uncertainty.
set -euo pipefail

BASE_REF="${1:-origin/master}"

if ! git rev-parse --verify "${BASE_REF}" >/dev/null 2>&1; then
	echo "l1-targets-from-diff: cannot resolve ${BASE_REF}, using full suite" >&2
	BASE_REF="HEAD~1"
fi

mapfile -t changed < <(
	git diff --name-only "${BASE_REF}"...HEAD 2>/dev/null \
		|| git diff --name-only "${BASE_REF}" HEAD
)

FULL_SUITE=0
declare -A selected=()

add_target() {
	selected["$1"]=1
}

for path in "${changed[@]}"; do
	case "${path}" in
	scripts/ci/l1-targets-from-diff.sh \
	| scripts/ci/rustfmt-check.sh \
	| scripts/ci/run-l1-unit-checks.sh)
		: # CI helper scripts — do not expand to full L1 suite
		;;
	.github/workflows/rust-lint.yml \
	| docs/rust-migration/dev-environment.md \
	| docs/rust-migration/test-plan.md)
		: # docs / rustfmt workflow — no L1 swap
		;;
	Makefile \
	| docs/rust-migration/scripts/* \
	| scripts/ci/* \
	| .github/workflows/module-l1.yml)
		FULL_SUITE=1
		break
		;;
	rust/rtw_chplan.rs \
	| core/rtw_chplan.c \
	| core/rtw_chplan.h)
		add_target rust-check-symbols-rtw-chplan
		;;
	rust/rtw_chplan_rest.rs \
	| core/rtw_chplan_rest.c \
	| tests/host/chplan/*rest*)
		add_target rust-check-symbols-rtw-chplan-rest
		;;
	rust/rtw_io_rest.rs \
	| core/rtw_io_rest.c \
	| tests/host/io/*)
		add_target rust-check-symbols-rtw-io-rest
		;;
	rust/rtw_swcrypto.rs \
	| core/rtw_swcrypto.c \
	| core/rtw_swcrypto_rest.c)
		add_target rust-check-symbols-rtw-swcrypto
		;;
	rust/rtw_ieee80211.rs \
	| core/rtw_ieee80211.c)
		add_target rust-check-symbols-rtw-ieee80211
		;;
	rust/rtw_ieee80211_rest.rs \
	| core/rtw_ieee80211_rest.c \
	| tests/host/ie/*rest*)
		add_target rust-check-symbols-rtw-ieee80211-rest
		;;
	tests/host/ie/*)
		add_target rust-check-symbols-rtw-ieee80211
		;;
	rust/rtw_security.rs \
	| rust/rtw_security_rest.rs \
	| core/rtw_security.c \
	| core/rtw_security_rest.c \
	| tests/host/security/*)
		add_target rust-check-symbols-rtw-security
		add_target rust-check-symbols-rtw-security-rest-misc
		;;
	rust/rtw_wlan_util.rs \
	| core/rtw_wlan_util.c \
	| tests/host/wlan_util/*)
		add_target rust-check-symbols-rtw-wlan-util
		;;
	rust/rtw_rf_rest.rs \
	| rust/rtw_rf_op_class_pref.rs \
	| rust/rtw_rf_op_class_dump.rs \
	| rust/rtw_rf_dump_txpwr_lmt.rs \
	| rust/rtw_rf_kfree_tx_gain.rs \
	| core/rtw_rf_rest.c \
	| core/rtw_rf_op_class_pref.c \
	| core/rtw_rf_op_class_dump.c \
	| core/rtw_rf_dump_txpwr_lmt.c \
	| core/rtw_rf_kfree_tx_gain.c \
	| tests/host/rf/*)
		add_target rust-check-symbols-rtw-rf-rest
		add_target rust-check-symbols-rtw-rf-op-class-pref
		add_target rust-check-symbols-rtw-rf-op-class-dump
		add_target rust-check-symbols-rtw-rf-dump-txpwr-lmt
		add_target rust-check-symbols-rtw-rf-kfree-tx-gain
		;;
	rust/rtw_vht.rs \
	| core/rtw_vht.c \
	| core/rtw_vht_rest.c \
	| tests/host/vht/*)
		add_target rust-check-symbols-rtw-vht
		add_target rust-check-symbols-rtw-vht-restructure
		;;
	rust/rtw_sta_mgt.rs \
	| rust/rtw_sta_mgt_aid.rs \
	| rust/rtw_sta_mgt_stctl.rs \
	| rust/rtw_sta_mgt_lookup.rs \
	| rust/rtw_ap_rest.rs \
	| core/rtw_sta_mgt.c \
	| core/rtw_sta_mgt_rest.c \
	| core/rtw_sta_mgt_stctl.c \
	| core/rtw_sta_mgt_lookup.c \
	| core/rtw_ap_rest.c \
	| tests/host/sta_mgt/*)
		add_target rust-check-symbols-rtw-sta-mgt
		add_target rust-check-symbols-rtw-sta-mgt-aid
		add_target rust-check-symbols-rtw-sta-mgt-stctl
		add_target rust-check-symbols-rtw-sta-mgt-lookup
		;;
	rust/rtw_ap_bcn_ie.rs \
	| core/rtw_ap_bcn_ie.c \
	| core/rtw_ap_bcn_ie_rust_acc.c \
	| tests/host/ap/ap_bcn_ie*)
		add_target rust-check-symbols-rtw-ap-bcn-ie
		;;
	rust/rtw_ap_rest.rs \
	| rust/rtw_ap_sta_alive.rs \
	| rust/rtw_ap_expire_asoc.rs \
	| rust/rtw_ap_expire_auth.rs \
	| rust/rtw_ap_aka_chk.rs \
	| rust/rtw_ap_rf18_restore.rs \
	| rust/rtw_ap_expire_timeout.rs \
	| core/rtw_ap_rest.c \
	| core/rtw_ap_sta_alive.c \
	| core/rtw_ap_sta_alive_rust_acc.c \
	| core/rtw_ap_expire_asoc.c \
	| core/rtw_ap_expire_asoc_rust_acc.c \
	| core/rtw_ap_expire_auth.c \
	| core/rtw_ap_expire_auth_rust_acc.c \
	| core/rtw_ap_aka_chk.c \
	| core/rtw_ap_aka_chk_rust_acc.c \
	| core/rtw_ap_rf18_restore.c \
	| core/rtw_ap_rf18_restore_rust_acc.c \
	| core/rtw_ap_expire_timeout.c \
	| core/rtw_ap_expire_timeout_rust_acc.c \
	| tests/host/ap/*)
		add_target rust-check-symbols-rtw-ap-rest
		add_target rust-check-symbols-rtw-ap-sta-alive
		add_target rust-check-symbols-rtw-ap-expire-asoc
		add_target rust-check-symbols-rtw-ap-expire-auth
		add_target rust-check-symbols-rtw-ap-aka-chk
		add_target rust-check-symbols-rtw-ap-rf18-restore
		add_target rust-check-symbols-rtw-ap-expire-timeout
		;;
	rust/rtw_recv.rs \
	| rust/rtw_recv_sta_count.rs \
	| rust/rtw_recv_sta_validate.rs \
	| core/rtw_recv_rest.c \
	| core/rtw_recv_llc_rest.c \
	| core/rtw_recv_pn_rest.c \
	| core/rtw_recv_sta_rest.c \
	| tests/host/recv/*)
		add_target rust-check-symbols-rtw-recv
		add_target rust-check-symbols-rtw-recv-pn
		add_target rust-check-symbols-rtw-recv-sta-count
		add_target rust-check-symbols-rtw-recv-sta-validate
		;;
	rust/rtw_xmit.rs \
	| core/rtw_xmit_rest.c \
	| core/rtw_xmit_qos_rest.c \
	| core/rtw_xmit_sctx_rest.c \
	| tests/host/xmit/*)
		add_target rust-check-symbols-rtw-xmit
		add_target rust-check-symbols-rtw-xmit-qos
		add_target rust-check-symbols-rtw-xmit-sctx
		;;
	rust/rtw_iol_rest.rs \
	| core/rtw_iol_rest.c \
	| tests/host/iol/*)
		add_target rust-check-symbols-rtw-iol-rest
		;;
	rust/rtw_cmd_rest.rs \
	| core/rtw_cmd_priv.c \
	| core/rtw_cmd_queue.c \
	| tests/host/cmd/*)
		add_target rust-check-symbols-rtw-cmd-rest
		add_target rust-check-symbols-rtw-cmd-queue
		;;
	rust/aes_*.rs \
	| rust/sha256*.rs \
	| rust/gcmp.rs \
	| rust/gcmp_support.rs \
	| rust/ccmp.rs \
	| rust/ccmp_support.rs \
	| rust/domain/* \
	| rust/scaffold.rs \
	| rust/ffi.rs \
	| rust/kbuild_stub.rs \
	| rust/bindings/* \
	| rust/domain_types.rs \
	| rust/rtw_crypto_wrap.rs \
	| rust/rtw_rm_util.rs)
		: # Wave 1 crypto / scaffold — no per-module L1 swap target yet
		;;
	rust/aes_*.rs \
	| rust/sha256*.rs \
	| rust/gcmp.rs \
	| rust/gcmp_support.rs \
	| rust/ccmp.rs \
	| rust/ccmp_support.rs \
	| rust/domain/* \
	| rust/scaffold.rs \
	| rust/ffi.rs \
	| rust/kbuild_stub.rs \
	| rust/bindings/* \
	| rust/domain_types.rs \
	| rust/rtw_crypto_wrap.rs \
	| rust/rtw_rm_util.rs)
		: # Wave 1 crypto / scaffold — no per-module L1 swap target yet
		;;
	rust/aes_*.rs \
	| rust/sha256*.rs \
	| rust/gcmp.rs \
	| rust/gcmp_support.rs \
	| rust/ccmp.rs \
	| rust/ccmp_support.rs \
	| rust/domain/* \
	| rust/scaffold.rs \
	| rust/ffi.rs \
	| rust/kbuild_stub.rs \
	| rust/bindings/* \
	| rust/domain_types.rs \
	| rust/rtw_crypto_wrap.rs \
	| rust/rtw_rm_util.rs)
		: # Wave 1 crypto / scaffold — no per-module L1 swap target yet
		;;
	rust/*)
		FULL_SUITE=1
		break
		;;
	esac
done

ALL_TARGETS=(
	rust-check-symbols-rtw-chplan
	rust-check-symbols-rtw-chplan-rest
	rust-check-symbols-rtw-io-rest
	rust-check-symbols-rtw-swcrypto
	rust-check-symbols-rtw-ieee80211
	rust-check-symbols-rtw-ieee80211-rest
	rust-check-symbols-rtw-security
	rust-check-symbols-rtw-security-rest-misc
	rust-check-symbols-rtw-wlan-util
	rust-check-symbols-rtw-rf-rest
	rust-check-symbols-rtw-rf-op-class-pref
	rust-check-symbols-rtw-rf-op-class-dump
	rust-check-symbols-rtw-rf-dump-txpwr-lmt
	rust-check-symbols-rtw-vht
	rust-check-symbols-rtw-vht-restructure
	rust-check-symbols-rtw-sta-mgt
	rust-check-symbols-rtw-sta-mgt-aid
	rust-check-symbols-rtw-sta-mgt-stctl
	rust-check-symbols-rtw-sta-mgt-lookup
	rust-check-symbols-rtw-ap-rest
	rust-check-symbols-rtw-recv
	rust-check-symbols-rtw-recv-pn
	rust-check-symbols-rtw-xmit
	rust-check-symbols-rtw-xmit-qos
	rust-check-symbols-rtw-xmit-sctx
	rust-check-symbols-rtw-cmd-rest
)

emit() {
	local t
	for t in "$@"; do
		printf '%s ' "${t}"
	done
}

if [ "${FULL_SUITE}" = 1 ] || [ "${#changed[@]}" -eq 0 ]; then
	emit "${ALL_TARGETS[@]}"
	exit 0
fi

if [ "${#selected[@]}" -eq 0 ]; then
	exit 0
fi

for t in "${ALL_TARGETS[@]}"; do
	if [ -n "${selected[$t]+x}" ]; then
		emit "${t}"
	fi
done
