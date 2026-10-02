ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
SHIM := host_tdls_lifecycle_shim.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC)

.PHONY: all test test-c clean
all: test
test: test-c

test-c: test_tdls_lifecycle
	./test_tdls_lifecycle tdls_lifecycle_vectors.json

test_tdls_lifecycle: test_tdls_lifecycle.c $(SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_tdls_lifecycle.c $(SHIM) $(HOST_JSON)

clean:
	rm -f test_tdls_lifecycle
