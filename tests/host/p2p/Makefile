ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC)

.PHONY: all test clean
all test: test_p2p_channel
	./test_p2p_channel p2p_channel_pure_vectors.json

test_p2p_channel: p2p_channel_l2.c $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ p2p_channel_l2.c $(HOST_JSON)

clean:
	rm -f test_p2p_channel
