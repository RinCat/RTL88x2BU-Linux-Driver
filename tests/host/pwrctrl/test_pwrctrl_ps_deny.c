// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-94 ps deny gate helpers. */

#include <stdio.h>
#include <string.h>

#include "host_pwrctrl_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 8
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME];
	int deny_a;
	int deny_b;
	int cancel_a;
	int expect_deny;
};

static _adapter g_adapter;

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_int_in(obj, len, "deny_a", &v->deny_a);
	host_json_parse_int_in(obj, len, "deny_b", &v->deny_b);
	host_json_parse_int_in(obj, len, "cancel_a", &v->cancel_a);
	host_json_parse_int_in(obj, len, "expect_deny", &v->expect_deny);
	return 0;
}

static int run_vec(struct vector *v)
{
	u32 got;

	memset(&g_adapter, 0, sizeof(g_adapter));
	if (v->deny_a >= 0)
		rtw_ps_deny(&g_adapter, (PS_DENY_REASON)v->deny_a);
	if (v->deny_b >= 0)
		rtw_ps_deny(&g_adapter, (PS_DENY_REASON)v->deny_b);
	if (v->cancel_a >= 0)
		rtw_ps_deny_cancel(&g_adapter, (PS_DENY_REASON)v->cancel_a);
	got = rtw_ps_deny_get(&g_adapter);
	if (got == (u32)v->expect_deny) {
		printf("PASS %s\n", v->name);
		return 0;
	}
	fprintf(stderr, "FAIL %s got=0x%x expect=0x%x\n", v->name, got, v->expect_deny);
	return 1;
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "ps_deny_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
