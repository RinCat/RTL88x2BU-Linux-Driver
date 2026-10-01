ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
SHIM := host_sreset_lifecycle_shim.c
ADAPTER_ORACLE := sreset_adapter_host_oracle.c
ADAPTER_STUBS := host_sreset_adapter_stubs.c
SECURITY_ORACLE := sreset_restore_security_host_oracle.c
SECURITY_STUBS := host_sreset_security_stubs.c
RUST_SRESET := $(ROOT)/rust/rtw_sreset.rs
RUSTC := rustc
RUST_CFG := --cfg host_sreset_test
RUST_ADAPTER_CFG := $(RUST_CFG) --cfg host_sreset_adapter_test --cfg CONFIG_CONCURRENT_MODE
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DCONFIG_CONCURRENT_MODE

.PHONY: all test test-c test-rust test-c-adapter test-rust-adapter \
	test-c-restore-security test-rust-restore-security clean
all: test
test: test-c test-rust test-c-adapter test-rust-adapter \
	test-c-restore-security test-rust-restore-security

test-c: test_sreset_lifecycle
	./test_sreset_lifecycle sreset_lifecycle_vectors.json

test-rust: test_sreset_lifecycle_rust
	./test_sreset_lifecycle_rust sreset_lifecycle_vectors.json

test-c-adapter: test_sreset_adapter
	./test_sreset_adapter

test-rust-adapter: test_sreset_adapter_rust
	./test_sreset_adapter_rust

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
		test_sreset_restore_security_rust librust_sreset.a librust_sreset_adapter.a \
		librust_sreset_security.a
