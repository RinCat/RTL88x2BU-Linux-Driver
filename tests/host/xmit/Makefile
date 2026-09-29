# Host L2 differential tests for core/rtw_xmit_rest.c (W3-40),
# core/rtw_xmit_qos_rest.c (W3-48), core/rtw_xmit_sctx_rest.c (W3-48 PR4),
# W3-86: host harness links core/rtw_xmit_update_attrib_rest.c (kernel via rtw_xmit_update_attrib_rest.o).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
XMIT_REST_SRC := $(CORE)/rtw_xmit_rest.c
XMIT_QOS_REST_SRC := $(CORE)/rtw_xmit_qos_rest.c
XMIT_SCTX_REST_SRC := $(CORE)/rtw_xmit_sctx_rest.c
UPDATE_ATTRIB_REST_SRC := $(CORE)/rtw_xmit_update_attrib_rest.c
HOST_JSON := ../crypto/host_vector_json.c
SHIM     := host_xmit_shim.c
RUST_XMIT := $(ROOT)/rust/rtw_xmit.rs
RUST_XMIT_UPDATE_ATTRIB := $(ROOT)/rust/rtw_xmit_update_attrib.rs

CC     := gcc
RUSTC  := rustc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
          -I$(HOST_INC) -I$(CORE) \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_XMIT_TEST

QOS_CFLAGS := $(CFLAGS) -DHOST_XMIT_QOS_TEST
SCTX_CFLAGS := $(CFLAGS) -DHOST_XMIT_SCTX_TEST
UPDATE_ATTRIB_CFLAGS := $(CFLAGS) -DHOST_XMIT_UPDATE_ATTRIB_TEST

.PHONY: all test test-c test-rust test-qos test-sctx test-update-attrib test-update-attrib-rust clean

all: test test-qos test-sctx test-update-attrib test-update-attrib-rust

test: test-c test-rust

test-qos: test-qos-c test-qos-rust

test-sctx: test-sctx-c test-sctx-rust

test-c: test_xmit
	./test_xmit xmit_vectors.json

test_xmit: test_xmit.c $(XMIT_REST_SRC) $(SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_xmit.c $(XMIT_REST_SRC) $(SHIM) $(HOST_JSON)

librust_xmit.a: $(RUST_XMIT)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on \
		--cfg host_xmit_test --cfg host_xmit_qos_test \
		--crate-type staticlib $(RUST_XMIT) -o $@

librust_xmit_qos.a: $(RUST_XMIT)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on \
		--cfg host_xmit_test --cfg host_xmit_qos_test \
		--crate-type staticlib $(RUST_XMIT) -o $@

test-rust: test_xmit_rust
	./test_xmit_rust xmit_vectors.json

test_xmit_rust: test_xmit.c librust_xmit.a $(SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_XMIT_ORACLE -o $@ test_xmit.c $(SHIM) $(HOST_JSON) librust_xmit.a

test-qos-c: test_xmit_qos
	./test_xmit_qos xmit_qos_vectors.json

test_xmit_qos: test_xmit_qos.c $(XMIT_QOS_REST_SRC) $(HOST_JSON)
	$(CC) $(QOS_CFLAGS) -o $@ test_xmit_qos.c $(XMIT_QOS_REST_SRC) $(HOST_JSON)

test-qos-rust: test_xmit_qos_rust
	./test_xmit_qos_rust xmit_qos_vectors.json

test_xmit_qos_rust: test_xmit_qos.c librust_xmit_qos.a $(HOST_JSON)
	$(CC) $(QOS_CFLAGS) -o $@ test_xmit_qos.c $(HOST_JSON) librust_xmit_qos.a

test-sctx-c: test_xmit_sctx
	./test_xmit_sctx xmit_sctx_vectors.json

test_xmit_sctx: test_xmit_sctx.c $(XMIT_SCTX_REST_SRC) $(HOST_JSON)
	$(CC) $(SCTX_CFLAGS) -o $@ test_xmit_sctx.c $(XMIT_SCTX_REST_SRC) $(HOST_JSON)

librust_xmit_sctx.a: $(RUST_XMIT)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on \
		--cfg host_xmit_test --cfg host_xmit_sctx_test \
		--crate-type staticlib $(RUST_XMIT) -o $@

test-sctx-rust: test_xmit_sctx_rust
	./test_xmit_sctx_rust xmit_sctx_vectors.json

test_xmit_sctx_rust: test_xmit_sctx.c librust_xmit_sctx.a $(HOST_JSON)
	$(CC) $(SCTX_CFLAGS) -o $@ test_xmit_sctx.c $(HOST_JSON) librust_xmit_sctx.a

test-update-attrib: test_xmit_update_attrib
	./test_xmit_update_attrib

test-update-attrib-rust: test_xmit_update_attrib_rust
	./test_xmit_update_attrib_rust

test_xmit_update_attrib: test_xmit_update_attrib.c xmit_sec_oracle.c xmit_sec_globals.c $(UPDATE_ATTRIB_REST_SRC)
	$(CC) $(UPDATE_ATTRIB_CFLAGS) -o $@ test_xmit_update_attrib.c xmit_sec_oracle.c xmit_sec_globals.c $(UPDATE_ATTRIB_REST_SRC)

librust_xmit_update_attrib.a: $(RUST_XMIT_UPDATE_ATTRIB)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on \
		--crate-type staticlib $(RUST_XMIT_UPDATE_ATTRIB) -o $@

test_xmit_update_attrib_rust: test_xmit_update_attrib.c xmit_sec_globals.c librust_xmit_update_attrib.a
	$(CC) $(UPDATE_ATTRIB_CFLAGS) -DRUST_XMIT_UPDATE_ATTRIB_ORACLE -o $@ \
		test_xmit_update_attrib.c xmit_sec_globals.c librust_xmit_update_attrib.a

clean:
		rm -f test_xmit test_xmit_rust test_xmit_qos test_xmit_qos_rust \
		test_xmit_sctx test_xmit_sctx_rust test_xmit_update_attrib \
		test_xmit_update_attrib_rust \
		librust_xmit.a librust_xmit_qos.a librust_xmit_sctx.a \
		librust_xmit_update_attrib.a \
		xmit_rest_c_ref.o xmit_qos_c_ref.o xmit_sctx_c_ref.o
