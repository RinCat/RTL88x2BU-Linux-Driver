// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_cmd_traffic_lps_types.h"
#include "host_vector_json.h"

static _adapter g_adapter;

struct vector {
	char name[64];
	int lps_ctrl_type, adhoc;
	int expect_lps_enter, expect_lps_leave, expect_hw_rpt, expect_deny;
	int expect_lps_idle_count, expect_hw_joinbss_val;
	int check_lps_idle_count, check_hw_joinbss_val;
};

static int parse_vec(const char *o, size_t l, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(o, l, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_int_in(o, l, "lps_ctrl_type", &v->lps_ctrl_type);
	host_json_parse_int_in(o, l, "adhoc", &v->adhoc);
	host_json_parse_int_in(o, l, "expect_lps_enter", &v->expect_lps_enter);
	host_json_parse_int_in(o, l, "expect_lps_leave", &v->expect_lps_leave);
	host_json_parse_int_in(o, l, "expect_hw_rpt", &v->expect_hw_rpt);
	host_json_parse_int_in(o, l, "expect_deny", &v->expect_deny);
	v->check_lps_idle_count =
		host_json_parse_int_in(o, l, "expect_lps_idle_count", &v->expect_lps_idle_count) == 0;
	v->check_hw_joinbss_val =
		host_json_parse_int_in(o, l, "expect_hw_joinbss_val", &v->expect_hw_joinbss_val) == 0;
	return 0;
}

static int run_vec(struct vector *v)
{
	struct host_traffic_lps_trace *tr;

	host_traffic_lps_reset();
	memset(&g_adapter, 0, sizeof(g_adapter));
	if (v->adhoc)
		g_adapter.mlmepriv.fw_state = WIFI_ADHOC_STATE;
	if (v->lps_ctrl_type == LPS_CTRL_CONNECT)
		g_adapter.pwrctrlpriv.LpsIdleCount = 7;
	lps_ctrl_wk_hdl(&g_adapter, (u8)v->lps_ctrl_type, NULL);
	tr = host_traffic_lps_get_trace();
	if (tr->lps_enter == v->expect_lps_enter && tr->lps_leave == v->expect_lps_leave &&
	    tr->hw_joinbss_rpt == v->expect_hw_rpt && tr->set_lps_deny == v->expect_deny &&
	    (!v->check_lps_idle_count ||
	     g_adapter.pwrctrlpriv.LpsIdleCount == (u8)v->expect_lps_idle_count) &&
	    (!v->check_hw_joinbss_val || tr->hw_joinbss_val == (u8)v->expect_hw_joinbss_val)) {
		printf("PASS %s\n", v->name);
		return 0;
	}
	fprintf(stderr, "FAIL %s\n", v->name);
	return -1;
}

int main(int argc, char **argv)
{
	struct vector v[8];
	size_t n = 0;
	int bad = 0;
	const char *p = argc > 1 ? argv[1] : "traffic_lps_vectors.json";

	if (host_load_vectors(p, v, sizeof(v[0]), 8, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&v[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, p);
	return bad ? 1 : 0;
}
