ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
RUST_P2P := $(ROOT)/rust/rtw_p2p.rs
RUSTC := rustc
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC)

.PHONY: all test test-c test-rust clean
all test: test-c test-rust

test-c: test_p2p_channel
	./test_p2p_channel p2p_channel_pure_vectors.json

test-rust: test_p2p_channel_rust
	./test_p2p_channel_rust p2p_channel_pure_vectors.json

test_p2p_channel: p2p_channel_l2.c $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ p2p_channel_l2.c $(HOST_JSON)

librust_p2p.a: $(RUST_P2P)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on \
		--crate-type staticlib $(RUST_P2P) -o $@

test_p2p_channel_rust: p2p_channel_l2.c librust_p2p.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_P2P_RUST -o $@ p2p_channel_l2.c $(HOST_JSON) librust_p2p.a

clean:
	rm -f test_p2p_channel test_p2p_channel_rust librust_p2p.a
