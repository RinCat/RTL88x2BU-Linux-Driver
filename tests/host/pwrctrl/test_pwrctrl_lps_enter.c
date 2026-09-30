// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_pwrctrl_lps_enter.h"
#include "host_vector_json.h"

#define MAX_VECTORS 8
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME];
	char action[8];
	int b_fw_ready, assoc_if_num, fw_state, b_leisure_ps, lps_idle_count;
	int pwr_mode, power_mgnt, smart_ps, bpower_saving;
	int expect_set_ps_mode, expect_bpower_saving, expect_lps_idle_count, expect_last_ps_mode;
};

static _adapter g_a;
static struct dvobj_priv g_d;

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	v->assoc_if_num = 1;
	v->power_mgnt = 2;
	v->expect_lps_idle_count = -1;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "action", v->action, sizeof(v->action));
	host_json_parse_int_in(obj, len, "b_fw_ready", &v->b_fw_ready);
	host_json_parse_int_in(obj, len, "assoc_if_num", &v->assoc_if_num);
	host_json_parse_int_in(obj, len, "fw_state", &v->fw_state);
	host_json_parse_int_in(obj, len, "b_leisure_ps", &v->b_leisure_ps);
	host_json_parse_int_in(obj, len, "lps_idle_count", &v->lps_idle_count);
	host_json_parse_int_in(obj, len, "pwr_mode", &v->pwr_mode);
	host_json_parse_int_in(obj, len, "power_mgnt", &v->power_mgnt);
	host_json_parse_int_in(obj, len, "smart_ps", &v->smart_ps);
	host_json_parse_int_in(obj, len, "bpower_saving", &v->bpower_saving);
	host_json_parse_int_in(obj, len, "expect_set_ps_mode", &v->expect_set_ps_mode);
	host_json_parse_int_in(obj, len, "expect_bpower_saving", &v->expect_bpower_saving);
	host_json_parse_int_in(obj, len, "expect_lps_idle_count", &v->expect_lps_idle_count);
	host_json_parse_int_in(obj, len, "expect_last_ps_mode", &v->expect_last_ps_mode);
	return 0;
}

static int run_vec(struct vector *v)
{
	struct host_pwrctrl_lps_enter_trace *tr;

	memset(&g_a, 0, sizeof(g_a));
	memset(&g_d, 0, sizeof(g_d));
	g_d.iface_nums = 1;
	g_d.padapters[0] = &g_a;
	g_a.dvobj = &g_d;
	g_a.hw_port = HW_PORT0;
	host_pwrctrl_lps_enter_reset_trace();
	host_pwrctrl_lps_enter_set_time(1000);
	host_pwrctrl_lps_enter_set_assoc_if_num(v->assoc_if_num);
	g_a.HalData.bFWReady = (u8)v->b_fw_ready;
	g_a.mlmepriv.fw_state = (u32)v->fw_state;
	g_a.pwrctrlpriv.bLeisurePs = (u8)v->b_leisure_ps;
	g_a.pwrctrlpriv.LpsIdleCount = (u8)v->lps_idle_count;
	g_a.pwrctrlpriv.pwr_mode = (u8)v->pwr_mode;
	g_a.pwrctrlpriv.power_mgnt = (u8)v->power_mgnt;
	g_a.pwrctrlpriv.bpower_saving = (u8)v->bpower_saving;
	g_a.registrypriv.smart_ps = (u8)v->smart_ps;
	if (v->action[0] && !strcmp(v->action, "leave"))
		LPS_Leave(&g_a, "L");
	else
		LPS_Enter(&g_a, "E");
	tr = host_pwrctrl_lps_enter_get_trace();
	if ((int)tr->set_ps_mode_calls != v->expect_set_ps_mode ||
	    (int)g_a.pwrctrlpriv.bpower_saving != v->expect_bpower_saving ||
	    (v->expect_lps_idle_count >= 0 &&
	     (int)g_a.pwrctrlpriv.LpsIdleCount != v->expect_lps_idle_count) ||
	    (v->expect_last_ps_mode && (int)tr->last_ps_mode != v->expect_last_ps_mode)) {
		fprintf(stderr, "FAIL %s\n", v->name);
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
	const char *path = argc > 1 ? argv[1] : "lps_enter_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
