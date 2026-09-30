// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_cmd_traffic_lps_types.h"
#include "host_vector_json.h"

static _adapter g_adapter;

enum vec_kind {
	VEC_LPS_CTRL = 0,
	VEC_WATCHDOG,
	VEC_LPS_TP,
	VEC_LPS_PKT,
};

struct vector {
	char name[64];
	char kind[32];
	int test_fn;
	int assoc;
	int tx_tp_mbits, rx_tp_mbits;
	int num_tx;
	int expect_lps_ctrl_wk;
	int lps_ctrl_type, adhoc, fw_state_adhoc_master;
	int from_timer;
	int fw_asoc, fw_sta, leisure_ps, lps_chk_by_tp, hw_port;
	int num_rx_ok, num_tx_ok, num_rx_unicast;
	int prev_busy_traffic;
	int tx_tp_kbits, rx_tp_kbits;
	int lps_bi_tp_th, lps_tx_tp_th, lps_rx_tp_th;
	int lps_chk_cnt, lps_chk_cnt_th;
	int busy_traffic_in;
	int expect_lps_enter, expect_lps_leave, expect_hw_rpt, expect_deny;
	int expect_lps_idle_count, expect_hw_joinbss_val;
	int check_lps_idle_count, check_hw_joinbss_val;
	int expect_lps_ctrl_wk_cmd;
	int expect_enter_ps;
	int expect_b_busy, expect_b_tx_busy, expect_b_rx_busy;
	int expect_b_higher_busy;
};

static enum vec_kind parse_kind(const char *k)
{
	if (!k[0] || !strcmp(k, "lps_ctrl"))
		return VEC_LPS_CTRL;
	if (!strcmp(k, "watchdog"))
		return VEC_WATCHDOG;
	if (!strcmp(k, "lps_tp"))
		return VEC_LPS_TP;
	if (!strcmp(k, "lps_pkt"))
		return VEC_LPS_PKT;
	return VEC_LPS_CTRL;
}

static int parse_vec(const char *o, size_t l, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(o, l, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(o, l, "kind", v->kind, sizeof(v->kind));
	host_json_parse_int_in(o, l, "test_fn", &v->test_fn);
	host_json_parse_int_in(o, l, "assoc", &v->assoc);
	host_json_parse_int_in(o, l, "tx_tp_mbits", &v->tx_tp_mbits);
	host_json_parse_int_in(o, l, "rx_tp_mbits", &v->rx_tp_mbits);
	host_json_parse_int_in(o, l, "num_tx", &v->num_tx);
	host_json_parse_int_in(o, l, "expect_lps_ctrl_wk", &v->expect_lps_ctrl_wk);
	host_json_parse_int_in(o, l, "lps_ctrl_type", &v->lps_ctrl_type);
	host_json_parse_int_in(o, l, "adhoc", &v->adhoc);
	host_json_parse_int_in(o, l, "fw_state_adhoc_master", &v->fw_state_adhoc_master);
	host_json_parse_int_in(o, l, "from_timer", &v->from_timer);
	host_json_parse_int_in(o, l, "fw_asoc", &v->fw_asoc);
	host_json_parse_int_in(o, l, "fw_sta", &v->fw_sta);
	host_json_parse_int_in(o, l, "leisure_ps", &v->leisure_ps);
	host_json_parse_int_in(o, l, "lps_chk_by_tp", &v->lps_chk_by_tp);
	host_json_parse_int_in(o, l, "hw_port", &v->hw_port);
	host_json_parse_int_in(o, l, "num_rx_ok", &v->num_rx_ok);
	host_json_parse_int_in(o, l, "num_tx_ok", &v->num_tx_ok);
	host_json_parse_int_in(o, l, "num_rx_unicast", &v->num_rx_unicast);
	host_json_parse_int_in(o, l, "prev_busy_traffic", &v->prev_busy_traffic);
	host_json_parse_int_in(o, l, "tx_tp_kbits", &v->tx_tp_kbits);
	host_json_parse_int_in(o, l, "rx_tp_kbits", &v->rx_tp_kbits);
	host_json_parse_int_in(o, l, "lps_bi_tp_th", &v->lps_bi_tp_th);
	host_json_parse_int_in(o, l, "lps_tx_tp_th", &v->lps_tx_tp_th);
	host_json_parse_int_in(o, l, "lps_rx_tp_th", &v->lps_rx_tp_th);
	host_json_parse_int_in(o, l, "lps_chk_cnt", &v->lps_chk_cnt);
	host_json_parse_int_in(o, l, "lps_chk_cnt_th", &v->lps_chk_cnt_th);
	host_json_parse_int_in(o, l, "busy_traffic_in", &v->busy_traffic_in);
	host_json_parse_int_in(o, l, "expect_lps_enter", &v->expect_lps_enter);
	host_json_parse_int_in(o, l, "expect_lps_leave", &v->expect_lps_leave);
	host_json_parse_int_in(o, l, "expect_hw_rpt", &v->expect_hw_rpt);
	host_json_parse_int_in(o, l, "expect_deny", &v->expect_deny);
	v->check_lps_idle_count =
		host_json_parse_int_in(o, l, "expect_lps_idle_count", &v->expect_lps_idle_count) == 0;
	v->check_hw_joinbss_val =
		host_json_parse_int_in(o, l, "expect_hw_joinbss_val", &v->expect_hw_joinbss_val) == 0;
	host_json_parse_int_in(o, l, "expect_lps_ctrl_wk_cmd", &v->expect_lps_ctrl_wk_cmd);
	host_json_parse_int_in(o, l, "expect_enter_ps", &v->expect_enter_ps);
	host_json_parse_int_in(o, l, "expect_b_busy", &v->expect_b_busy);
	host_json_parse_int_in(o, l, "expect_b_tx_busy", &v->expect_b_tx_busy);
	host_json_parse_int_in(o, l, "expect_b_rx_busy", &v->expect_b_rx_busy);
	host_json_parse_int_in(o, l, "expect_b_higher_busy", &v->expect_b_higher_busy);
	return 0;
}

static void setup_adapter(struct vector *v)
{
	struct pwrctrl_priv *pwr = adapter_to_pwrctl(&g_adapter);
	struct sta_info *sta;

	memset(&g_adapter, 0, sizeof(g_adapter));
	if (v->fw_asoc)
		g_adapter.mlmepriv.fw_state |= WIFI_ASOC_STATE;
	if (v->fw_sta)
		g_adapter.mlmepriv.fw_state |= WIFI_STATION_STATE;
	if (v->adhoc)
		g_adapter.mlmepriv.fw_state = WIFI_ADHOC_STATE;
	if (v->fw_state_adhoc_master)
		g_adapter.mlmepriv.fw_state = WIFI_ADHOC_MASTER_STATE;
	pwr->bLeisurePs = (u8)v->leisure_ps;
	pwr->lps_chk_by_tp = (u8)v->lps_chk_by_tp;
	pwr->lps_bi_tp_th = v->lps_bi_tp_th ? v->lps_bi_tp_th : 2;
	pwr->lps_tx_tp_th = v->lps_tx_tp_th ? v->lps_tx_tp_th : 2;
	pwr->lps_rx_tp_th = v->lps_rx_tp_th ? v->lps_rx_tp_th : 2;
	pwr->lps_chk_cnt = v->lps_chk_cnt;
	pwr->lps_chk_cnt_th = v->lps_chk_cnt_th ? v->lps_chk_cnt_th : 2;
	g_adapter.hw_port = (u8)v->hw_port;
	g_adapter.mlmepriv.LinkDetectInfo.NumRxOkInPeriod = (u32)v->num_rx_ok;
	g_adapter.mlmepriv.LinkDetectInfo.NumTxOkInPeriod = (u32)v->num_tx_ok;
	g_adapter.mlmepriv.LinkDetectInfo.NumRxUnicastOkInPeriod = (u32)v->num_rx_unicast;
	g_adapter.mlmepriv.LinkDetectInfo.bBusyTraffic = (u8)v->prev_busy_traffic;
	host_traffic_lps_set_sta(&g_adapter);
	sta = g_adapter.stapriv.sta;
	if (sta) {
		sta->sta_stats.tx_tp_kbits = (u32)v->tx_tp_kbits;
		sta->sta_stats.rx_tp_kbits = (u32)v->rx_tp_kbits;
	}
}

static int trace_ok(struct vector *v, struct host_traffic_lps_trace *tr)
{
	if (tr->lps_enter != v->expect_lps_enter || tr->lps_leave != v->expect_lps_leave ||
	    tr->hw_joinbss_rpt != v->expect_hw_rpt || tr->set_lps_deny != v->expect_deny)
		return 0;
	if (v->expect_lps_ctrl_wk_cmd && tr->lps_ctrl_wk_cmd != v->expect_lps_ctrl_wk_cmd)
		return 0;
	if (v->expect_lps_ctrl_wk && tr->lps_ctrl_wk_cmd != v->expect_lps_ctrl_wk)
		return 0;
	if (!v->expect_lps_ctrl_wk_cmd && !v->expect_lps_ctrl_wk && tr->lps_ctrl_wk_cmd)
		return 0;
	return 1;
}

static int run_lps_ctrl(struct vector *v)
{
	struct host_traffic_lps_trace *tr;

	setup_adapter(v);
	if (v->lps_ctrl_type == LPS_CTRL_CONNECT)
		g_adapter.pwrctrlpriv.LpsIdleCount = 7;
	lps_ctrl_wk_hdl(&g_adapter, (u8)v->lps_ctrl_type, NULL);
	tr = host_traffic_lps_get_trace();
	if (!trace_ok(v, tr))
		return 0;
	if (v->check_lps_idle_count &&
	    g_adapter.pwrctrlpriv.LpsIdleCount != (u8)v->expect_lps_idle_count)
		return 0;
	if (v->check_hw_joinbss_val && tr->hw_joinbss_val != (u8)v->expect_hw_joinbss_val)
		return 0;
	return 1;
}

static int run_watchdog(struct vector *v)
{
	struct host_traffic_lps_trace *tr;
	struct RT_LINK_DETECT_T *ld;
	u8 enter_ps;

	setup_adapter(v);
	enter_ps = traffic_status_watchdog(&g_adapter, (u8)v->from_timer);
	tr = host_traffic_lps_get_trace();
	ld = &g_adapter.mlmepriv.LinkDetectInfo;
	if (enter_ps != (u8)v->expect_enter_ps)
		return 0;
	if (ld->bBusyTraffic != (u8)v->expect_b_busy ||
	    ld->bTxBusyTraffic != (u8)v->expect_b_tx_busy ||
	    ld->bRxBusyTraffic != (u8)v->expect_b_rx_busy ||
	    ld->bHigherBusyTraffic != (u8)v->expect_b_higher_busy)
		return 0;
	if (ld->NumRxOkInPeriod || ld->NumTxOkInPeriod || ld->NumRxUnicastOkInPeriod)
		return 0;
	return trace_ok(v, tr);
}

static int run_lps_tp(struct vector *v)
{
	struct host_traffic_lps_trace *tr;
	u8 enter_ps;

	setup_adapter(v);
	enter_ps = _lps_chk_by_tp(&g_adapter, (u8)v->from_timer);
	tr = host_traffic_lps_get_trace();
	if (enter_ps != (u8)v->expect_enter_ps)
		return 0;
	return trace_ok(v, tr);
}

static int run_lps_pkt(struct vector *v)
{
	struct host_traffic_lps_trace *tr;
	u8 enter_ps;

	setup_adapter(v);
	enter_ps = _lps_chk_by_pkt_cnts(&g_adapter, (u8)v->from_timer,
					(u8)v->busy_traffic_in);
	tr = host_traffic_lps_get_trace();
	if (enter_ps != (u8)v->expect_enter_ps)
		return 0;
	return trace_ok(v, tr);
}

static void setup_watchdog_json(struct vector *v)
{
	struct pwrctrl_priv *pwr = adapter_to_pwrctl(&g_adapter);

	memset(&g_adapter, 0, sizeof(g_adapter));
	host_traffic_lps_set_sta(&g_adapter);
	pwr->bLeisurePs = 1;
	pwr->lps_chk_by_tp = (u8)v->lps_chk_by_tp;
	pwr->lps_chk_cnt = v->lps_chk_cnt;
	pwr->lps_chk_cnt_th = v->lps_chk_cnt_th ? v->lps_chk_cnt_th : 2;
	pwr->lps_bi_tp_th = pwr->lps_tx_tp_th = pwr->lps_rx_tp_th = 2;
	if (g_adapter.stapriv.sta) {
		g_adapter.stapriv.sta->sta_stats.tx_tp_kbits = (u32)v->tx_tp_mbits << 10;
		g_adapter.stapriv.sta->sta_stats.rx_tp_kbits = (u32)v->rx_tp_mbits << 10;
	}
	g_adapter.mlmepriv.LinkDetectInfo.NumTxOkInPeriod = (u32)v->num_tx;
	g_adapter.mlmepriv.LinkDetectInfo.NumRxUnicastOkInPeriod = (u32)v->num_rx_unicast;
	if (v->assoc)
		g_adapter.mlmepriv.fw_state = WIFI_ASOC_STATE | WIFI_STATION_STATE;
}

static int run_test_fn_vec(struct vector *v)
{
	struct host_traffic_lps_trace *tr;
	u8 ret;

	setup_watchdog_json(v);
	tr = host_traffic_lps_get_trace();
	switch (v->test_fn) {
	case 1:
		ret = _lps_chk_by_pkt_cnts(&g_adapter, (u8)v->from_timer, _FALSE);
		break;
	case 2:
		ret = _lps_chk_by_tp(&g_adapter, (u8)v->from_timer);
		break;
	case 3:
		ret = traffic_status_watchdog(&g_adapter, (u8)v->from_timer);
		break;
	default:
		return 0;
	}
	if ((int)ret != v->expect_enter_ps)
		return 0;
	if (!trace_ok(v, tr))
		return 0;
	if (v->expect_lps_ctrl_wk && tr->lps_ctrl_wk_cmd != v->expect_lps_ctrl_wk)
		return 0;
	if (!v->expect_lps_ctrl_wk && tr->lps_ctrl_wk_cmd)
		return 0;
	return 1;
}

static int run_vec(struct vector *v)
{
	int ok = 0;

	host_traffic_lps_reset();
	if (v->test_fn != 0) {
		ok = run_test_fn_vec(v);
		goto done;
	}
	switch (parse_kind(v->kind)) {
	case VEC_WATCHDOG:
		ok = run_watchdog(v);
		break;
	case VEC_LPS_TP:
		ok = run_lps_tp(v);
		break;
	case VEC_LPS_PKT:
		ok = run_lps_pkt(v);
		break;
	case VEC_LPS_CTRL:
	default:
		ok = run_lps_ctrl(v);
		break;
	}
done:
	if (ok) {
		printf("PASS %s\n", v->name);
		return 0;
	}
	fprintf(stderr, "FAIL %s\n", v->name);
	return -1;
}

int main(int argc, char **argv)
{
	struct vector v[24];
	size_t n = 0;
	int bad = 0;
	const char *p = argc > 1 ? argv[1] : "traffic_lps_vectors.json";

	if (host_load_vectors(p, v, sizeof(v[0]), 24, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&v[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, p);
	return bad ? 1 : 0;
}
