/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
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

#include <drv_types.h>
#include <hal_data.h>
#include <rtw_sreset.h>

#if !defined(CONFIG_RUST) || defined(HOST_SRESET_TEST)

void sreset_init_value(_adapter *padapter)
{
#if defined(DBG_CONFIG_ERROR_DETECT)
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	struct sreset_priv *psrtpriv = &pHalData->srestpriv;

	_rtw_mutex_init(&psrtpriv->silentreset_mutex);
	psrtpriv->silent_reset_inprogress = _FALSE;
	psrtpriv->Wifi_Error_Status = WIFI_STATUS_SUCCESS;
	psrtpriv->last_tx_time = 0;
	psrtpriv->last_tx_complete_time = 0;
#endif
}
void sreset_reset_value(_adapter *padapter)
{
#if defined(DBG_CONFIG_ERROR_DETECT)
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	struct sreset_priv *psrtpriv = &pHalData->srestpriv;

	psrtpriv->Wifi_Error_Status = WIFI_STATUS_SUCCESS;
	psrtpriv->last_tx_time = 0;
	psrtpriv->last_tx_complete_time = 0;
#endif
}

u8 sreset_get_wifi_status(_adapter *padapter)
{
#if defined(DBG_CONFIG_ERROR_DETECT)
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	struct sreset_priv *psrtpriv = &pHalData->srestpriv;
	u8 status = WIFI_STATUS_SUCCESS;
	u32 val32 = 0;

	if (psrtpriv->silent_reset_inprogress == _TRUE)
		return status;
	val32 = rtw_read32(padapter, REG_TXDMA_STATUS);
	if (val32 == 0xeaeaeaea)
		psrtpriv->Wifi_Error_Status = WIFI_IF_NOT_EXIST;
	else if (val32 != 0) {
		RTW_INFO("txdmastatu(%x)\n", val32);
		psrtpriv->Wifi_Error_Status = WIFI_MAC_TXDMA_ERROR;
	}

	if (WIFI_STATUS_SUCCESS != psrtpriv->Wifi_Error_Status) {
		RTW_INFO("==>%s error_status(0x%x)\n", __FUNCTION__, psrtpriv->Wifi_Error_Status);
		status = (psrtpriv->Wifi_Error_Status & (~(USB_READ_PORT_FAIL | USB_WRITE_PORT_FAIL)));
	}
	RTW_INFO("==> %s wifi_status(0x%x)\n", __FUNCTION__, status);

	/* status restore */
	psrtpriv->Wifi_Error_Status = WIFI_STATUS_SUCCESS;

	return status;
#else
	return WIFI_STATUS_SUCCESS;
#endif
}

void sreset_set_wifi_error_status(_adapter *padapter, u32 status)
{
#if defined(DBG_CONFIG_ERROR_DETECT)
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	pHalData->srestpriv.Wifi_Error_Status = status;
#endif
}

bool sreset_inprogress(_adapter *padapter)
{
#if defined(DBG_CONFIG_ERROR_RESET)
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	return pHalData->srestpriv.silent_reset_inprogress;
#else
	return _FALSE;
#endif
}

#endif /* !CONFIG_RUST || HOST_SRESET_TEST */

void sreset_set_trigger_point(_adapter *padapter, s32 tgp)
{
#if defined(DBG_CONFIG_ERROR_DETECT)
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	pHalData->srestpriv.dbg_trigger_point = tgp;
#endif
}

#if !defined(CONFIG_RUST) || defined(HOST_SRESET_TEST)

void sreset_restore_security_station(_adapter *padapter)
{
	struct mlme_priv *mlmepriv = &padapter->mlmepriv;
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct sta_info *psta;
	struct mlme_ext_info	*pmlmeinfo = &padapter->mlmeextpriv.mlmext_info;

	{
		u8 val8;

		if (pmlmeinfo->auth_algo == dot11AuthAlgrthm_8021X) {
			val8 = 0xcc;
#ifdef CONFIG_WAPI_SUPPORT
		} else if (padapter->wapiInfo.bWapiEnable && pmlmeinfo->auth_algo == dot11AuthAlgrthm_WAPI) {
			/* Disable TxUseDefaultKey, RxUseDefaultKey, RxBroadcastUseDefaultKey. */
			val8 = 0x4c;
#endif
		} else
			val8 = 0xcf;
		rtw_hal_set_hwreg(padapter, HW_VAR_SEC_CFG, (u8 *)(&val8));
	}

	if ((padapter->securitypriv.dot11PrivacyAlgrthm == _TKIP_) ||
	    (padapter->securitypriv.dot11PrivacyAlgrthm == _AES_)) {
		psta = rtw_get_stainfo(pstapriv, get_bssid(mlmepriv));
		if (psta == NULL) {
			/* DEBUG_ERR( ("Set wpa_set_encryption: Obtain Sta_info fail\n")); */
		} else {
			/* pairwise key */
			rtw_setstakey_cmd(padapter, psta, UNICAST_KEY, _FALSE);
			/* group key */
			rtw_set_key(padapter, &padapter->securitypriv, padapter->securitypriv.dot118021XGrpKeyid, 0, _FALSE);
		}
	}
}

#endif /* !CONFIG_RUST || HOST_SRESET_TEST */

#if defined(CONFIG_RUST) && !defined(HOST_SRESET_TEST)
void sreset_restore_security_station(_adapter *padapter);
#endif

void sreset_restore_network_station(_adapter *padapter)
{
	struct mlme_priv *mlmepriv = &padapter->mlmepriv;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	u8 doiqk = _FALSE;

	rtw_setopmode_cmd(padapter, Ndis802_11Infrastructure, RTW_CMDF_DIRECTLY);

	{
		u8 threshold;
#ifdef CONFIG_USB_HCI
		/* TH=1 => means that invalidate usb rx aggregation */
		/* TH=0 => means that validate usb rx aggregation, use init value. */
#ifdef CONFIG_80211N_HT
		if (mlmepriv->htpriv.ht_option) {
			if (padapter->registrypriv.wifi_spec == 1)
				threshold = 1;
			else
				threshold = 0;
			rtw_hal_set_hwreg(padapter, HW_VAR_RXDMA_AGG_PG_TH, (u8 *)(&threshold));
		} else {
			threshold = 1;
			rtw_hal_set_hwreg(padapter, HW_VAR_RXDMA_AGG_PG_TH, (u8 *)(&threshold));
		}
#endif /* CONFIG_80211N_HT */
#endif
	}

	doiqk = _TRUE;
	rtw_hal_set_hwreg(padapter, HW_VAR_DO_IQK , &doiqk);

	set_channel_bwmode(padapter, pmlmeext->cur_channel, pmlmeext->cur_ch_offset, pmlmeext->cur_bwmode);

	doiqk = _FALSE;
	rtw_hal_set_hwreg(padapter , HW_VAR_DO_IQK , &doiqk);
	/* disable dynamic functions, such as high power, DIG */
	/*rtw_phydm_func_disable_all(padapter);*/

	rtw_hal_set_hwreg(padapter, HW_VAR_BSSID, pmlmeinfo->network.MacAddress);

	{
		u8	join_type = 0;

		rtw_hal_rcr_set_chk_bssid(padapter, MLME_STA_CONNECTING);
		rtw_hal_set_hwreg(padapter, HW_VAR_MLME_JOIN, (u8 *)(&join_type));

		rtw_btcoex_connect_notify(padapter, join_type);
	}

	Set_MSR(padapter, (pmlmeinfo->state & 0x3));

	mlmeext_joinbss_event_callback(padapter, 1);
	/* restore Sequence No. */
	rtw_hal_set_hwreg(padapter, HW_VAR_RESTORE_HW_SEQ, 0);

	sreset_restore_security_station(padapter);
}


void sreset_restore_network_status(_adapter *padapter)
{
	struct mlme_priv *mlmepriv = &padapter->mlmepriv;

	if (check_fwstate(mlmepriv, WIFI_STATION_STATE)) {
		RTW_INFO(FUNC_ADPT_FMT" fwstate:0x%08x - WIFI_STATION_STATE\n", FUNC_ADPT_ARG(padapter), get_fwstate(mlmepriv));
		sreset_restore_network_station(padapter);
	}
#ifdef CONFIG_AP_MODE
	else if (MLME_IS_AP(padapter) || MLME_IS_MESH(padapter)) {
		RTW_INFO(FUNC_ADPT_FMT" %s\n", FUNC_ADPT_ARG(padapter), MLME_IS_AP(padapter) ? "AP" : "MESH");
		rtw_ap_restore_network(padapter);
	}
#endif
	else if (check_fwstate(mlmepriv, WIFI_ADHOC_STATE))
		RTW_INFO(FUNC_ADPT_FMT" fwstate:0x%08x - WIFI_ADHOC_STATE\n", FUNC_ADPT_ARG(padapter), get_fwstate(mlmepriv));
	else
		RTW_INFO(FUNC_ADPT_FMT" fwstate:0x%08x - ???\n", FUNC_ADPT_ARG(padapter), get_fwstate(mlmepriv));
}

#if !defined(CONFIG_RUST) || defined(HOST_SRESET_TEST)

void sreset_stop_adapter(_adapter *padapter)
{
	struct mlme_priv	*pmlmepriv = &(padapter->mlmepriv);
	struct xmit_priv	*pxmitpriv = &padapter->xmitpriv;

	if (padapter == NULL)
		return;

	RTW_INFO(FUNC_ADPT_FMT"\n", FUNC_ADPT_ARG(padapter));

	rtw_netif_stop_queue(padapter->pnetdev);

	rtw_cancel_all_timer(padapter);

	/* TODO: OS and HCI independent */
#if defined(PLATFORM_LINUX) && defined(CONFIG_USB_HCI)
	tasklet_kill(&pxmitpriv->xmit_tasklet);
#endif

	if (check_fwstate(pmlmepriv, WIFI_UNDER_SURVEY))
		rtw_scan_abort(padapter);

	if (check_fwstate(pmlmepriv, WIFI_UNDER_LINKING)) {
		rtw_set_to_roam(padapter, 0);
		rtw_join_timeout_handler(padapter);
	}

}

void sreset_start_adapter(_adapter *padapter)
{
	struct mlme_priv	*pmlmepriv = &(padapter->mlmepriv);
	struct xmit_priv	*pxmitpriv = &padapter->xmitpriv;

	if (padapter == NULL)
		return;

	RTW_INFO(FUNC_ADPT_FMT"\n", FUNC_ADPT_ARG(padapter));

	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE))
		sreset_restore_network_status(padapter);

	/* TODO: OS and HCI independent */
#if defined(PLATFORM_LINUX) && defined(CONFIG_USB_HCI)
	tasklet_hi_schedule(&pxmitpriv->xmit_tasklet);
#endif

	if (is_primary_adapter(padapter))
		_set_timer(&adapter_to_dvobj(padapter)->dynamic_chk_timer, 2000);

	rtw_netif_wake_queue(padapter->pnetdev);
}

#endif /* !CONFIG_RUST || HOST_SRESET_TEST */

#if !defined(CONFIG_RUST) || defined(HOST_SRESET_TEST)

void sreset_reset(_adapter *padapter)
{
#ifdef DBG_CONFIG_ERROR_RESET
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	struct sreset_priv *psrtpriv = &pHalData->srestpriv;
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	struct mlme_priv	*pmlmepriv = &(padapter->mlmepriv);
	struct xmit_priv	*pxmitpriv = &padapter->xmitpriv;
	_irqL irqL;
	systime start = rtw_get_current_time();
	struct dvobj_priv *psdpriv = padapter->dvobj;
	struct debug_priv *pdbgpriv = &psdpriv->drv_dbg;

	RTW_INFO("%s\n", __FUNCTION__);

	psrtpriv->Wifi_Error_Status = WIFI_STATUS_SUCCESS;


#ifdef CONFIG_LPS
	rtw_set_ps_mode(padapter, PS_MODE_ACTIVE, 0, 0, "SRESET");
#endif/* #ifdef CONFIG_LPS */

	_enter_pwrlock(&pwrpriv->lock);

	psrtpriv->silent_reset_inprogress = _TRUE;
	pwrpriv->change_rfpwrstate = rf_off;

	rtw_mi_sreset_adapter_hdl(padapter, _FALSE);/*sreset_stop_adapter*/
#ifdef CONFIG_IPS
	_ips_enter(padapter);
	_ips_leave(padapter);
#endif
#if defined(CONFIG_AP_MODE) && defined(CONFIG_CONCURRENT_MODE)
	rtw_mi_ap_info_restore(padapter);
#endif
	rtw_mi_sreset_adapter_hdl(padapter, _TRUE);/*sreset_start_adapter*/

	psrtpriv->silent_reset_inprogress = _FALSE;

	_exit_pwrlock(&pwrpriv->lock);

	RTW_INFO("%s done in %d ms\n", __FUNCTION__, rtw_get_passing_time_ms(start));
	pdbgpriv->dbg_sreset_cnt++;

	psrtpriv->self_dect_fw = _FALSE;
	psrtpriv->rx_cnt = 0;
#endif
}

#endif /* !CONFIG_RUST || HOST_SRESET_TEST */

#if defined(CONFIG_RUST) && !defined(HOST_SRESET_TEST)

void rtw_rust_sreset_mutex_init(_adapter *padapter)
{
#if defined(DBG_CONFIG_ERROR_DETECT)
	struct sreset_priv *psrtpriv = &GET_HAL_DATA(padapter)->srestpriv;

	_rtw_mutex_init(&psrtpriv->silentreset_mutex);
#else
	(void)padapter;
#endif
}

u8 *rtw_rust_sreset_silent_inprogress_ptr(_adapter *padapter)
{
	return &GET_HAL_DATA(padapter)->srestpriv.silent_reset_inprogress;
}

u8 *rtw_rust_sreset_wifi_error_status_ptr(_adapter *padapter)
{
	return &GET_HAL_DATA(padapter)->srestpriv.Wifi_Error_Status;
}

systime *rtw_rust_sreset_last_tx_time_ptr(_adapter *padapter)
{
	return &GET_HAL_DATA(padapter)->srestpriv.last_tx_time;
}

systime *rtw_rust_sreset_last_tx_complete_time_ptr(_adapter *padapter)
{
	return &GET_HAL_DATA(padapter)->srestpriv.last_tx_complete_time;
}

u32 rtw_rust_sreset_read32(_adapter *padapter, u32 addr)
{
	return rtw_read32(padapter, addr);
}

u8 rtw_rust_sreset_check_fwstate(_adapter *padapter, u32 state)
{
	return check_fwstate(&padapter->mlmepriv, state) ? 1 : 0;
}

void rtw_rust_sreset_netif_stop_queue(_adapter *padapter)
{
	rtw_netif_stop_queue(padapter->pnetdev);
}

void rtw_rust_sreset_netif_wake_queue(_adapter *padapter)
{
	rtw_netif_wake_queue(padapter->pnetdev);
}

void rtw_rust_sreset_cancel_all_timer(_adapter *padapter)
{
	rtw_cancel_all_timer(padapter);
}

void rtw_rust_sreset_tasklet_kill(_adapter *padapter)
{
#if defined(PLATFORM_LINUX) && defined(CONFIG_USB_HCI)
	tasklet_kill(&padapter->xmitpriv.xmit_tasklet);
#else
	(void)padapter;
#endif
}

void rtw_rust_sreset_tasklet_hi_schedule(_adapter *padapter)
{
#if defined(PLATFORM_LINUX) && defined(CONFIG_USB_HCI)
	tasklet_hi_schedule(&padapter->xmitpriv.xmit_tasklet);
#else
	(void)padapter;
#endif
}

void rtw_rust_sreset_scan_abort(_adapter *padapter)
{
	rtw_scan_abort(padapter);
}

void rtw_rust_sreset_set_to_roam(_adapter *padapter, u8 to_roam)
{
	rtw_set_to_roam(padapter, to_roam);
}

void rtw_rust_sreset_join_timeout_handler(_adapter *padapter)
{
	rtw_join_timeout_handler(padapter);
}

void rtw_rust_sreset_restore_network_status(_adapter *padapter)
{
	sreset_restore_network_status(padapter);
}

void rtw_rust_sreset_set_dynamic_chk_timer(_adapter *padapter, u32 ms)
{
	_set_timer(&adapter_to_dvobj(padapter)->dynamic_chk_timer, ms);
}

u8 rtw_rust_sreset_is_primary_adapter(_adapter *padapter)
{
	return is_primary_adapter(padapter) ? 1 : 0;
}

void rtw_rust_sreset_set_ps_mode_active(_adapter *padapter)
{
#ifdef CONFIG_LPS
	rtw_set_ps_mode(padapter, PS_MODE_ACTIVE, 0, 0, "SRESET");
#else
	(void)padapter;
#endif
}
void rtw_rust_sreset_enter_pwrlock(_adapter *padapter)
{
	_enter_pwrlock(&adapter_to_pwrctl(padapter)->lock);
}
void rtw_rust_sreset_exit_pwrlock(_adapter *padapter)
{
	_exit_pwrlock(&adapter_to_pwrctl(padapter)->lock);
}
void rtw_rust_sreset_mi_adapter_hdl(_adapter *padapter, u8 bstart)
{
	rtw_mi_sreset_adapter_hdl(padapter, bstart);
}
void rtw_rust_sreset_ips_enter(_adapter *padapter)
{
#ifdef CONFIG_IPS
	_ips_enter(padapter);
#else
	(void)padapter;
#endif
}
void rtw_rust_sreset_ips_leave(_adapter *padapter)
{
#ifdef CONFIG_IPS
	_ips_leave(padapter);
#else
	(void)padapter;
#endif
}
void rtw_rust_sreset_ap_info_restore(_adapter *padapter)
{
#if defined(CONFIG_AP_MODE) && defined(CONFIG_CONCURRENT_MODE)
	rtw_mi_ap_info_restore(padapter);
#else
	(void)padapter;
#endif
}
u8 rtw_rust_sreset_error_reset_enabled(void)
{
#ifdef DBG_CONFIG_ERROR_RESET
	return 1;
#else
	return 0;
#endif
}
rt_rf_power_state *rtw_rust_sreset_change_rfpwrstate_ptr(_adapter *padapter)
{
	return &adapter_to_pwrctl(padapter)->change_rfpwrstate;
}
u32 *rtw_rust_sreset_dbg_sreset_cnt_ptr(_adapter *padapter)
{
	return &adapter_to_dvobj(padapter)->drv_dbg.dbg_sreset_cnt;
}
u8 *rtw_rust_sreset_self_dect_fw_ptr(_adapter *padapter)
{
	return &GET_HAL_DATA(padapter)->srestpriv.self_dect_fw;
}
u8 *rtw_rust_sreset_rx_cnt_ptr(_adapter *padapter)
{
	return &GET_HAL_DATA(padapter)->srestpriv.rx_cnt;
}

u32 rtw_rust_sreset_auth_algo(_adapter *padapter)
{
	return padapter->mlmeextpriv.mlmext_info.auth_algo;
}

u8 rtw_rust_sreset_sec_cfg_val8(_adapter *padapter)
{
	struct mlme_ext_info *pmlmeinfo = &padapter->mlmeextpriv.mlmext_info;

	if (pmlmeinfo->auth_algo == dot11AuthAlgrthm_8021X)
		return 0xcc;
#ifdef CONFIG_WAPI_SUPPORT
	if (padapter->wapiInfo.bWapiEnable && pmlmeinfo->auth_algo == dot11AuthAlgrthm_WAPI)
		return 0x4c;
#endif
	return 0xcf;
}

u32 rtw_rust_sreset_privacy_algrthm(_adapter *padapter)
{
	return padapter->securitypriv.dot11PrivacyAlgrthm;
}

u8 rtw_rust_sreset_grp_keyid(_adapter *padapter)
{
	return padapter->securitypriv.dot118021XGrpKeyid;
}

void rtw_rust_sreset_hal_set_hwreg_sec_cfg(_adapter *padapter, u8 val)
{
	rtw_hal_set_hwreg(padapter, HW_VAR_SEC_CFG, &val);
}

u8 rtw_rust_sreset_get_stainfo(_adapter *padapter)
{
	struct sta_info *psta;

	psta = rtw_get_stainfo(&padapter->stapriv, get_bssid(&padapter->mlmepriv));
	return psta != NULL ? 1 : 0;
}

void rtw_rust_sreset_setstakey_unicast(_adapter *padapter)
{
	struct sta_info *psta;

	psta = rtw_get_stainfo(&padapter->stapriv, get_bssid(&padapter->mlmepriv));
	if (psta != NULL)
		rtw_setstakey_cmd(padapter, psta, UNICAST_KEY, _FALSE);
}

void rtw_rust_sreset_set_group_key(_adapter *padapter)
{
	rtw_set_key(padapter, &padapter->securitypriv,
		    padapter->securitypriv.dot118021XGrpKeyid, 0, _FALSE);
}

#endif /* CONFIG_RUST && !HOST_SRESET_TEST */
