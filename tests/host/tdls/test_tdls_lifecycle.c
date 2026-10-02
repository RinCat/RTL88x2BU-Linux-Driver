// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_tdls_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64
#define MAX_FRAME 32

struct vector {
	char name[MAX_NAME];
	char op[24];
	char frame_hex[64];
	int pkt_len;
	u32 tdls_state;
	u32 mlme_state;
	int wifi_spec;
	int ap_prohibited;
	int ch_switch_prohibited;
	int en_tdls;
	int set_enable;
	int expect_int;
};

static _adapter g_adapter;
static struct tdls_info g_tdls_free;

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	v->pkt_len = -1;
	v->set_enable = -1;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	host_json_parse_string_in(obj, len, "frame_hex", v->frame_hex,
				  sizeof(v->frame_hex));
	host_json_parse_int_in(obj, len, "pkt_len", &v->pkt_len);
	host_json_parse_int_in(obj, len, "tdls_state", (int *)&v->tdls_state);
	host_json_parse_int_in(obj, len, "mlme_state", (int *)&v->mlme_state);
	host_json_parse_int_in(obj, len, "wifi_spec", &v->wifi_spec);
	host_json_parse_int_in(obj, len, "ap_prohibited", &v->ap_prohibited);
	host_json_parse_int_in(obj, len, "ch_switch_prohibited",
			      &v->ch_switch_prohibited);
	host_json_parse_int_in(obj, len, "en_tdls", &v->en_tdls);
	host_json_parse_int_in(obj, len, "set_enable", &v->set_enable);
	host_json_parse_int_in(obj, len, "expect_int", &v->expect_int);
	return 0;
}

static int run_vec(struct vector *v)
{
	u8 frame[MAX_FRAME];
	size_t frame_len = 0;
	int got = 0;

	memset(&g_adapter, 0, sizeof(g_adapter));
	g_adapter.registrypriv.wifi_spec = (u8)v->wifi_spec;
	g_adapter.registrypriv.en_tdls = (u8)v->en_tdls;
	g_adapter.mlmeextpriv.mlmext_info.state = v->mlme_state;
	g_adapter.tdlsinfo.ap_prohibited = (u8)v->ap_prohibited;
	g_adapter.tdlsinfo.ch_switch_prohibited = (u8)v->ch_switch_prohibited;

	if (v->frame_hex[0] &&
	    host_hex_decode(v->frame_hex, frame, sizeof(frame), &frame_len))
		return 1;

	if (!strcmp(v->op, "ap_prohibited"))
		got = check_ap_tdls_prohibited(frame, (u8)v->pkt_len);
	else if (!strcmp(v->op, "ap_chsw_prohibited"))
		got = check_ap_tdls_ch_switching_prohibited(frame, (u8)v->pkt_len);
	else if (!strcmp(v->op, "check_ch_state"))
		got = TDLS_check_ch_state(v->tdls_state);
	else if (!strcmp(v->op, "reset")) {
		rtw_reset_tdls_info(&g_adapter);
		got = g_adapter.tdlsinfo.ch_switch_prohibited;
	} else if (!strcmp(v->op, "init")) {
		got = rtw_init_tdls_info(&g_adapter);
		if (got == _SUCCESS && g_adapter.tdlsinfo.driver_setup != _TRUE)
			got = -1;
	}
	else if (!strcmp(v->op, "free")) {
		memset(&g_tdls_free, 0xab, sizeof(g_tdls_free));
		rtw_free_tdls_info(&g_tdls_free);
		got = 1;
		for (size_t i = 0; i < sizeof(g_tdls_free); i++) {
			if (((u8 *)&g_tdls_free)[i] != 0) {
				got = 0;
				break;
			}
		}
	} else if (!strcmp(v->op, "is_enabled")) {
		if (v->set_enable >= 0)
			rtw_set_tdls_enable(&g_adapter, (u8)v->set_enable);
		got = rtw_is_tdls_enabled(&g_adapter);
	} else if (!strcmp(v->op, "setup_allowed"))
		got = rtw_tdls_is_setup_allowed(&g_adapter);
#ifdef CONFIG_TDLS_CH_SW
	else if (!strcmp(v->op, "chsw_allowed"))
		got = rtw_tdls_is_chsw_allowed(&g_adapter);
#endif
	else
		return 1;

	if (got == v->expect_int) {
		printf("PASS %s\n", v->name);
		return 0;
	}
	fprintf(stderr, "FAIL %s got=%d expect=%d\n", v->name, got, v->expect_int);
	return 1;
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "tdls_lifecycle_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
