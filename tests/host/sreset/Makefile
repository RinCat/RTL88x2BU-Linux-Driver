ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
SHIM := host_sreset_lifecycle_shim.c
RUST_SRESET := $(ROOT)/rust/rtw_sreset.rs
RUSTC := rustc
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h

.PHONY: all test test-c test-rust clean
all: test
test: test-c test-rust

test-c: test_sreset_lifecycle
	./test_sreset_lifecycle sreset_lifecycle_vectors.json

test-rust: test_sreset_lifecycle_rust
	./test_sreset_lifecycle_rust sreset_lifecycle_vectors.json

test_sreset_lifecycle: test_sreset_lifecycle.c $(SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_sreset_lifecycle.c $(SHIM) $(HOST_JSON)

librust_sreset.a: $(RUST_SRESET)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_sreset_test \
		--crate-type staticlib $(RUST_SRESET) -o $@

test_sreset_lifecycle_rust: test_sreset_lifecycle.c librust_sreset.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_sreset_lifecycle.c $(HOST_JSON) librust_sreset.a

clean:
	rm -f test_sreset_lifecycle test_sreset_lifecycle_rust librust_sreset.a
