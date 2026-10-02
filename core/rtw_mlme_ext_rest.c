/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#define _RTW_MLME_EXT_REST_C_

#if defined(HOST_MLME_EXT_MGNT_ATTRIB_TEST)
#include "host_mlme_ext_mgnt_attrib_types.h"
#undef rtw_warn_on
extern int rtw_warn_on(int cond);
#elif defined(HOST_MLME_EXT_PEER_ALIVE_TEST)
#include "host_mlme_ext_peer_alive_types.h"
#elif defined(HOST_MLME_EXT_SCAN_TEST)
#include "host_mlme_ext_scan_types.h"
#elif defined(HOST_MLME_EXT_BAND_IE_TEST)
#include "host_mlme_ext_band_ie_types.h"
#elif defined(HOST_MLME_EXT_JOIN_CMD_TEST)
#include "host_mlme_ext_join_cmd_types.h"
#elif defined(HOST_MLME_EXT_TEST)
#include "host_mlme_ext_types.h"
#undef rtw_warn_on
extern int rtw_warn_on(int cond);
#else
#include <drv_types.h>
#include <hal_data.h>
#endif

#if (!defined(CONFIG_RUST) || defined(HOST_MLME_EXT_TEST) || \
      !defined(CONFIG_RUST_MLME_EXT_REST)) && \
     !defined(HOST_MLME_EXT_MGNT_ATTRIB_TEST) && \
     !defined(HOST_MLME_EXT_PEER_ALIVE_TEST) && \
     !defined(HOST_MLME_EXT_SCAN_TEST) && \
     !defined(HOST_MLME_EXT_BAND_IE_TEST) && \
     !defined(HOST_MLME_EXT_JOIN_CMD_TEST)

#ifdef CONFIG_DFS_MASTER
bool rtw_chset_is_chbw_non_ocp(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw, u8 offset)
{
	bool ret = _FALSE;
	u32 hi = 0, lo = 0;
	int i;

	if (rtw_chbw_to_freq_range(ch, bw, offset, &hi, &lo) == _FALSE)
		goto exit;

	for (i = 0; i < MAX_CHANNEL_NUM && ch_set[i].ChannelNum != 0; i++) {
		if (!rtw_ch2freq(ch_set[i].ChannelNum)) {
			rtw_warn_on(1);
			continue;
		}

		if (!CH_IS_NON_OCP(&ch_set[i]))
			continue;

		if (lo <= (u32)rtw_ch2freq(ch_set[i].ChannelNum)
		    && (u32)rtw_ch2freq(ch_set[i].ChannelNum) <= hi) {
			ret = _TRUE;
			break;
		}
	}

exit:
	return ret;
}

bool rtw_chset_is_ch_non_ocp(RT_CHANNEL_INFO *ch_set, u8 ch)
{
	return rtw_chset_is_chbw_non_ocp(ch_set, ch, CHANNEL_WIDTH_20,
					 HAL_PRIME_CHNL_OFFSET_DONT_CARE);
}

u32 rtw_chset_get_ch_non_ocp_ms(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw, u8 offset)
{
	int ms = 0;
	systime current_time;
	u32 hi = 0, lo = 0;
	int i;

	if (rtw_chbw_to_freq_range(ch, bw, offset, &hi, &lo) == _FALSE)
		goto exit;

	current_time = rtw_get_current_time();

	for (i = 0; i < MAX_CHANNEL_NUM && ch_set[i].ChannelNum != 0; i++) {
		if (!rtw_ch2freq(ch_set[i].ChannelNum)) {
			rtw_warn_on(1);
			continue;
		}

		if (!CH_IS_NON_OCP(&ch_set[i]))
			continue;

		if (lo <= (u32)rtw_ch2freq(ch_set[i].ChannelNum)
		    && (u32)rtw_ch2freq(ch_set[i].ChannelNum) <= hi) {
			if ((int)rtw_systime_to_ms(ch_set[i].non_ocp_end_time -
						   current_time) > ms)
				ms = rtw_systime_to_ms(ch_set[i].non_ocp_end_time -
						       current_time);
		}
	}

exit:
	return ms;
}

static bool _rtw_chset_update_non_ocp(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw,
				      u8 offset, int ms)
{
	u32 hi = 0, lo = 0;
	int i;
	bool updated = 0;

	if (rtw_chbw_to_freq_range(ch, bw, offset, &hi, &lo) == _FALSE)
		goto exit;

	for (i = 0; i < MAX_CHANNEL_NUM && ch_set[i].ChannelNum != 0; i++) {
		if (!rtw_ch2freq(ch_set[i].ChannelNum)) {
			rtw_warn_on(1);
			continue;
		}

		if (lo <= (u32)rtw_ch2freq(ch_set[i].ChannelNum)
		    && (u32)rtw_ch2freq(ch_set[i].ChannelNum) <= hi) {
			if (ms >= 0)
				ch_set[i].non_ocp_end_time =
					rtw_get_current_time() +
					rtw_ms_to_systime(ms);
			else
				ch_set[i].non_ocp_end_time =
					rtw_get_current_time() +
					rtw_ms_to_systime(NON_OCP_TIME_MS);
			ch_set[i].flags |= RTW_CHF_NON_OCP;
			updated = 1;
		}
	}

exit:
	return updated;
}

bool rtw_chset_update_non_ocp(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw, u8 offset)
{
	return _rtw_chset_update_non_ocp(ch_set, ch, bw, offset, -1);
}

bool rtw_chset_update_non_ocp_ms(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw,
				 u8 offset, int ms)
{
	return _rtw_chset_update_non_ocp(ch_set, ch, bw, offset, ms);
}
#endif /* CONFIG_DFS_MASTER */

int rtw_chset_search_ch(RT_CHANNEL_INFO *ch_set, const u32 ch)
{
	int i;

	if (ch == 0)
		return -1;

	for (i = 0; i < MAX_CHANNEL_NUM && ch_set[i].ChannelNum != 0; i++) {
		if (ch == ch_set[i].ChannelNum)
			return i;
	}

	return -1;
}

u8 rtw_chset_is_chbw_valid(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw, u8 offset,
			   bool allow_primary_passive, bool allow_passive)
{
	u8 cch;
	u8 *op_chs;
	u8 op_ch_num;
	u8 valid = 0;
	int i;
	int ch_idx;

	cch = rtw_get_center_ch(ch, bw, offset);

	if (!rtw_get_op_chs_by_cch_bw(cch, bw, &op_chs, &op_ch_num))
		goto exit;

	for (i = 0; i < op_ch_num; i++) {
		ch_idx = rtw_chset_search_ch(ch_set, *(op_chs + i));
		if (ch_idx == -1)
			break;
		if (ch_set[ch_idx].flags & RTW_CHF_NO_IR) {
			if ((!allow_primary_passive &&
			     ch_set[ch_idx].ChannelNum == ch) ||
			    (!allow_passive &&
			     ch_set[ch_idx].ChannelNum != ch))
				break;
		}
		if (bw >= CHANNEL_WIDTH_40) {
			if ((ch_set[ch_idx].flags & RTW_CHF_NO_HT40U) &&
			    i % 2 == 0)
				break;
			if ((ch_set[ch_idx].flags & RTW_CHF_NO_HT40L) &&
			    i % 2 == 1)
				break;
		}
		if (bw >= CHANNEL_WIDTH_80 &&
		    (ch_set[ch_idx].flags & RTW_CHF_NO_80MHZ))
			break;
		if (bw >= CHANNEL_WIDTH_160 &&
		    (ch_set[ch_idx].flags & RTW_CHF_NO_160MHZ))
			break;
	}

	if (op_ch_num != 0 && i == op_ch_num)
		valid = 1;

exit:
	return valid;
}

void rtw_chset_sync_chbw(RT_CHANNEL_INFO *ch_set, u8 *req_ch, u8 *req_bw,
			 u8 *req_offset, u8 *g_ch, u8 *g_bw, u8 *g_offset,
			 bool allow_primary_passive, bool allow_passive)
{
	u8 r_ch, r_bw, r_offset;
	u8 u_ch, u_bw, u_offset;
	u8 cur_bw = *req_bw;

	while (1) {
		r_ch = *req_ch;
		r_bw = cur_bw;
		r_offset = *req_offset;
		u_ch = *g_ch;
		u_bw = *g_bw;
		u_offset = *g_offset;

		rtw_sync_chbw(&r_ch, &r_bw, &r_offset, &u_ch, &u_bw, &u_offset);

		if (rtw_chset_is_chbw_valid(ch_set, r_ch, r_bw, r_offset,
					    allow_primary_passive,
					    allow_passive))
			break;
		if (cur_bw == CHANNEL_WIDTH_20) {
			rtw_warn_on(1);
			break;
		}
		cur_bw--;
	}

	*req_ch = r_ch;
	*req_bw = r_bw;
	*req_offset = r_offset;
	*g_ch = u_ch;
	*g_bw = u_bw;
	*g_offset = u_offset;
}

#endif /* (!CONFIG_RUST || HOST_MLME_EXT_TEST || !CONFIG_RUST_MLME_EXT_REST) && !HOST_MLME_EXT_MGNT_ATTRIB_TEST */

#if defined(HOST_MLME_EXT_MGNT_ATTRIB_TEST) || \
	(((!defined(CONFIG_RUST) || !defined(CONFIG_RUST_MLME_EXT_MGNT_ATTRIB)) && \
	  !defined(HOST_MLME_EXT_TEST) && !defined(HOST_MLME_EXT_PEER_ALIVE_TEST) && \
	  !defined(HOST_MLME_EXT_SCAN_TEST) && !defined(HOST_MLME_EXT_BAND_IE_TEST) && \
	  !defined(HOST_MLME_EXT_JOIN_CMD_TEST)))

void update_monitor_frame_attrib(_adapter *padapter, struct pkt_attrib *pattrib)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	u8	wireless_mode;
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	struct xmit_priv		*pxmitpriv = &padapter->xmitpriv;
	struct sta_info		*psta = NULL;
	struct sta_priv		*pstapriv = &padapter->stapriv;

	psta = rtw_get_stainfo(pstapriv, pattrib->ra);
	(void)psta;

	pattrib->hdrlen = 24;
	pattrib->nr_frags = 1;
	pattrib->priority = 7;
	pattrib->mac_id = RTW_DEFAULT_MGMT_MACID;
	pattrib->qsel = QSLT_MGNT;

	pattrib->pktlen = 0;

	if (pmlmeext->tx_rate == IEEE80211_CCK_RATE_1MB)
		wireless_mode = WIRELESS_11B;
	else
		wireless_mode = WIRELESS_11G;

	pattrib->raid = rtw_get_mgntframe_raid(padapter, wireless_mode);
#ifdef CONFIG_80211AC_VHT
	if (pHalData->rf_type == RF_1T1R)
		pattrib->raid = RATEID_IDX_VHT_1SS;
	else if (pHalData->rf_type == RF_2T2R || pHalData->rf_type == RF_2T4R)
		pattrib->raid = RATEID_IDX_VHT_2SS;
	else if (pHalData->rf_type == RF_3T3R)
		pattrib->raid = RATEID_IDX_VHT_3SS;
	else
		pattrib->raid = RATEID_IDX_BGN_40M_1SS;
#endif

#ifdef CONFIG_80211AC_VHT
	pattrib->rate = MGN_VHT1SS_MCS9;
#else
	pattrib->rate = MGN_MCS7;
#endif

	pattrib->encrypt = _NO_PRIVACY_;
	pattrib->bswenc = _FALSE;

	pattrib->qos_en = _FALSE;
	pattrib->ht_en = 1;
	pattrib->bwmode = CHANNEL_WIDTH_20;
	pattrib->ch_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
	pattrib->sgi = _FALSE;

	pattrib->seqnum = pmlmeext->mgnt_seq;

	pattrib->retry_ctrl = _TRUE;

	pattrib->mbssid = 0;
	pattrib->hw_ssn_sel = pxmitpriv->hw_ssn_seq_no;

}

#ifdef CONFIG_RTW_MGMT_QUEUE
void update_mgntframe_subtype(_adapter *padapter, struct xmit_frame *pmgntframe)
{
	struct pkt_attrib *pattrib = &pmgntframe->attrib;
	u8 *pframe;
	u8 subtype, category ,action;

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	subtype = get_frame_sub_type(pframe); /* bit(7)~bit(2) */
	pattrib->subtype = subtype;

	rtw_action_frame_parse(pframe, pattrib->pktlen, &category, &action);

	if ((subtype == WIFI_ACTION && !(action == ACT_PUBLIC_FTM_REQ || action == ACT_PUBLIC_FTM)) ||
		subtype == WIFI_DISASSOC || subtype == WIFI_DEAUTH ||
		(subtype == WIFI_PROBERSP && MLME_IS_ADHOC(padapter)))
		pattrib->ps_dontq = 0;
	else
		pattrib->ps_dontq = 1;
}
#endif

void update_mgntframe_attrib(_adapter *padapter, struct pkt_attrib *pattrib)
{
	u8	wireless_mode;
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	struct xmit_priv		*pxmitpriv = &padapter->xmitpriv;
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct sta_info *psta;

#ifdef CONFIG_P2P_PS_NOA_USE_MACID_SLEEP
	struct wifidirect_info *pwdinfo = &(padapter->wdinfo);
#endif /* CONFIG_P2P_PS_NOA_USE_MACID_SLEEP */

	/* _rtw_memset((u8 *)(pattrib), 0, sizeof(struct pkt_attrib)); */

	pattrib->hdrlen = 24;
	pattrib->nr_frags = 1;
	pattrib->priority = 7;
	pattrib->mac_id = RTW_DEFAULT_MGMT_MACID;
	pattrib->qsel = QSLT_MGNT;

#ifdef CONFIG_MCC_MODE
	update_mcc_mgntframe_attrib(padapter, pattrib);
#endif


#ifdef CONFIG_P2P_PS_NOA_USE_MACID_SLEEP
#ifdef CONFIG_CONCURRENT_MODE
	if (rtw_mi_buddy_check_fwstate(padapter, WIFI_ASOC_STATE))
#endif /* CONFIG_CONCURRENT_MODE */
		if (MLME_IS_GC(padapter)) {
			if (pwdinfo->p2p_ps_mode > P2P_PS_NONE) {
				struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
				struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
				WLAN_BSSID_EX *cur_network = &(pmlmeinfo->network);

				psta = rtw_get_stainfo(pstapriv, cur_network->MacAddress);
				if (psta) {
					/* use macid sleep during NoA, mgmt frame use ac queue & ap macid */
					pattrib->mac_id = psta->cmn.mac_id;
					pattrib->qsel = QSLT_VO;
				} else {
					if (pwdinfo->p2p_ps_state != P2P_PS_DISABLE)
						RTW_ERR("%s , psta was NULL\n", __func__);
				}
			}
		}
#endif /* CONFIG_P2P_PS_NOA_USE_MACID_SLEEP */

	pattrib->pktlen = 0;

	if (IS_CCK_RATE(pmlmeext->tx_rate))
		wireless_mode = WIRELESS_11B;
	else
		wireless_mode = WIRELESS_11G;
	pattrib->raid =  rtw_get_mgntframe_raid(padapter, wireless_mode);
	pattrib->rate = pmlmeext->tx_rate;

	pattrib->encrypt = _NO_PRIVACY_;
	pattrib->bswenc = _FALSE;

	pattrib->qos_en = _FALSE;
	pattrib->ht_en = _FALSE;
	pattrib->bwmode = CHANNEL_WIDTH_20;
	pattrib->ch_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
	pattrib->sgi = _FALSE;

	pattrib->seqnum = pmlmeext->mgnt_seq;

	pattrib->retry_ctrl = _TRUE;

	pattrib->mbssid = 0;
	pattrib->hw_ssn_sel = pxmitpriv->hw_ssn_seq_no;
#ifdef CONFIG_RTW_MGMT_QUEUE
	pattrib->ps_dontq = 1;
#endif
}

void update_mgntframe_attrib_addr(_adapter *padapter, struct xmit_frame *pmgntframe)
{
	u8 *pframe;
	struct pkt_attrib *pattrib = &pmgntframe->attrib;
#if defined(CONFIG_BEAMFORMING) || defined(CONFIG_ANTENNA_DIVERSITY) || defined(CONFIG_RTW_MGMT_QUEUE)
	struct sta_info *sta = NULL;
#endif

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;

	_rtw_memcpy(pattrib->ra, GetAddr1Ptr(pframe), ETH_ALEN);
	_rtw_memcpy(pattrib->ta, get_addr2_ptr(pframe), ETH_ALEN);

#if defined(CONFIG_BEAMFORMING) || defined(CONFIG_ANTENNA_DIVERSITY) || defined(CONFIG_RTW_MGMT_QUEUE)
	sta = pattrib->psta;
	if (!sta) {
		sta = rtw_get_stainfo(&padapter->stapriv, pattrib->ra);
		pattrib->psta = sta;
	}

	#ifdef CONFIG_BEAMFORMING
	if (sta)
		update_attrib_txbf_info(padapter, pattrib, sta);
	#endif
#endif /* defined(CONFIG_BEAMFORMING) || defined(CONFIG_ANTENNA_DIVERSITY) || defined(CONFIG_RTW_MGMT_QUEUE) */
}

#endif /* HOST_MLME_EXT_MGNT_ATTRIB_TEST || ((!CONFIG_RUST || !CONFIG_RUST_MLME_EXT_MGNT_ATTRIB) && !HOST_MLME_EXT_TEST) */

#if defined(HOST_MLME_EXT_PEER_ALIVE_TEST) || \
	(((!defined(CONFIG_RUST) || !defined(CONFIG_RUST_MLME_EXT_PEER_ALIVE)) && \
	  !defined(HOST_MLME_EXT_TEST) && !defined(HOST_MLME_EXT_MGNT_ATTRIB_TEST) && \
	  !defined(HOST_MLME_EXT_SCAN_TEST) && !defined(HOST_MLME_EXT_BAND_IE_TEST) && \
	  !defined(HOST_MLME_EXT_JOIN_CMD_TEST)))

/********************************************************************

When station does not receive any packet in MAX_CONTINUAL_NORXPACKET_COUNT*2 seconds,
recipient station will teardown the block ack by issuing DELBA frame.

*********************************************************************/
void rtw_delba_check(_adapter *padapter, struct sta_info *psta, u8 from_timer)
{
	int i = 0;
	int ret = _SUCCESS;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);

	/*
		IOT issue,occur Broadcom ap(Buffalo WZR-D1800H,Netgear R6300).
		AP is originator.AP does not transmit unicast packets when STA response its BAR.
		This case probably occur ap issue BAR after AP builds BA.

		Follow 802.11 spec, STA shall maintain an inactivity timer for every negotiated Block Ack setup.
		The inactivity timer is not reset when MPDUs corresponding to other TIDs are received.
	*/
	if (pmlmeinfo->assoc_AP_vendor == HT_IOT_PEER_BROADCOM) {
		for (i = 0; i < TID_NUM; i++) {
			if ((psta->recvreorder_ctrl[i].enable) &&
			    (sta_rx_data_qos_pkts(psta, i) ==
			     sta_last_rx_data_qos_pkts(psta, i))) {
				if (_TRUE ==
				    rtw_inc_and_chk_continual_no_rx_packet(psta,
									   i)) {
					/* send a DELBA frame to the peer STA with the Reason Code field set to TIMEOUT */
					if (!from_timer)
						ret = issue_del_ba_ex(
							padapter,
							psta->cmn.mac_addr, i,
							39, 0, 3, 1);
					else
						issue_del_ba(
							padapter,
							psta->cmn.mac_addr, i,
							39, 0);
					psta->recvreorder_ctrl[i].enable =
						_FALSE;
					if (ret != _FAIL)
						psta->recvreorder_ctrl[i]
							.ampdu_size =
							RX_AMPDU_SIZE_INVALID;
					rtw_reset_continual_no_rx_packet(psta,
									 i);
				}
			} else {
				/* The inactivity timer is reset when MPDUs to the TID is received. */
				rtw_reset_continual_no_rx_packet(psta, i);
			}
		}
	}
}

u8 chk_ap_is_alive(_adapter *padapter, struct sta_info *psta)
{
	u8 ret = _FALSE;
#ifdef DBG_EXPIRATION_CHK
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	RTW_INFO(FUNC_ADPT_FMT" rx:"STA_PKTS_FMT", beacon:%llu, probersp_to_self:%llu"
		/*", probersp_bm:%llu, probersp_uo:%llu, probereq:%llu, BI:%u"*/
		 ", retry:%u\n"
		 , FUNC_ADPT_ARG(padapter)
		 , STA_RX_PKTS_DIFF_ARG(psta)
		, psta->sta_stats.rx_beacon_pkts - psta->sta_stats.last_rx_beacon_pkts
		, psta->sta_stats.rx_probersp_pkts - psta->sta_stats.last_rx_probersp_pkts
		/*, psta->sta_stats.rx_probersp_bm_pkts - psta->sta_stats.last_rx_probersp_bm_pkts
		, psta->sta_stats.rx_probersp_uo_pkts - psta->sta_stats.last_rx_probersp_uo_pkts
		, psta->sta_stats.rx_probereq_pkts - psta->sta_stats.last_rx_probereq_pkts
		 , pmlmeinfo->bcn_interval*/
		 , pmlmeext->retry
		);

	RTW_INFO(FUNC_ADPT_FMT" tx_pkts:%llu, link_count:%u\n", FUNC_ADPT_ARG(padapter)
		 , sta_tx_pkts(psta)
		 , pmlmeinfo->link_count
		);
#endif

	if ((sta_rx_data_pkts(psta) == sta_last_rx_data_pkts(psta)) &&
	    sta_rx_beacon_pkts(psta) == sta_last_rx_beacon_pkts(psta) &&
	    sta_rx_probersp_pkts(psta) == sta_last_rx_probersp_pkts(psta))
		ret = _FALSE;
	else
		ret = _TRUE;

	sta_update_last_rx_pkts(psta);

	return ret;
}

u8 chk_adhoc_peer_is_alive(struct sta_info *psta)
{
	u8 ret = _TRUE;

#ifdef DBG_EXPIRATION_CHK
	RTW_INFO("sta:"MAC_FMT", rssi:%d, rx:"STA_PKTS_FMT", beacon:%llu, probersp_to_self:%llu"
		/*", probersp_bm:%llu, probersp_uo:%llu, probereq:%llu, BI:%u"*/
		 ", expire_to:%u\n"
		 , MAC_ARG(psta->cmn.mac_addr)
		 , psta->cmn.rssi_stat.rssi
		 , STA_RX_PKTS_DIFF_ARG(psta)
		, psta->sta_stats.rx_beacon_pkts - psta->sta_stats.last_rx_beacon_pkts
		, psta->sta_stats.rx_probersp_pkts - psta->sta_stats.last_rx_probersp_pkts
		/*, psta->sta_stats.rx_probersp_bm_pkts - psta->sta_stats.last_rx_probersp_bm_pkts
		, psta->sta_stats.rx_probersp_uo_pkts - psta->sta_stats.last_rx_probersp_uo_pkts
		, psta->sta_stats.rx_probereq_pkts - psta->sta_stats.last_rx_probereq_pkts
		 , pmlmeinfo->bcn_interval*/
		 , psta->expire_to
		);
#endif

	if (sta_rx_data_pkts(psta) == sta_last_rx_data_pkts(psta) &&
	    sta_rx_beacon_pkts(psta) == sta_last_rx_beacon_pkts(psta) &&
	    sta_rx_probersp_pkts(psta) == sta_last_rx_probersp_pkts(psta))
		ret = _FALSE;

	sta_update_last_rx_pkts(psta);

	return ret;
}

#ifdef CONFIG_TDLS
u8 chk_tdls_peer_sta_is_alive(_adapter *padapter, struct sta_info *psta)
{
	(void)padapter;

	if ((psta->sta_stats.rx_data_pkts ==
	     psta->sta_stats.last_rx_data_pkts) &&
	    (psta->sta_stats.rx_tdls_disc_rsp_pkts ==
	     psta->sta_stats.last_rx_tdls_disc_rsp_pkts))
		return _FALSE;

	return _TRUE;
}
#endif /* CONFIG_TDLS */

#endif /* HOST_MLME_EXT_PEER_ALIVE_TEST || ((!CONFIG_RUST || !CONFIG_RUST_MLME_EXT_PEER_ALIVE) && !HOST_MLME_EXT_TEST) */

#if defined(CONFIG_RUST) && defined(CONFIG_RUST_MLME_EXT_PEER_ALIVE)
#include <drv_types.h>

u8 rtw_rust_peer_assoc_ap_vendor(_adapter *padapter)
{
	return padapter->mlmeextpriv.mlmext_info.assoc_AP_vendor;
}

u8 *rtw_rust_peer_sta_mac(struct sta_info *psta)
{
	return psta->cmn.mac_addr;
}

u8 rtw_rust_peer_reorder_enable(struct sta_info *psta, int tid)
{
	return psta->recvreorder_ctrl[tid].enable;
}

void rtw_rust_peer_reorder_disable(struct sta_info *psta, int tid)
{
	psta->recvreorder_ctrl[tid].enable = _FALSE;
}

void rtw_rust_peer_reorder_invalidate_ampdu(struct sta_info *psta, int tid)
{
	psta->recvreorder_ctrl[tid].ampdu_size = RX_AMPDU_SIZE_INVALID;
}

u64 rtw_rust_peer_rx_qos(struct sta_info *psta, int tid)
{
	return sta_rx_data_qos_pkts(psta, tid);
}

u64 rtw_rust_peer_last_rx_qos(struct sta_info *psta, int tid)
{
	return sta_last_rx_data_qos_pkts(psta, tid);
}

u64 rtw_rust_peer_rx_data(struct sta_info *psta)
{
	return sta_rx_data_pkts(psta);
}

u64 rtw_rust_peer_last_rx_data(struct sta_info *psta)
{
	return sta_last_rx_data_pkts(psta);
}

u64 rtw_rust_peer_rx_beacon(struct sta_info *psta)
{
	return sta_rx_beacon_pkts(psta);
}

u64 rtw_rust_peer_last_rx_beacon(struct sta_info *psta)
{
	return sta_last_rx_beacon_pkts(psta);
}

u64 rtw_rust_peer_rx_probersp(struct sta_info *psta)
{
	return sta_rx_probersp_pkts(psta);
}

u64 rtw_rust_peer_last_rx_probersp(struct sta_info *psta)
{
	return sta_last_rx_probersp_pkts(psta);
}

void rtw_rust_peer_sta_update_last_rx(struct sta_info *psta)
{
	sta_update_last_rx_pkts(psta);
}

#ifdef CONFIG_TDLS
u64 rtw_rust_peer_rx_tdls_disc(struct sta_info *psta)
{
	return psta->sta_stats.rx_tdls_disc_rsp_pkts;
}

u64 rtw_rust_peer_last_rx_tdls_disc(struct sta_info *psta)
{
	return psta->sta_stats.last_rx_tdls_disc_rsp_pkts;
}
#endif /* CONFIG_TDLS */
#endif /* CONFIG_RUST && CONFIG_RUST_MLME_EXT_PEER_ALIVE */

#if defined(CONFIG_RUST) && defined(CONFIG_RUST_MLME_EXT_SCAN)
#include <drv_types.h>

systime rtw_rust_scan_last_scan_time(_adapter *adapter)
{
	return adapter->mlmeextpriv.last_scan_time;
}

void rtw_rust_scan_set_last_scan_time(_adapter *adapter, systime t)
{
	adapter->mlmeextpriv.last_scan_time = t;
}

u32 rtw_rust_scan_wireless_mode(_adapter *adapter)
{
	return adapter->registrypriv.wireless_mode;
}

u16 rtw_rust_scan_ch_ms(_adapter *adapter)
{
	return adapter->mlmeextpriv.sitesurvey_res.scan_ch_ms;
}

u16 rtw_rust_scan_duration(_adapter *adapter)
{
	return adapter->mlmeextpriv.sitesurvey_res.duration;
}

u8 rtw_rust_scan_cnt_max(_adapter *adapter)
{
	return adapter->mlmeextpriv.sitesurvey_res.scan_cnt_max;
}

u16 rtw_rust_scan_backop_ms(_adapter *adapter)
{
	return adapter->mlmeextpriv.sitesurvey_res.backop_ms;
}

void rtw_rust_scan_set_timeout_ms(_adapter *adapter, u32 ms)
{
	adapter->mlmeextpriv.sitesurvey_res.scan_timeout_ms = ms;
}

u8 rtw_rust_scan_backop_flags_sta(_adapter *adapter)
{
	return mlmeext_scan_backop_flags_sta(&adapter->mlmeextpriv);
}

u8 rtw_rust_scan_backop_flags_ap(_adapter *adapter)
{
	return mlmeext_scan_backop_flags_ap(&adapter->mlmeextpriv);
}

/*
 * Kernel `struct mi_state` is not the 6-byte host stub: `lg_sta_num` sits
 * between `ld_sta_num` and the AP fields, and `rtw_mi_status` memsets
 * `sizeof(struct mi_state)` (~16+ bytes). Hand the counts out so Rust never
 * overlays the stub.
 */
void rtw_rust_scan_mi_counts(_adapter *adapter,
			     u8 *sta_num, u8 *ld_sta_num,
			     u8 *ap_num, u8 *ld_ap_num,
			     u8 *mesh_num, u8 *ld_mesh_num)
{
	struct mi_state mstate;

	rtw_mi_status(adapter, &mstate);
	if (sta_num)
		*sta_num = MSTATE_STA_NUM(&mstate);
	if (ld_sta_num)
		*ld_sta_num = MSTATE_STA_LD_NUM(&mstate);
	if (ap_num)
		*ap_num = MSTATE_AP_NUM(&mstate);
	if (ld_ap_num)
		*ld_ap_num = MSTATE_AP_LD_NUM(&mstate);
	if (mesh_num)
		*mesh_num = MSTATE_MESH_NUM(&mstate);
	if (ld_mesh_num)
		*ld_mesh_num = MSTATE_MESH_LD_NUM(&mstate);
}

#ifdef CONFIG_RTW_MESH
u8 rtw_rust_scan_backop_flags_mesh(_adapter *adapter)
{
	return mlmeext_scan_backop_flags_mesh(&adapter->mlmeextpriv);
}
#endif /* CONFIG_RTW_MESH */

u16 rtw_rust_scan_acs_adv_ms(_adapter *adapter)
{
#if defined(CONFIG_RTW_ACS) && defined(CONFIG_RTW_ACS_DBG)
	if (IS_ACS_ENABLE(adapter) && rtw_is_acs_st_valid(adapter))
		return rtw_acs_get_adv_st(adapter);
#endif
	return 0;
}
#endif /* CONFIG_RUST && CONFIG_RUST_MLME_EXT_SCAN */

#if defined(CONFIG_RUST) && defined(CONFIG_RUST_MLME_EXT_PICK_CH)
#include <drv_types.h>

u8 rtw_rust_pick_ch_scan_abort(_adapter *adapter)
{
	return adapter->mlmeextpriv.scan_abort;
}

void rtw_rust_pick_ch_set_channel_idx(_adapter *adapter, int idx)
{
	adapter->mlmeextpriv.sitesurvey_res.channel_idx = idx;
}

int rtw_rust_pick_ch_channel_idx(_adapter *adapter)
{
	return adapter->mlmeextpriv.sitesurvey_res.channel_idx;
}

u8 rtw_rust_pick_ch_force_ssid_scan(_adapter *adapter)
{
	return adapter->mlmeextpriv.sitesurvey_res.force_ssid_scan;
}

void rtw_rust_pick_ch_set_force_ssid_scan(_adapter *adapter, u8 v)
{
	adapter->mlmeextpriv.sitesurvey_res.force_ssid_scan = v;
}

u8 rtw_rust_pick_ch_ssid_num(_adapter *adapter)
{
	return adapter->mlmeextpriv.sitesurvey_res.ssid_num;
}

u8 rtw_rust_pick_ch_ch_num(_adapter *adapter)
{
	return adapter->mlmeextpriv.sitesurvey_res.ch_num;
}

u16 rtw_rust_pick_ch_ch_hw_value(_adapter *adapter, int idx)
{
	return adapter->mlmeextpriv.sitesurvey_res.ch[idx].hw_value;
}

u32 rtw_rust_pick_ch_ch_flags(_adapter *adapter, int idx)
{
	return adapter->mlmeextpriv.sitesurvey_res.ch[idx].flags;
}

u8 rtw_rust_pick_ch_scan_cnt(_adapter *adapter)
{
	return adapter->mlmeextpriv.sitesurvey_res.scan_cnt;
}

void rtw_rust_pick_ch_set_scan_cnt(_adapter *adapter, u8 v)
{
	adapter->mlmeextpriv.sitesurvey_res.scan_cnt = v;
}

u8 rtw_rust_pick_ch_scan_cnt_max(_adapter *adapter)
{
	return adapter->mlmeextpriv.sitesurvey_res.scan_cnt_max;
}

void rtw_rust_pick_ch_set_backop_flags(_adapter *adapter, u8 v)
{
	adapter->mlmeextpriv.sitesurvey_res.backop_flags = v;
}

RT_CHANNEL_INFO *rtw_rust_pick_ch_channel_set(_adapter *adapter)
{
	return adapter_to_rfctl(adapter)->channel_set;
}

u8 rtw_rust_pick_ch_chset_flags(_adapter *adapter, int idx)
{
	return adapter_to_rfctl(adapter)->channel_set[idx].flags;
}

u8 rtw_rust_pick_ch_hidden_bss_cnt(_adapter *adapter, int idx)
{
	return adapter_to_rfctl(adapter)->channel_set[idx].hidden_bss_cnt;
}

systime rtw_rust_pick_ch_non_ocp_end_time(_adapter *adapter, int idx)
{
	return adapter_to_rfctl(adapter)->channel_set[idx].non_ocp_end_time;
}

u8 rtw_rust_pick_ch_dfs_slave_with_rd(_adapter *adapter)
{
	return IS_DFS_SLAVE_WITH_RD(adapter_to_rfctl(adapter));
}

struct rf_ctl_t *rtw_rust_pick_ch_rfctl(_adapter *adapter)
{
	return adapter_to_rfctl(adapter);
}

#ifdef CONFIG_P2P
u8 rtw_rust_pick_ch_p2p_state_not_none(_adapter *adapter)
{
	return !rtw_p2p_chk_state(&adapter->wdinfo, P2P_STATE_NONE);
}

void rtw_rust_pick_ch_p2p_findphase_ex_max(_adapter *adapter)
{
	rtw_p2p_findphase_ex_set(&adapter->wdinfo, P2P_FINDPHASE_EX_MAX);
}
#endif /* CONFIG_P2P */

#ifdef CONFIG_P2P
u8 rtw_rust_pick_ch_rx_scan_op_ch_only(_adapter *adapter)
{
	return adapter->wdinfo.rx_invitereq_info.scan_op_ch_only;
}

u8 rtw_rust_pick_ch_p2p_scan_op_ch_only(_adapter *adapter)
{
	return adapter->wdinfo.p2p_info.scan_op_ch_only;
}

u8 rtw_rust_pick_ch_rx_op_ch(_adapter *adapter, int idx)
{
	return adapter->wdinfo.rx_invitereq_info.operation_ch[idx];
}

u8 rtw_rust_pick_ch_p2p_op_ch(_adapter *adapter, int idx)
{
	return adapter->wdinfo.p2p_info.operation_ch[idx];
}

u8 rtw_rust_pick_ch_social_chan(_adapter *adapter, int idx)
{
	return adapter->wdinfo.social_chan[idx];
}

u8 rtw_rust_pick_ch_p2p_social(_adapter *adapter)
{
	return rtw_p2p_findphase_ex_is_social(&adapter->wdinfo);
}

u8 rtw_rust_pick_ch_p2p_needed(_adapter *adapter)
{
	return rtw_p2p_findphase_ex_is_needed(&adapter->wdinfo);
}
#else
u8 rtw_rust_pick_ch_p2p_state_not_none(_adapter *adapter)
{
	(void)adapter;
	return 0;
}

void rtw_rust_pick_ch_p2p_findphase_ex_max(_adapter *adapter)
{
	(void)adapter;
}

u8 rtw_rust_pick_ch_rx_scan_op_ch_only(_adapter *adapter)
{
	(void)adapter;
	return 0;
}

u8 rtw_rust_pick_ch_p2p_scan_op_ch_only(_adapter *adapter)
{
	(void)adapter;
	return 0;
}

u8 rtw_rust_pick_ch_rx_op_ch(_adapter *adapter, int idx)
{
	(void)adapter;
	(void)idx;
	return 0;
}

u8 rtw_rust_pick_ch_p2p_op_ch(_adapter *adapter, int idx)
{
	(void)adapter;
	(void)idx;
	return 0;
}

u8 rtw_rust_pick_ch_social_chan(_adapter *adapter, int idx)
{
	(void)adapter;
	(void)idx;
	return 0;
}

u8 rtw_rust_pick_ch_p2p_social(_adapter *adapter)
{
	(void)adapter;
	return 0;
}

u8 rtw_rust_pick_ch_p2p_needed(_adapter *adapter)
{
	(void)adapter;
	return 0;
}
#endif /* CONFIG_P2P */
#endif /* CONFIG_RUST && CONFIG_RUST_MLME_EXT_PICK_CH */

#if defined(CONFIG_RUST) && defined(CONFIG_RUST_MLME_EXT_SITESURVEY_CMD)
#include <drv_types.h>

u8 rtw_rust_ss_state(_adapter *adapter)
{
	return adapter->mlmeextpriv.sitesurvey_res.state;
}

void rtw_rust_ss_set_state(_adapter *adapter, u8 state)
{
	mlmeext_set_scan_state(&adapter->mlmeextpriv, state);
}

u8 rtw_rust_ss_next_state(_adapter *adapter)
{
	return adapter->mlmeextpriv.sitesurvey_res.next_state;
}

void rtw_rust_ss_set_next_state(_adapter *adapter, u8 state)
{
	mlmeext_set_scan_next_state(&adapter->mlmeextpriv, state);
}

void rtw_rust_ss_bump_channel_idx(_adapter *adapter)
{
	adapter->mlmeextpriv.sitesurvey_res.channel_idx++;
}

u8 rtw_rust_ss_rx_ampdu_accept(_adapter *adapter)
{
	return adapter->mlmeextpriv.sitesurvey_res.rx_ampdu_accept;
}

u8 rtw_rust_ss_rx_ampdu_size(_adapter *adapter)
{
	return adapter->mlmeextpriv.sitesurvey_res.rx_ampdu_size;
}

u16 rtw_rust_ss_scan_ch_ms(_adapter *adapter)
{
	return adapter->mlmeextpriv.sitesurvey_res.scan_ch_ms;
}

u8 rtw_rust_ss_backop_flags(_adapter *adapter)
{
	return adapter->mlmeextpriv.sitesurvey_res.backop_flags;
}

u16 rtw_rust_ss_backop_ms(_adapter *adapter)
{
	return adapter->mlmeextpriv.sitesurvey_res.backop_ms;
}

systime rtw_rust_ss_backop_time(_adapter *adapter)
{
	return adapter->mlmeextpriv.sitesurvey_res.backop_time;
}

void rtw_rust_ss_set_backop_time(_adapter *adapter, systime t)
{
	adapter->mlmeextpriv.sitesurvey_res.backop_time = t;
}

u8 rtw_rust_ss_scan_abort(_adapter *adapter)
{
	return adapter->mlmeextpriv.scan_abort;
}

u8 rtw_rust_ss_cur_channel(_adapter *adapter)
{
	return adapter->mlmeextpriv.cur_channel;
}

u8 rtw_rust_ss_cur_bwmode(_adapter *adapter)
{
	return adapter->mlmeextpriv.cur_bwmode;
}

u8 rtw_rust_ss_cur_ch_offset(_adapter *adapter)
{
	return adapter->mlmeextpriv.cur_ch_offset;
}

void rtw_rust_ss_set_survey_timer(_adapter *adapter, u32 ms)
{
	set_survey_timer(&adapter->mlmeextpriv, ms);
}

void rtw_rust_sitesurvey_phydm_backup(_adapter *adapter)
{
	rtw_phydm_ability_backup(adapter);
}

void rtw_rust_sitesurvey_phydm_restore(_adapter *adapter)
{
	rtw_phydm_ability_restore(adapter);
}

void rtw_rust_sitesurvey_phydm_offchannel(_adapter *adapter)
{
	rtw_phydm_func_for_offchannel(adapter);
}

int rtw_rust_hw_var_check_txbuf(void)
{
	return HW_VAR_CHECK_TXBUF;
}

int rtw_rust_hw_var_mlme_sitesurvey(void)
{
	return HW_VAR_MLME_SITESURVEY;
}

void survey_done_set_ch_bw(_adapter *padapter);
void sitesurvey_set_igi(_adapter *adapter);
void sitesurvey_set_msr(_adapter *adapter, bool enter);
u8 rtw_ps_annc(_adapter *adapter, bool ps);

/*
 * States not yet ported in Rust (P2P listen transition, SW antdiv back-to-back).
 * Return 0 if the current state is not an aux state.
 * Return 1 if handled and the Rust FSM should continue in the same invocation
 *       (C `goto operation_by_state` — SCAN_P2P_LISTEN only).
 * Return 2 if handled and the Rust FSM should break (timer-driven next step).
 */
u8 rtw_rust_sitesurvey_aux_state(_adapter *padapter)
{
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct ss_res *ss = &pmlmeext->sitesurvey_res;
	u8 state = ss->state;

#if defined(CONFIG_ANTENNA_DIVERSITY) || defined(DBG_SCAN_SW_ANTDIV_BL)
	if (state == SCAN_SW_ANTDIV_BL) {
		ss->bss_cnt = 0;
		ss->channel_idx = 0;
		ss->is_sw_antdiv_bl_scan = 1;
		mlmeext_set_scan_next_state(pmlmeext, SCAN_PROCESS);
		set_survey_timer(pmlmeext, ss->scan_ch_ms);
		return 2;
	}
#endif

#ifdef CONFIG_P2P
	{
		struct wifidirect_info *pwdinfo = &padapter->wdinfo;

		if (state == SCAN_TO_P2P_LISTEN) {
			set_channel_bwmode(padapter, pwdinfo->listen_channel,
					   HAL_PRIME_CHNL_OFFSET_DONT_CARE, CHANNEL_WIDTH_20);
			rtw_p2p_set_state(pwdinfo, P2P_STATE_FIND_PHASE_LISTEN);
			rtw_phydm_ability_restore(padapter);
			sitesurvey_set_igi(padapter);
			mlmeext_set_scan_state(pmlmeext, SCAN_P2P_LISTEN);
			_set_timer(&pwdinfo->find_phase_timer,
				   (u32)((u32)pwdinfo->listen_dwell * 100));
			return 2;
		}
		if (state == SCAN_P2P_LISTEN) {
			mlmeext_set_scan_state(pmlmeext, SCAN_PROCESS);
			ss->channel_idx = 0;
			return 1;
		}
	}
#endif

	(void)padapter;
	(void)pmlmeext;
	(void)ss;
	return 0;
}

void rtw_rust_sitesurvey_scan_complete(_adapter *padapter)
{
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct ss_res *ss = &pmlmeext->sitesurvey_res;
	u8 val8;

#if defined(CONFIG_RTW_CFGVENDOR_RANDOM_MAC_OUI) || defined(CONFIG_RTW_SCAN_RAND)
	rtw_hal_set_hw_mac_addr(padapter, adapter_mac_addr(padapter));
#endif
#ifdef CONFIG_P2P
	{
		struct wifidirect_info *pwdinfo = &padapter->wdinfo;
#ifdef CONFIG_CONCURRENT_MODE
		struct roch_info *prochinfo = &padapter->rochinfo;
#endif

		if (rtw_p2p_chk_state(pwdinfo, P2P_STATE_SCAN) ||
		    rtw_p2p_chk_state(pwdinfo, P2P_STATE_FIND_PHASE_SEARCH)) {
#ifdef CONFIG_CONCURRENT_MODE
			if (pwdinfo->driver_interface == DRIVER_WEXT) {
				if (rtw_mi_check_status(padapter, MI_LINKED))
					_set_timer(&prochinfo->ap_roch_ch_switch_timer, 500);
			}
#endif
			rtw_p2p_set_state(pwdinfo, rtw_p2p_pre_state(pwdinfo));
		}
		rtw_p2p_findphase_ex_set(pwdinfo, P2P_FINDPHASE_EX_NONE);
	}
#endif

	survey_done_set_ch_bw(padapter);
	sitesurvey_set_msr(padapter, _FALSE);
	val8 = 0;
	rtw_hal_set_hwreg(padapter, HW_VAR_MLME_SITESURVEY, (u8 *)(&val8));
	rtw_phydm_ability_restore(padapter);
	sitesurvey_set_igi(padapter);
#ifdef CONFIG_MCC_MODE
	if (!rtw_hal_set_mcc_setting_scan_complete(padapter))
#endif
	{
		rtw_hal_macid_wakeup_all_used(padapter);
		rtw_ps_annc(padapter, 0);
	}
	rtw_rx_ampdu_apply(padapter);
	mlmeext_set_scan_state(pmlmeext, SCAN_DISABLE);
	report_surveydone_event(padapter, ss->acs);
#ifdef CONFIG_RTW_ACS
	if (IS_ACS_ENABLE(padapter))
		rtw_acs_select_best_chan(padapter);
#endif
#if defined(CONFIG_BACKGROUND_NOISE_MONITOR) && defined(DBG_NOISE_MONITOR)
	if (IS_NM_ENABLE(padapter))
		rtw_noise_info_dump(RTW_DBGDUMP, padapter);
#endif
	issue_action_BSSCoexistPacket(padapter);
	issue_action_BSSCoexistPacket(padapter);
	issue_action_BSSCoexistPacket(padapter);
#ifdef CONFIG_RTW_80211K
	if (ss->token)
		rm_post_event(padapter, ss->token, RM_EV_survey_done);
#endif
#ifdef CONFIG_RTW_ROAM_QUICKSCAN
	if (padapter->mlmepriv.need_to_roam == _TRUE)
		generate_quickss(padapter);
#endif
}
#endif /* CONFIG_RUST && CONFIG_RUST_MLME_EXT_SITESURVEY_CMD */

#if defined(CONFIG_RUST) && defined(CONFIG_RUST_MLME_EXT_BAND_IE)
#include <drv_types.h>

#ifdef CONFIG_AP_MODE
u8 rtw_rust_band_ie_ht_option(_adapter *a)
{
	return a->mlmepriv.htpriv.ht_option;
}

u32 rtw_rust_band_ie_wireless_mode(_adapter *a)
{
	return a->registrypriv.wireless_mode;
}

u8 rtw_rust_band_ie_vht_enable(_adapter *a)
{
	return a->registrypriv.vht_enable;
}

u8 rtw_rust_band_ie_ori_vht_en(_adapter *a)
{
	return a->mlmepriv.ori_vht_en;
}

u8 rtw_rust_band_ie_country_en_11ac(_adapter *a)
{
	struct rf_ctl_t *rfctl = adapter_to_rfctl(a);

	return (!rfctl->country_ent || COUNTRY_CHPLAN_EN_11AC(rfctl->country_ent)) ? 1 : 0;
}

u8 *rtw_rust_band_ie_supported_rates(WLAN_BSSID_EX *n)
{
	return n->SupportedRates;
}

void rtw_rust_band_ie_set_length(WLAN_BSSID_EX *n)
{
	n->Length = get_WLAN_BSSID_EX_sz(n);
}
#endif /* CONFIG_AP_MODE */
#endif /* CONFIG_RUST && CONFIG_RUST_MLME_EXT_BAND_IE */

#if defined(CONFIG_RUST) && defined(CONFIG_RUST_MLME_EXT_MGNT_ATTRIB)
#include <drv_types.h>
#include <hal_data.h>

u8 rtw_rust_mgnt_tx_rate(_adapter *padapter)
{
	return padapter->mlmeextpriv.tx_rate;
}

u16 rtw_rust_mgnt_mgnt_seq(_adapter *padapter)
{
	return padapter->mlmeextpriv.mgnt_seq;
}

u8 rtw_rust_mgnt_hw_ssn_seq_no(_adapter *padapter)
{
	return padapter->xmitpriv.hw_ssn_seq_no;
}

u8 rtw_rust_mgnt_hal_rf_type(_adapter *padapter)
{
	return GET_HAL_DATA(padapter)->rf_type;
}

u8 rtw_rust_mgnt_mlme_is_adhoc(_adapter *padapter)
{
	return MLME_IS_ADHOC(padapter) ? 1 : 0;
}

struct sta_priv *rtw_rust_mgnt_stapriv(_adapter *padapter)
{
	return &padapter->stapriv;
}

u32 rtw_rust_mgnt_txdesc_offset(void)
{
	/* USB 8822B: TXDESC_SIZE (48) + PACKET_OFFSET_SZ (8) = 56.
	 * Host L2 uses TXDESC_SIZE 40 so TXDESC_OFFSET is 48 there. */
	return TXDESC_OFFSET;
}

#ifdef CONFIG_P2P_PS_NOA_USE_MACID_SLEEP
u8 rtw_rust_mgnt_p2p_noa_override(_adapter *padapter, u8 *mac_id, u8 *qsel)
{
	struct mlme_ext_priv *pmlmeext = &(padapter->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *cur_network = &(pmlmeinfo->network);
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct sta_info *psta;
#ifdef CONFIG_P2P_PS
	struct wifidirect_info *pwdinfo = &(padapter->wdinfo);
#endif /* CONFIG_P2P_PS */

#ifdef CONFIG_CONCURRENT_MODE
	if (!rtw_mi_buddy_check_fwstate(padapter, WIFI_ASOC_STATE))
		return 0;
#endif /* CONFIG_CONCURRENT_MODE */
	if (!MLME_IS_GC(padapter))
		return 0;
#ifdef CONFIG_P2P_PS
	if (pwdinfo->p2p_ps_mode <= P2P_PS_NONE)
		return 0;
#else
	return 0;
#endif /* CONFIG_P2P_PS */

	psta = rtw_get_stainfo(pstapriv, cur_network->MacAddress);
	if (!psta)
		return 0;

	*mac_id = psta->cmn.mac_id;
	*qsel = QSLT_VO;
	return 1;
}
#endif /* CONFIG_P2P_PS_NOA_USE_MACID_SLEEP */
#endif

#if defined(HOST_MLME_EXT_SCAN_TEST) || \
	(((!defined(CONFIG_RUST) || !defined(CONFIG_RUST_MLME_EXT_SCAN)) && \
	  !defined(HOST_MLME_EXT_TEST) && !defined(HOST_MLME_EXT_MGNT_ATTRIB_TEST) && \
	  !defined(HOST_MLME_EXT_PEER_ALIVE_TEST) && !defined(HOST_MLME_EXT_BAND_IE_TEST) && \
	  !defined(HOST_MLME_EXT_JOIN_CMD_TEST)))

#ifndef RTW_SCAN_SPARSE_BG_INTERVAL_MS
#define RTW_SCAN_SPARSE_BG_INTERVAL_MS 12000
#endif

#ifndef RTW_SCAN_SPARSE_CH_NUM_MIRACAST
#define RTW_SCAN_SPARSE_CH_NUM_MIRACAST 1
#endif
#ifndef RTW_SCAN_SPARSE_CH_NUM_BG
#define RTW_SCAN_SPARSE_CH_NUM_BG 4
#endif

u8 rtw_scan_sparse(_adapter *adapter, struct rtw_ieee80211_channel *ch, u8 ch_num)
{
#define SCAN_SPARSE_CH_NUM_INVALID 255

	static u8 token = 255;
	u32 interval;
	bool busy_traffic = _FALSE;
	bool miracast_enabled = _FALSE;
	bool bg_scan = _FALSE;
	u8 max_allow_ch = SCAN_SPARSE_CH_NUM_INVALID;
	u8 scan_division_num;
	u8 ret_num = ch_num;
	struct mlme_ext_priv *mlmeext = &adapter->mlmeextpriv;

	if (mlmeext->last_scan_time == 0)
		mlmeext->last_scan_time = rtw_get_current_time();

	interval = rtw_get_passing_time_ms(mlmeext->last_scan_time);

	if (rtw_mi_busy_traffic_check(adapter))
		busy_traffic = _TRUE;

	if (rtw_mi_check_miracast_enabled(adapter))
		miracast_enabled = _TRUE;

	if (interval > RTW_SCAN_SPARSE_BG_INTERVAL_MS)
		bg_scan = _TRUE;

#if RTW_SCAN_SPARSE_MIRACAST
	if (miracast_enabled == _TRUE && busy_traffic == _TRUE)
		max_allow_ch = rtw_min(max_allow_ch, RTW_SCAN_SPARSE_CH_NUM_MIRACAST);
#endif

#if RTW_SCAN_SPARSE_BG
	if (bg_scan == _TRUE)
		max_allow_ch = rtw_min(max_allow_ch, RTW_SCAN_SPARSE_CH_NUM_BG);
#endif

	if (max_allow_ch != SCAN_SPARSE_CH_NUM_INVALID) {
		int i;
		int k = 0;

		scan_division_num = (ch_num / max_allow_ch) +
				    ((ch_num % max_allow_ch) ? 1 : 0);
		token = (token + 1) % scan_division_num;

		for (i = 0; i < ch_num; i++) {
			if (ch[i].hw_value && (i % scan_division_num) == token) {
				if (i != k)
					_rtw_memcpy(&ch[k], &ch[i],
						    sizeof(struct rtw_ieee80211_channel));
				k++;
			}
		}

		_rtw_memset(&ch[k], 0, sizeof(struct rtw_ieee80211_channel));

		ret_num = k;
		mlmeext->last_scan_time = rtw_get_current_time();
	}

	return ret_num;
}

#ifdef CONFIG_SCAN_BACKOP
u8 rtw_scan_backop_decision(_adapter *adapter)
{
	struct mlme_ext_priv *mlmeext = &adapter->mlmeextpriv;
	struct mi_state mstate;
	u8 backop_flags = 0;

	rtw_mi_status(adapter, &mstate);

	if ((MSTATE_STA_LD_NUM(&mstate) &&
	     mlmeext_chk_scan_backop_flags_sta(mlmeext, SS_BACKOP_EN)) ||
	    (MSTATE_STA_NUM(&mstate) &&
	     mlmeext_chk_scan_backop_flags_sta(mlmeext, SS_BACKOP_EN_NL)))
		backop_flags |= mlmeext_scan_backop_flags_sta(mlmeext);

#ifdef CONFIG_AP_MODE
	if ((MSTATE_AP_LD_NUM(&mstate) &&
	     mlmeext_chk_scan_backop_flags_ap(mlmeext, SS_BACKOP_EN)) ||
	    (MSTATE_AP_NUM(&mstate) &&
	     mlmeext_chk_scan_backop_flags_ap(mlmeext, SS_BACKOP_EN_NL)))
		backop_flags |= mlmeext_scan_backop_flags_ap(mlmeext);
#endif

#ifdef CONFIG_RTW_MESH
	if ((MSTATE_MESH_LD_NUM(&mstate) &&
	     mlmeext_chk_scan_backop_flags_mesh(mlmeext, SS_BACKOP_EN)) ||
	    (MSTATE_MESH_NUM(&mstate) &&
	     mlmeext_chk_scan_backop_flags_mesh(mlmeext, SS_BACKOP_EN_NL)))
		backop_flags |= mlmeext_scan_backop_flags_mesh(mlmeext);
#endif

	return backop_flags;
}
#endif /* CONFIG_SCAN_BACKOP */

#define SCANNING_TIMEOUT_EX 2000
u32 rtw_scan_timeout_decision(_adapter *padapter)
{
	u32 back_op_times = 0;
	u8 max_chan_num;
	u16 scan_ms;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct ss_res *ss = &pmlmeext->sitesurvey_res;

	if (is_supported_5g(padapter->registrypriv.wireless_mode) &&
	    IsSupported24G(padapter->registrypriv.wireless_mode))
		max_chan_num = MAX_CHANNEL_NUM;
	else
		max_chan_num = MAX_CHANNEL_NUM_2G;

#ifdef CONFIG_SCAN_BACKOP
	if (rtw_scan_backop_decision(padapter))
		back_op_times =
			(max_chan_num / ss->scan_cnt_max) * ss->backop_ms;
#endif

	if (ss->duration)
		scan_ms = ss->duration;
	else
#if defined(CONFIG_RTW_ACS) && defined(CONFIG_RTW_ACS_DBG)
	if (IS_ACS_ENABLE(padapter) && rtw_is_acs_st_valid(padapter))
		scan_ms = rtw_acs_get_adv_st(padapter);
	else
#endif /* CONFIG_RTW_ACS */
		scan_ms = ss->scan_ch_ms;

	ss->scan_timeout_ms =
		(scan_ms * max_chan_num) + back_op_times + SCANNING_TIMEOUT_EX;
#ifdef DBG_SITESURVEY
	RTW_INFO("%s , scan_timeout_ms = %d (ms)\n", __func__, ss->scan_timeout_ms);
#endif /* DBG_SITESURVEY */
	return ss->scan_timeout_ms;
}

#endif /* scan sparse/backop/timeout block uses CONFIG_RUST_MLME_EXT_SCAN when added */

#if (defined(HOST_MLME_EXT_SCAN_TEST) && !defined(CONFIG_RUST_MLME_EXT_PICK_CH)) || \
	(((!defined(CONFIG_RUST) || !defined(CONFIG_RUST_MLME_EXT_PICK_CH)) && \
	  !defined(HOST_MLME_EXT_TEST) && !defined(HOST_MLME_EXT_MGNT_ATTRIB_TEST) && \
	  !defined(HOST_MLME_EXT_PEER_ALIVE_TEST) && !defined(HOST_MLME_EXT_BAND_IE_TEST) && \
	  !defined(HOST_MLME_EXT_JOIN_CMD_TEST)))

static bool scan_abort_hdl(_adapter *adapter)
{
	struct mlme_ext_priv *pmlmeext = &adapter->mlmeextpriv;
	struct ss_res *ss = &pmlmeext->sitesurvey_res;
#ifdef CONFIG_P2P
	struct wifidirect_info *pwdinfo = &adapter->wdinfo;
#endif
	bool ret = _FALSE;

	if (pmlmeext->scan_abort == _TRUE) {
#ifdef CONFIG_P2P
		if (!rtw_p2p_chk_state(&adapter->wdinfo, P2P_STATE_NONE)) {
			rtw_p2p_findphase_ex_set(pwdinfo, P2P_FINDPHASE_EX_MAX);
			ss->channel_idx = 3;
			RTW_INFO("%s idx:%d, cnt:%u\n", __func__
				 , ss->channel_idx
				 , pwdinfo->find_phase_state_exchange_cnt
				);
		} else
#endif
		{
			ss->channel_idx = ss->ch_num;
			RTW_INFO("%s idx:%d\n", __func__
				 , ss->channel_idx
				);
		}
		ret = _TRUE;
	}

	return ret;
}

u8 sitesurvey_pick_ch_behavior(_adapter *padapter, u8 *ch, RT_SCAN_TYPE *type)
{
	u8 next_state;
	u8 scan_ch = 0;
	RT_SCAN_TYPE scan_type = SCAN_PASSIVE;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct ss_res *ss = &pmlmeext->sitesurvey_res;
	struct rf_ctl_t *rfctl = adapter_to_rfctl(padapter);
	int ch_set_idx;
#ifdef CONFIG_P2P
	struct wifidirect_info *pwdinfo = &padapter->wdinfo;
#endif
#ifdef CONFIG_SCAN_BACKOP
	u8 backop_flags = 0;
#endif

	scan_abort_hdl(padapter);

#ifdef CONFIG_P2P
	if (pwdinfo->rx_invitereq_info.scan_op_ch_only || pwdinfo->p2p_info.scan_op_ch_only) {
		if (pwdinfo->rx_invitereq_info.scan_op_ch_only)
			scan_ch = pwdinfo->rx_invitereq_info.operation_ch[ss->channel_idx];
		else
			scan_ch = pwdinfo->p2p_info.operation_ch[ss->channel_idx];
		scan_type = SCAN_ACTIVE;
	} else if (rtw_p2p_findphase_ex_is_social(pwdinfo)) {
		scan_ch = pwdinfo->social_chan[ss->channel_idx];
		ch_set_idx = rtw_chset_search_ch(rfctl->channel_set, scan_ch);
		if (ch_set_idx >= 0)
			scan_type = rfctl->channel_set[ch_set_idx].flags & RTW_CHF_NO_IR ? SCAN_PASSIVE : SCAN_ACTIVE;
		else
			scan_type = SCAN_ACTIVE;
	} else
#endif /* CONFIG_P2P */
	{
		struct rtw_ieee80211_channel *ch;

#ifdef CONFIG_SCAN_BACKOP
		backop_flags = rtw_scan_backop_decision(padapter);
#endif

#ifdef CONFIG_SCAN_BACKOP
		if (!(backop_flags && ss->scan_cnt >= ss->scan_cnt_max))
#endif
		{
#ifdef CONFIG_RTW_WIFI_HAL
			if (adapter_to_dvobj(padapter)->nodfs) {
				while (ss->channel_idx < ss->ch_num && rtw_chset_is_dfs_ch(rfctl->channel_set, ss->ch[ss->channel_idx].hw_value))
					ss->channel_idx++;
			} else
#endif
			if (ss->channel_idx != 0 && ss->force_ssid_scan == 0
				&& pmlmeext->sitesurvey_res.ssid_num
				&& (ss->ch[ss->channel_idx - 1].flags & RTW_IEEE80211_CHAN_PASSIVE_SCAN)
			) {
				ch_set_idx = rtw_chset_search_ch(rfctl->channel_set, ss->ch[ss->channel_idx - 1].hw_value);
				if (ch_set_idx != -1 && rfctl->channel_set[ch_set_idx].hidden_bss_cnt
					&& (!IS_DFS_SLAVE_WITH_RD(rfctl)
						|| rtw_rfctl_dfs_domain_unknown(rfctl)
						|| !CH_IS_NON_OCP(&rfctl->channel_set[ch_set_idx]))
				) {
					ss->channel_idx--;
					ss->force_ssid_scan = 1;
				}
			} else
				ss->force_ssid_scan = 0;
		}

		if (ss->channel_idx < ss->ch_num) {
			ch = &ss->ch[ss->channel_idx];
			scan_ch = ch->hw_value;

#if defined(CONFIG_RTW_ACS) && defined(CONFIG_RTW_ACS_DBG)
			if (IS_ACS_ENABLE(padapter) && rtw_is_acs_passiv_scan(padapter))
				scan_type = SCAN_PASSIVE;
			else
#endif /* CONFIG_RTW_ACS */
				scan_type = (ch->flags & RTW_IEEE80211_CHAN_PASSIVE_SCAN) ? SCAN_PASSIVE : SCAN_ACTIVE;
		}
	}

	if (scan_ch != 0) {
		next_state = SCAN_PROCESS;

#ifdef CONFIG_SCAN_BACKOP
		if (backop_flags) {
			if (ss->scan_cnt < ss->scan_cnt_max)
				ss->scan_cnt++;
			else {
				mlmeext_assign_scan_backop_flags(pmlmeext, backop_flags);
				next_state = SCAN_BACKING_OP;
			}
		}
#endif

	} else if (rtw_p2p_findphase_ex_is_needed(pwdinfo)) {
		/* go p2p listen */
		next_state = SCAN_TO_P2P_LISTEN;

#ifdef CONFIG_ANTENNA_DIVERSITY
	} else if (rtw_hal_antdiv_before_linked(padapter)) {
		/* go sw antdiv before link */
		next_state = SCAN_SW_ANTDIV_BL;
#endif
	} else {
		next_state = SCAN_COMPLETE;

#if defined(DBG_SCAN_SW_ANTDIV_BL)
		{
			struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
			int i;
			bool is_linked = _FALSE;

			for (i = 0; i < dvobj->iface_nums; i++) {
				if (rtw_linked_check(dvobj->padapters[i]))
					is_linked = _TRUE;
			}

			if (!is_linked) {
				static bool fake_sw_antdiv_bl_state = 0;

				if (fake_sw_antdiv_bl_state == 0) {
					next_state = SCAN_SW_ANTDIV_BL;
					fake_sw_antdiv_bl_state = 1;
				} else
					fake_sw_antdiv_bl_state = 0;
			}
		}
#endif /* defined(DBG_SCAN_SW_ANTDIV_BL) */
	}

#ifdef CONFIG_SCAN_BACKOP
	if (next_state != SCAN_PROCESS)
		ss->scan_cnt = 0;
#endif

#ifdef DBG_FIXED_CHAN
	if (pmlmeext->fixed_chan != 0xff && next_state == SCAN_PROCESS)
		scan_ch = pmlmeext->fixed_chan;
#endif

	if (ch)
		*ch = scan_ch;
	if (type)
		*type = scan_type;

	return next_state;
}

#endif /* (HOST_MLME_EXT_SCAN_TEST && !CONFIG_RUST_MLME_EXT_PICK_CH) || ((!CONFIG_RUST || !CONFIG_RUST_MLME_EXT_PICK_CH) && ...) */

#if defined(HOST_MLME_EXT_BAND_IE_TEST) || \
	(((!defined(CONFIG_RUST) || !defined(CONFIG_RUST_MLME_EXT_BAND_IE)) && \
	  !defined(HOST_MLME_EXT_TEST) && !defined(HOST_MLME_EXT_MGNT_ATTRIB_TEST) && \
	  !defined(HOST_MLME_EXT_PEER_ALIVE_TEST) && !defined(HOST_MLME_EXT_SCAN_TEST) && \
	  !defined(HOST_MLME_EXT_JOIN_CMD_TEST)))

#ifdef CONFIG_AP_MODE
/*
 * according to channel
 * add/remove WLAN_BSSID_EX.IEs's ERP ie
 * set WLAN_BSSID_EX.SupportedRates
 * update WLAN_BSSID_EX.IEs's Supported Rate and Extended Supported Rate ie
 */
void change_band_update_ie(_adapter *padapter, WLAN_BSSID_EX *pnetwork, u8 ch)
{
	u8 network_type, rate_len, total_rate_len, remainder_rate_len;
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct rf_ctl_t *rfctl = adapter_to_rfctl(padapter);
	u8 erpinfo = 0x4;

	if (ch >= 36) {
		network_type = WIRELESS_11A;
		total_rate_len = IEEE80211_NUM_OFDM_RATESLEN;
		rtw_remove_bcn_ie(padapter, pnetwork, _ERPINFO_IE_);
#ifdef CONFIG_80211AC_VHT
		if ((pmlmepriv->htpriv.ht_option == _TRUE)
		    && REGSTY_IS_11AC_ENABLE(&padapter->registrypriv)
		    && is_supported_vht(padapter->registrypriv.wireless_mode)
		    && (!rfctl->country_ent || COUNTRY_CHPLAN_EN_11AC(rfctl->country_ent))
		) {
			if (REGSTY_IS_11AC_AUTO(&padapter->registrypriv)
			    || pmlmepriv->ori_vht_en)
				rtw_vht_ies_attach(padapter, pnetwork);
		}
#endif
	} else {
		network_type = 0;
		total_rate_len = 0;
		if (padapter->registrypriv.wireless_mode & WIRELESS_11B) {
			network_type |= WIRELESS_11B;
			total_rate_len += IEEE80211_CCK_RATE_LEN;
		}
		if (padapter->registrypriv.wireless_mode & WIRELESS_11G) {
			network_type |= WIRELESS_11G;
			total_rate_len += IEEE80211_NUM_OFDM_RATESLEN;
		}
		rtw_add_bcn_ie(padapter, pnetwork, _ERPINFO_IE_, &erpinfo, 1);
#ifdef CONFIG_80211AC_VHT
		rtw_vht_ies_detach(padapter, pnetwork);
#endif
	}

	rtw_set_supported_rate(pnetwork->SupportedRates, network_type);

	UpdateBrateTbl(padapter, pnetwork->SupportedRates);

	if (total_rate_len > 8) {
		rate_len = 8;
		remainder_rate_len = total_rate_len - 8;
	} else {
		rate_len = total_rate_len;
		remainder_rate_len = 0;
	}

	rtw_add_bcn_ie(padapter, pnetwork, _SUPPORTEDRATES_IE_, pnetwork->SupportedRates, rate_len);

	if (remainder_rate_len)
		rtw_add_bcn_ie(padapter, pnetwork, _EXT_SUPPORTEDRATES_IE_, (pnetwork->SupportedRates + 8), remainder_rate_len);
	else
		rtw_remove_bcn_ie(padapter, pnetwork, _EXT_SUPPORTEDRATES_IE_);

	pnetwork->Length = get_WLAN_BSSID_EX_sz(pnetwork);
}
#endif /* CONFIG_AP_MODE */

#endif /* HOST_MLME_EXT_BAND_IE_TEST || ((!CONFIG_RUST || !CONFIG_RUST_MLME_EXT_BAND_IE) && ...) */

#if defined(HOST_MLME_EXT_JOIN_CMD_TEST) || \
	(((!defined(CONFIG_RUST) || !defined(CONFIG_RUST_MLME_EXT_JOIN_CMD)) && \
	  !defined(HOST_MLME_EXT_TEST) && !defined(HOST_MLME_EXT_MGNT_ATTRIB_TEST) && \
	  !defined(HOST_MLME_EXT_PEER_ALIVE_TEST) && !defined(HOST_MLME_EXT_SCAN_TEST) && \
	  !defined(HOST_MLME_EXT_BAND_IE_TEST)))

#ifdef HOST_MLME_EXT_JOIN_CMD_TEST
u8 join_cmd_hdl(_adapter *padapter, u8 *pbuf)
{
	u8 join_type;
	PNDIS_802_11_VARIABLE_IEs pIE;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *pnetwork = (WLAN_BSSID_EX *)(&(pmlmeinfo->network));
	u32 i;
	u8 u_ch, u_bw, u_offset;
	u8 doiqk = _FALSE;

	if (pmlmeinfo->state & WIFI_FW_ASSOC_SUCCESS) {
		if (pmlmeinfo->state & WIFI_FW_STATION_STATE)
			issue_deauth_ex(padapter, pnetwork->MacAddress,
					WLAN_REASON_DEAUTH_LEAVING, 1, 100);
		pmlmeinfo->state = WIFI_FW_NULL_STATE;
		flush_all_cam_entry(padapter);
		_cancel_timer_ex(&pmlmeext->link_timer);
		Set_MSR(padapter, _HW_STATE_STATION_);
		rtw_hal_set_hwreg(padapter, HW_VAR_MLME_DISCONNECT, 0);
	}

	rtw_joinbss_reset(padapter);

	pmlmeinfo->ERP_enable = 0;
	pmlmeinfo->WMM_enable = 0;
	pmlmeinfo->HT_enable = 0;
	pmlmeinfo->HT_caps_enable = 0;
	pmlmeinfo->HT_info_enable = 0;
	pmlmeinfo->agg_enable_bitmap = 0;
	pmlmeinfo->candidate_tid_bitmap = 0;
	pmlmeinfo->bwmode_updated = _FALSE;
	pmlmeinfo->VHT_enable = 0;

	_rtw_memcpy(pnetwork, pbuf, FIELD_OFFSET(WLAN_BSSID_EX, IELength));
	pnetwork->IELength = ((WLAN_BSSID_EX *)pbuf)->IELength;

	if (pnetwork->IELength > MAX_IE_SZ)
		return H2C_PARAMETERS_ERROR;

	if (pnetwork->IELength < 2) {
		report_join_res(padapter, (-4), WLAN_STATUS_UNSPECIFIED_FAILURE);
		return H2C_SUCCESS;
	}
	_rtw_memcpy(pnetwork->IEs, ((WLAN_BSSID_EX *)pbuf)->IEs, pnetwork->IELength);

	pmlmeinfo->bcn_interval = get_beacon_interval(pnetwork);

	for (i = _FIXED_IE_LENGTH_; i < pnetwork->IELength - 2;) {
		pIE = (PNDIS_802_11_VARIABLE_IEs)(pnetwork->IEs + i);

		switch (pIE->ElementID) {
		case _VENDOR_SPECIFIC_IE_:
			if (_rtw_memcmp(pIE->data, WMM_OUI, 4))
				WMM_param_handler(padapter, pIE);
			break;
		case _HT_CAPABILITY_IE_:
			pmlmeinfo->HT_caps_enable = 1;
			break;
		case _HT_EXTRA_INFO_IE_:
			pmlmeinfo->HT_info_enable = 1;
			break;
		case EID_VHTCapability:
			pmlmeinfo->VHT_enable = 1;
			break;
		default:
			break;
		}

		i += (pIE->Length + 2);
	}

	rtw_bss_get_chbw(pnetwork, &pmlmeext->cur_channel, &pmlmeext->cur_bwmode,
			 &pmlmeext->cur_ch_offset, 1, 1);
	rtw_adjust_chbw(padapter, pmlmeext->cur_channel, &pmlmeext->cur_bwmode,
			&pmlmeext->cur_ch_offset);

	if (rtw_chk_start_clnt_join(padapter, &u_ch, &u_bw, &u_offset) == _FAIL) {
		report_join_res(padapter, (-4), WLAN_STATUS_UNSPECIFIED_FAILURE);
		return H2C_SUCCESS;
	}

	rtw_hal_set_hwreg(padapter, HW_VAR_BSSID, pmlmeinfo->network.MacAddress);
	if (MLME_IS_STA(padapter))
		rtw_hal_rcr_set_chk_bssid(padapter, MLME_STA_CONNECTING);
	else
		rtw_hal_rcr_set_chk_bssid(padapter, MLME_ADHOC_STARTED);

	join_type = 0;
	rtw_hal_set_hwreg(padapter, HW_VAR_MLME_JOIN, (u8 *)(&join_type));
	rtw_btcoex_connect_notify(padapter, join_type);

	doiqk = _TRUE;
	rtw_hal_set_hwreg(padapter, HW_VAR_DO_IQK, &doiqk);
	set_channel_bwmode(padapter, u_ch, u_offset, u_bw);
	doiqk = _FALSE;
	rtw_hal_set_hwreg(padapter, HW_VAR_DO_IQK, &doiqk);

	_cancel_timer_ex(&pmlmeext->link_timer);
	start_clnt_join(padapter);

	return H2C_SUCCESS;
}
#else /* HOST_MLME_EXT_JOIN_CMD_TEST */
u8 join_cmd_hdl(_adapter *padapter, u8 *pbuf)
{
	u8	join_type;
	PNDIS_802_11_VARIABLE_IEs	pIE;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX		*pnetwork = (WLAN_BSSID_EX *)(&(pmlmeinfo->network));
#ifdef CONFIG_ANTENNA_DIVERSITY
	struct joinbss_parm	*pparm = (struct joinbss_parm *)pbuf;
#endif /* CONFIG_ANTENNA_DIVERSITY */
#ifdef  CONFIG_LAYER2_ROAMING
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct wlan_network     *rnetwork = pmlmepriv->roam_network;
	struct beacon_keys bcn_keys;
	u32 roam_ielen;
#endif
	u32 i;
	u8 u_ch, u_bw, u_offset;
	u8 doiqk = _FALSE;

	if (pmlmeinfo->state & WIFI_FW_ASSOC_SUCCESS) {
		if (pmlmeinfo->state & WIFI_FW_STATION_STATE)
			issue_deauth_ex(padapter, pnetwork->MacAddress, WLAN_REASON_DEAUTH_LEAVING, 1, 100);
		pmlmeinfo->state = WIFI_FW_NULL_STATE;
		flush_all_cam_entry(padapter);
		_cancel_timer_ex(&pmlmeext->link_timer);
		Set_MSR(padapter, _HW_STATE_STATION_);
		rtw_hal_set_hwreg(padapter, HW_VAR_MLME_DISCONNECT, 0);
		if (pmlmeinfo->state & WIFI_FW_STATION_STATE)
			rtw_hal_rcr_set_chk_bssid(padapter, MLME_STA_DISCONNECTED);
	}

#ifdef CONFIG_ANTENNA_DIVERSITY
	rtw_antenna_select_cmd(padapter, pparm->network.PhyInfo.Optimum_antenna, _FALSE);
#endif

#ifdef CONFIG_WAPI_SUPPORT
	rtw_wapi_clear_all_cam_entry(padapter);
#endif

	rtw_joinbss_reset(padapter);

	pmlmeinfo->ERP_enable = 0;
	pmlmeinfo->WMM_enable = 0;
	pmlmeinfo->HT_enable = 0;
	pmlmeinfo->HT_caps_enable = 0;
	pmlmeinfo->HT_info_enable = 0;
	pmlmeinfo->agg_enable_bitmap = 0;
	pmlmeinfo->candidate_tid_bitmap = 0;
	pmlmeinfo->bwmode_updated = _FALSE;
	pmlmeinfo->VHT_enable = 0;
#ifdef ROKU_PRIVATE
	pmlmeinfo->ht_vht_received = 0;
	_rtw_memset(pmlmeinfo->SupportedRates_infra_ap, 0, NDIS_802_11_LENGTH_RATES_EX);
#endif /* ROKU_PRIVATE */
	_rtw_memcpy(pnetwork, pbuf, FIELD_OFFSET(WLAN_BSSID_EX, IELength));
	pnetwork->IELength = ((WLAN_BSSID_EX *)pbuf)->IELength;

	if (pnetwork->IELength > MAX_IE_SZ)
		return H2C_PARAMETERS_ERROR;

	if (pnetwork->IELength < 2) {
		report_join_res(padapter, (-4), WLAN_STATUS_UNSPECIFIED_FAILURE);
		return H2C_SUCCESS;
	}
	_rtw_memcpy(pnetwork->IEs, ((WLAN_BSSID_EX *)pbuf)->IEs, pnetwork->IELength);

	pmlmeinfo->bcn_interval = get_beacon_interval(pnetwork);

	for (i = _FIXED_IE_LENGTH_ ; i < pnetwork->IELength - 2 ;) {
		pIE = (PNDIS_802_11_VARIABLE_IEs)(pnetwork->IEs + i);

		switch (pIE->ElementID) {
		case _VENDOR_SPECIFIC_IE_:
			if (_rtw_memcmp(pIE->data, WMM_OUI, 4))
				WMM_param_handler(padapter, pIE);
			break;

#ifdef CONFIG_80211N_HT
		case _HT_CAPABILITY_IE_:
			pmlmeinfo->HT_caps_enable = 1;
			break;

		case _HT_EXTRA_INFO_IE_:
			pmlmeinfo->HT_info_enable = 1;
			break;
#endif /* CONFIG_80211N_HT */

#ifdef CONFIG_80211AC_VHT
		case EID_VHTCapability:
			pmlmeinfo->VHT_enable = 1;
			break;

		case EID_VHTOperation:
			break;
#endif /* CONFIG_80211AC_VHT */
		default:
			break;
		}

		i += (pIE->Length + 2);
	}

	rtw_bss_get_chbw(pnetwork
		, &pmlmeext->cur_channel, &pmlmeext->cur_bwmode, &pmlmeext->cur_ch_offset, 1, 1);

	rtw_adjust_chbw(padapter, pmlmeext->cur_channel, &pmlmeext->cur_bwmode, &pmlmeext->cur_ch_offset);

	if (rtw_chk_start_clnt_join(padapter, &u_ch, &u_bw, &u_offset) == _FAIL) {
		report_join_res(padapter, (-4), WLAN_STATUS_UNSPECIFIED_FAILURE);
		return H2C_SUCCESS;
	}

	rtw_hal_set_hwreg(padapter, HW_VAR_BSSID, pmlmeinfo->network.MacAddress);
	if (MLME_IS_STA(padapter))
		rtw_hal_rcr_set_chk_bssid(padapter, MLME_STA_CONNECTING);
	else
		rtw_hal_rcr_set_chk_bssid(padapter, MLME_ADHOC_STARTED);

	join_type = 0;
	rtw_hal_set_hwreg(padapter, HW_VAR_MLME_JOIN, (u8 *)(&join_type));

	rtw_btcoex_connect_notify(padapter, join_type);

	doiqk = _TRUE;
	rtw_hal_set_hwreg(padapter , HW_VAR_DO_IQK , &doiqk);

	set_channel_bwmode(padapter, u_ch, u_offset, u_bw);

	doiqk = _FALSE;
	rtw_hal_set_hwreg(padapter , HW_VAR_DO_IQK , &doiqk);

	_cancel_timer_ex(&pmlmeext->link_timer);

	start_clnt_join(padapter);

#ifdef	CONFIG_LAYER2_ROAMING
	rtw_msleep_os(2);
	if (rnetwork && (pmlmeinfo->state & (WIFI_FW_AUTH_NULL | WIFI_FW_AUTH_STATE))) {
		if (!rnetwork) {
			RTW_INFO("%s: pmlmepriv->roam_network is NULL. roaming fail!!\n", __func__);
			return H2C_SUCCESS;
		}
		roam_ielen = rnetwork->network.IELength;
		update_network(&(pmlmepriv->cur_network.network), &(rnetwork->network), padapter, _TRUE);

		if (rtw_get_bcn_keys_from_bss(&rnetwork->network, &bcn_keys) == _TRUE) {
			_rtw_memcpy(&pmlmepriv->cur_beacon_keys, &bcn_keys, sizeof(bcn_keys));
			if (is_hidden_ssid(bcn_keys.ssid, bcn_keys.ssid_len)) {
				_rtw_memcpy(pmlmepriv->cur_beacon_keys.ssid, pmlmeinfo->network.Ssid.Ssid, IW_ESSID_MAX_SIZE);
				pmlmepriv->cur_beacon_keys.ssid_len = pmlmeinfo->network.Ssid.SsidLength;
			}
		} else {
			RTW_ERR("%s: get beacon keys failed\n", __func__);
			_rtw_memset(&pmlmepriv->cur_beacon_keys, 0, sizeof(bcn_keys));
		}
		#ifdef CONFIG_BCN_CNT_CONFIRM_HDL
		pmlmepriv->new_beacon_cnts = 0;
		#endif

		pmlmeinfo->assoc_AP_vendor = check_assoc_AP(rnetwork->network.IEs, rnetwork->network.IELength);

		pmlmeext->TSFValue = rnetwork->network.tsf + rtw_get_passing_time_ms(rnetwork->last_scanned)*1000;
		pmlmeext->bcn_cnt = 0;
		pmlmeext->last_bcn_cnt = 0;
		start_clnt_auth(padapter);
	}
#endif
	return H2C_SUCCESS;
}
#endif /* HOST_MLME_EXT_JOIN_CMD_TEST */

#endif /* HOST_MLME_EXT_JOIN_CMD_TEST || kernel join_cmd path */
