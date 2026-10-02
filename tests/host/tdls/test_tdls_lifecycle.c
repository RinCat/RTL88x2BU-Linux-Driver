// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_tdls_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 8
#define MAX_NAME 64
#define MAX_FRAME 32

struct vector {
	char name[MAX_NAME];
	char op[24];
	char frame_hex[64];
	int pkt_len;
	u32 ch_state;
	int expect_int;
};

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	v->pkt_len = -1;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	host_json_parse_string_in(obj, len, "frame_hex", v->frame_hex,
				  sizeof(v->frame_hex));
	host_json_parse_int_in(obj, len, "pkt_len", &v->pkt_len);
	host_json_parse_int_in(obj, len, "tdls_state", (int *)&v->ch_state);
	host_json_parse_int_in(obj, len, "expect_int", &v->expect_int);
	return 0;
}

static int run_vec(struct vector *v)
{
	u8 frame[MAX_FRAME];
	size_t frame_len = 0;
	int got = 0;

	if (v->frame_hex[0] &&
	    host_hex_decode(v->frame_hex, frame, sizeof(frame), &frame_len))
		return 1;

	if (!strcmp(v->op, "ap_prohibited"))
		got = check_ap_tdls_prohibited(frame, (u8)v->pkt_len);
	else if (!strcmp(v->op, "ap_chsw_prohibited"))
		got = check_ap_tdls_ch_switching_prohibited(frame, (u8)v->pkt_len);
	else if (!strcmp(v->op, "check_ch_state"))
		got = TDLS_check_ch_state(v->ch_state);
	else
		return 1;

	if (got == v->expect_int) {
		printf("PASS %s\n", v->name);
		return 0;
	}
	fprintf(stderr, "FAIL %s got=%d expect=%d\n", v->name, got, v->expect_int);
	return 1;
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "tdls_lifecycle_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
