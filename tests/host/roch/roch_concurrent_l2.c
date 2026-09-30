// SPDX-License-Identifier: GPL-2.0
/* W3-97 L2 C oracle: concurrent roch init + timer (PR1 slice). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_types.h"
#include "host_vector_json.h"

#define _TRUE 1
#define _FALSE 0
#define ROCH_AP_ROCH_CH_SWITCH_PROCESS_WK 2

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
};
typedef struct _adapter *PADAPTER;

struct host_roch_concurrent_trace {
	int wk_cmd, wk_cmd_type;
};

static struct host_roch_concurrent_trace g_tr;
static struct _adapter g_a;

struct host_roch_concurrent_trace *host_roch_concurrent_trace(void) { return &g_tr; }

static u8 freq_to_ch(int f) { return f ? (u8)((f - 2407) / 5) : 0; }

static void setup(void)
{
	memset(&g_tr, 0, sizeof(g_tr));
	memset(&g_a, 0, sizeof(g_a));
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

#ifndef HOST_ROCH_CONCURRENT_RUST
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

u8 rtw_roch_wk_cmd(PADAPTER padapter, int cmd, void *parm, u8 flags)
{
	(void)padapter;
	(void)parm;
	(void)flags;
	g_tr.wk_cmd = 1;
	g_tr.wk_cmd_type = cmd;
	return _TRUE;
}

void rtw_ap_roch_ch_switch_timer_process(void *ctx)
{
	PADAPTER adapter = ctx;

	adapter->wdev.switch_ch_to = 1;
	rtw_roch_wk_cmd(adapter, ROCH_AP_ROCH_CH_SWITCH_PROCESS_WK, NULL, 0);
}
#else
void rtw_init_roch_info(PADAPTER padapter);
void rtw_ap_roch_ch_switch_timer_process(void *ctx);
u8 rtw_roch_wk_cmd(PADAPTER padapter, int cmd, void *parm, u8 flags);
#endif

typedef struct {
	char name[48], op[16];
	int freq;
	int exp_min_home, exp_max_away, exp_ap_timer, exp_ro_timer;
	int exp_ch, exp_stay, exp_switch_to, exp_wk_cmd, exp_wk_type;
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
	I("exp_min_home", exp_min_home);
	I("exp_max_away", exp_max_away);
	I("exp_ap_timer", exp_ap_timer);
	I("exp_ro_timer", exp_ro_timer);
	I("exp_ch", exp_ch);
	I("exp_stay", exp_stay);
	I("exp_switch_to", exp_switch_to);
	I("exp_wk_cmd", exp_wk_cmd);
	I("exp_wk_type", exp_wk_type);
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
	vector_t v[8];
	size_t n = 0;
	int bad = 0;
	const char *p = argc > 1 ? argv[1] : "roch_concurrent_vectors.json";

	if (host_load_vectors(p, v, sizeof(v[0]), 8, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&v[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, p);
	return bad ? 1 : 0;
}
