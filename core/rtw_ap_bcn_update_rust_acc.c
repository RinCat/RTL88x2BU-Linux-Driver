// SPDX-License-Identifier: GPL-2.0
/* Kernel accessors for rust/rtw_ap_bcn_update_kern.rs (W3-81 PR3). */
#include <drv_types.h>
#include <ieee80211.h>
#include <hal_data.h>

#if defined(CONFIG_RUST_AP_BCN_UPDATE) && !defined(HOST_AP_BCN_UPDATE_TEST)

WLAN_BSSID_EX *rtw_rust_bcn_update_network(_adapter *adapter)
{
	return &adapter->mlmeextpriv.mlmext_info.network;
}

u8 rtw_rust_bcn_update_erp_enable(_adapter *adapter)
{
	return adapter->mlmeextpriv.mlmext_info.ERP_enable;
}

u8 rtw_rust_bcn_update_ht_info_enable(_adapter *adapter)
{
	return adapter->mlmeextpriv.mlmext_info.HT_info_enable;
}

int rtw_rust_bcn_update_num_sta_non_erp(_adapter *adapter)
{
	return adapter->mlmepriv.num_sta_non_erp;
}

int rtw_rust_bcn_update_num_sta_no_short_preamble(_adapter *adapter)
{
	return adapter->mlmepriv.num_sta_no_short_preamble;
}

u8 rtw_rust_bcn_update_ht_option(_adapter *adapter)
{
	return adapter->mlmepriv.htpriv.ht_option;
}

u16 rtw_rust_bcn_update_ht_op_mode(_adapter *adapter)
{
	return adapter->mlmepriv.ht_op_mode;
}

u8 *rtw_rust_bcn_update_wps_beacon_ie(_adapter *adapter)
{
	return adapter->mlmepriv.wps_beacon_ie;
}

u8 rtw_rust_bcn_update_sw_to_20mhz(_adapter *adapter)
{
	return adapter->mlmepriv.sw_to_20mhz;
}

void rtw_rust_bcn_update_set_sw_to_20mhz(_adapter *adapter, u8 v)
{
	adapter->mlmepriv.sw_to_20mhz = v;
}

int rtw_rust_bcn_update_num_sta_40mhz_intolerant(_adapter *adapter)
{
	return adapter->mlmepriv.num_sta_40mhz_intolerant;
}

u8 rtw_rust_bcn_update_ht_20mhz_width_req(_adapter *adapter)
{
	return adapter->mlmepriv.ht_20mhz_width_req;
}

u8 rtw_rust_bcn_update_ht_intolerant_ch_reported(_adapter *adapter)
{
	return adapter->mlmepriv.ht_intolerant_ch_reported;
}

u8 rtw_rust_bcn_update_olbc_is_true(_adapter *adapter)
{
	return ATOMIC_READ(&adapter->mlmepriv.olbc) == _TRUE ? 1 : 0;
}

u8 rtw_rust_bcn_update_cur_channel(_adapter *adapter)
{
	return adapter->mlmeextpriv.cur_channel;
}

u8 rtw_rust_bcn_update_cur_bwmode(_adapter *adapter)
{
	return adapter->mlmeextpriv.cur_bwmode;
}

u8 rtw_rust_bcn_update_cur_ch_offset(_adapter *adapter)
{
	return adapter->mlmeextpriv.cur_ch_offset;
}

#if defined(CONFIG_INTERRUPT_BASED_TXBCN) || defined(CONFIG_PCI_HCI)
void rtw_rust_bcn_update_wps_fwstate(_adapter *adapter, u8 *pwps_ie_src, u32 wps_ielen)
{
	struct mlme_ext_info *pmlmeinfo = &adapter->mlmeextpriv.mlmext_info;
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	u8 sr = 0;

	if ((pmlmeinfo->state & 0x03) != WIFI_FW_AP_STATE)
		return;

	rtw_get_wps_attr_content(pwps_ie_src, wps_ielen, WPS_ATTR_SELECTED_REGISTRAR,
				 (u8 *)(&sr), NULL);
	if (sr)
		set_fwstate(pmlmepriv, WIFI_UNDER_WPS);
	else
		clr_fwstate(pmlmepriv, WIFI_UNDER_WPS);
}
#endif

#endif /* CONFIG_RUST_AP_BCN_UPDATE && !HOST_AP_BCN_UPDATE_TEST */
