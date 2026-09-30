# Host L2 differential tests for W3-94 ps deny gate helpers.

ROOT     := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
SHIM     := host_pwrctrl_ps_deny_shim.c
LPS_SHIM := host_pwrctrl_lps_shim.c
LEAVE_ALL_SHIM := host_pwrctrl_leave_all_ps_shim.c

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -O2 \
          -I$(HOST_INC) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h
LPS_CFLAGS := $(CFLAGS)

RUST_PWRCTRL := $(ROOT)/rust/rtw_pwrctrl.rs
RUSTC := rustc

.PHONY: all test test-c test-rust test-lps test-lps-rust test-leave-all clean

all: test test-lps test-leave-all

test: test-c test-rust test-lps test-lps-rust test-leave-all

test-c: test_pwrctrl_ps_deny
	./test_pwrctrl_ps_deny ps_deny_vectors.json

test-rust: test_pwrctrl_ps_deny_rust
	./test_pwrctrl_ps_deny_rust ps_deny_vectors.json

test-lps: test_pwrctrl_lps
	./test_pwrctrl_lps lps_vectors.json

test-lps-rust: test_pwrctrl_lps_rust
	./test_pwrctrl_lps_rust lps_vectors.json

test-leave-all: test_pwrctrl_leave_all_ps
	./test_pwrctrl_leave_all_ps leave_all_ps_vectors.json

test_pwrctrl_leave_all_ps: test_pwrctrl_leave_all_ps.c $(LEAVE_ALL_SHIM) $(HOST_JSON)
	$(CC) $(LPS_CFLAGS) -o $@ test_pwrctrl_leave_all_ps.c $(LEAVE_ALL_SHIM) $(HOST_JSON)

test_pwrctrl_lps: test_pwrctrl_lps.c $(LPS_SHIM) $(HOST_JSON)
	$(CC) $(LPS_CFLAGS) -o $@ test_pwrctrl_lps.c $(LPS_SHIM) $(HOST_JSON)

test_pwrctrl_ps_deny: test_pwrctrl_ps_deny.c $(SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_pwrctrl_ps_deny.c $(SHIM) $(HOST_JSON)

librust_pwrctrl.a: $(RUST_PWRCTRL)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_pwrctrl_test \
		--crate-type staticlib $(RUST_PWRCTRL) -o $@

test_pwrctrl_ps_deny_rust: test_pwrctrl_ps_deny.c librust_pwrctrl.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_pwrctrl_ps_deny.c $(HOST_JSON) librust_pwrctrl.a

test_pwrctrl_lps_rust: test_pwrctrl_lps.c librust_pwrctrl.a $(HOST_JSON)
	$(CC) $(LPS_CFLAGS) -o $@ test_pwrctrl_lps.c $(HOST_JSON) librust_pwrctrl.a

clean:
	rm -f test_pwrctrl_ps_deny test_pwrctrl_ps_deny_rust test_pwrctrl_lps \
		test_pwrctrl_lps_rust test_pwrctrl_leave_all_ps librust_pwrctrl.a
