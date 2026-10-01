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
#define _RTW_XMIT_UPDATE_ATTRIB_REST_C_

#ifdef HOST_XMIT_UPDATE_ATTRIB_TEST
#include "host_xmit_update_attrib_types.h"
#else
#include <drv_types.h>
#endif

#if !defined(CONFIG_RUST) || defined(HOST_XMIT_UPDATE_ATTRIB_TEST)

static u8 validate_vcs(_adapter *padapter, u8 mode)
{
	u8 vcs_mode = NONE_VCS;

	switch (padapter->registrypriv.vrtl_carrier_sense) {
	case DISABLE_VCS:
		vcs_mode = NONE_VCS;
		break;
	case ENABLE_VCS:
		vcs_mode = padapter->registrypriv.vcs_type;
		break;
	case AUTO_VCS:
		vcs_mode = mode;
		break;
	default:
		vcs_mode = NONE_VCS;
		break;
	}

	return vcs_mode;
}

void update_attrib_vcs_info(_adapter *padapter, struct xmit_frame *pxmitframe)
{
	u32 sz;
	struct pkt_attrib *pattrib = &pxmitframe->attrib;
	struct mlme_ext_priv *pmlmeext = &(padapter->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	#ifdef RTW_FORCE_CTS_TO_SELF_UNDER_LOW_RSSI
	s8 rssi = 0;
	struct sta_info *psta = pattrib->psta;
	#endif

	if (pattrib->nr_frags != 1)
		sz = padapter->xmitpriv.frag_len;
	else
		sz = pattrib->last_txcmdsz;

	if (pmlmeext->cur_wireless_mode < WIRELESS_11_24N ||
	    padapter->registrypriv.wifi_spec) {
		if (sz > padapter->registrypriv.rts_thresh)
			pattrib->vcs_mode = RTS_CTS;
		else if (pattrib->rtsen)
			pattrib->vcs_mode = RTS_CTS;
		else if (pattrib->cts2self)
			pattrib->vcs_mode = CTS_TO_SELF;
		else
			pattrib->vcs_mode = NONE_VCS;
	} else {
		while (_TRUE) {
			if ((pmlmeinfo->assoc_AP_vendor == HT_IOT_PEER_ATHEROS) &&
			    (pattrib->ampdu_en == _TRUE) &&
			    (padapter->securitypriv.dot11PrivacyAlgrthm == _AES_)) {
				pattrib->vcs_mode = CTS_TO_SELF;
				break;
			}

			if (pattrib->rtsen || pattrib->cts2self) {
				if (pattrib->rtsen)
					pattrib->vcs_mode = RTS_CTS;
				else if (pattrib->cts2self)
					pattrib->vcs_mode = CTS_TO_SELF;
				break;
			}

			if (pattrib->ht_en) {
				u8 HTOpMode = pmlmeinfo->HT_protection;

				if ((pmlmeext->cur_bwmode &&
				     (HTOpMode == 2 || HTOpMode == 3)) ||
				    (!pmlmeext->cur_bwmode && HTOpMode == 3)) {
					pattrib->vcs_mode = RTS_CTS;
					break;
				}
			}

			if (sz > padapter->registrypriv.rts_thresh) {
				pattrib->vcs_mode = RTS_CTS;
				break;
			}

			if ((pattrib->ampdu_en == _TRUE) &&
			    (!IS_HARDWARE_TYPE_8812(padapter))) {
				pattrib->vcs_mode = RTS_CTS;
				break;
			}

			pattrib->vcs_mode = NONE_VCS;
			break;
		}
		#ifdef RTW_FORCE_CTS_TO_SELF_UNDER_LOW_RSSI
		if (psta != NULL) {
			rssi = psta->cmn.rssi_stat.rssi;
			if ((rssi < 18) && (pattrib->vcs_mode == RTS_CTS))
				pattrib->vcs_mode = CTS_TO_SELF;
		}
		#endif
	}

	pattrib->vcs_mode = validate_vcs(padapter, pattrib->vcs_mode);

	if (padapter->driver_vcs_en == 1)
		pattrib->vcs_mode = padapter->driver_vcs_type;
}

#endif /* !CONFIG_RUST || HOST_XMIT_UPDATE_ATTRIB_TEST */

#if defined(CONFIG_RUST) && !defined(HOST_XMIT_UPDATE_ATTRIB_TEST)

struct rtw_rust_xmit_vcs_in {
	u8 cur_wireless_mode;
	u8 cur_bwmode;
	u8 assoc_ap_vendor;
	u8 ht_protection;
	u8 wifi_spec;
	u16 rts_thresh;
	u8 dot11_privacy;
	u8 vrtl_carrier_sense;
	u8 vcs_type;
	u8 driver_vcs_en;
	u8 driver_vcs_type;
	u8 is_hw_8812;
	u32 frag_len;
	u8 nr_frags;
	u32 last_txcmdsz;
	u8 rtsen;
	u8 cts2self;
	u8 ht_en;
	u8 ampdu_en;
	s8 psta_rssi;
};

void rtw_rust_xmit_attrib_vcs_gather(_adapter *padapter, struct xmit_frame *pxmitframe,
				     struct rtw_rust_xmit_vcs_in *out)
{
	struct pkt_attrib *pattrib = &pxmitframe->attrib;
	struct sta_info *psta = pattrib->psta;

	out->cur_wireless_mode = padapter->mlmeextpriv.cur_wireless_mode;
	out->cur_bwmode = padapter->mlmeextpriv.cur_bwmode;
	out->assoc_ap_vendor = padapter->mlmeextpriv.mlmext_info.assoc_AP_vendor;
	out->ht_protection = padapter->mlmeextpriv.mlmext_info.HT_protection;
	out->wifi_spec = padapter->registrypriv.wifi_spec;
	out->rts_thresh = padapter->registrypriv.rts_thresh;
	out->dot11_privacy = padapter->securitypriv.dot11PrivacyAlgrthm;
	out->vrtl_carrier_sense = padapter->registrypriv.vrtl_carrier_sense;
	out->vcs_type = padapter->registrypriv.vcs_type;
	out->driver_vcs_en = padapter->driver_vcs_en;
	out->driver_vcs_type = padapter->driver_vcs_type;
	out->is_hw_8812 = IS_HARDWARE_TYPE_8812(padapter) ? 1 : 0;
	out->frag_len = padapter->xmitpriv.frag_len;
	out->nr_frags = pattrib->nr_frags;
	out->last_txcmdsz = pattrib->last_txcmdsz;
	out->rtsen = pattrib->rtsen;
	out->cts2self = pattrib->cts2self;
	out->ht_en = pattrib->ht_en;
	out->ampdu_en = pattrib->ampdu_en;
	out->psta_rssi = (psta != NULL) ? psta->cmn.rssi_stat.rssi : (s8)-128;
}

void rtw_rust_xmit_attrib_vcs_set_mode(struct xmit_frame *pxmitframe, u8 mode)
{
	pxmitframe->attrib.vcs_mode = mode;
}

#endif /* CONFIG_RUST && !HOST_XMIT_UPDATE_ATTRIB_TEST */

#ifdef HOST_XMIT_UPDATE_ATTRIB_TEST

u8 rtw_get_tx_bw_mode(_adapter *adapter, struct sta_info *sta)
{
	(void)adapter;
	return sta->cmn.bw_mode;
}

u8 query_ra_short_GI(struct sta_info *psta, u8 bw)
{
	u8 sgi = _FALSE, sgi_20m = _FALSE, sgi_40m = _FALSE, sgi_80m = _FALSE;

#ifdef CONFIG_80211N_HT
#ifdef CONFIG_80211AC_VHT
	if (psta->vhtpriv.vht_option)
		sgi_80m = psta->vhtpriv.sgi_80m;
#endif
	sgi_20m = psta->htpriv.sgi_20m;
	sgi_40m = psta->htpriv.sgi_40m;
#endif

	switch (bw) {
	case CHANNEL_WIDTH_80:
		sgi = sgi_80m;
		break;
	case CHANNEL_WIDTH_40:
		sgi = sgi_40m;
		break;
	case CHANNEL_WIDTH_20:
	default:
		sgi = sgi_20m;
		break;
	}

	return sgi;
}

#endif /* HOST_XMIT_UPDATE_ATTRIB_TEST */

void update_attrib_phy_info(_adapter *padapter, struct pkt_attrib *pattrib,
			    struct sta_info *psta)
{
	struct mlme_ext_priv *mlmeext = &padapter->mlmeextpriv;
	u8 bw;

	pattrib->rtsen = psta->rtsen;
	pattrib->cts2self = psta->cts2self;

	pattrib->mdata = 0;
	pattrib->eosp = 0;
	pattrib->triggered = 0;
	pattrib->ampdu_spacing = 0;

	pattrib->raid = psta->cmn.ra_info.rate_id;

	bw = rtw_get_tx_bw_mode(padapter, psta);
	pattrib->bwmode = rtw_min(bw, mlmeext->cur_bwmode);
	pattrib->sgi = query_ra_short_GI(psta, pattrib->bwmode);

	pattrib->ldpc = psta->cmn.ldpc_en;
	pattrib->stbc = psta->cmn.stbc_en;

#ifdef CONFIG_80211N_HT
	if (padapter->registrypriv.ht_enable &&
	    is_supported_ht(padapter->registrypriv.wireless_mode)) {
		pattrib->ht_en = psta->htpriv.ht_option;
		pattrib->ch_offset = psta->htpriv.ch_offset;
		pattrib->ampdu_en = _FALSE;

		if (padapter->driver_ampdu_spacing != 0xFF)
			pattrib->ampdu_spacing = padapter->driver_ampdu_spacing;
		else
			pattrib->ampdu_spacing = psta->htpriv.rx_ampdu_min_spacing;

		if (pattrib->ht_en && psta->htpriv.ampdu_enable) {
			if (psta->htpriv.agg_enable_bitmap & BIT(pattrib->priority)) {
				pattrib->ampdu_en = _TRUE;
				if (psta->htpriv.tx_amsdu_enable == _TRUE)
					pattrib->amsdu_ampdu_en = _TRUE;
				else
					pattrib->amsdu_ampdu_en = _FALSE;
			}
		}
	}
#endif /* CONFIG_80211N_HT */

#ifdef CONFIG_TDLS
	if (pattrib->direct_link == _TRUE) {
		psta = pattrib->ptdls_sta;

		pattrib->raid = psta->cmn.ra_info.rate_id;
#ifdef CONFIG_80211N_HT
		if (padapter->registrypriv.ht_enable &&
		    is_supported_ht(padapter->registrypriv.wireless_mode)) {
			pattrib->bwmode = rtw_get_tx_bw_mode(padapter, psta);
			pattrib->ht_en = psta->htpriv.ht_option;
			pattrib->ch_offset = psta->htpriv.ch_offset;
			pattrib->sgi = query_ra_short_GI(psta, pattrib->bwmode);
		}
#endif /* CONFIG_80211N_HT */
	}
#endif /* CONFIG_TDLS */

	pattrib->retry_ctrl = _FALSE;
}
