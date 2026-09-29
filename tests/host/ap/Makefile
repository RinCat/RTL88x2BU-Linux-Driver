# Host L2 differential tests for core/rtw_ap_rest.c (W3-55, W3-73).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
AP_REST_SRC := $(CORE)/rtw_ap_rest.c
HOST_JSON := ../crypto/host_vector_json.c
RUST_AP_REST := $(ROOT)/rust/rtw_ap_rest.rs
RUST_AP_BMC_UPDATE := $(ROOT)/rust/rtw_ap_bmc_update.rs

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_AP_REST_TEST -DCONFIG_FW_HANDLE_TXBCN -DCONFIG_LIMITED_AP_NUM=4

AP_STA_IE_SRC := $(CORE)/rtw_ap_sta_ie.c
AP_STA_IE_RATES_SRC := $(CORE)/rtw_ap_sta_ie_rates.c
AP_STA_IE_WMM_HT_SRC := $(CORE)/rtw_ap_sta_ie_wmm_ht.c
AP_STA_IE_VHT_MULTIAP_SRC := $(CORE)/rtw_ap_sta_ie_vht_multiap.c
AP_STA_IE_SEC_SRC := $(CORE)/rtw_ap_sta_ie_sec.c
AP_BCN_IE_SRC := $(CORE)/rtw_ap_bcn_ie.c
AP_BCN_UPDATE_SRC := $(CORE)/rtw_ap_bcn_update.c
BCN_UPDATE_CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -O2 -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
	-include $(HOST_INC)/host_autoconf.h -DHOST_AP_BCN_UPDATE_TEST
BCN_UPDATE_DEPS := host_ap_bcn_update_shim.c
IE_REST_SRC := $(CORE)/rtw_ieee80211_rest.c
WLAN_UTIL := ../wlan_util
IE_ORACLE := ../ie/ie_c_oracle.c
STA_IE_CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_AP_STA_IE_TEST
STA_IE_RATES_CFLAGS := $(STA_IE_CFLAGS)
STA_IE_WMM_HT_CFLAGS := $(STA_IE_CFLAGS)
STA_IE_VHT_MULTIAP_CFLAGS := $(STA_IE_CFLAGS) -DCONFIG_RTW_MULTI_AP
STA_IE_SEC_CFLAGS := $(STA_IE_CFLAGS) -DHOST_AP_STA_IE_SEC_TEST \
	-DHOST_IEEE80211_REST_TEST -I$(WLAN_UTIL) -Wno-sign-compare
BCN_IE_CFLAGS := $(STA_IE_CFLAGS) -DHOST_AP_BCN_IE_TEST -Wno-sign-compare -Wno-pointer-sign
BCN_IE_DEPS := ap_bcn_ie_oracle.c
STA_IE_RATES_DEPS := ap_sta_ie_rates_oracle.c $(IE_ORACLE)
STA_IE_SEC_DEPS := ap_sta_ie_sec_oracle.c $(IE_ORACLE) ../ie/ie_cipher_c_oracle.c \
	../ie/ie_wpa_rsn_parse_c_oracle.c $(IE_REST_SRC) \
	$(WLAN_UTIL)/wlan_util_c_oracle.c ../ie/ie_mac_str_host_shim.c
STA_IE_WMM_HT_DEPS := $(IE_ORACLE)
STA_IE_VHT_MULTIAP_DEPS := ap_sta_ie_vht_multiap_oracle.c $(IE_ORACLE)

BMC_RATE_CFLAGS := $(CFLAGS) -DCONFIG_BMC_TX_RATE_SELECT
BMC_UPDATE_CFLAGS := $(BCN_UPDATE_CFLAGS) -DCONFIG_BMC_TX_RATE_SELECT -DCONFIG_BMC_TX_LOW_RATE

AP_STA_ALIVE_SRC := $(CORE)/rtw_ap_sta_alive.c
STA_ALIVE_CFLAGS := $(BCN_UPDATE_CFLAGS) -DHOST_AP_STA_ALIVE_TEST -I$(CORE)
AP_EXPIRE_AUTH_SRC := $(CORE)/rtw_ap_expire_auth.c
EXPIRE_AUTH_CFLAGS := $(BCN_UPDATE_CFLAGS) -DHOST_AP_EXPIRE_AUTH_TEST -I$(CORE)
EXPIRE_AUTH_DEPS := host_expire_auth_shim.c
AP_EXPIRE_ASOC_SRC := $(CORE)/rtw_ap_expire_asoc.c $(CORE)/rtw_ap_sta_alive.c
EXPIRE_ASOC_CFLAGS := $(BCN_UPDATE_CFLAGS) -DHOST_AP_EXPIRE_ASOC_TEST -I$(CORE)
AKA_CHK_CFLAGS := $(BCN_UPDATE_CFLAGS) -DCONFIG_ACTIVE_KEEP_ALIVE_CHECK \
	-DHOST_AP_AKA_CHK_TEST -I$(CORE)
AP_AKA_CHK_SRC := $(CORE)/rtw_ap_aka_chk.c
AKA_CHK_DEPS := host_aka_chk_shim.c
RF18_RESTORE_CFLAGS := $(BCN_UPDATE_CFLAGS) -DRTW_CONFIG_RFREG18_WA \
	-DHOST_AP_RF18_RESTORE_TEST -I$(CORE)
AP_RF18_RESTORE_SRC := $(CORE)/rtw_ap_rf18_restore.c
RF18_RESTORE_DEPS := host_rf18_restore_shim.c
AP_STA_RA_SRC := $(CORE)/rtw_ap_sta_ra.c
STA_RA_CFLAGS := $(BCN_UPDATE_CFLAGS) -DHOST_AP_STA_RA_TEST -I$(CORE)
STA_RA_DEPS :=
AP_STA_INFO_SRC := $(CORE)/rtw_ap_sta_info.c
STA_INFO_CFLAGS := $(BCN_UPDATE_CFLAGS) -DHOST_AP_STA_INFO_TEST -DCONFIG_BEAMFORMING -I$(CORE)
AP_STA_INFO_APMODE_SRC := $(CORE)/rtw_ap_sta_info_apmode.c
STA_INFO_APMODE_CFLAGS := $(BCN_UPDATE_CFLAGS) -DHOST_AP_STA_INFO_APMODE_TEST -I$(CORE)
STA_INFO_APMODE_DEPS := ap_sta_info_apmode_shim.c

.PHONY: all test test-c test-rust test-sta-ie test-bmc-rate test-bmc-update test-bcn-ie test-rust-bcn-ie test-bcn-update test-sta-alive test-rust-sta-alive test-expire-auth test-rust-expire-auth test-expire-asoc test-rust-expire-asoc test-aka-chk test-rust-aka-chk test-rf18-restore test-rust-rf18-restore test-sta-ra test-rust-sta-ra test-sta-info test-rust-sta-info test-sta-info-apmode test-rust-sta-info-apmode test-hw-ht-param test-rust-hw-ht-param clean

all: test

test: test-c test-rust test-sta-ie test-bcn-ie test-rust-bcn-ie test-bcn-update test-sta-alive test-rust-sta-alive test-expire-auth test-rust-expire-auth test-expire-asoc test-rust-expire-asoc test-aka-chk test-rust-aka-chk test-rf18-restore test-rust-rf18-restore test-sta-ra test-rust-sta-ra test-sta-info test-rust-sta-info test-sta-info-apmode test-rust-sta-info-apmode test-hw-ht-param test-rust-hw-ht-param test-bmc-rate test-bmc-update

test-c: test_ap_rest
	./test_ap_rest ap_rest_vectors.json

test_ap_rest: test_ap_rest.c $(AP_REST_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_ap_rest.c $(AP_REST_SRC) $(HOST_JSON)

librust_ap_rest.a: $(RUST_AP_REST)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_ap_rest_test --cfg fw_handle_txbcn \
		--crate-type staticlib $(RUST_AP_REST) -o $@

test-rust: test_ap_rest_rust
	./test_ap_rest_rust ap_rest_vectors.json

test_ap_rest_rust: test_ap_rest.c librust_ap_rest.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_AP_REST_ORACLE -o $@ test_ap_rest.c $(HOST_JSON) librust_ap_rest.a

test-sta-ie: test_ap_sta_ie test_ap_sta_ie_rates test_ap_sta_ie_wmm_ht test_ap_sta_ie_vht_multiap test_ap_sta_ie_sec
	./test_ap_sta_ie ap_sta_ie_vectors.json
	./test_ap_sta_ie_rates
	./test_ap_sta_ie_wmm_ht
	./test_ap_sta_ie_vht_multiap
	./test_ap_sta_ie_sec ap_sta_ie_sec_vectors.json

test-bcn-ie: test_ap_bcn_ie
	./test_ap_bcn_ie ap_bcn_ie_vectors.json

librust_ap_bcn_ie.a: ../../../rust/rtw_ap_bcn_ie.rs
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_ap_bcn_ie_test \
		--crate-type staticlib ../../../rust/rtw_ap_bcn_ie.rs -o $@

test-rust-bcn-ie: test_ap_bcn_ie_rust
	./test_ap_bcn_ie_rust ap_bcn_ie_vectors.json

test_ap_bcn_ie_rust: test_ap_bcn_ie.c librust_ap_bcn_ie.a $(HOST_JSON) $(BCN_IE_DEPS)
	$(CC) $(BCN_IE_CFLAGS) -DRUST_AP_BCN_IE_ORACLE -o $@ test_ap_bcn_ie.c \
		$(HOST_JSON) $(BCN_IE_DEPS) librust_ap_bcn_ie.a

test-bcn-update: test_ap_bcn_update
	./test_ap_bcn_update ap_bcn_update_vectors.json

test-sta-alive: test_ap_sta_alive
	./test_ap_sta_alive ap_sta_alive_vectors.json

test-expire-auth: test_ap_expire_auth
	./test_ap_expire_auth ap_expire_auth_vectors.json

test-rust-expire-auth: test_ap_expire_auth_rust
	./test_ap_expire_auth_rust ap_expire_auth_vectors.json

librust_ap_expire_auth.a: ../../../rust/rtw_ap_expire_auth.rs
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_ap_expire_auth_test \
		--crate-type staticlib ../../../rust/rtw_ap_expire_auth.rs -o $@

test_ap_expire_auth_rust: test_ap_expire_auth.c librust_ap_expire_auth.a $(HOST_JSON) host_expire_auth_shim.c
	$(CC) $(EXPIRE_AUTH_CFLAGS) -DRUST_AP_EXPIRE_AUTH_ORACLE -o $@ test_ap_expire_auth.c \
		$(HOST_JSON) host_expire_auth_shim.c librust_ap_expire_auth.a

test-expire-asoc: test_ap_expire_asoc
	./test_ap_expire_asoc ap_expire_asoc_vectors.json

test-aka-chk: test_ap_aka_chk
	./test_ap_aka_chk ap_aka_chk_vectors.json

test-rf18-restore: test_ap_rf18_restore
	./test_ap_rf18_restore ap_rf18_restore_vectors.json

test-rust-rf18-restore: test_ap_rf18_restore_rust
	./test_ap_rf18_restore_rust ap_rf18_restore_vectors.json

librust_ap_rf18_restore.a: ../../../rust/rtw_ap_rf18_restore.rs
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_ap_rf18_restore_test \
		--crate-type staticlib ../../../rust/rtw_ap_rf18_restore.rs -o $@

test_ap_rf18_restore_rust: test_ap_rf18_restore.c librust_ap_rf18_restore.a $(HOST_JSON) host_rf18_restore_shim.c
	$(CC) $(RF18_RESTORE_CFLAGS) -DRUST_AP_RF18_RESTORE_ORACLE -o $@ test_ap_rf18_restore.c \
		$(HOST_JSON) host_rf18_restore_shim.c librust_ap_rf18_restore.a

test_ap_rf18_restore: test_ap_rf18_restore.c $(HOST_JSON) $(AP_RF18_RESTORE_SRC) $(RF18_RESTORE_DEPS)
	$(CC) $(RF18_RESTORE_CFLAGS) -o $@ test_ap_rf18_restore.c $(HOST_JSON) \
		$(AP_RF18_RESTORE_SRC) $(RF18_RESTORE_DEPS)

test-sta-ra: test_ap_sta_ra
	./test_ap_sta_ra ap_sta_ra_vectors.json

test-hw-ht-param: test_ap_hw_ht_param
	./test_ap_hw_ht_param ap_hw_ht_param_vectors.json

test_ap_hw_ht_param: test_ap_hw_ht_param.c $(HOST_JSON) $(AP_STA_INFO_SRC)
	$(CC) $(STA_INFO_CFLAGS) -o $@ test_ap_hw_ht_param.c $(HOST_JSON) $(AP_STA_INFO_SRC)

test-rust-hw-ht-param: test_ap_hw_ht_param_rust
	./test_ap_hw_ht_param_rust ap_hw_ht_param_vectors.json

test_ap_hw_ht_param_rust: test_ap_hw_ht_param.c librust_ap_sta_info.a $(HOST_JSON)
	$(CC) $(STA_INFO_CFLAGS) -o $@ test_ap_hw_ht_param.c \
		$(HOST_JSON) librust_ap_sta_info.a

test-sta-info: test_ap_sta_info
	./test_ap_sta_info ap_sta_info_vectors.json

test-sta-info-apmode: test_ap_sta_info_apmode
	./test_ap_sta_info_apmode ap_sta_info_apmode_vectors.json

test_ap_sta_info_apmode: test_ap_sta_info_apmode.c $(HOST_JSON) $(AP_STA_INFO_APMODE_SRC) $(STA_INFO_APMODE_DEPS)
	$(CC) $(STA_INFO_APMODE_CFLAGS) -o $@ test_ap_sta_info_apmode.c $(HOST_JSON) \
		$(AP_STA_INFO_APMODE_SRC) $(STA_INFO_APMODE_DEPS)

test-rust-sta-info-apmode: test_ap_sta_info_apmode_rust
	./test_ap_sta_info_apmode_rust ap_sta_info_apmode_vectors.json

librust_ap_sta_info_apmode.a: ../../../rust/rtw_ap_sta_info_apmode.rs
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_ap_sta_info_apmode_test \
		--crate-type staticlib ../../../rust/rtw_ap_sta_info_apmode.rs -o $@

test_ap_sta_info_apmode_rust: test_ap_sta_info_apmode.c librust_ap_sta_info_apmode.a $(HOST_JSON) $(STA_INFO_APMODE_DEPS)
	$(CC) $(STA_INFO_APMODE_CFLAGS) -DRUST_AP_STA_INFO_APMODE_ORACLE -o $@ test_ap_sta_info_apmode.c \
		$(HOST_JSON) $(STA_INFO_APMODE_DEPS) librust_ap_sta_info_apmode.a

test_ap_sta_info: test_ap_sta_info.c $(HOST_JSON) $(AP_STA_INFO_SRC)
	$(CC) $(STA_INFO_CFLAGS) -o $@ test_ap_sta_info.c $(HOST_JSON) $(AP_STA_INFO_SRC)

test-rust-sta-info: test_ap_sta_info_rust
	./test_ap_sta_info_rust ap_sta_info_vectors.json

librust_ap_sta_info.a: ../../../rust/rtw_ap_sta_info.rs
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_ap_sta_info_test \
		--crate-type staticlib ../../../rust/rtw_ap_sta_info.rs -o $@

test_ap_sta_info_rust: test_ap_sta_info.c librust_ap_sta_info.a $(HOST_JSON)
	$(CC) $(STA_INFO_CFLAGS) -o $@ test_ap_sta_info.c $(HOST_JSON) librust_ap_sta_info.a

test-rust-sta-ra: test_ap_sta_ra_rust
	./test_ap_sta_ra_rust ap_sta_ra_vectors.json

test_ap_sta_ra: test_ap_sta_ra.c $(HOST_JSON) $(AP_STA_RA_SRC)
	$(CC) $(STA_RA_CFLAGS) -o $@ test_ap_sta_ra.c $(HOST_JSON) $(AP_STA_RA_SRC)

librust_ap_sta_ra.a: ../../../rust/rtw_ap_sta_ra.rs
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_ap_sta_ra_test \
		--crate-type staticlib ../../../rust/rtw_ap_sta_ra.rs -o $@

test_ap_sta_ra_rust: test_ap_sta_ra.c librust_ap_sta_ra.a $(HOST_JSON)
	$(CC) $(STA_RA_CFLAGS) -DRUST_AP_STA_RA_ORACLE -o $@ test_ap_sta_ra.c \
		$(HOST_JSON) librust_ap_sta_ra.a

test-rust-aka-chk: test_ap_aka_chk_rust
	./test_ap_aka_chk_rust ap_aka_chk_vectors.json

librust_ap_aka_chk.a: ../../../rust/rtw_ap_aka_chk.rs
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_ap_aka_chk_test \
		--crate-type staticlib ../../../rust/rtw_ap_aka_chk.rs -o $@

test_ap_aka_chk_rust: test_ap_aka_chk.c librust_ap_aka_chk.a $(HOST_JSON) host_aka_chk_shim.c
	$(CC) $(AKA_CHK_CFLAGS) -DRUST_AP_AKA_CHK_ORACLE -o $@ test_ap_aka_chk.c \
		$(HOST_JSON) host_aka_chk_shim.c librust_ap_aka_chk.a

test_ap_aka_chk: test_ap_aka_chk.c $(HOST_JSON) $(AP_AKA_CHK_SRC) $(AKA_CHK_DEPS)
	$(CC) $(AKA_CHK_CFLAGS) -o $@ test_ap_aka_chk.c $(HOST_JSON) \
		$(AP_AKA_CHK_SRC) $(AKA_CHK_DEPS)

test-rust-expire-asoc: test_ap_expire_asoc_rust
	./test_ap_expire_asoc_rust ap_expire_asoc_vectors.json

librust_ap_expire_asoc.a: ../../../rust/rtw_ap_expire_asoc.rs
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_ap_expire_asoc_test \
		--crate-type staticlib ../../../rust/rtw_ap_expire_asoc.rs -o $@

test_ap_expire_asoc_rust: test_ap_expire_asoc.c librust_ap_expire_asoc.a $(HOST_JSON) $(CORE)/rtw_ap_sta_alive.c
	$(CC) $(EXPIRE_ASOC_CFLAGS) -DRUST_AP_EXPIRE_ASOC_ORACLE -o $@ test_ap_expire_asoc.c \
		$(HOST_JSON) $(CORE)/rtw_ap_sta_alive.c librust_ap_expire_asoc.a

test_ap_expire_asoc: test_ap_expire_asoc.c $(HOST_JSON) $(AP_EXPIRE_ASOC_SRC)
	$(CC) $(EXPIRE_ASOC_CFLAGS) -o $@ test_ap_expire_asoc.c $(HOST_JSON) \
		$(AP_EXPIRE_ASOC_SRC)

test_ap_expire_auth: test_ap_expire_auth.c $(HOST_JSON) $(AP_EXPIRE_AUTH_SRC) $(EXPIRE_AUTH_DEPS)
	$(CC) $(EXPIRE_AUTH_CFLAGS) -o $@ test_ap_expire_auth.c $(HOST_JSON) \
		$(AP_EXPIRE_AUTH_SRC) $(EXPIRE_AUTH_DEPS)

test-rust-sta-alive: test_ap_sta_alive_rust
	./test_ap_sta_alive_rust ap_sta_alive_vectors.json

librust_ap_sta_alive.a: ../../../rust/rtw_ap_sta_alive.rs
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_ap_sta_alive_test \
		--crate-type staticlib ../../../rust/rtw_ap_sta_alive.rs -o $@

test_ap_sta_alive_rust: test_ap_sta_alive.c librust_ap_sta_alive.a $(HOST_JSON)
	$(CC) $(STA_ALIVE_CFLAGS) -DRUST_AP_STA_ALIVE_ORACLE -o $@ test_ap_sta_alive.c \
		$(HOST_JSON) librust_ap_sta_alive.a

test_ap_sta_alive: test_ap_sta_alive.c $(HOST_JSON) $(AP_STA_ALIVE_SRC)
	$(CC) $(STA_ALIVE_CFLAGS) -o $@ test_ap_sta_alive.c $(HOST_JSON) \
		$(AP_STA_ALIVE_SRC)

test_ap_bcn_update: test_ap_bcn_update.c $(HOST_JSON) $(AP_BCN_UPDATE_SRC) $(BCN_UPDATE_DEPS)
	$(CC) $(BCN_UPDATE_CFLAGS) -o $@ test_ap_bcn_update.c $(HOST_JSON) \
		$(AP_BCN_UPDATE_SRC) $(BCN_UPDATE_DEPS)

test-bmc-rate: test-c-bmc test-rust-bmc

test-c-bmc: test_ap_bmc_rate
	./test_ap_bmc_rate ap_bmc_rate_vectors.json

test-rust-bmc: test_ap_bmc_rate_rust
	./test_ap_bmc_rate_rust ap_bmc_rate_vectors.json

test_ap_bmc_rate: test_ap_bmc_rate.c ap_bmc_rate_oracle.c $(HOST_JSON)
	$(CC) $(BMC_RATE_CFLAGS) -o $@ test_ap_bmc_rate.c ap_bmc_rate_oracle.c $(HOST_JSON)

librust_ap_bmc_rate.a: $(RUST_AP_REST)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_ap_bmc_rate_test \
		--crate-type staticlib $(RUST_AP_REST) -o $@

test_ap_bmc_rate_rust: test_ap_bmc_rate.c librust_ap_bmc_rate.a $(HOST_JSON)
	$(CC) $(BMC_RATE_CFLAGS) -DRUST_AP_BMC_RATE_ORACLE -o $@ test_ap_bmc_rate.c $(HOST_JSON) librust_ap_bmc_rate.a

test-bmc-update: test-c-bmc-update test-rust-bmc-update

test-c-bmc-update: test_ap_bmc_update
	./test_ap_bmc_update ap_bmc_update_vectors.json

test-rust-bmc-update: test_ap_bmc_update_rust
	./test_ap_bmc_update_rust ap_bmc_update_vectors.json

BMC_STA_UPDATE_SRC := ap_bmc_sta_helpers.c host_ap_bmc_sta_shim.c

test_ap_bmc_update: test_ap_bmc_update.c ap_bmc_update_oracle.c $(BMC_STA_UPDATE_SRC) $(HOST_JSON)
	$(CC) $(BMC_UPDATE_CFLAGS) -o $@ test_ap_bmc_update.c ap_bmc_update_oracle.c \
		$(BMC_STA_UPDATE_SRC) $(HOST_JSON)

librust_ap_bmc_update.a: $(RUST_AP_BMC_UPDATE)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_ap_bmc_update_test --cfg bmc_tx_low_rate \
		--crate-type staticlib $(RUST_AP_BMC_UPDATE) -o $@

test_ap_bmc_update_rust: test_ap_bmc_update.c librust_ap_bmc_update.a librust_ap_bmc_rate.a \
		$(BMC_STA_UPDATE_SRC) $(HOST_JSON)
	$(CC) $(BMC_UPDATE_CFLAGS) -DRUST_AP_BMC_UPDATE_ORACLE -o $@ test_ap_bmc_update.c \
		$(BMC_STA_UPDATE_SRC) $(HOST_JSON) \
		librust_ap_bmc_update.a librust_ap_bmc_rate.a

test_ap_sta_ie: test_ap_sta_ie.c $(AP_STA_IE_SRC) $(HOST_JSON)
	$(CC) $(STA_IE_CFLAGS) -o $@ test_ap_sta_ie.c $(AP_STA_IE_SRC) $(HOST_JSON)

test_ap_sta_ie_rates: test_ap_sta_ie_rates.c $(AP_STA_IE_RATES_SRC) $(STA_IE_RATES_DEPS)
	$(CC) $(STA_IE_RATES_CFLAGS) -o $@ test_ap_sta_ie_rates.c $(AP_STA_IE_RATES_SRC) \
		$(STA_IE_RATES_DEPS)

test_ap_sta_ie_wmm_ht: test_ap_sta_ie_wmm_ht.c $(AP_STA_IE_WMM_HT_SRC) $(STA_IE_WMM_HT_DEPS)
	$(CC) $(STA_IE_WMM_HT_CFLAGS) -o $@ test_ap_sta_ie_wmm_ht.c $(AP_STA_IE_WMM_HT_SRC) \
		$(STA_IE_WMM_HT_DEPS)

test_ap_sta_ie_vht_multiap: test_ap_sta_ie_vht_multiap.c $(AP_STA_IE_VHT_MULTIAP_SRC) $(STA_IE_VHT_MULTIAP_DEPS)
	$(CC) $(STA_IE_VHT_MULTIAP_CFLAGS) -o $@ test_ap_sta_ie_vht_multiap.c \
		$(AP_STA_IE_VHT_MULTIAP_SRC) $(STA_IE_VHT_MULTIAP_DEPS)

test_ap_sta_ie_sec: test_ap_sta_ie_sec.c $(AP_STA_IE_SEC_SRC) $(HOST_JSON) $(STA_IE_SEC_DEPS)
	$(CC) $(STA_IE_SEC_CFLAGS) -o $@ test_ap_sta_ie_sec.c $(AP_STA_IE_SEC_SRC) \
		$(HOST_JSON) $(STA_IE_SEC_DEPS)

test_ap_bcn_ie: test_ap_bcn_ie.c $(AP_BCN_IE_SRC) $(HOST_JSON) $(BCN_IE_DEPS)
	$(CC) $(BCN_IE_CFLAGS) -o $@ test_ap_bcn_ie.c $(AP_BCN_IE_SRC) \
		$(HOST_JSON) $(BCN_IE_DEPS)

clean:
	rm -f test_ap_rest test_ap_rest_rust test_ap_sta_ie test_ap_sta_ie_rates \
		test_ap_sta_ie_wmm_ht test_ap_sta_ie_vht_multiap test_ap_sta_ie_sec \
		test_ap_bcn_ie test_ap_bcn_ie_rust test_ap_bcn_update test_ap_sta_alive test_ap_sta_alive_rust \
		librust_ap_bcn_ie.a \
		test_ap_expire_auth test_ap_expire_auth_rust test_ap_expire_asoc test_ap_expire_asoc_rust \
		test_ap_aka_chk test_ap_aka_chk_rust test_ap_rf18_restore test_ap_rf18_restore_rust \
		test_ap_sta_ra test_ap_sta_ra_rust test_ap_sta_info test_ap_sta_info_rust \
		test_ap_sta_info_apmode test_ap_sta_info_apmode_rust \
		librust_ap_sta_info.a librust_ap_sta_info_apmode.a test_ap_bmc_rate test_ap_bmc_rate_rust test_ap_bmc_update \
		test_ap_bmc_update_rust librust_ap_bmc_update.a librust_ap_sta_ra.a \
		librust_ap_rest.a librust_ap_bmc_rate.a librust_ap_sta_alive.a \
		librust_ap_expire_asoc.a librust_ap_expire_auth.a librust_ap_aka_chk.a \
		librust_ap_rf18_restore.a
