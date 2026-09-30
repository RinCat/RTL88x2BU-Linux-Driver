// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_sreset_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 8
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME];
	char op[12];
	int silent_inprogress;
	int seed_error;
	int txdma_reg;
	int error_status;
	int expect_u8;
};

static _adapter g_adapter;

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	host_json_parse_int_in(obj, len, "silent_inprogress", &v->silent_inprogress);
	host_json_parse_int_in(obj, len, "seed_error", &v->seed_error);
	host_json_parse_int_in(obj, len, "txdma_reg", &v->txdma_reg);
	host_json_parse_int_in(obj, len, "error_status", &v->error_status);
	host_json_parse_int_in(obj, len, "expect_u8", &v->expect_u8);
	return 0;
}

static int run_vec(struct vector *v)
{
	struct sreset_priv *p;
	u8 got = 0;

	memset(&g_adapter, 0, sizeof(g_adapter));
	host_sreset_clear_reg_reads();
	host_sreset_set_reg_read(REG_TXDMA_STATUS, (u32)v->txdma_reg);
	p = &GET_HAL_DATA(&g_adapter)->srestpriv;

	if (!strcmp(v->op, "init"))
		sreset_init_value(&g_adapter);
	else if (!strcmp(v->op, "reset")) {
		p->Wifi_Error_Status = (u8)v->seed_error;
		p->last_tx_time = 99;
		sreset_reset_value(&g_adapter);
	} else if (!strcmp(v->op, "get_wifi")) {
		p->silent_reset_inprogress = (u8)v->silent_inprogress;
		got = sreset_get_wifi_status(&g_adapter);
	} else if (!strcmp(v->op, "set_error")) {
		sreset_set_wifi_error_status(&g_adapter, (u32)v->error_status);
		got = p->Wifi_Error_Status;
	} else if (!strcmp(v->op, "inprogress")) {
		p->silent_reset_inprogress = (u8)v->silent_inprogress;
		got = sreset_inprogress(&g_adapter);
	} else
		return 1;

	if (!strcmp(v->op, "init") || !strcmp(v->op, "reset"))
		got = p->Wifi_Error_Status;
	if (got != (u8)v->expect_u8) {
		fprintf(stderr, "FAIL %s got=%u expect=%u\n", v->name, got, v->expect_u8);
		return 1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "sreset_lifecycle_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
