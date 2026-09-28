// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_cmd_thread_types.h"
#include "host_vector_json.h"

struct _adapter g_adapter;

struct vector {
	char name[64];
	char fn[16];
	int expect_done_cnt, expect_stop_calls, expect_sema_up;
};

static int parse_vec(const char *o, size_t l, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(o, l, "name", v->name, sizeof(v->name)) ||
	    host_json_parse_string_in(o, l, "fn", v->fn, sizeof(v->fn)))
		return -1;
	host_json_parse_int_in(o, l, "expect_done_cnt", &v->expect_done_cnt);
	host_json_parse_int_in(o, l, "expect_stop_calls", &v->expect_stop_calls);
	host_json_parse_int_in(o, l, "expect_sema_up", &v->expect_sema_up);
	return 0;
}

static int run(struct vector *v)
{
	struct cmd_priv *cp = &g_adapter.cmdpriv;

	host_cmd_thread_reset();
	memset(&g_adapter, 0, sizeof(g_adapter));
	cp->padapter = &g_adapter;

	if (!strcmp(v->fn, "clr_isr")) {
		cp->cmd_done_cnt = 0;
		rtw_cmd_clr_isr(cp);
		if ((int)cp->cmd_done_cnt != v->expect_done_cnt)
			goto fail;
	} else if (!strcmp(v->fn, "stop")) {
		g_adapter.cmdThread = (void *)0x1;
		rtw_stop_cmd_thread(&g_adapter);
		if (g_adapter.cmdThread != NULL ||
		    host_cmd_thread_stop_calls() != v->expect_stop_calls ||
		    host_cmd_thread_sema_up_count() != v->expect_sema_up)
			goto fail;
	} else {
		goto fail;
	}

	printf("PASS %s\n", v->name);
	return 0;
fail:
	fprintf(stderr, "FAIL %s\n", v->name);
	return -1;
}

int main(int argc, char **argv)
{
	struct vector v[4];
	size_t n = 0;
	int bad = 0;
	const char *p = argc > 1 ? argv[1] : "cmd_thread_vectors.json";

	if (host_load_vectors(p, v, sizeof(v[0]), 4, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run(&v[i]) != 0;
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, p);
	return bad ? 1 : 0;
}
