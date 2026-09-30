ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -DCONFIG_IOCTL_CFG80211

.PHONY: all test clean
all: test
test: test_roch_handlers
	./test_roch_handlers roch_handlers_vectors.json

test_roch_handlers: roch_handlers_l2.c $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ roch_handlers_l2.c $(HOST_JSON)

clean:
	rm -f test_roch_handlers
