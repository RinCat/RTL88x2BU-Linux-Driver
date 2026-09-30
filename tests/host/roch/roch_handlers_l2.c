// SPDX-License-Identifier: GPL-2.0
/* W3-96 L2 C oracle (subset: no WPS/AP/concurrent/cfg80211; union_ch stub). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_types.h"
#include "host_vector_json.h"

#define _TRUE 1
#define _FALSE 0
#define H2C_SUCCESS 0
#define RTW_CMDF_DIRECTLY 1
#define ROCH_RO_CH_WK 0
#define ROCH_CANCEL_RO_CH_WK 1
#define WIFI_UNDER_LINKING 0x80
#define WIFI_ASOC_STATE 0x1

typedef s32 mlme_state_t;
struct mlme_priv { mlme_state_t fwstate; };
struct ieee80211_channel { int center_freq; };
struct rtw_roch_parm { struct ieee80211_channel ch; unsigned int duration; };
struct roch_info { u8 restore_channel; u8 is_roch; };
struct dvobj_priv { u8 iface_nums, union_ch; struct _adapter *padapters[2]; };
struct _adapter {
	struct mlme_priv mlmepriv;
	struct roch_info rochinfo;
	u8 oper_ch;
	struct dvobj_priv *dvobj;
};
typedef struct _adapter *PADAPTER;

struct host_roch_trace {
	int set_channel, set_timer, cancel_timer, roch_expired, wk_cmd_direct, mfree;
	u8 set_channel_ch;
	unsigned int set_timer_ms;
};

static struct host_roch_trace g_tr;
static struct dvobj_priv g_dv;
static struct _adapter g_if[2], g_a;

struct host_roch_trace *host_roch_trace(void) { return &g_tr; }

#ifndef HOST_ROCH_RUST
static s32 chk(struct mlme_priv *m, s32 st) { return (m->fwstate & st) ? _TRUE : _FALSE; }
#endif

static void setup(mlme_state_t st, u8 union_ch)
{
	memset(&g_tr, 0, sizeof(g_tr));
	memset(&g_dv, 0, sizeof(g_dv));
	memset(g_if, 0, sizeof(g_if));
	g_dv.iface_nums = 1;
	g_dv.union_ch = union_ch;
	g_if[0].mlmepriv.fwstate = st;
	g_if[0].dvobj = &g_dv;
	g_dv.padapters[0] = &g_if[0];
	g_a = g_if[0];
}

#ifndef HOST_ROCH_RUST
static u8 union_chan(PADAPTER a) { return (a && a->dvobj && a->dvobj->union_ch) ? a->dvobj->union_ch : 6; }
static u8 freq_to_ch(int f) { return f ? (u8)((f - 2407) / 5) : 0; }

u8 rtw_roch_stay_in_cur_chan(PADAPTER padapter)
{
	u8 i;

	if (!padapter || !padapter->dvobj)
		return _FALSE;
	for (i = 0; i < padapter->dvobj->iface_nums; i++) {
		PADAPTER iface = padapter->dvobj->padapters[i];

		if (iface && chk(&iface->mlmepriv, WIFI_UNDER_LINKING))
			return _TRUE;
	}
	return _FALSE;
}

static int ro_ch(PADAPTER a, struct rtw_roch_parm *p)
{
	u8 remain = freq_to_ch(p->ch.center_freq);

	if (a->rochinfo.is_roch != _TRUE)
		return H2C_SUCCESS;
	if (rtw_roch_stay_in_cur_chan(a))
		remain = union_chan(a);
	if (remain != a->oper_ch && !chk(&a->mlmepriv, WIFI_ASOC_STATE)) {
		g_tr.set_channel = 1;
		g_tr.set_channel_ch = remain;
		a->oper_ch = remain;
	}
	g_tr.set_timer = 1;
	g_tr.set_timer_ms = p->duration;
	return H2C_SUCCESS;
}

static int cancel_ro(PADAPTER a)
{
	if (a->rochinfo.is_roch != _TRUE)
		return H2C_SUCCESS;
	g_tr.cancel_timer = 1;
	g_tr.set_channel = 1;
	g_tr.set_channel_ch = a->rochinfo.restore_channel;
	a->oper_ch = a->rochinfo.restore_channel;
	a->rochinfo.is_roch = _FALSE;
	g_tr.roch_expired = 1;
	return H2C_SUCCESS;
}

s32 rtw_roch_wk_hdl(PADAPTER a, int cmd, struct rtw_roch_parm *p)
{
	if (cmd == ROCH_RO_CH_WK)
		return ro_ch(a, p);
	if (cmd == ROCH_CANCEL_RO_CH_WK)
		return cancel_ro(a);
	return H2C_SUCCESS;
}

u8 rtw_roch_wk_cmd(PADAPTER a, int cmd, struct rtw_roch_parm *p, u8 flags)
{
	if (flags & RTW_CMDF_DIRECTLY) {
		g_tr.wk_cmd_direct++;
		if (H2C_SUCCESS != rtw_roch_wk_hdl(a, cmd, p))
			return _FALSE;
		if (p) {
			g_tr.mfree++;
			free(p);
		}
	}
	return _TRUE;
}
#else
u8 rtw_roch_stay_in_cur_chan(PADAPTER padapter);
s32 rtw_roch_wk_hdl(PADAPTER a, int cmd, struct rtw_roch_parm *p);
u8 rtw_roch_wk_cmd(PADAPTER a, int cmd, struct rtw_roch_parm *p, u8 flags);
#endif

typedef struct {
	char name[48], op[12];
	int fwstate, is_roch, freq, dur, oper, restore, cmd, flags, union_ch;
	int exp_stay, exp_set_ch, exp_ch, exp_timer, exp_dur, exp_cancel, exp_expired, exp_direct, exp_mfree;
} vector_t;

static int parse_vec(const char *o, size_t l, void *vv)
{
	vector_t *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(o, l, "name", v->name, sizeof(v->name)))
		return -1;
#define I(k, f) host_json_parse_int_in(o, l, k, &v->f)
	host_json_parse_string_in(o, l, "op", v->op, sizeof(v->op));
	I("fwstate", fwstate); I("is_roch", is_roch); I("freq", freq); I("dur", dur);
	I("oper", oper); I("restore", restore); I("cmd", cmd); I("flags", flags);
	I("union_ch", union_ch); I("exp_stay", exp_stay); I("exp_set_ch", exp_set_ch);
	I("exp_ch", exp_ch); I("exp_timer", exp_timer); I("exp_dur", exp_dur);
	I("exp_cancel", exp_cancel); I("exp_expired", exp_expired);
	I("exp_direct", exp_direct); I("exp_mfree", exp_mfree);
#undef I
	return 0;
}

static int run_vec(vector_t *v)
{
	struct rtw_roch_parm parm, *heap;

	setup(v->fwstate, (u8)v->union_ch);
	g_a.rochinfo.is_roch = (u8)v->is_roch;
	g_a.oper_ch = (u8)v->oper;
	g_a.rochinfo.restore_channel = (u8)v->restore;

	if (!strcmp(v->op, "stay")) {
		if (rtw_roch_stay_in_cur_chan(&g_a) != (u8)v->exp_stay)
			goto fail;
	} else if (!strcmp(v->op, "ro_ch")) {
		memset(&parm, 0, sizeof(parm));
		parm.ch.center_freq = v->freq;
		parm.duration = (unsigned int)v->dur;
		rtw_roch_wk_hdl(&g_a, ROCH_RO_CH_WK, &parm);
		if (g_tr.set_channel != v->exp_set_ch || g_tr.set_timer != v->exp_timer ||
		    (v->exp_ch && g_tr.set_channel_ch != (u8)v->exp_ch) ||
		    (v->exp_dur && g_tr.set_timer_ms != (unsigned int)v->exp_dur))
			goto fail;
	} else if (!strcmp(v->op, "cancel")) {
		rtw_roch_wk_hdl(&g_a, ROCH_CANCEL_RO_CH_WK, NULL);
		if (g_tr.cancel_timer != v->exp_cancel || g_tr.roch_expired != v->exp_expired ||
		    g_a.rochinfo.is_roch != _FALSE ||
		    (v->exp_ch && g_a.oper_ch != (u8)v->exp_ch))
			goto fail;
	} else if (!strcmp(v->op, "wk_cmd")) {
		heap = calloc(1, sizeof(*heap));
		heap->ch.center_freq = v->freq;
		heap->duration = (unsigned int)v->dur;
		if (rtw_roch_wk_cmd(&g_a, v->cmd, heap, (u8)v->flags) != _TRUE ||
		    g_tr.wk_cmd_direct != v->exp_direct || g_tr.mfree != v->exp_mfree)
			goto fail;
	} else
		goto fail;
	printf("PASS %s\n", v->name);
	return 0;
fail:
	fprintf(stderr, "FAIL %s\n", v->name);
	return 1;
}

int main(int argc, char **argv)
{
	vector_t v[12];
	size_t n = 0;
	int bad = 0;
	const char *p = argc > 1 ? argv[1] : "roch_handlers_vectors.json";

	if (host_load_vectors(p, v, sizeof(v[0]), 12, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&v[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, p);
	return bad ? 1 : 0;
}
