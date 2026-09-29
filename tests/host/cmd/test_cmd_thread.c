// SPDX-License-Identifier: GPL-2.0
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_cmd_thread_types.h"
#include "host_vector_json.h"

struct _adapter g_adapter;

struct vector {
	char name[64];
	char fn[16];
	int sema_credits, loop_budget;
	int expect_done_cnt, expect_stop_calls, expect_sema_up;
	int expect_issued, expect_cmd_hdl, expect_cmd_seq;
};

static void init_adapter(struct _adapter *a)
{
	struct cmd_priv *cp = &a->cmdpriv;

	memset(a, 0, sizeof(*a));
	_rtw_init_listhead(&cp->cmd_queue.queue);
	cp->padapter = a;
	cp->cmd_buf = rtw_zmalloc(MAX_CMDSZ);
	cp->cmd_seq = 1;
	host_cmd_thread_set_hw_init(_TRUE);
}

static int parse_vec(const char *o, size_t l, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(o, l, "name", v->name, sizeof(v->name)) ||
	    host_json_parse_string_in(o, l, "fn", v->fn, sizeof(v->fn)))
		return -1;
#define I(k, fld) host_json_parse_int_in(o, l, k, &v->fld)
	I("sema_credits", sema_credits);
	I("loop_budget", loop_budget);
	I("expect_done_cnt", expect_done_cnt);
	I("expect_stop_calls", expect_stop_calls);
	I("expect_sema_up", expect_sema_up);
	I("expect_issued", expect_issued);
	I("expect_cmd_hdl", expect_cmd_hdl);
	I("expect_cmd_seq", expect_cmd_seq);
#undef I
	return 0;
}

static int run(struct vector *v)
{
	struct cmd_priv *cp = &g_adapter.cmdpriv;

	host_cmd_thread_reset();
	memset(&g_adapter, 0, sizeof(g_adapter));
	init_adapter(&g_adapter);
	cp = &g_adapter.cmdpriv;

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
	} else if (!strcmp(v->fn, "thread_one")) {
		struct cmd_obj *cmd = rtw_zmalloc(sizeof(*cmd));

		if (!cmd)
			goto fail;
		cmd->cmdcode = 0;
		cmd->cmdsz = 4;
		cmd->parmbuf = rtw_zmalloc(4);
		_rtw_init_listhead(&cmd->list);
		host_cmd_thread_set_sema_credits(v->sema_credits);
		host_cmd_thread_loop_budget(v->loop_budget);
		_rtw_enqueue_cmd(&cp->cmd_queue, cmd, false);
		rtw_cmd_thread(&g_adapter);
		if ((int)cp->cmd_issued_cnt != v->expect_issued ||
		    host_cmd_thread_cmd_hdl_calls() != v->expect_cmd_hdl ||
		    (int)cp->cmd_seq != v->expect_cmd_seq)
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
	struct vector v[8];
	size_t n = 0;
	int bad = 0;
	const char *p = argc > 1 ? argv[1] : "cmd_thread_vectors.json";

	if (host_load_vectors(p, v, sizeof(v[0]), 8, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run(&v[i]) != 0;
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, p);
	return bad ? 1 : 0;
}
