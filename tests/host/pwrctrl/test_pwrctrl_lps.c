// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle for W3-94 rtw_pwr_unassociated_idle. */

#include <stdio.h>
#include <string.h>

#include "host_pwrctrl_lps_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 8
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME];
	int bpower_saving;
	int ips_deny_time;
	int current_time;
	int fw_state;
	int free_xmitbuf_cnt;
	int free_xmit_extbuf_cnt;
	int expect_idle;
};

static struct _adapter g_adapter;
static struct dvobj_priv g_dvobj;

static void setup_adapter(void)
{
	memset(&g_adapter, 0, sizeof(g_adapter));
	memset(&g_dvobj, 0, sizeof(g_dvobj));
	g_dvobj.iface_nums = 1;
	g_dvobj.padapters[0] = &g_adapter;
	g_adapter.dvobj = &g_dvobj;
	g_adapter.xmitpriv.free_xmitbuf_cnt = NR_XMITBUFF;
	g_adapter.xmitpriv.free_xmit_extbuf_cnt = NR_XMIT_EXTBUFF;
}

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	v->free_xmitbuf_cnt = -1;
	v->free_xmit_extbuf_cnt = -1;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_int_in(obj, len, "bpower_saving", &v->bpower_saving);
	host_json_parse_int_in(obj, len, "ips_deny_time", &v->ips_deny_time);
	host_json_parse_int_in(obj, len, "current_time", &v->current_time);
	host_json_parse_int_in(obj, len, "fw_state", &v->fw_state);
	host_json_parse_int_in(obj, len, "free_xmitbuf_cnt", &v->free_xmitbuf_cnt);
	host_json_parse_int_in(obj, len, "free_xmit_extbuf_cnt", &v->free_xmit_extbuf_cnt);
	host_json_parse_int_in(obj, len, "expect_idle", &v->expect_idle);
	return 0;
}

static int run_vec(struct vector *v)
{
	int idle;

	setup_adapter();
	host_pwrctrl_lps_set_time((u32)v->current_time);
	g_adapter.pwrctrlpriv.bpower_saving = (u8)v->bpower_saving;
	g_adapter.pwrctrlpriv.ips_deny_time = (u32)v->ips_deny_time;
	g_adapter.mlmepriv.fw_state = (u32)v->fw_state;
	if (v->free_xmitbuf_cnt >= 0)
		g_adapter.xmitpriv.free_xmitbuf_cnt = (u16)v->free_xmitbuf_cnt;
	if (v->free_xmit_extbuf_cnt >= 0)
		g_adapter.xmitpriv.free_xmit_extbuf_cnt = (u16)v->free_xmit_extbuf_cnt;

	idle = rtw_pwr_unassociated_idle(&g_adapter) ? 1 : 0;
	if (idle != v->expect_idle) {
		fprintf(stderr, "FAIL %s idle=%d expect=%d\n", v->name, idle, v->expect_idle);
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
	const char *path = argc > 1 ? argv[1] : "lps_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
