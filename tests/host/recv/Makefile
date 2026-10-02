# Host L2 differential tests for core/rtw_recv_rest.c (W3-39),
# core/rtw_recv_llc_rest.c (W3-46), and core/rtw_recv_pn_rest.c (W3-47).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
RECV_REST_SRC := $(CORE)/rtw_recv_rest.c
RECV_LLC_REST_SRC := $(CORE)/rtw_recv_llc_rest.c
RECV_PN_REST_SRC := $(CORE)/rtw_recv_pn_rest.c
RECV_STA_REST_SRC := $(CORE)/rtw_recv_sta_rest.c
RECV_STA_SHIM := host_recv_sta_shim.c
LLC_SHIM := host_recv_llc_shim.c
WFD_ORACLE := recv_wfd_c_oracle.c
HOST_JSON := ../crypto/host_vector_json.c
RUST_RECV := $(ROOT)/rust/rtw_recv.rs
RUST_RECV_STA_COUNT := $(ROOT)/rust/rtw_recv_sta_count.rs
RUST_RECV_STA_VALIDATE := $(ROOT)/rust/rtw_recv_sta_validate.rs

CC     := gcc
RUSTC  := rustc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
          -I$(HOST_INC) -I$(CORE) \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_RECV_TEST -DHOST_RECV_WFD_TEST -DHOST_RECV_LLC_TEST -DHOST_RECV_PN_TEST

STA_CFLAGS := $(CFLAGS) -DHOST_RECV_STA_TEST
RECV_STA_DEPS := test_recv_sta.c $(RECV_STA_REST_SRC) $(RECV_STA_SHIM) $(HOST_JSON)
STA_COUNT_RUST_CFLAGS := $(STA_CFLAGS) -DHOST_RECV_STA_RUST_COUNT
STA_RUST_CFLAGS := $(STA_CFLAGS) -DHOST_RECV_STA_RUST_COUNT -DHOST_RECV_STA_RUST_VALIDATE

.PHONY: all test test-c test-rust test-sta test-sta-count-rust test-sta-rust clean

all: test

test: test-c test-rust test-sta test-sta-count-rust test-sta-rust

test-c: test_recv
	./test_recv recv_vectors.json

test_recv: test_recv.c $(RECV_REST_SRC) $(RECV_LLC_REST_SRC) $(RECV_PN_REST_SRC) $(LLC_SHIM) $(WFD_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_recv.c $(RECV_REST_SRC) $(RECV_LLC_REST_SRC) $(RECV_PN_REST_SRC) $(LLC_SHIM) $(WFD_ORACLE) $(HOST_JSON)

librust_recv.a: $(RUST_RECV)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_recv_test \
		--crate-type staticlib $(RUST_RECV) -o $@

test-rust: test_recv_rust
	./test_recv_rust recv_vectors.json

test_recv_rust: test_recv.c librust_recv.a $(LLC_SHIM) $(WFD_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_recv.c $(LLC_SHIM) $(WFD_ORACLE) $(HOST_JSON) librust_recv.a

test-sta: test_recv_sta
	./test_recv_sta recv_sta_vectors.json

test_recv_sta: $(RECV_STA_DEPS)
	$(CC) $(STA_CFLAGS) -o $@ $(RECV_STA_DEPS)

librust_recv_sta_count.a: $(RUST_RECV_STA_COUNT)
	$(RUSTC) --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_recv_sta_test \
		--crate-type staticlib $(RUST_RECV_STA_COUNT) -o $@

test-sta-count-rust: test_recv_sta_count_rust
	./test_recv_sta_count_rust recv_sta_count_vectors.json

test_recv_sta_count_rust: test_recv_sta.c librust_recv_sta_count.a $(RECV_STA_SHIM) $(HOST_JSON)
	$(CC) $(STA_COUNT_RUST_CFLAGS) -o $@ test_recv_sta.c $(RECV_STA_SHIM) $(HOST_JSON) librust_recv_sta_count.a \
		$(RECV_STA_REST_SRC)

librust_recv_sta_validate.a: $(RUST_RECV_STA_VALIDATE)
	$(RUSTC) --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_recv_sta_test \
		--crate-type staticlib $(RUST_RECV_STA_VALIDATE) -o $@

test-sta-rust: test_recv_sta_rust
	./test_recv_sta_rust recv_sta_vectors.json

test_recv_sta_rust: test_recv_sta.c librust_recv_sta_count.a librust_recv_sta_validate.a $(RECV_STA_SHIM) $(HOST_JSON)
	$(CC) $(STA_RUST_CFLAGS) -o $@ test_recv_sta.c $(RECV_STA_SHIM) $(HOST_JSON) \
		librust_recv_sta_count.a librust_recv_sta_validate.a

clean:
	rm -f test_recv test_recv_rust test_recv_sta test_recv_sta_count_rust test_recv_sta_rust \
		librust_recv.a librust_recv_sta_count.a librust_recv_sta_validate.a
