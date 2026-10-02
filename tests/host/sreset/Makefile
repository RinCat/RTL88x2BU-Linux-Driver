ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
SHIM := host_sreset_lifecycle_shim.c
ADAPTER_ORACLE := sreset_adapter_host_oracle.c
ADAPTER_STUBS := host_sreset_adapter_stubs.c
SECURITY_ORACLE := sreset_restore_security_host_oracle.c
SECURITY_STUBS := host_sreset_security_stubs.c
RESET_ORACLE := sreset_reset_host_oracle.c
RESET_STUBS := host_sreset_reset_stubs.c
RUST_SRESET := $(ROOT)/rust/rtw_sreset.rs
RUSTC := rustc
RUST_CFG := --cfg host_sreset_test
RUST_ADAPTER_CFG := $(RUST_CFG) --cfg host_sreset_adapter_test --cfg CONFIG_CONCURRENT_MODE
RUST_RESET_CFG := $(RUST_ADAPTER_CFG) --cfg host_sreset_reset_test
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DCONFIG_CONCURRENT_MODE

.PHONY: all test test-c test-rust test-c-adapter test-rust-adapter \
	test-c-restore-security test-rust-restore-security test-c-reset test-rust-reset clean
all: test
test: test-c test-rust test-c-adapter test-rust-adapter \
	test-c-restore-security test-rust-restore-security test-c-reset test-rust-reset

test-c: test_sreset_lifecycle
	./test_sreset_lifecycle sreset_lifecycle_vectors.json

test-rust: test_sreset_lifecycle_rust
	./test_sreset_lifecycle_rust sreset_lifecycle_vectors.json

test-c-adapter: test_sreset_adapter
	./test_sreset_adapter

test-rust-adapter: test_sreset_adapter_rust
	./test_sreset_adapter_rust

test-c-reset: test_sreset_reset
	./test_sreset_reset

test-rust-reset: test_sreset_reset_rust
	./test_sreset_reset_rust

test_sreset_reset: test_sreset_reset.c $(RESET_ORACLE) $(RESET_STUBS)
	$(CC) $(CFLAGS) -o $@ test_sreset_reset.c $(RESET_ORACLE) $(RESET_STUBS)

librust_sreset_reset.a: $(RUST_SRESET)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on $(RUST_RESET_CFG) \
		--crate-type staticlib $(RUST_SRESET) -o $@

test_sreset_reset_rust: test_sreset_reset.c librust_sreset_reset.a $(RESET_STUBS) $(ADAPTER_STUBS)
	$(CC) $(CFLAGS) -o $@ test_sreset_reset.c $(RESET_STUBS) $(ADAPTER_STUBS) librust_sreset_reset.a

test_sreset_adapter: test_sreset_adapter.c $(ADAPTER_ORACLE)
	$(CC) $(CFLAGS) -o $@ test_sreset_adapter.c $(ADAPTER_ORACLE)

librust_sreset_adapter.a: $(RUST_SRESET)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on $(RUST_ADAPTER_CFG) \
		--crate-type staticlib $(RUST_SRESET) -o $@

test_sreset_adapter_rust: test_sreset_adapter.c librust_sreset_adapter.a $(ADAPTER_STUBS)
	$(CC) $(CFLAGS) -o $@ test_sreset_adapter.c $(ADAPTER_STUBS) librust_sreset_adapter.a

test_sreset_lifecycle: test_sreset_lifecycle.c $(SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_sreset_lifecycle.c $(SHIM) $(HOST_JSON)

librust_sreset.a: $(RUST_SRESET)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on $(RUST_CFG) \
		--crate-type staticlib $(RUST_SRESET) -o $@

test_sreset_lifecycle_rust: test_sreset_lifecycle.c librust_sreset.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_sreset_lifecycle.c $(HOST_JSON) librust_sreset.a

test-c-restore-security: test_sreset_restore_security
	./test_sreset_restore_security

test-rust-restore-security: test_sreset_restore_security_rust
	./test_sreset_restore_security_rust

test_sreset_restore_security: test_sreset_restore_security.c $(SECURITY_ORACLE) $(SECURITY_STUBS)
	$(CC) $(CFLAGS) -o $@ test_sreset_restore_security.c $(SECURITY_ORACLE) $(SECURITY_STUBS)

RUST_SECURITY_CFG := $(RUST_CFG) --cfg host_sreset_security_test

librust_sreset_security.a: $(RUST_SRESET)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on $(RUST_SECURITY_CFG) \
		--crate-type staticlib $(RUST_SRESET) -o $@

test_sreset_restore_security_rust: test_sreset_restore_security.c librust_sreset_security.a $(SECURITY_STUBS)
	$(CC) $(CFLAGS) -o $@ test_sreset_restore_security.c $(SECURITY_STUBS) librust_sreset_security.a

clean:
	rm -f test_sreset_lifecycle test_sreset_lifecycle_rust test_sreset_adapter \
		test_sreset_adapter_rust test_sreset_restore_security \
		test_sreset_restore_security_rust test_sreset_reset \
		test_sreset_reset_rust librust_sreset.a librust_sreset_adapter.a \
		librust_sreset_security.a librust_sreset_reset.a
