// SPDX-License-Identifier: GPL-2.0
#include "host_cmd_traffic_lps_types.h"

u8 _lps_chk_by_tp(_adapter *adapter, u8 from_timer)
{
	u8 enter_ps = _FALSE;
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	struct sta_priv *pstapriv = &adapter->stapriv;
	struct sta_info *psta;
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(adapter);
	u32 tx_tp_mbits, rx_tp_mbits, bi_tp_mbits;
	psta = rtw_get_stainfo(pstapriv, get_bssid(pmlmepriv));
	if (psta == NULL)
		return enter_ps;

	(void)rtw_get_bcn_cnt(adapter);
	psta->sta_stats.acc_tx_bytes = psta->sta_stats.tx_bytes;
	psta->sta_stats.acc_rx_bytes = psta->sta_stats.rx_bytes;

	tx_tp_mbits = psta->sta_stats.tx_tp_kbits >> 10;
	rx_tp_mbits = psta->sta_stats.rx_tp_kbits >> 10;
	bi_tp_mbits = tx_tp_mbits + rx_tp_mbits;

	if ((bi_tp_mbits >= (u32)pwrpriv->lps_bi_tp_th) ||
	    (tx_tp_mbits >= (u32)pwrpriv->lps_tx_tp_th) ||
	    (rx_tp_mbits >= (u32)pwrpriv->lps_rx_tp_th)) {
		enter_ps = _FALSE;
		pwrpriv->lps_chk_cnt = pwrpriv->lps_chk_cnt_th;
	} else {
		if (pwrpriv->lps_chk_cnt && --pwrpriv->lps_chk_cnt)
			enter_ps = _FALSE;
		else
			enter_ps = _TRUE;
	}

	if (enter_ps) {
		if (!from_timer)
			LPS_Enter(adapter, "TRAFFIC_IDLE");
	} else {
		if (!from_timer)
			LPS_Leave(adapter, "TRAFFIC_BUSY");
		else {
#ifdef CONFIG_CONCURRENT_MODE
#ifndef CONFIG_FW_MULTI_PORT_SUPPORT
			if (adapter->hw_port == HW_PORT0)
#endif
#endif
				rtw_lps_ctrl_wk_cmd(adapter, LPS_CTRL_TRAFFIC_BUSY, 0);
		}
	}

	return enter_ps;
}

u8 _lps_chk_by_pkt_cnts(_adapter *padapter, u8 from_timer, u8 bBusyTraffic)
{
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	u8 bEnterPS = _FALSE;

	(void)bBusyTraffic;
	if (((pmlmepriv->LinkDetectInfo.NumRxUnicastOkInPeriod +
	      pmlmepriv->LinkDetectInfo.NumTxOkInPeriod) > 8) ||
	    (pmlmepriv->LinkDetectInfo.NumRxUnicastOkInPeriod > 4))
		bEnterPS = _FALSE;
	else
		bEnterPS = _TRUE;

	if (bEnterPS) {
		if (!from_timer)
			LPS_Enter(padapter, "TRAFFIC_IDLE");
	} else {
		if (!from_timer)
			LPS_Leave(padapter, "TRAFFIC_BUSY");
		else {
#ifdef CONFIG_CONCURRENT_MODE
#ifndef CONFIG_FW_MULTI_PORT_SUPPORT
			if (padapter->hw_port == HW_PORT0)
#endif
#endif
				rtw_lps_ctrl_wk_cmd(padapter, LPS_CTRL_TRAFFIC_BUSY, 0);
		}
	}

	return bEnterPS;
}

u8 traffic_status_watchdog(_adapter *padapter, u8 from_timer)
{
	u8 bEnterPS = _FALSE;
	u16 BusyThresholdHigh = 100;
	u16 BusyThresholdLow = 75;
	u16 BusyThreshold = BusyThresholdHigh;
	u8 bBusyTraffic = _FALSE, bTxBusyTraffic = _FALSE, bRxBusyTraffic = _FALSE;
	u8 bHigherBusyTraffic = _FALSE, bHigherBusyRxTraffic = _FALSE, bHigherBusyTxTraffic = _FALSE;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);

	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE) {
		if (pmlmepriv->LinkDetectInfo.bBusyTraffic)
			BusyThreshold = BusyThresholdLow;

		if (pmlmepriv->LinkDetectInfo.NumRxOkInPeriod > BusyThreshold ||
		    pmlmepriv->LinkDetectInfo.NumTxOkInPeriod > BusyThreshold) {
			bBusyTraffic = _TRUE;

			if (pmlmepriv->LinkDetectInfo.NumRxOkInPeriod >
			    pmlmepriv->LinkDetectInfo.NumTxOkInPeriod)
				bRxBusyTraffic = _TRUE;
			else
				bTxBusyTraffic = _TRUE;
		}

		if (pmlmepriv->LinkDetectInfo.NumRxOkInPeriod > 4000 ||
		    pmlmepriv->LinkDetectInfo.NumTxOkInPeriod > 4000) {
			bHigherBusyTraffic = _TRUE;

			if (pmlmepriv->LinkDetectInfo.NumRxOkInPeriod >
			    pmlmepriv->LinkDetectInfo.NumTxOkInPeriod)
				bHigherBusyRxTraffic = _TRUE;
			else
				bHigherBusyTxTraffic = _TRUE;
		}

		if (pwrpriv->bLeisurePs && MLME_IS_STA(padapter)) {
			if (pwrpriv->lps_chk_by_tp)
				bEnterPS = _lps_chk_by_tp(padapter, from_timer);
			else
				bEnterPS = _lps_chk_by_pkt_cnts(padapter, from_timer, bBusyTraffic);
		}
	} else {
		if (!from_timer && rtw_mi_get_assoc_if_num(padapter) == 0)
			LPS_Leave(padapter, "NON_LINKED");
	}

	session_tracker_chk_cmd(padapter, NULL);

	pmlmepriv->LinkDetectInfo.NumRxOkInPeriod = 0;
	pmlmepriv->LinkDetectInfo.NumTxOkInPeriod = 0;
	pmlmepriv->LinkDetectInfo.NumRxUnicastOkInPeriod = 0;
	pmlmepriv->LinkDetectInfo.bBusyTraffic = bBusyTraffic;
	pmlmepriv->LinkDetectInfo.bTxBusyTraffic = bTxBusyTraffic;
	pmlmepriv->LinkDetectInfo.bRxBusyTraffic = bRxBusyTraffic;
	pmlmepriv->LinkDetectInfo.bHigherBusyTraffic = bHigherBusyTraffic;
	pmlmepriv->LinkDetectInfo.bHigherBusyRxTraffic = bHigherBusyRxTraffic;
	pmlmepriv->LinkDetectInfo.bHigherBusyTxTraffic = bHigherBusyTxTraffic;

	return bEnterPS;
}
