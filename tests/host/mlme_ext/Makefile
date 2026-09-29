# Host L2 differential tests for core/rtw_mlme_ext_rest.c (W3-54 chset, W3-68 mgnt attrib).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
MLME_EXT_REST_SRC := $(CORE)/rtw_mlme_ext_rest.c
RF_REST_SRC := $(CORE)/rtw_rf_rest.c
RUST_MLME_EXT_REST := $(ROOT)/rust/rtw_mlme_ext_rest.rs
RUST_MLME_EXT_MGNT_ATTRIB := $(ROOT)/rust/rtw_mlme_ext_mgnt_attrib.rs
RUST_MLME_EXT_PICK_CH := $(ROOT)/rust/rtw_mlme_ext_pick_ch.rs
RUST_MLME_EXT_SCAN := $(ROOT)/rust/rtw_mlme_ext_scan.rs
HOST_JSON := ../crypto/host_vector_json.c
SHIM := host_mlme_ext_shim.c
MGNT_SHIM := host_mlme_ext_mgnt_attrib_shim.c
PEER_ALIVE_SHIM := host_mlme_ext_peer_alive_shim.c
SCAN_SHIM := host_mlme_ext_scan_shim.c
BAND_IE_SHIM := host_mlme_ext_band_ie_shim.c
JOIN_CMD_SHIM := host_mlme_ext_join_cmd_shim.c
SITESURVEY_CMD_SHIM := host_mlme_ext_sitesurvey_cmd_shim.c
RUST_MLME_EXT_JOIN_CMD := $(ROOT)/rust/rtw_mlme_ext_join_cmd.rs
RUST_MLME_EXT_SITESURVEY_CMD := $(ROOT)/rust/rtw_mlme_ext_sitesurvey_cmd.rs
BAND_IE_RATE_ORACLE := band_ie_rate_oracle.c
RUST_MLME_EXT_BAND_IE := $(ROOT)/rust/rtw_mlme_ext_band_ie.rs
WLAN_UTIL_ORACLE := ../wlan_util/wlan_util_c_oracle.c

SCAN_CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_MLME_EXT_SCAN_TEST -DCONFIG_SCAN_BACKOP -DCONFIG_P2P

PICK_CH_CFLAGS := $(SCAN_CFLAGS) -DHOST_MLME_EXT_TEST -DCONFIG_P2P -DCONFIG_DFS_MASTER

BAND_IE_CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include -I../wlan_util \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_MLME_EXT_BAND_IE_TEST

MGNT_CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-format-truncation -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_MLME_EXT_MGNT_ATTRIB_TEST -DCONFIG_RTW_MGMT_QUEUE -DCONFIG_BEAMFORMING \
          -DCONFIG_P2P -DCONFIG_P2P_PS -DCONFIG_P2P_PS_NOA_USE_MACID_SLEEP -DCONFIG_CONCURRENT_MODE

PEER_ALIVE_CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-format-truncation -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_MLME_EXT_PEER_ALIVE_TEST -DCONFIG_TDLS

CC     := gcc
RUSTC  := rustc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-format-truncation -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_MLME_EXT_TEST -DHOST_RF_TEST -DCONFIG_DFS_MASTER

JOIN_CMD_CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_MLME_EXT_JOIN_CMD_TEST

SITESURVEY_CMD_CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_MLME_EXT_SITESURVEY_CMD_TEST -DCONFIG_SCAN_BACKOP

.PHONY: all test test-c test-rust test-mgnt-attrib test-mgnt-attrib-rust \
	test-mgnt-attrib-kernel-layout test-peer-alive test-scan-decision \
	test-scan-decision-rust test-scan-kernel-layout test-pick-ch \
	test-pick-ch-rust test-band-ie test-band-ie-rust test-join-cmd \
	test-join-cmd-rust test-sitesurvey-cmd test-sitesurvey-cmd-rust clean

all: test

test: test-c test-rust test-mgnt-attrib test-mgnt-attrib-rust \
	test-mgnt-attrib-kernel-layout test-peer-alive test-scan-decision \
	test-scan-decision-rust test-scan-kernel-layout test-pick-ch \
	test-pick-ch-rust test-band-ie test-band-ie-rust test-join-cmd \
	test-join-cmd-rust test-sitesurvey-cmd test-sitesurvey-cmd-rust

test-c: test_chset
	./test_chset chset_vectors.json

test_chset: test_chset.c $(MLME_EXT_REST_SRC) $(SHIM) $(RF_REST_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_chset.c $(MLME_EXT_REST_SRC) $(SHIM) \
		$(RF_REST_SRC) $(HOST_JSON)

librust_mlme_ext_rest.a: $(RUST_MLME_EXT_REST)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on \
		--cfg host_mlme_ext_test --cfg dfs_master \
		--crate-type staticlib $(RUST_MLME_EXT_REST) -o $@

test-rust: test_chset_rust
	./test_chset_rust chset_vectors.json

test_chset_rust: test_chset.c librust_mlme_ext_rest.a $(SHIM) $(RF_REST_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_MLME_EXT_REST_ORACLE -o $@ test_chset.c $(SHIM) \
		$(RF_REST_SRC) $(HOST_JSON) librust_mlme_ext_rest.a

test-mgnt-attrib: test_mgnt_attrib
	./test_mgnt_attrib mgnt_attrib_vectors.json

test_mgnt_attrib: test_mgnt_attrib.c $(MLME_EXT_REST_SRC) $(MGNT_SHIM) $(HOST_JSON)
	$(CC) $(MGNT_CFLAGS) -o $@ test_mgnt_attrib.c $(MLME_EXT_REST_SRC) $(MGNT_SHIM) \
		$(HOST_JSON)

librust_mlme_ext_mgnt_attrib.a: $(RUST_MLME_EXT_MGNT_ATTRIB)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on \
		--cfg host_mlme_ext_mgnt_attrib_test --cfg config_rtw_mgmt_queue \
		--cfg config_beamforming --cfg config_80211ac_vht \
		--cfg config_p2p_ps_noa_use_macid_sleep --cfg config_concurrent_mode \
		--crate-type staticlib $(RUST_MLME_EXT_MGNT_ATTRIB) -o $@

test-mgnt-attrib-rust: test_mgnt_attrib_rust
	./test_mgnt_attrib_rust mgnt_attrib_vectors.json

test_mgnt_attrib_rust: test_mgnt_attrib.c librust_mlme_ext_mgnt_attrib.a $(MGNT_SHIM) $(HOST_JSON)
	$(CC) $(MGNT_CFLAGS) -DRUST_MLME_EXT_MGNT_ATTRIB_ORACLE -o $@ test_mgnt_attrib.c \
		$(MGNT_SHIM) $(HOST_JSON) librust_mlme_ext_mgnt_attrib.a

# Kernel path uses --cfg rust_mlme_ext_mgnt_attrib and USB 8822B TXDESC_OFFSET 56.
librust_mlme_ext_mgnt_attrib_kernel.a: $(RUST_MLME_EXT_MGNT_ATTRIB)
	$(RUSTC) --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg rust_mlme_ext_mgnt_attrib --cfg config_rtw_mgmt_queue \
		--cfg config_beamforming \
		--crate-type staticlib $(RUST_MLME_EXT_MGNT_ATTRIB) -o $@

test-mgnt-attrib-kernel-layout: test_mgnt_attrib_kernel_layout
	./test_mgnt_attrib_kernel_layout

test_mgnt_attrib_kernel_layout: test_mgnt_attrib_kernel_layout.c \
		librust_mlme_ext_mgnt_attrib_kernel.a
	$(CC) -Wall -Wextra -Werror -O2 -o $@ test_mgnt_attrib_kernel_layout.c \
		librust_mlme_ext_mgnt_attrib_kernel.a -lpthread -ldl

test-peer-alive: test_peer_alive
	./test_peer_alive

test_peer_alive: test_peer_alive.c $(MLME_EXT_REST_SRC) $(PEER_ALIVE_SHIM)
	$(CC) $(PEER_ALIVE_CFLAGS) -o $@ test_peer_alive.c $(MLME_EXT_REST_SRC) \
		$(PEER_ALIVE_SHIM)

test-scan-decision: test_scan_decision
	./test_scan_decision

test_scan_decision: test_scan_decision.c $(MLME_EXT_REST_SRC) $(SCAN_SHIM)
	$(CC) $(SCAN_CFLAGS) -o $@ test_scan_decision.c $(MLME_EXT_REST_SRC) $(SCAN_SHIM)

librust_mlme_ext_scan.a: $(RUST_MLME_EXT_SCAN)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on \
		--cfg host_mlme_ext_scan_test \
		--cfg config_scan_sparse_bg --cfg config_scan_sparse_miracast \
		--crate-type staticlib $(RUST_MLME_EXT_SCAN) -o $@

test-scan-decision-rust: test_scan_decision_rust
	./test_scan_decision_rust

test_scan_decision_rust: test_scan_decision.c librust_mlme_ext_scan.a $(SCAN_SHIM)
	$(CC) $(SCAN_CFLAGS) -o $@ test_scan_decision.c $(SCAN_SHIM) librust_mlme_ext_scan.a

# Kernel path: --cfg rust_mlme_ext_scan (no host overlay of packed mi_state).
librust_mlme_ext_scan_kernel.a: $(RUST_MLME_EXT_SCAN)
	$(RUSTC) --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg rust_mlme_ext_scan --cfg config_scan_backop \
		--crate-type staticlib $(RUST_MLME_EXT_SCAN) -o $@

test-scan-kernel-layout: test_scan_kernel_layout
	./test_scan_kernel_layout

test_scan_kernel_layout: test_scan_kernel_layout.c librust_mlme_ext_scan_kernel.a
	$(CC) -Wall -Wextra -Werror -O2 -o $@ test_scan_kernel_layout.c \
		librust_mlme_ext_scan_kernel.a -lpthread -ldl

test-pick-ch: test_pick_ch
	./test_pick_ch

test_pick_ch: test_pick_ch.c $(MLME_EXT_REST_SRC) $(SCAN_SHIM)
	$(CC) $(PICK_CH_CFLAGS) -o $@ test_pick_ch.c $(MLME_EXT_REST_SRC) $(SCAN_SHIM)

librust_mlme_ext_pick_ch.a: $(RUST_MLME_EXT_PICK_CH)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on \
		--cfg host_mlme_ext_pick_ch_test \
		--crate-type staticlib $(RUST_MLME_EXT_PICK_CH) -o $@

test-pick-ch-rust: test_pick_ch_rust
	./test_pick_ch_rust

test_pick_ch_rust: test_pick_ch.c librust_mlme_ext_pick_ch.a $(MLME_EXT_REST_SRC) $(SCAN_SHIM)
	$(CC) $(PICK_CH_CFLAGS) -DCONFIG_RUST_MLME_EXT_PICK_CH -DRUST_MLME_EXT_PICK_CH_ORACLE -o $@ test_pick_ch.c \
		$(MLME_EXT_REST_SRC) $(SCAN_SHIM) librust_mlme_ext_pick_ch.a

test-band-ie: test_band_ie
	./test_band_ie

librust_mlme_ext_band_ie.a: $(RUST_MLME_EXT_BAND_IE)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on \
		--cfg host_mlme_ext_band_ie_test --cfg config_80211ac_vht \
		--crate-type staticlib $(RUST_MLME_EXT_BAND_IE) -o $@

test-band-ie-rust: test_band_ie_rust
	./test_band_ie_rust

test_band_ie_rust: test_band_ie.c librust_mlme_ext_band_ie.a $(BAND_IE_SHIM) \
		$(BAND_IE_RATE_ORACLE) $(WLAN_UTIL_ORACLE)
	$(CC) $(BAND_IE_CFLAGS) -DRUST_MLME_EXT_BAND_IE_ORACLE -o $@ test_band_ie.c \
		$(BAND_IE_SHIM) $(BAND_IE_RATE_ORACLE) $(WLAN_UTIL_ORACLE) \
		librust_mlme_ext_band_ie.a

test_band_ie: test_band_ie.c $(MLME_EXT_REST_SRC) $(BAND_IE_SHIM) $(BAND_IE_RATE_ORACLE) \
		$(WLAN_UTIL_ORACLE)
	$(CC) $(BAND_IE_CFLAGS) -o $@ test_band_ie.c $(MLME_EXT_REST_SRC) $(BAND_IE_SHIM) \
		$(BAND_IE_RATE_ORACLE) $(WLAN_UTIL_ORACLE)

test-join-cmd: test_join_cmd
	./test_join_cmd mlme_ext_join_cmd_vectors.json

test_join_cmd: test_join_cmd.c $(MLME_EXT_REST_SRC) $(JOIN_CMD_SHIM) $(HOST_JSON)
	$(CC) $(JOIN_CMD_CFLAGS) -o $@ test_join_cmd.c $(MLME_EXT_REST_SRC) \
		$(JOIN_CMD_SHIM) $(HOST_JSON)

librust_mlme_ext_join_cmd.a: $(RUST_MLME_EXT_JOIN_CMD)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on \
		--cfg host_mlme_ext_join_cmd_test \
		--crate-type staticlib $(RUST_MLME_EXT_JOIN_CMD) -o $@

test-join-cmd-rust: test_join_cmd_rust
	./test_join_cmd_rust mlme_ext_join_cmd_vectors.json

test_join_cmd_rust: test_join_cmd.c librust_mlme_ext_join_cmd.a $(JOIN_CMD_SHIM) $(HOST_JSON)
	$(CC) $(JOIN_CMD_CFLAGS) -DRUST_MLME_EXT_JOIN_CMD_ORACLE -o $@ test_join_cmd.c \
		$(JOIN_CMD_SHIM) $(HOST_JSON) librust_mlme_ext_join_cmd.a

test-sitesurvey-cmd: test_sitesurvey_cmd
	./test_sitesurvey_cmd mlme_ext_sitesurvey_cmd_vectors.json

SITESURVEY_CMD_ORACLE := sitesurvey_cmd_host_oracle.c

test_sitesurvey_cmd: test_sitesurvey_cmd.c $(SITESURVEY_CMD_ORACLE) $(SITESURVEY_CMD_SHIM) \
		$(HOST_JSON)
	$(CC) $(SITESURVEY_CMD_CFLAGS) -o $@ test_sitesurvey_cmd.c $(SITESURVEY_CMD_ORACLE) \
		$(SITESURVEY_CMD_SHIM) $(HOST_JSON)

librust_mlme_ext_sitesurvey_cmd.a: $(RUST_MLME_EXT_SITESURVEY_CMD)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on \
		--cfg host_mlme_ext_sitesurvey_cmd_test \
		--crate-type staticlib $(RUST_MLME_EXT_SITESURVEY_CMD) -o $@

SITESURVEY_CMD_RUST_VECTORS ?= mlme_ext_sitesurvey_cmd_vectors_rust.json

test-sitesurvey-cmd-rust: test_sitesurvey_cmd_rust
	./test_sitesurvey_cmd_rust $(SITESURVEY_CMD_RUST_VECTORS)

test_sitesurvey_cmd_rust: test_sitesurvey_cmd.c librust_mlme_ext_sitesurvey_cmd.a \
		$(SITESURVEY_CMD_SHIM) $(HOST_JSON)
	$(CC) $(SITESURVEY_CMD_CFLAGS) -DRUST_MLME_EXT_SITESURVEY_CMD_ORACLE -o $@ \
		test_sitesurvey_cmd.c $(SITESURVEY_CMD_SHIM) $(HOST_JSON) \
		librust_mlme_ext_sitesurvey_cmd.a

clean:
	rm -f test_chset test_chset_rust librust_mlme_ext_rest.a \
		test_mgnt_attrib test_mgnt_attrib_rust librust_mlme_ext_mgnt_attrib.a \
		test_mgnt_attrib_kernel_layout librust_mlme_ext_mgnt_attrib_kernel.a \
		test_peer_alive test_peer_alive_rust librust_mlme_ext_peer_alive.a \
		test_scan_decision test_scan_decision_rust librust_mlme_ext_scan.a \
		test_scan_kernel_layout librust_mlme_ext_scan_kernel.a \
		test_pick_ch test_pick_ch_rust librust_mlme_ext_pick_ch.a \
		test_band_ie test_band_ie_rust librust_mlme_ext_band_ie.a \
		test_join_cmd test_join_cmd_rust librust_mlme_ext_join_cmd.a \
		test_sitesurvey_cmd test_sitesurvey_cmd_rust librust_mlme_ext_sitesurvey_cmd.a
