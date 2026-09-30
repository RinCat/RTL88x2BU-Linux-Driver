ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
RUST_ROCH := $(ROOT)/rust/rtw_roch.rs
RUSTC := rustc
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -DCONFIG_IOCTL_CFG80211

.PHONY: all test test-c test-rust clean
all test: test-c test-rust

test-c: test_roch_handlers
	./test_roch_handlers roch_handlers_vectors.json

test-rust: test_roch_handlers_rust
	./test_roch_handlers_rust roch_handlers_vectors.json

test_roch_handlers: roch_handlers_l2.c $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ roch_handlers_l2.c $(HOST_JSON)

librust_roch.a: $(RUST_ROCH)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_roch_test \
		--crate-type staticlib $(RUST_ROCH) -o $@

test_roch_handlers_rust: roch_handlers_l2.c librust_roch.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_ROCH_RUST -o $@ roch_handlers_l2.c $(HOST_JSON) librust_roch.a

clean:
	rm -f test_roch_handlers test_roch_handlers_rust librust_roch.a
