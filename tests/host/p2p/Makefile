ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
RUST_P2P := $(ROOT)/rust/rtw_p2p.rs
RUSTC := rustc
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC)

.PHONY: all test test-c test-rust test-ie-build test-ie-build-rust test-wfd-beacon \
	test-wfd-probe test-wfd-probe-resp test-wfd-assoc clean
all test: test-c test-rust test-ie-build test-ie-build-rust test-wfd-beacon test-wfd-probe \
	test-wfd-probe-resp test-wfd-assoc test-wfd-rust

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
	$(CC) $(CFLAGS) -DHOST_P2P_RUST -DHOST_P2P_RUST_IE -o $@ p2p_channel_l2.c $(HOST_JSON) librust_p2p.a

test-ie-build: test_p2p_ie_builders
	./test_p2p_ie_builders p2p_ie_builders_vectors.json

test_p2p_ie_builders: p2p_ie_builders_l2.c host_p2p_ie_build_oracle.c host_p2p_ie_build_oracle_probe.c $(HOST_JSON)
	$(CC) $(CFLAGS) -I. -o $@ p2p_ie_builders_l2.c host_p2p_ie_build_oracle.c \
		host_p2p_ie_build_oracle_probe.c $(HOST_JSON)

librust_p2p_ie.a: $(RUST_P2P)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_p2p_ie_build \
		--cfg host_p2p_ie_build_probe --crate-type staticlib $(RUST_P2P) -o $@

test-ie-build-rust: test_p2p_ie_builders_rust
	./test_p2p_ie_builders_rust p2p_ie_builders_vectors.json

test_p2p_ie_builders_rust: p2p_ie_builders_l2.c librust_p2p_ie.a $(HOST_JSON)
	$(CC) $(CFLAGS) -I. -DHOST_P2P_RUST_IE_BUILD -o $@ p2p_ie_builders_l2.c \
		$(HOST_JSON) librust_p2p_ie.a

test-wfd-beacon: p2p_wfd_builders_l2.c host_p2p_wfd_oracle.c $(HOST_JSON)
	$(CC) $(CFLAGS) -I. -o test_p2p_wfd_beacon p2p_wfd_builders_l2.c host_p2p_wfd_oracle.c $(HOST_JSON)
	./test_p2p_wfd_beacon p2p_wfd_beacon_vectors.json

test-wfd-probe: p2p_wfd_builders_l2.c host_p2p_wfd_oracle.c $(HOST_JSON)
	$(CC) $(CFLAGS) -I. -DHOST_P2P_WFD_PROBE -o test_p2p_wfd_probe p2p_wfd_builders_l2.c host_p2p_wfd_oracle.c $(HOST_JSON)
	./test_p2p_wfd_probe p2p_wfd_probe_vectors.json

test-wfd-probe-resp: p2p_wfd_builders_l2.c host_p2p_wfd_oracle.c $(HOST_JSON)
	$(CC) $(CFLAGS) -I. -DHOST_P2P_WFD_PROBE_ASSOC -o test_p2p_wfd_probe_resp p2p_wfd_builders_l2.c host_p2p_wfd_oracle.c $(HOST_JSON)
	./test_p2p_wfd_probe_resp p2p_wfd_probe_resp_vectors.json

test-wfd-assoc: p2p_wfd_builders_l2.c host_p2p_wfd_oracle.c $(HOST_JSON)
	$(CC) $(CFLAGS) -I. -DHOST_P2P_WFD_PROBE_ASSOC -o test_p2p_wfd_assoc p2p_wfd_builders_l2.c host_p2p_wfd_oracle.c $(HOST_JSON)
	./test_p2p_wfd_assoc p2p_wfd_assoc_vectors.json

librust_p2p_wfd.a: $(RUST_P2P)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_p2p_wfd_build \
		--cfg host_p2p_wfd_probe --crate-type staticlib $(RUST_P2P) -o $@

test-wfd-rust: librust_p2p_wfd.a
	$(CC) $(CFLAGS) -I. -DHOST_P2P_RUST_WFD_BUILD -DHOST_P2P_WFD_PROBE -o test_p2p_wfd_beacon_rust p2p_wfd_builders_l2.c $(HOST_JSON) librust_p2p_wfd.a
	$(CC) $(CFLAGS) -I. -DHOST_P2P_RUST_WFD_BUILD -DHOST_P2P_WFD_PROBE -o test_p2p_wfd_probe_rust p2p_wfd_builders_l2.c $(HOST_JSON) librust_p2p_wfd.a
	./test_p2p_wfd_beacon_rust p2p_wfd_beacon_vectors.json
	./test_p2p_wfd_probe_rust p2p_wfd_probe_vectors.json

clean:
	rm -f test_p2p_channel test_p2p_channel_rust test_p2p_ie_builders \
		test_p2p_ie_builders_rust test_p2p_wfd_beacon test_p2p_wfd_probe \
		test_p2p_wfd_probe_resp test_p2p_wfd_assoc test_p2p_wfd_beacon_rust \
		test_p2p_wfd_probe_rust librust_p2p_wfd.a \
		librust_p2p.a librust_p2p_ie.a
