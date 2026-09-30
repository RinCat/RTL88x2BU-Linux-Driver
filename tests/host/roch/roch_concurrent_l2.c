// SPDX-License-Identifier: GPL-2.0
/* W3-97 L2 C oracle: concurrent roch init, timer, and handler. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_types.h"
#include "host_vector_json.h"

#define _TRUE 1
#define _FALSE 0
#define H2C_SUCCESS 0
#define MI_LINKED 1
#define ROCH_AP_ROCH_CH_SWITCH_PROCESS_WK 2
#define RTW_CMDF_DIRECTLY 1
#define HAL_PRIME_CHNL_OFFSET_DONT_CARE 0
#define CHANNEL_WIDTH_20 0

struct ieee80211_channel { int center_freq; };
struct roch_info {
	struct ieee80211_channel remain_on_ch_channel;
	unsigned int min_home_dur;
	unsigned int max_away_dur;
	int ap_timer_inited;
	int ro_ch_timer_inited;
};
struct rtw_wdev_priv { int switch_ch_to; };
struct _adapter {
	struct roch_info rochinfo;
	struct rtw_wdev_priv wdev;
	u8 oper_ch;
	int mi_linked;
	u8 union_ch, union_bw, union_offset;
	u8 cfg80211_is_roch;
};
typedef struct _adapter *PADAPTER;

struct host_roch_concurrent_trace {
	int set_channel, leave_opch, back_opch, set_timer, wk_cmd, wk_cmd_type;
	u8 set_ch;
	unsigned int set_timer_ms;
};

static struct host_roch_concurrent_trace g_tr;
static struct _adapter g_a;

struct host_roch_concurrent_trace *host_roch_concurrent_trace(void) { return &g_tr; }

static u8 freq_to_ch(int f) { return f ? (u8)((f - 2407) / 5) : 0; }

static void setup(void)
{
	memset(&g_tr, 0, sizeof(g_tr));
	memset(&g_a, 0, sizeof(g_a));
	g_a.cfg80211_is_roch = _TRUE;
}

static u8 get_remain_ch(PADAPTER padapter)
{
	return freq_to_ch(padapter->rochinfo.remain_on_ch_channel.center_freq);
}

static u8 chk_need_stay_in_cur_chan(PADAPTER padapter)
{
	(void)padapter;
	return _FALSE;
}

static void set_channel_bwmode(PADAPTER a, u8 ch, u8 off, u8 bw)
{
	g_tr.set_channel = 1;
	g_tr.set_ch = ch;
	(void)off;
	(void)bw;
	a->oper_ch = ch;
}

static void _set_timer(int *inited, unsigned int ms)
{
	(void)inited;
	g_tr.set_timer = 1;
	g_tr.set_timer_ms = ms;
}

static u8 rtw_mi_check_status(PADAPTER a, int st)
{
	(void)st;
	return a->mi_linked ? _TRUE : _FALSE;
}

static u8 rtw_mi_get_union_chan(PADAPTER a) { return a->union_ch; }
static u8 rtw_mi_get_union_bw(PADAPTER a) { return a->union_bw; }
static u8 rtw_mi_get_union_offset(PADAPTER a) { return a->union_offset; }
static u8 rtw_get_oper_ch(PADAPTER a) { return a->oper_ch; }
static u8 rtw_cfg80211_get_is_roch(PADAPTER a) { return a->cfg80211_is_roch; }
static void rtw_leave_opch(PADAPTER a) { (void)a; g_tr.leave_opch = 1; }
static void rtw_back_opch(PADAPTER a) { (void)a; g_tr.back_opch = 1; }

#ifndef HOST_ROCH_CONCURRENT_RUST
void rtw_concurrent_handler(PADAPTER padapter);
u8 rtw_roch_wk_cmd(PADAPTER padapter, int cmd, void *parm, u8 flags);

static void rtw_init_timer(int *flag, void *cb, void *ctx)
{
	(void)cb;
	(void)ctx;
	*flag = 1;
}

void rtw_init_roch_info(PADAPTER padapter)
{
	memset(&padapter->rochinfo, 0, sizeof(padapter->rochinfo));
	rtw_init_timer(&padapter->rochinfo.ap_timer_inited, NULL, padapter);
	padapter->rochinfo.min_home_dur = 1500;
	padapter->rochinfo.max_away_dur = 250;
	rtw_init_timer(&padapter->rochinfo.ro_ch_timer_inited, NULL, padapter);
}

static s32 rtw_roch_wk_hdl(PADAPTER padapter, int cmd, u8 *buf)
{
	(void)buf;
	if (cmd == ROCH_AP_ROCH_CH_SWITCH_PROCESS_WK) {
		rtw_concurrent_handler(padapter);
		return H2C_SUCCESS;
	}
	return H2C_SUCCESS;
}

u8 rtw_roch_wk_cmd(PADAPTER padapter, int cmd, void *parm, u8 flags)
{
	(void)parm;
	g_tr.wk_cmd = 1;
	g_tr.wk_cmd_type = cmd;
	if (flags & RTW_CMDF_DIRECTLY) {
		if (rtw_roch_wk_hdl(padapter, cmd, NULL) != H2C_SUCCESS)
			return _FALSE;
	}
	return _TRUE;
}

void rtw_ap_roch_ch_switch_timer_process(void *ctx)
{
	PADAPTER adapter = ctx;

	adapter->wdev.switch_ch_to = 1;
	rtw_roch_wk_cmd(adapter, ROCH_AP_ROCH_CH_SWITCH_PROCESS_WK, NULL, 0);
}

void rtw_concurrent_handler(PADAPTER padapter)
{
	struct roch_info *prochinfo = &padapter->rochinfo;
	u8 remain_ch = get_remain_ch(padapter);

	if (!rtw_cfg80211_get_is_roch(padapter))
		return;

	if (rtw_mi_check_status(padapter, MI_LINKED)) {
		u8 union_ch = rtw_mi_get_union_chan(padapter);
		u8 union_bw = rtw_mi_get_union_bw(padapter);
		u8 union_offset = rtw_mi_get_union_offset(padapter);
		unsigned int duration;

		if (rtw_get_oper_ch(padapter) != union_ch) {
			set_channel_bwmode(padapter, union_ch, union_offset, union_bw);
			rtw_back_opch(padapter);
			duration = prochinfo->min_home_dur;
		} else {
			rtw_leave_opch(padapter);
			set_channel_bwmode(padapter, remain_ch, HAL_PRIME_CHNL_OFFSET_DONT_CARE, CHANNEL_WIDTH_20);
			duration = prochinfo->max_away_dur;
		}
		padapter->wdev.switch_ch_to = 0;
		_set_timer(&prochinfo->ap_timer_inited, duration);
	} else if (!chk_need_stay_in_cur_chan(padapter)) {
		set_channel_bwmode(padapter, remain_ch, HAL_PRIME_CHNL_OFFSET_DONT_CARE, CHANNEL_WIDTH_20);
	}
}
#else
void rtw_init_roch_info(PADAPTER padapter);
void rtw_ap_roch_ch_switch_timer_process(void *ctx);
void rtw_concurrent_handler(PADAPTER padapter);
u8 rtw_roch_wk_cmd(PADAPTER padapter, int cmd, void *parm, u8 flags);
#endif

typedef struct {
	char name[48], op[16];
	int freq, is_roch, mi_linked, oper, union_ch, union_bw, union_off;
	int min_home, max_away;
	int exp_min_home, exp_max_away, exp_ap_timer, exp_ro_timer;
	int exp_ch, exp_stay, exp_switch_to, exp_wk_cmd, exp_wk_type;
	int exp_set_ch, exp_leave, exp_back, exp_timer, exp_dur;
} vector_t;

static int parse_vec(const char *o, size_t l, void *vv)
{
	vector_t *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(o, l, "name", v->name, sizeof(v->name)))
		return -1;
#define I(k, f) host_json_parse_int_in(o, l, k, &v->f)
	host_json_parse_string_in(o, l, "op", v->op, sizeof(v->op));
	I("freq", freq);
	I("is_roch", is_roch);
	I("mi_linked", mi_linked);
	I("oper", oper);
	I("union_ch", union_ch);
	I("union_bw", union_bw);
	I("union_off", union_off);
	I("min_home", min_home);
	I("max_away", max_away);
	I("exp_min_home", exp_min_home);
	I("exp_max_away", exp_max_away);
	I("exp_ap_timer", exp_ap_timer);
	I("exp_ro_timer", exp_ro_timer);
	I("exp_ch", exp_ch);
	I("exp_stay", exp_stay);
	I("exp_switch_to", exp_switch_to);
	I("exp_wk_cmd", exp_wk_cmd);
	I("exp_wk_type", exp_wk_type);
	I("exp_set_ch", exp_set_ch);
	I("exp_leave", exp_leave);
	I("exp_back", exp_back);
	I("exp_timer", exp_timer);
	I("exp_dur", exp_dur);
#undef I
	return 0;
}

static int run_vec(vector_t *v)
{
	setup();
	if (!strcmp(v->op, "init")) {
		rtw_init_roch_info(&g_a);
		if (g_a.rochinfo.min_home_dur != (unsigned int)v->exp_min_home ||
		    g_a.rochinfo.max_away_dur != (unsigned int)v->exp_max_away ||
		    g_a.rochinfo.ap_timer_inited != v->exp_ap_timer ||
		    g_a.rochinfo.ro_ch_timer_inited != v->exp_ro_timer)
			goto fail;
	} else if (!strcmp(v->op, "remain")) {
		g_a.rochinfo.remain_on_ch_channel.center_freq = v->freq;
		if (get_remain_ch(&g_a) != (u8)v->exp_ch)
			goto fail;
	} else if (!strcmp(v->op, "chk_stay")) {
		if (chk_need_stay_in_cur_chan(&g_a) != (u8)v->exp_stay)
			goto fail;
	} else if (!strcmp(v->op, "ap_timer")) {
		rtw_ap_roch_ch_switch_timer_process(&g_a);
		if (g_a.wdev.switch_ch_to != v->exp_switch_to || g_tr.wk_cmd != v->exp_wk_cmd ||
		    g_tr.wk_cmd_type != v->exp_wk_type)
			goto fail;
	} else if (!strcmp(v->op, "concurrent")) {
		g_a.cfg80211_is_roch = (u8)v->is_roch;
		g_a.mi_linked = (u8)v->mi_linked;
		g_a.oper_ch = (u8)v->oper;
		g_a.union_ch = (u8)v->union_ch;
		g_a.union_bw = (u8)v->union_bw;
		g_a.union_offset = (u8)v->union_off;
		g_a.rochinfo.remain_on_ch_channel.center_freq = v->freq;
		if (v->min_home)
			g_a.rochinfo.min_home_dur = (unsigned int)v->min_home;
		if (v->max_away)
			g_a.rochinfo.max_away_dur = (unsigned int)v->max_away;
		rtw_concurrent_handler(&g_a);
		if (g_tr.set_channel != v->exp_set_ch ||
		    (v->exp_ch && g_tr.set_ch != (u8)v->exp_ch) ||
		    g_tr.leave_opch != v->exp_leave || g_tr.back_opch != v->exp_back ||
		    g_tr.set_timer != v->exp_timer ||
		    (v->exp_dur && g_tr.set_timer_ms != (unsigned int)v->exp_dur))
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
	const char *p = argc > 1 ? argv[1] : "roch_concurrent_vectors.json";

	if (host_load_vectors(p, v, sizeof(v[0]), 12, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&v[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, p);
	return bad ? 1 : 0;
}
