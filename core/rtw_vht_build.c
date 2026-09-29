/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 *****************************************************************************/
#define _RTW_VHT_BUILD_C_

#ifdef HOST_VHT_BUILD_TEST
#include "host_vht_build_types.h"
#else
#include <drv_types.h>
#include <hal_data.h>
#endif

#ifdef CONFIG_80211AC_VHT

#if !defined(CONFIG_RUST_VHT_BUILD) || defined(HOST_VHT_BUILD_TEST)

#ifndef HOST_VHT_BUILD_TEST
extern const u16 VHT_MCS_DATA_RATE[3][2][40];
#endif

u32 rtw_build_vht_cap_ie(_adapter *padapter, u8 *pbuf)
{
	u8	bw, rf_num, rx_stbc_nss = 0;
	u16	HighestRate;
	u8	*pcap, *pcap_mcs;
	u32	len = 0;
	u32 rx_packet_offset, max_recvbuf_sz;
	struct registry_priv *pregistrypriv = &padapter->registrypriv;
	struct mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	struct vht_priv	*pvhtpriv = &pmlmepriv->vhtpriv;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
#ifdef HOST_VHT_BUILD_TEST
	(void)pmlmeext;
	(void)pmlmeinfo;
	(void)rf_num;
#endif

	pcap = pvhtpriv->vht_cap;
	_rtw_memset(pcap, 0, 32);

	/* B0 B1 Maximum MPDU Length */
	rtw_hal_get_def_var(padapter, HAL_DEF_RX_PACKET_OFFSET, &rx_packet_offset);
	rtw_hal_get_def_var(padapter, HAL_DEF_MAX_RECVBUF_SZ, &max_recvbuf_sz);

	RTW_DBG("%s, line%d, Available RX buf size = %d bytes\n", __FUNCTION__, __LINE__, max_recvbuf_sz - rx_packet_offset);

	if ((max_recvbuf_sz - rx_packet_offset) >= 11454) {
		SET_VHT_CAPABILITY_ELE_MAX_MPDU_LENGTH(pcap, 2);
		RTW_INFO("%s, line%d, Set MAX MPDU len = 11454 bytes\n", __FUNCTION__, __LINE__);
	} else if ((max_recvbuf_sz - rx_packet_offset) >= 7991) {
		SET_VHT_CAPABILITY_ELE_MAX_MPDU_LENGTH(pcap, 1);
		RTW_INFO("%s, line%d, Set MAX MPDU len = 7991 bytes\n", __FUNCTION__, __LINE__);
	} else if ((max_recvbuf_sz - rx_packet_offset) >= 3895) {
		SET_VHT_CAPABILITY_ELE_MAX_MPDU_LENGTH(pcap, 0);
		RTW_INFO("%s, line%d, Set MAX MPDU len = 3895 bytes\n", __FUNCTION__, __LINE__);
	} else
		RTW_ERR("%s, line%d, Error!! Available RX buf size < 3895 bytes\n", __FUNCTION__, __LINE__);

	/* B2 B3 Supported Channel Width Set */
	if (hal_chk_bw_cap(padapter, BW_CAP_160M) && REGSTY_IS_BW_5G_SUPPORT(pregistrypriv, CHANNEL_WIDTH_160)) {
		if (hal_chk_bw_cap(padapter, BW_CAP_80_80M) && REGSTY_IS_BW_5G_SUPPORT(pregistrypriv, CHANNEL_WIDTH_80_80))
			SET_VHT_CAPABILITY_ELE_CHL_WIDTH(pcap, 2);
		else
			SET_VHT_CAPABILITY_ELE_CHL_WIDTH(pcap, 1);
	} else
		SET_VHT_CAPABILITY_ELE_CHL_WIDTH(pcap, 0);

	/* B4 Rx LDPC */
	if (TEST_FLAG(pvhtpriv->ldpc_cap, LDPC_VHT_ENABLE_RX)) {
		SET_VHT_CAPABILITY_ELE_RX_LDPC(pcap, 1);
		RTW_INFO("[VHT] Declare supporting RX LDPC\n");
	}

	/* B5 ShortGI for 80MHz */
	SET_VHT_CAPABILITY_ELE_SHORT_GI80M(pcap, pvhtpriv->sgi_80m ? 1 : 0); /* We can receive Short GI of 80M */
	if (pvhtpriv->sgi_80m)
		RTW_INFO("[VHT] Declare supporting SGI 80MHz\n");

	/* B6 ShortGI for 160MHz */
	/* SET_VHT_CAPABILITY_ELE_SHORT_GI160M(pcap, pvhtpriv->sgi_80m? 1 : 0); */

	/* B7 Tx STBC */
	if (TEST_FLAG(pvhtpriv->stbc_cap, STBC_VHT_ENABLE_TX)) {
		SET_VHT_CAPABILITY_ELE_TX_STBC(pcap, 1);
		RTW_INFO("[VHT] Declare supporting TX STBC\n");
	}

	/* B8 B9 B10 Rx STBC */
	if (TEST_FLAG(pvhtpriv->stbc_cap, STBC_VHT_ENABLE_RX)) {
		rtw_hal_get_def_var(padapter, HAL_DEF_RX_STBC, (u8 *)(&rx_stbc_nss));

		SET_VHT_CAPABILITY_ELE_RX_STBC(pcap, rx_stbc_nss);
		RTW_INFO("[VHT] Declare supporting RX STBC = %d\n", rx_stbc_nss);
	}
	#ifdef CONFIG_BEAMFORMING
	/* B11 SU Beamformer Capable */
	if (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMER_ENABLE)) {
		SET_VHT_CAPABILITY_ELE_SU_BFER(pcap, 1);
		RTW_INFO("[VHT] Declare supporting SU Bfer\n");
		/* B16 17 18 Number of Sounding Dimensions */
		rtw_hal_get_def_var(padapter, HAL_DEF_BEAMFORMER_CAP, (u8 *)&rf_num);
		SET_VHT_CAPABILITY_ELE_SOUNDING_DIMENSIONS(pcap, rf_num);
		/* B19 MU Beamformer Capable */
		if (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_MU_MIMO_AP_ENABLE)) {
			SET_VHT_CAPABILITY_ELE_MU_BFER(pcap, 1);
			RTW_INFO("[VHT] Declare supporting MU Bfer\n");
		}
	}

	/* B12 SU Beamformee Capable */
	if (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMEE_ENABLE)) {
		SET_VHT_CAPABILITY_ELE_SU_BFEE(pcap, 1);
		RTW_INFO("[VHT] Declare supporting SU Bfee\n");

		rtw_hal_get_def_var(padapter, HAL_DEF_BEAMFORMEE_CAP, (u8 *)&rf_num);

		/* IOT action suggested by Yu Chen 2017/3/3 */
#ifdef CONFIG_80211AC_VHT
		if ((pmlmeinfo->assoc_AP_vendor == HT_IOT_PEER_BROADCOM) &&
			!pvhtpriv->ap_bf_cap.is_mu_bfer &&
			pvhtpriv->ap_bf_cap.su_sound_dim == 2)
			rf_num = (rf_num >= 2 ? 2 : rf_num);
#endif
		/* B13 14 15 Compressed Steering Number of Beamformer Antennas Supported */
		SET_VHT_CAPABILITY_ELE_BFER_ANT_SUPP(pcap, rf_num);
		/* B20 SU Beamformee Capable */
		if (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_MU_MIMO_STA_ENABLE)) {
			SET_VHT_CAPABILITY_ELE_MU_BFEE(pcap, 1);
			RTW_INFO("[VHT] Declare supporting MU Bfee\n");
		}
	}
	#endif/*CONFIG_BEAMFORMING*/

	/* B21 VHT TXOP PS */
	SET_VHT_CAPABILITY_ELE_TXOP_PS(pcap, 0);
	/* B22 +HTC-VHT Capable */
	SET_VHT_CAPABILITY_ELE_HTC_VHT(pcap, 1);
	/* B23 24 25 Maximum A-MPDU Length Exponent */
	if (pregistrypriv->ampdu_factor != 0xFE)
		SET_VHT_CAPABILITY_ELE_MAX_RXAMPDU_FACTOR(pcap, pregistrypriv->ampdu_factor);
	else
		SET_VHT_CAPABILITY_ELE_MAX_RXAMPDU_FACTOR(pcap, 7);
	/* B26 27 VHT Link Adaptation Capable */
	SET_VHT_CAPABILITY_ELE_LINK_ADAPTION(pcap, 0);

	pcap_mcs = GET_VHT_CAPABILITY_ELE_RX_MCS(pcap);
	_rtw_memcpy(pcap_mcs, pvhtpriv->vht_mcs_map, 2);

	pcap_mcs = GET_VHT_CAPABILITY_ELE_TX_MCS(pcap);
	_rtw_memcpy(pcap_mcs, pvhtpriv->vht_mcs_map, 2);

	/* find the largest bw supported by both registry and hal */
	bw = hal_largest_bw(padapter, REGSTY_BW_5G(pregistrypriv));

	HighestRate = VHT_MCS_DATA_RATE[bw][pvhtpriv->sgi_80m][((pvhtpriv->vht_highest_rate - MGN_VHT1SS_MCS0) & 0x3f)];
	HighestRate = (HighestRate + 1) >> 1;

	SET_VHT_CAPABILITY_ELE_MCS_RX_HIGHEST_RATE(pcap, HighestRate); /* indicate we support highest rx rate is 600Mbps. */
	SET_VHT_CAPABILITY_ELE_MCS_TX_HIGHEST_RATE(pcap, HighestRate); /* indicate we support highest tx rate is 600Mbps. */

	pbuf = rtw_set_ie(pbuf, EID_VHTCapability, 12, pcap, &len);

	return len;
}

u32 rtw_build_vht_operation_ie(_adapter *padapter, u8 *pbuf, u8 channel)
{
	struct registry_priv *pregistrypriv = &padapter->registrypriv;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct vht_priv *pvhtpriv = &pmlmepriv->vhtpriv;
	u8 ChnlWidth, center_freq, bw_mode;
	u32 len = 0;
	u8 operation[5];

	_rtw_memset(operation, 0, 5);

	bw_mode = REGSTY_BW_5G(pregistrypriv);

	if (hal_chk_bw_cap(padapter, BW_CAP_80M | BW_CAP_160M)
	    && REGSTY_BW_5G(pregistrypriv) >= CHANNEL_WIDTH_80) {
		center_freq = rtw_get_center_ch(channel, bw_mode, HAL_PRIME_CHNL_OFFSET_LOWER);
		ChnlWidth = 1;
	} else {
		center_freq = 0;
		ChnlWidth = 0;
	}

	SET_VHT_OPERATION_ELE_CHL_WIDTH(operation, ChnlWidth);
	SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ1(operation, center_freq);
	SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ2(operation, 0);

	_rtw_memcpy(operation + 3, pvhtpriv->vht_mcs_map, 2);

	rtw_set_ie(pbuf, EID_VHTOperation, 5, operation, &len);

	return len;
}

#endif /* !CONFIG_RUST_VHT_BUILD || HOST_VHT_BUILD_TEST */

#if !defined(HOST_VHT_BUILD_TEST) || defined(HOST_VHT_CAPS_HANDLER_TEST)

void VHT_caps_handler(_adapter *padapter, PNDIS_802_11_VARIABLE_IEs pIE)
{
	struct mlme_priv		*pmlmepriv = &padapter->mlmepriv;
	struct vht_priv		*pvhtpriv = &pmlmepriv->vhtpriv;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	u8	cur_ldpc_cap = 0, cur_stbc_cap = 0, rx_nss = 0;
#ifdef CONFIG_BEAMFORMING
	u16	cur_beamform_cap = 0;
#endif
	u8	*pcap_mcs;

	if (pIE == NULL)
		return;

	if (pvhtpriv->vht_option == _FALSE)
		return;

	pmlmeinfo->VHT_enable = 1;

	/* B4 Rx LDPC */
	if (TEST_FLAG(pvhtpriv->ldpc_cap, LDPC_VHT_ENABLE_TX) &&
	    GET_VHT_CAPABILITY_ELE_RX_LDPC(pIE->data)) {
		SET_FLAG(cur_ldpc_cap, (LDPC_VHT_ENABLE_TX | LDPC_VHT_CAP_TX));
		RTW_INFO("Current VHT LDPC Setting = %02X\n", cur_ldpc_cap);
	}
	pvhtpriv->ldpc_cap = cur_ldpc_cap;

	/* B5 Short GI for 80 MHz */
	pvhtpriv->sgi_80m = (GET_VHT_CAPABILITY_ELE_SHORT_GI80M(pIE->data) & pvhtpriv->sgi_80m) ? _TRUE : _FALSE;
	/* RTW_INFO("Current ShortGI80MHz = %d\n", pvhtpriv->sgi_80m); */

	/* B8 B9 B10 Rx STBC */
	if (TEST_FLAG(pvhtpriv->stbc_cap, STBC_VHT_ENABLE_TX) &&
	    GET_VHT_CAPABILITY_ELE_RX_STBC(pIE->data)) {
		SET_FLAG(cur_stbc_cap, (STBC_VHT_ENABLE_TX | STBC_VHT_CAP_TX));
		RTW_INFO("Current VHT STBC Setting = %02X\n", cur_stbc_cap);
	}
	pvhtpriv->stbc_cap = cur_stbc_cap;
#ifdef CONFIG_BEAMFORMING
#ifdef RTW_BEAMFORMING_VERSION_2
	/*
	 * B11 SU Beamformer Capable,
	 * the target supports Beamformer and we are Beamformee
	 */
	if (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMEE_ENABLE)
	    && GET_VHT_CAPABILITY_ELE_SU_BFER(pIE->data)) {
		SET_FLAG(cur_beamform_cap, BEAMFORMING_VHT_BEAMFORMEE_ENABLE);

		/* Shift to BEAMFORMING_VHT_BEAMFORMEE_STS_CAP */
		SET_FLAG(cur_beamform_cap, GET_VHT_CAPABILITY_ELE_SU_BFEE_STS_CAP(pIE->data) << 8);

		/*
		 * B19 MU Beamformer Capable,
		 * the target supports Beamformer and we are Beamformee
		 */
		if (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_MU_MIMO_STA_ENABLE)
		    && GET_VHT_CAPABILITY_ELE_MU_BFER(pIE->data))
			SET_FLAG(cur_beamform_cap, BEAMFORMING_VHT_MU_MIMO_STA_ENABLE);
	}

	/*
	 * B12 SU Beamformee Capable,
	 * the target supports Beamformee and we are Beamformer
	 */
	if (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMER_ENABLE)
	    && GET_VHT_CAPABILITY_ELE_SU_BFEE(pIE->data)) {
		SET_FLAG(cur_beamform_cap, BEAMFORMING_VHT_BEAMFORMER_ENABLE);

		/* Shit to BEAMFORMING_VHT_BEAMFORMER_SOUND_DIM */
		SET_FLAG(cur_beamform_cap, GET_VHT_CAPABILITY_ELE_SU_BFER_SOUND_DIM_NUM(pIE->data) << 12);

		/*
		 * B20 MU Beamformee Capable,
		 * the target supports Beamformee and we are Beamformer
		 */
		if (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_MU_MIMO_AP_ENABLE)
		    && GET_VHT_CAPABILITY_ELE_MU_BFEE(pIE->data))
			SET_FLAG(cur_beamform_cap, BEAMFORMING_VHT_MU_MIMO_AP_ENABLE);
	}

	pvhtpriv->beamform_cap = cur_beamform_cap;
	RTW_INFO("Current VHT Beamforming Setting=0x%04X\n", cur_beamform_cap);
#else /* !RTW_BEAMFORMING_VERSION_2 */
	/* B11 SU Beamformer Capable, the target supports Beamformer and we are Beamformee */
	if (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMER_ENABLE) &&
	    GET_VHT_CAPABILITY_ELE_SU_BFEE(pIE->data)) {
		SET_FLAG(cur_beamform_cap, BEAMFORMING_VHT_BEAMFORMEE_ENABLE);
		/*Shift to BEAMFORMING_VHT_BEAMFORMER_STS_CAP*/
		SET_FLAG(cur_beamform_cap, GET_VHT_CAPABILITY_ELE_SU_BFEE_STS_CAP(pIE->data) << 8);
	}

	/* B12 SU Beamformee Capable, the target supports Beamformee and we are Beamformer */
	if (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMEE_ENABLE) &&
	    GET_VHT_CAPABILITY_ELE_SU_BFER(pIE->data)) {
		SET_FLAG(cur_beamform_cap, BEAMFORMING_VHT_BEAMFORMER_ENABLE);
		/*Shit to BEAMFORMING_VHT_BEAMFORMEE_SOUND_DIM*/
		SET_FLAG(cur_beamform_cap, GET_VHT_CAPABILITY_ELE_SU_BFER_SOUND_DIM_NUM(pIE->data) << 12);

	}
	pvhtpriv->beamform_cap = cur_beamform_cap;
	if (cur_beamform_cap)
		RTW_INFO("Current VHT Beamforming Setting = %02X\n", cur_beamform_cap);
#endif /* !RTW_BEAMFORMING_VERSION_2 */
#endif /* CONFIG_BEAMFORMING */
	/* B23 B24 B25 Maximum A-MPDU Length Exponent */
	pvhtpriv->ampdu_len = GET_VHT_CAPABILITY_ELE_MAX_RXAMPDU_FACTOR(pIE->data);

	pcap_mcs = GET_VHT_CAPABILITY_ELE_RX_MCS(pIE->data);
	rx_nss = GET_HAL_RX_NSS(padapter);
	rtw_vht_nss_to_mcsmap(rx_nss, pvhtpriv->vht_mcs_map, pcap_mcs);
	pvhtpriv->vht_highest_rate = rtw_get_vht_highest_rate(pvhtpriv->vht_mcs_map);
}

#endif /* !HOST_VHT_BUILD_TEST || HOST_VHT_CAPS_HANDLER_TEST */

#ifndef HOST_VHT_BUILD_TEST
void rtw_vht_use_default_setting(_adapter *padapter)
{
	struct mlme_priv		*pmlmepriv = &padapter->mlmepriv;
	struct vht_priv		*pvhtpriv = &pmlmepriv->vhtpriv;
	struct registry_priv	*pregistrypriv = &padapter->registrypriv;
	BOOLEAN		bHwLDPCSupport = _FALSE, bHwSTBCSupport = _FALSE;
#ifdef CONFIG_BEAMFORMING
	BOOLEAN		bHwSupportBeamformer = _FALSE, bHwSupportBeamformee = _FALSE;
	u8	mu_bfer, mu_bfee;
#endif /* CONFIG_BEAMFORMING */
	u8 tx_nss, rx_nss;
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	pvhtpriv->sgi_80m = TEST_FLAG(pregistrypriv->short_gi, BIT2) ? _TRUE : _FALSE;

	/* LDPC support */
	rtw_hal_get_def_var(padapter, HAL_DEF_RX_LDPC, (u8 *)&bHwLDPCSupport);
	CLEAR_FLAGS(pvhtpriv->ldpc_cap);
	if (bHwLDPCSupport) {
		if (TEST_FLAG(pregistrypriv->ldpc_cap, BIT0))
			SET_FLAG(pvhtpriv->ldpc_cap, LDPC_VHT_ENABLE_RX);
	}
	rtw_hal_get_def_var(padapter, HAL_DEF_TX_LDPC, (u8 *)&bHwLDPCSupport);
	if (bHwLDPCSupport) {
		if (TEST_FLAG(pregistrypriv->ldpc_cap, BIT1))
			SET_FLAG(pvhtpriv->ldpc_cap, LDPC_VHT_ENABLE_TX);
	}
	if (pvhtpriv->ldpc_cap)
		RTW_INFO("[VHT] Support LDPC = 0x%02X\n", pvhtpriv->ldpc_cap);

	/* STBC */
	rtw_hal_get_def_var(padapter, HAL_DEF_TX_STBC, (u8 *)&bHwSTBCSupport);
	CLEAR_FLAGS(pvhtpriv->stbc_cap);
	if (bHwSTBCSupport) {
		if (TEST_FLAG(pregistrypriv->stbc_cap, BIT1))
			SET_FLAG(pvhtpriv->stbc_cap, STBC_VHT_ENABLE_TX);
	}
	rtw_hal_get_def_var(padapter, HAL_DEF_RX_STBC, (u8 *)&bHwSTBCSupport);
	if (bHwSTBCSupport) {
		if (TEST_FLAG(pregistrypriv->stbc_cap, BIT0))
			SET_FLAG(pvhtpriv->stbc_cap, STBC_VHT_ENABLE_RX);
	}
	if (pvhtpriv->stbc_cap)
		RTW_INFO("[VHT] Support STBC = 0x%02X\n", pvhtpriv->stbc_cap);

	/* Beamforming setting */
	CLEAR_FLAGS(pvhtpriv->beamform_cap);
#ifdef CONFIG_BEAMFORMING
#ifdef RTW_BEAMFORMING_VERSION_2
#ifdef CONFIG_CONCURRENT_MODE
	/* only enable beamforming in STA client mode */
	if (MLME_IS_STA(padapter) && !MLME_IS_GC(padapter))
#else
	if ((MLME_IS_AP(padapter) && !MLME_IS_GO(padapter)) ||
	    (MLME_IS_STA(padapter) && !MLME_IS_GC(padapter)))
#endif
#endif
	{
		rtw_hal_get_def_var(padapter, HAL_DEF_EXPLICIT_BEAMFORMER,
			(u8 *)&bHwSupportBeamformer);
		rtw_hal_get_def_var(padapter, HAL_DEF_EXPLICIT_BEAMFORMEE,
			(u8 *)&bHwSupportBeamformee);
		mu_bfer = _FALSE;
		mu_bfee = _FALSE;
		rtw_hal_get_def_var(padapter, HAL_DEF_VHT_MU_BEAMFORMER, &mu_bfer);
		rtw_hal_get_def_var(padapter, HAL_DEF_VHT_MU_BEAMFORMEE, &mu_bfee);
		if (TEST_FLAG(pregistrypriv->beamform_cap, BIT0) && bHwSupportBeamformer) {
#ifdef CONFIG_CONCURRENT_MODE
			if ((pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE) {
				SET_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMER_ENABLE);
				RTW_INFO("[VHT] CONCURRENT AP Support Beamformer\n");
				if (TEST_FLAG(pregistrypriv->beamform_cap, BIT(2))
				    && (_TRUE == mu_bfer)) {
					SET_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_MU_MIMO_AP_ENABLE);
					RTW_INFO("[VHT] Support MU-MIMO AP\n");
				}
			} else
				RTW_INFO("[VHT] CONCURRENT not AP ;not allow  Support Beamformer\n");
#else
			SET_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMER_ENABLE);
			RTW_INFO("[VHT] Support Beamformer\n");
			if (TEST_FLAG(pregistrypriv->beamform_cap, BIT(2))
			    && (_TRUE == mu_bfer)
			    && ((pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE)) {
				SET_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_MU_MIMO_AP_ENABLE);
				RTW_INFO("[VHT] Support MU-MIMO AP\n");
			}
#endif
		}
		if (TEST_FLAG(pregistrypriv->beamform_cap, BIT1) && bHwSupportBeamformee) {
			SET_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMEE_ENABLE);
			RTW_INFO("[VHT] Support Beamformee\n");
			if (TEST_FLAG(pregistrypriv->beamform_cap, BIT(3))
			    && (_TRUE == mu_bfee)
			    && ((pmlmeinfo->state & 0x03) != WIFI_FW_AP_STATE)) {
				SET_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_MU_MIMO_STA_ENABLE);
				RTW_INFO("[VHT] Support MU-MIMO STA\n");
			}
		}
	}
#endif /* CONFIG_BEAMFORMING */

	pvhtpriv->ampdu_len = pregistrypriv->ampdu_factor;

	tx_nss = GET_HAL_TX_NSS(padapter);
	rx_nss = GET_HAL_RX_NSS(padapter);

	/* for now, vhtpriv.vht_mcs_map comes from RX NSS */
	rtw_vht_nss_to_mcsmap(rx_nss, pvhtpriv->vht_mcs_map, pregistrypriv->vht_rx_mcs_map);
	pvhtpriv->vht_highest_rate = rtw_get_vht_highest_rate(pvhtpriv->vht_mcs_map);
}
#endif /* !HOST_VHT_BUILD_TEST */

#endif /* CONFIG_80211AC_VHT */
