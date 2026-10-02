ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
SHIM := host_tdls_lifecycle_shim.c host_tdls_adapter_shim.c
RUST_TDLS := $(ROOT)/rust/rtw_tdls.rs
RUSTC := rustc
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) \
          -DCONFIG_TDLS -DCONFIG_TDLS_CH_SW -DCONFIG_TDLS_DRIVER_SETUP

.PHONY: all test test-c test-rust clean
all: test
test: test-c test-rust

test-c: test_tdls_lifecycle
	./test_tdls_lifecycle tdls_lifecycle_vectors.json

test-rust: test_tdls_lifecycle_rust
	./test_tdls_lifecycle_rust tdls_lifecycle_vectors.json

test_tdls_lifecycle: test_tdls_lifecycle.c $(SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_tdls_lifecycle.c $(SHIM) $(HOST_JSON)

librust_tdls.a: $(RUST_TDLS)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_tdls_test \
		--crate-type staticlib $(RUST_TDLS) -o $@

test_tdls_lifecycle_rust: test_tdls_lifecycle.c librust_tdls.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_tdls_lifecycle.c $(HOST_JSON) librust_tdls.a

clean:
	rm -f test_tdls_lifecycle test_tdls_lifecycle_rust librust_tdls.a
