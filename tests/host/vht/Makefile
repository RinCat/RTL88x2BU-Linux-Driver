# Host L2 differential tests for core/rtw_vht_rest.c (W3-35, W3-36).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
VHT_REST_SRC := $(CORE)/rtw_vht_rest.c
VHT_REST_SHIM := host_vht_rest_shim.c
HOST_JSON := ../crypto/host_vector_json.c
VHT_MCS_RATE_ORACLE := vht_mcs_rate_c_oracle.c
VHT_VHT20_ORACLE := vht_vht20_c_oracle.c
VHT_MCS_SHIM := host_vht_mcs_shim.c
IE_DIR := ../ie
WLAN_UTIL := ../wlan_util
IE_REST_SRC := $(CORE)/rtw_ieee80211_rest.c
RF_REST_SRC := $(CORE)/rtw_rf_rest.c
VHT20_DEPS := $(IE_DIR)/ie_c_oracle.c $(IE_DIR)/ie_cipher_c_oracle.c \
	$(IE_DIR)/ie_wpa_rsn_parse_c_oracle.c $(WLAN_UTIL)/wlan_util_c_oracle.c \
	$(IE_DIR)/ie_mac_str_host_shim.c $(IE_REST_SRC) $(RF_REST_SRC)
RUST_VHT := $(ROOT)/rust/rtw_vht.rs

CC     := gcc
BASE_CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DROKU_PRIVATE
CFLAGS := $(BASE_CFLAGS) -DHOST_VHT_TEST
MCS_RATE_CFLAGS := $(BASE_CFLAGS) -DHOST_VHT_MCS_RATE_TEST
VHT20_CFLAGS := $(BASE_CFLAGS) -I$(WLAN_UTIL) -Wno-format-truncation \
	-DHOST_IEEE80211_REST_TEST -DHOST_IEEE80211_REST_CHBW_TEST \
	-DHOST_VHT_MCS_RATE_TEST -DHOST_RF_TEST
RESTRUCT_CFLAGS := $(BASE_CFLAGS) -DHOST_VHT_RESTRUCTURE_TEST -Wno-pointer-sign
VHT_BUILD_SRC := $(CORE)/rtw_vht_build.c
VHT_BUILD_SHIM := host_vht_build_shim.c
BUILD_CFLAGS := $(BASE_CFLAGS) -DHOST_VHT_BUILD_TEST

RUST_VHT_BUILD := $(ROOT)/rust/rtw_vht_build.rs

.PHONY: all test test-c test-rust test-mcs-rate test-mcs-rate-rust test-vht20 \
	test-vht20-rust vht-restructure-compile test-restructure test-restructure-rust \
	test-build-operation test-build-operation-rust test-build-cap clean

all: test

test: test-c test-rust test-mcs-rate test-mcs-rate-rust test-vht20 test-vht20-rust \
	test-restructure test-restructure-rust test-build-operation test-build-operation-rust \
	test-build-cap

test-c: test_vht
	./test_vht vht_vectors.json

test_vht: test_vht.c $(VHT_REST_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_vht.c $(VHT_REST_SRC) $(HOST_JSON)

librust_vht.a: $(RUST_VHT)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_vht_test \
		--crate-type staticlib $(RUST_VHT) -o $@

test-rust: test_vht_rust
	./test_vht_rust vht_vectors.json

test_vht_rust: test_vht.c librust_vht.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_VHT_ORACLE -o $@ test_vht.c $(HOST_JSON) librust_vht.a

test-mcs-rate: test_vht_mcs_rate
	./test_vht_mcs_rate vht_mcs_rate_vectors.json

librust_vht_mcs_rate.a: $(RUST_VHT)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_vht_mcs_rate_test \
		--crate-type staticlib $(RUST_VHT) -o $@

test-mcs-rate-rust: test_vht_mcs_rate_rust
	./test_vht_mcs_rate_rust vht_mcs_rate_vectors.json

test_vht_mcs_rate_rust: test_vht_mcs_rate.c librust_vht_mcs_rate.a $(HOST_JSON)
	$(CC) $(MCS_RATE_CFLAGS) -DRUST_VHT_MCS_RATE_ORACLE -o $@ test_vht_mcs_rate.c $(HOST_JSON) librust_vht_mcs_rate.a

test_vht_mcs_rate: test_vht_mcs_rate.c $(VHT_MCS_RATE_ORACLE) $(HOST_JSON)
	$(CC) $(MCS_RATE_CFLAGS) -o $@ test_vht_mcs_rate.c $(VHT_MCS_RATE_ORACLE) $(HOST_JSON)

test-vht20: test_vht_vht20
	./test_vht_vht20 vht_vht20_vectors.json

test_vht_vht20: test_vht_vht20.c $(VHT_VHT20_ORACLE) $(VHT_MCS_SHIM) $(VHT20_DEPS) $(HOST_JSON)
	$(CC) $(VHT20_CFLAGS) -o $@ test_vht_vht20.c $(VHT_VHT20_ORACLE) $(VHT_MCS_SHIM) \
		$(VHT20_DEPS) $(HOST_JSON)

librust_vht_vht20.a: $(RUST_VHT)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_vht_vht20_test \
		--crate-type staticlib $(RUST_VHT) -o $@

test-vht20-rust: test_vht_vht20_rust
	./test_vht_vht20_rust vht_vht20_vectors.json

test_vht_vht20_rust: test_vht_vht20.c librust_vht_vht20.a $(VHT_MCS_SHIM) $(VHT20_DEPS) $(HOST_JSON)
	$(CC) $(VHT20_CFLAGS) -DRUST_VHT_VHT20_ORACLE -o $@ test_vht_vht20.c $(VHT_MCS_SHIM) \
		$(VHT20_DEPS) $(HOST_JSON) librust_vht_vht20.a

vht-restructure-compile: $(VHT_REST_SRC) $(VHT_REST_SHIM)
	$(CC) $(RESTRUCT_CFLAGS) -c $(VHT_REST_SRC) -o vht_rest_host.o
	$(CC) $(RESTRUCT_CFLAGS) -c $(VHT_REST_SHIM) -o vht_rest_shim_host.o

librust_vht_restructure.a: $(RUST_VHT)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_vht_restructure_test \
		--crate-type staticlib $(RUST_VHT) -o $@

test-restructure-rust: test_vht_restructure_rust
	./test_vht_restructure_rust vht_restructure_vectors.json

test_vht_restructure_rust: test_vht_restructure.c librust_vht_restructure.a $(VHT_REST_SHIM) $(HOST_JSON)
	$(CC) $(RESTRUCT_CFLAGS) -DRUST_VHT_ORACLE -o $@ test_vht_restructure.c $(VHT_REST_SHIM) $(HOST_JSON) librust_vht_restructure.a

test-restructure: test_vht_restructure
	./test_vht_restructure vht_restructure_vectors.json

test_vht_restructure: test_vht_restructure.c $(VHT_REST_SRC) $(VHT_REST_SHIM) $(HOST_JSON)
	$(CC) $(RESTRUCT_CFLAGS) -o $@ test_vht_restructure.c $(VHT_REST_SRC) $(VHT_REST_SHIM) $(HOST_JSON)

test-build-operation: test_vht_build_operation
	./test_vht_build_operation vht_build_operation_vectors.json

test_vht_build_operation: test_vht_build_operation.c $(VHT_BUILD_SRC) $(VHT_BUILD_SHIM) $(VHT_MCS_RATE_ORACLE) $(HOST_JSON)
	$(CC) $(BUILD_CFLAGS) -o $@ test_vht_build_operation.c $(VHT_BUILD_SRC) $(VHT_BUILD_SHIM) \
		$(VHT_MCS_RATE_ORACLE) $(HOST_JSON)

librust_vht_build.a: $(RUST_VHT_BUILD)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_vht_build_test \
		--crate-type staticlib $(RUST_VHT_BUILD) -o $@

test-build-operation-rust: test_vht_build_operation_rust
	./test_vht_build_operation_rust vht_build_operation_vectors.json

test_vht_build_operation_rust: test_vht_build_operation.c librust_vht_build.a $(VHT_BUILD_SHIM) $(HOST_JSON)
	$(CC) $(BUILD_CFLAGS) -o $@ test_vht_build_operation.c $(VHT_BUILD_SHIM) $(HOST_JSON) librust_vht_build.a

test-build-cap: test_vht_build_cap
	./test_vht_build_cap vht_build_cap_vectors.json

test_vht_build_cap: test_vht_build_cap.c $(VHT_BUILD_SRC) $(VHT_BUILD_SHIM) $(VHT_MCS_RATE_ORACLE) $(HOST_JSON)
	$(CC) $(BUILD_CFLAGS) -o $@ test_vht_build_cap.c $(VHT_BUILD_SRC) $(VHT_BUILD_SHIM) \
		$(VHT_MCS_RATE_ORACLE) $(HOST_JSON)

clean:
	rm -f test_vht test_vht_rust test_vht_mcs_rate test_vht_mcs_rate_rust test_vht_vht20 \
		test_vht_vht20_rust test_vht_restructure test_vht_restructure_rust librust_vht.a \
		librust_vht_mcs_rate.a librust_vht_vht20.a librust_vht_restructure.a \
		vht_rest_host.o vht_rest_shim_host.o vht_mcs_rate_c_ref.o vht_mcs_rate_rust_ref.o \
		test_vht_build_operation test_vht_build_operation_rust test_vht_build_cap \
		librust_vht_build.a
