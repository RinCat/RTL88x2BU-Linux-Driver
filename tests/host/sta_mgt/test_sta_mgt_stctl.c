// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-55 st_ctl + stainfo_offset helpers. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_sta_mgt_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 128

enum stctl_fn {
	FN_CHK_PROTO,
	FN_CHK_RULE,
	FN_OFFSET,
	FN_UNREGISTER,
};

struct vector {
	char name[MAX_NAME];
	enum stctl_fn fn;
	u8 s_proto;
	u8 local_port[2];
	u8 remote_port[2];
	u8 sta_index;
	int with_reg;
	int then_unregister;
	int add_tracker;
	int expect;
	int expect_trackers;
	int expect_setup_fail;
};

static int parse_fn(const char *obj, size_t len, enum stctl_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, len, "fn", fn, sizeof(fn)))
		return -1;
	if (!strcmp(fn, "rtw_st_ctl_chk_reg_s_proto"))
		*out = FN_CHK_PROTO;
	else if (!strcmp(fn, "rtw_st_ctl_chk_reg_rule"))
		*out = FN_CHK_RULE;
	else if (!strcmp(fn, "rtw_stainfo_offset"))
		*out = FN_OFFSET;
	else if (!strcmp(fn, "rtw_st_ctl_unregister"))
		*out = FN_UNREGISTER;
	else
		return -1;
	return 0;
}

static int parse_hex_opt(const char *obj, size_t len, const char *key, u8 *out,
			 size_t out_len)
{
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t decoded = 0;

	if (host_json_parse_string_in(obj, len, key, hex, sizeof(hex)))
		return 0;
	if (host_hex_decode(hex, out, out_len, &decoded) || decoded != out_len)
		return -1;
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	v->expect_trackers = -1;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, len, &v->fn))
		return -1;
	host_json_parse_int_in(obj, len, "expect", &v->expect);
	host_json_parse_int_in(obj, len, "s_proto", (int *)&v->s_proto);
	host_json_parse_int_in(obj, len, "with_reg", &v->with_reg);
	host_json_parse_int_in(obj, len, "then_unregister", &v->then_unregister);
	host_json_parse_int_in(obj, len, "add_tracker", &v->add_tracker);
	if (host_json_parse_int_in(obj, len, "expect_trackers", &v->expect_trackers) == 0 &&
	    v->fn == FN_UNREGISTER)
		v->expect = v->expect_trackers;
	host_json_parse_int_in(obj, len, "expect_setup_fail", &v->expect_setup_fail);
	host_json_parse_int_in(obj, len, "sta_index", (int *)&v->sta_index);
	if (parse_hex_opt(obj, len, "local_port", v->local_port, 2))
		return -1;
	if (parse_hex_opt(obj, len, "remote_port", v->remote_port, 2))
		return -1;
	return 0;
}

static int run_vector(struct vector *v)
{
	_adapter adapter;
	struct st_ctl_t st_ctl;
	struct sta_info *sta = NULL;
	u8 ln[ETH_ALEN] = {0}, rn[ETH_ALEN] = {0};
	int got;

	memset(&adapter, 0, sizeof(adapter));
	host_sta_mgt_stctl_reset(&st_ctl);
	if (v->with_reg)
		rtw_st_ctl_register(&st_ctl, 0, &test_st_reg);
	/* Leaves reg[0].rule NULL, as rtw_wfd_st_switch(sta, 0) does. */
	if (v->then_unregister)
		rtw_st_ctl_unregister(&st_ctl, 0);
	if (v->add_tracker)
		host_sta_mgt_stctl_tracker_add(&st_ctl);

	switch (v->fn) {
	case FN_CHK_PROTO:
		got = rtw_st_ctl_chk_reg_s_proto(&st_ctl, v->s_proto) ? 1 : 0;
		break;
	case FN_CHK_RULE:
		got = rtw_st_ctl_chk_reg_rule(&st_ctl, &adapter, ln, v->local_port,
					      rn, v->remote_port) ?
		      1 :
		      0;
		break;
	case FN_OFFSET:
		if (host_sta_mgt_offset_setup(&adapter, v->sta_index, &sta)) {
			if (v->expect_setup_fail) {
				printf("PASS %s\n", v->name);
				return 0;
			}
			return -1;
		}
		got = rtw_stainfo_offset(&adapter.stapriv, sta);
		break;
	case FN_UNREGISTER:
		rtw_st_ctl_unregister(&st_ctl, 0);
		got = host_sta_mgt_stctl_tracker_count(&st_ctl);
		break;
	default:
		return -1;
	}

	if (got != v->expect) {
		fprintf(stderr, "FAIL %s: expect %d got %d\n", v->name, v->expect,
			got);
		return -1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0, i;
	int failures = 0;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <vectors.json>\n", argv[0]);
		return 2;
	}
	if (host_load_vectors(argv[1], vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", argv[1]);
		return 2;
	}
	for (i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			failures++;
	printf("%zu vectors, %d failures\n", count, failures);
	return failures ? 1 : 0;
}
