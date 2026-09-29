// SPDX-License-Identifier: GPL-2.0
#include <string.h>
#include "host_cmd_traffic_lps_types.h"

static struct host_traffic_lps_trace g_trace;
static struct sta_info g_sta;
static u8 g_bcn_cnt;

void host_traffic_lps_reset(void)
{
	g_trace = (struct host_traffic_lps_trace){0};
	g_bcn_cnt = 0;
	memset(&g_sta, 0, sizeof(g_sta));
}

struct host_traffic_lps_trace *host_traffic_lps_get_trace(void) { return &g_trace; }

void host_traffic_lps_set_sta(_adapter *a)
{
	g_sta.padapter = a;
	a->stapriv.sta = &g_sta;
}

void host_traffic_lps_set_bcn_cnt(u8 c) { g_bcn_cnt = c; }

sint check_fwstate(struct mlme_priv *m, sint s)
{
	return (!s && !m->fw_state) || (m->fw_state & (u32)s) ? _TRUE : _FALSE;
}

u8 *get_bssid(struct mlme_priv *pmlmepriv) { return pmlmepriv->assoc_bssid; }

struct sta_info *rtw_get_stainfo(struct sta_priv *pstapriv, u8 *bssid)
{
	(void)bssid;
	return pstapriv->sta;
}

u8 rtw_get_bcn_cnt(_adapter *adapter) { (void)adapter; return g_bcn_cnt; }

void rtw_lps_ctrl_wk_cmd(_adapter *adapter, u8 lps_ctrl_type, u8 flags)
{
	(void)adapter;
	(void)flags;
	g_trace.lps_ctrl_wk_cmd++;
	g_trace.last_lps_ctrl_type = lps_ctrl_type;
}

sint rtw_mi_get_assoc_if_num(_adapter *padapter)
{
	(void)padapter;
	return 0;
}

void session_tracker_chk_cmd(_adapter *padapter, void *parm)
{
	(void)padapter;
	(void)parm;
}

void LPS_Enter(_adapter *a, const char *r) { (void)a; (void)r; g_trace.lps_enter++; }
void LPS_Leave(_adapter *a, const char *r) { (void)a; (void)r; g_trace.lps_leave++; }

void rtw_hal_set_hwreg(_adapter *a, int id, u8 *val)
{
	(void)a;
	if (id == HW_VAR_H2C_FW_JOINBSSRPT && val) {
		g_trace.hw_joinbss_rpt++;
		g_trace.hw_joinbss_val = *val;
	}
}

void rtw_set_lps_deny(_adapter *a, int ms) { (void)a; (void)ms; g_trace.set_lps_deny++; }

#ifndef HOST_CMD_TRAFFIC_LPS_RUST_TEST
void lps_ctrl_wk_hdl(_adapter *padapter, u8 t, u8 *buf)
{
	struct pwrctrl_priv *pwr = adapter_to_pwrctl(padapter);
	struct mlme_priv *mlme = &padapter->mlmepriv;
	u8 mstatus;

	if (check_fwstate(mlme, WIFI_ADHOC_MASTER_STATE) == _TRUE ||
	    check_fwstate(mlme, WIFI_ADHOC_STATE) == _TRUE)
		return;
	switch (t) {
	case LPS_CTRL_CONNECT:
		mstatus = 1;
		pwr->LpsIdleCount = 0;
		rtw_hal_set_hwreg(padapter, HW_VAR_H2C_FW_JOINBSSRPT, &mstatus);
		break;
	case LPS_CTRL_SPECIAL_PACKET:
		rtw_set_lps_deny(padapter, LPS_DELAY_MS);
		LPS_Leave(padapter, "SPECIAL");
		break;
	case LPS_CTRL_LEAVE:
		LPS_Leave(padapter, "LEAVE");
		break;
	case LPS_CTRL_ENTER:
		/* Reason strings are not part of the L2 contract until PR3 wires production/Rust oracle. */
		LPS_Enter(padapter, "ENTER");
		break;
	default:
		(void)buf;
		break;
	}
}
#endif /* HOST_CMD_TRAFFIC_LPS_RUST_TEST */
