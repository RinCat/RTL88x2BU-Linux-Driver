ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
SHIM := host_sreset_lifecycle_shim.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h

.PHONY: all test clean
all test: test_sreset_lifecycle
	./test_sreset_lifecycle sreset_lifecycle_vectors.json

test_sreset_lifecycle: test_sreset_lifecycle.c $(SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_sreset_lifecycle.c $(SHIM) $(HOST_JSON)

clean:
	rm -f test_sreset_lifecycle
