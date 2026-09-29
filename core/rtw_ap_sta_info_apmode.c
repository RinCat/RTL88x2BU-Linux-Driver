/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 *****************************************************************************/
#define _RTW_AP_STA_INFO_APMODE_C_

#ifdef HOST_AP_STA_INFO_APMODE_TEST
#include "host_ap_sta_info_apmode_types.h"
#else
#include <drv_types.h>
#include <hal_data.h>
#endif

#ifdef CONFIG_AP_MODE

#if !defined(CONFIG_RUST_AP_STA_INFO_APMODE) || defined(HOST_AP_STA_INFO_APMODE_TEST)

/* notes:
 * AID: 1~MAX for sta and 0 for bc/mc in ap/adhoc mode  */
void update_sta_info_apmode(_adapter *padapter, struct sta_info *psta)
{
	_irqL	irqL;
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct security_priv *psecuritypriv = &padapter->securitypriv;
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
#ifdef CONFIG_80211N_HT
	struct ht_priv	*phtpriv_ap = &pmlmepriv->htpriv;
	struct ht_priv	*phtpriv_sta = &psta->htpriv;
#endif /* CONFIG_80211N_HT */
	u8	cur_ldpc_cap = 0, cur_stbc_cap = 0;
	/* set intf_tag to if1 */
	/* psta->intf_tag = 0; */

#ifndef HOST_AP_STA_INFO_APMODE_TEST
	RTW_INFO("%s\n", __FUNCTION__);
#endif

	/*alloc macid when call rtw_alloc_stainfo(),release macid when call rtw_free_stainfo()*/

	if (!MLME_IS_MESH(padapter) && psecuritypriv->dot11AuthAlgrthm == dot11AuthAlgrthm_8021X)
		psta->ieee8021x_blocked = _TRUE;
	else
		psta->ieee8021x_blocked = _FALSE;


	/* update sta's cap */

	/* ERP */
	VCS_update(padapter, psta);
#ifdef CONFIG_80211N_HT
	/* HT related cap */
	if (phtpriv_sta->ht_option) {
		/* check if sta supports rx ampdu */
		phtpriv_sta->ampdu_enable = phtpriv_ap->ampdu_enable;

		phtpriv_sta->rx_ampdu_min_spacing = (phtpriv_sta->ht_cap.ampdu_params_info & IEEE80211_HT_CAP_AMPDU_DENSITY) >> 2;

		/* bwmode */
		if ((phtpriv_sta->ht_cap.cap_info & phtpriv_ap->ht_cap.cap_info) & cpu_to_le16(IEEE80211_HT_CAP_SUP_WIDTH))
			psta->cmn.bw_mode = CHANNEL_WIDTH_40;
		else
			psta->cmn.bw_mode = CHANNEL_WIDTH_20;

		if (phtpriv_sta->op_present
			&& !GET_HT_OP_ELE_STA_CHL_WIDTH(phtpriv_sta->ht_op))
			psta->cmn.bw_mode = CHANNEL_WIDTH_20;

		if (psta->ht_40mhz_intolerant)
			psta->cmn.bw_mode = CHANNEL_WIDTH_20;

		if (pmlmeext->cur_bwmode < psta->cmn.bw_mode)
			psta->cmn.bw_mode = pmlmeext->cur_bwmode;

		phtpriv_sta->ch_offset = pmlmeext->cur_ch_offset;


		/* check if sta support s Short GI 20M */
		if ((phtpriv_sta->ht_cap.cap_info & phtpriv_ap->ht_cap.cap_info) & cpu_to_le16(IEEE80211_HT_CAP_SGI_20))
			phtpriv_sta->sgi_20m = _TRUE;

		/* check if sta support s Short GI 40M */
		if ((phtpriv_sta->ht_cap.cap_info & phtpriv_ap->ht_cap.cap_info) & cpu_to_le16(IEEE80211_HT_CAP_SGI_40)) {
			if (psta->cmn.bw_mode == CHANNEL_WIDTH_40) /* according to psta->bw_mode */
				phtpriv_sta->sgi_40m = _TRUE;
			else
				phtpriv_sta->sgi_40m = _FALSE;
		}

		psta->qos_option = _TRUE;

		/* B0 Config LDPC Coding Capability */
		if (TEST_FLAG(phtpriv_ap->ldpc_cap, LDPC_HT_ENABLE_TX) &&
		    GET_HT_CAP_ELE_LDPC_CAP((u8 *)(&phtpriv_sta->ht_cap))) {
			SET_FLAG(cur_ldpc_cap, (LDPC_HT_ENABLE_TX | LDPC_HT_CAP_TX));
#ifndef HOST_AP_STA_INFO_APMODE_TEST
			RTW_INFO("Enable HT Tx LDPC for STA(%d)\n", psta->cmn.aid);
#endif
		}

		/* B7 B8 B9 Config STBC setting */
		if (TEST_FLAG(phtpriv_ap->stbc_cap, STBC_HT_ENABLE_TX) &&
		    GET_HT_CAP_ELE_RX_STBC((u8 *)(&phtpriv_sta->ht_cap))) {
			SET_FLAG(cur_stbc_cap, (STBC_HT_ENABLE_TX | STBC_HT_CAP_TX));
#ifndef HOST_AP_STA_INFO_APMODE_TEST
			RTW_INFO("Enable HT Tx STBC for STA(%d)\n", psta->cmn.aid);
#endif
		}

		#ifdef CONFIG_BEAMFORMING
		update_sta_info_apmode_ht_bf_cap(padapter, psta);
		#endif
	} else {
		phtpriv_sta->ampdu_enable = _FALSE;

		phtpriv_sta->sgi_20m = _FALSE;
		phtpriv_sta->sgi_40m = _FALSE;
		psta->cmn.bw_mode = CHANNEL_WIDTH_20;
		phtpriv_sta->ch_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
	}

	phtpriv_sta->ldpc_cap = cur_ldpc_cap;
	phtpriv_sta->stbc_cap = cur_stbc_cap;

	/* Rx AMPDU */
	send_delba(padapter, 0, psta->cmn.mac_addr);/* recipient */

	/* TX AMPDU */
	send_delba(padapter, 1, psta->cmn.mac_addr);/*  */ /* originator */
	phtpriv_sta->agg_enable_bitmap = 0x0;/* reset */
	phtpriv_sta->candidate_tid_bitmap = 0x0;/* reset */
#endif /* CONFIG_80211N_HT */

#ifdef CONFIG_80211AC_VHT
	update_sta_vht_info_apmode(padapter, psta);
#endif
	psta->cmn.ra_info.is_support_sgi = query_ra_short_GI(psta, rtw_get_tx_bw_mode(padapter, psta));
	update_ldpc_stbc_cap(psta);

	/* todo: init other variables */

	_rtw_memset((void *)&psta->sta_stats, 0, sizeof(struct stainfo_stats));


	/* add ratid */
	/* add_RATid(padapter, psta); */ /* move to ap_sta_info_defer_update() */

	/* ap mode */
	rtw_hal_set_odm_var(padapter, HAL_ODM_STA_INFO, psta, _TRUE);

	_enter_critical_bh(&psta->lock, &irqL);

	/* Check encryption */
	if (!MLME_IS_MESH(padapter) && psecuritypriv->dot11AuthAlgrthm == dot11AuthAlgrthm_8021X)
		psta->state |= WIFI_UNDER_KEY_HANDSHAKE;

	psta->state |= WIFI_ASOC_STATE;

	_exit_critical_bh(&psta->lock, &irqL);
}

#endif /* !CONFIG_RUST_AP_STA_INFO_APMODE || HOST_AP_STA_INFO_APMODE_TEST */

#endif /* CONFIG_AP_MODE */
