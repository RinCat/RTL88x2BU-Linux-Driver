# Host L2 differential tests for W3-94 ps deny gate helpers.

ROOT     := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
SHIM     := host_pwrctrl_ps_deny_shim.c

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -O2 \
          -I$(HOST_INC) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h

RUST_PWRCTRL := $(ROOT)/rust/rtw_pwrctrl.rs
RUSTC := rustc

.PHONY: all test test-c test-rust clean

all: test

test: test-c test-rust

test-c: test_pwrctrl_ps_deny
	./test_pwrctrl_ps_deny ps_deny_vectors.json

test-rust: test_pwrctrl_ps_deny_rust
	./test_pwrctrl_ps_deny_rust ps_deny_vectors.json

test_pwrctrl_ps_deny: test_pwrctrl_ps_deny.c $(SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_pwrctrl_ps_deny.c $(SHIM) $(HOST_JSON)

librust_pwrctrl.a: $(RUST_PWRCTRL)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_pwrctrl_test \
		--crate-type staticlib $(RUST_PWRCTRL) -o $@

test_pwrctrl_ps_deny_rust: test_pwrctrl_ps_deny.c librust_pwrctrl.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_pwrctrl_ps_deny.c $(HOST_JSON) librust_pwrctrl.a

clean:
	rm -f test_pwrctrl_ps_deny test_pwrctrl_ps_deny_rust librust_pwrctrl.a
