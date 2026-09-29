// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_cmd_joinbss_types.h"
#include "host_vector_json.h"

static struct _adapter g_adapter;

struct vector {
	char name[64];
	int infra_mode, infra_set, malloc_fail, expect_ret, expect_enqueue, expect_fw;
};

static int parse_vec(const char *o, size_t l, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(o, l, "name", v->name, sizeof(v->name)))
		return -1;
#define I(k, fld) host_json_parse_int_in(o, l, k, &v->fld)
	I("infra_mode", infra_mode);
	I("infra_set", infra_set);
	I("malloc_fail", malloc_fail);
	I("expect_ret", expect_ret);
	I("expect_enqueue", expect_enqueue);
	I("expect_fw", expect_fw);
#undef I
	return 0;
}

static int run_vec(struct vector *v)
{
	struct wlan_network net;
	struct host_joinbss_trace *tr;
	u8 ret;

	host_joinbss_reset();
	host_joinbss_set_malloc_fail(v->malloc_fail);
	memset(&g_adapter, 0, sizeof(g_adapter));
	memset(&net, 0, sizeof(net));
	net.network.IELength = 12;
	net.network.InfrastructureMode = v->infra_set ? v->infra_mode : Ndis802_11Infrastructure;
	ret = rtw_joinbss_cmd(&g_adapter, &net);
	tr = host_joinbss_get_trace();
	if ((int)ret != v->expect_ret || tr->enqueue_ok != v->expect_enqueue ||
	    (int)g_adapter.mlmepriv.fw_state != v->expect_fw)
		goto fail;
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
	const char *p = argc > 1 ? argv[1] : "joinbss_cmd_vectors.json";

	if (host_load_vectors(p, v, sizeof(v[0]), 8, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&v[i]) != 0;
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, p);
	return bad ? 1 : 0;
}
