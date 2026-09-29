// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_cmd_drvextra_types.h"
#include "host_vector_json.h"

extern struct host_drvextra_trace *host_drvextra_get_trace(void);

static struct _adapter g_adapter;

struct vector {
	char name[64];
	int ec_id, type, payload_size, expect_ret;
	int expect_dynamic, expect_reset_sec, expect_free_assoc, expect_hiq;
	int expect_mfree;
};

static int parse_vec(const char *o, size_t l, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(o, l, "name", v->name, sizeof(v->name)))
		return -1;
#define I(k, fld) host_json_parse_int_in(o, l, k, &v->fld)
	I("ec_id", ec_id);
	I("type", type);
	I("payload_size", payload_size);
	I("expect_ret", expect_ret);
	I("expect_dynamic", expect_dynamic);
	I("expect_reset_sec", expect_reset_sec);
	I("expect_free_assoc", expect_free_assoc);
	I("expect_hiq", expect_hiq);
	I("expect_mfree", expect_mfree);
#undef I
	return 0;
}

static int run_vec(struct vector *v)
{
	struct drvextra_cmd_parm parm;
	struct host_drvextra_trace *tr;
	u8 *payload = NULL;
	u8 ret;

	host_drvextra_reset(NULL);
	memset(&g_adapter, 0, sizeof(g_adapter));
	memset(&parm, 0, sizeof(parm));
	parm.ec_id = v->ec_id;
	parm.type = v->type;
	if (v->payload_size > 0) {
		payload = rtw_zmalloc((u32)v->payload_size);
		if (!payload)
			return -1;
		parm.pbuf = payload;
		parm.size = v->payload_size;
	}

	ret = v->ec_id < 0 ? rtw_drvextra_cmd_hdl(&g_adapter, NULL)
			   : rtw_drvextra_cmd_hdl(&g_adapter, (u8 *)&parm);
	tr = host_drvextra_get_trace();
	if ((int)ret != v->expect_ret || tr->dynamic != v->expect_dynamic ||
	    tr->reset_sec != v->expect_reset_sec || tr->free_assoc != v->expect_free_assoc ||
	    tr->hiq != v->expect_hiq || tr->mfree != v->expect_mfree)
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
	const char *p = argc > 1 ? argv[1] : "drvextra_cmd_vectors.json";

	if (host_load_vectors(p, v, sizeof(v[0]), 8, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&v[i]) != 0;
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, p);
	return bad ? 1 : 0;
}
