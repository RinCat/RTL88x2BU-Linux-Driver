// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_pwrctrl_leave_all_ps.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME], fn[8];
	int bup, bSurpriseRemoved, mi_linked, assoc_if_num, pwr_mode, rf_pwrstate;
	int expect_lps_ctrl, expect_lps_type, expect_lps_flags, expect_ips_leave;
};

static _adapter g_a;
static struct dvobj_priv g_d;

static int parse_vec(const char *o, size_t l, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	v->bup = -1;
	if (host_json_parse_string_in(o, l, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(o, l, "fn", v->fn, sizeof(v->fn));
	host_json_parse_int_in(o, l, "bup", &v->bup);
	host_json_parse_int_in(o, l, "bSurpriseRemoved", &v->bSurpriseRemoved);
	host_json_parse_int_in(o, l, "mi_linked", &v->mi_linked);
	host_json_parse_int_in(o, l, "assoc_if_num", &v->assoc_if_num);
	host_json_parse_int_in(o, l, "pwr_mode", &v->pwr_mode);
	host_json_parse_int_in(o, l, "rf_pwrstate", &v->rf_pwrstate);
	host_json_parse_int_in(o, l, "expect_lps_ctrl", &v->expect_lps_ctrl);
	host_json_parse_int_in(o, l, "expect_lps_type", &v->expect_lps_type);
	host_json_parse_int_in(o, l, "expect_lps_flags", &v->expect_lps_flags);
	host_json_parse_int_in(o, l, "expect_ips_leave", &v->expect_ips_leave);
	return 0;
}

static int run_vec(struct vector *v)
{
	struct host_pwrctrl_leave_all_ps_trace *tr;

	memset(&g_a, 0, sizeof(g_a));
	memset(&g_d, 0, sizeof(g_d));
	g_d.iface_nums = 1;
	g_d.padapters[0] = &g_a;
	g_a.dvobj = &g_d;
	g_a.bup = v->bup >= 0 ? (u8)v->bup : 1;
	g_a.bSurpriseRemoved = (u8)v->bSurpriseRemoved;
	g_a.pwrctrlpriv.pwr_mode = (u8)v->pwr_mode;
	g_a.pwrctrlpriv.rf_pwrstate = (u8)v->rf_pwrstate;
	host_pwrctrl_leave_all_ps_reset_trace();
	host_pwrctrl_leave_all_ps_set_mi_linked((u8)v->mi_linked);
	host_pwrctrl_leave_all_ps_set_assoc_if_num(v->assoc_if_num);
	if (v->fn[0] && !strcmp(v->fn, "direct"))
		LeaveAllPowerSaveModeDirect(&g_a);
	else
		LeaveAllPowerSaveMode(&g_a);
	tr = host_pwrctrl_leave_all_ps_get_trace();
	if (tr->lps_ctrl_wk_cmd != v->expect_lps_ctrl ||
	    (v->expect_lps_ctrl && (tr->last_lps_ctrl_type != (u8)v->expect_lps_type ||
	     tr->last_lps_ctrl_flags != (u8)v->expect_lps_flags)) ||
	    tr->ips_leave != v->expect_ips_leave) {
		fprintf(stderr,
			"FAIL %s lps_ctrl=%d expect=%d type=%u expect=%d flags=%u expect=%d ips=%d expect=%d\n",
			v->name, tr->lps_ctrl_wk_cmd, v->expect_lps_ctrl,
			tr->last_lps_ctrl_type, v->expect_lps_type,
			tr->last_lps_ctrl_flags, v->expect_lps_flags,
			tr->ips_leave, v->expect_ips_leave);
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
	const char *path = argc > 1 ? argv[1] : "leave_all_ps_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
