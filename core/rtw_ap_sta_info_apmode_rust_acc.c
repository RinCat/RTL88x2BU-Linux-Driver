// SPDX-License-Identifier: GPL-2.0
/* Kernel accessors for rust/rtw_ap_sta_info_apmode.rs (W3-83 PR15). */
#include <drv_types.h>
#include <hal_data.h>

#if defined(CONFIG_RUST_AP_STA_INFO_APMODE) && !defined(HOST_AP_STA_INFO_APMODE_TEST)

struct rtw_rust_apmode_ht_in {
	u8 ap_ampdu_en;
	u16 ap_cap;
	u8 ap_ldpc;
	u8 ap_stbc;
	u8 sta_ht_cap[26];
	u16 sta_cap;
	u8 sta_ampdu_para;
	u8 op_present;
	u8 ht_op_sta_width;
	u8 ht_40_intol;
	u8 cur_bwmode;
	u8 cur_ch_offset;
};

u32 rtw_rust_apmode_dot11_auth(_adapter *padapter)
{
	return padapter->securitypriv.dot11AuthAlgrthm;
}

u8 rtw_rust_apmode_is_mesh(_adapter *padapter)
{
	return MLME_IS_MESH(padapter) ? _TRUE : _FALSE;
}

void rtw_rust_apmode_set_8021x_blocked(struct sta_info *psta, u32 blocked)
{
	psta->ieee8021x_blocked = blocked;
}

void rtw_rust_apmode_read_ht_inputs(_adapter *padapter, struct sta_info *psta,
				      struct rtw_rust_apmode_ht_in *out)
{
	memcpy(out->sta_ht_cap, &psta->htpriv.ht_cap, 26);
	out->ap_ampdu_en = padapter->mlmepriv.htpriv.ampdu_enable;
	out->ap_cap = padapter->mlmepriv.htpriv.ht_cap.cap_info;
	out->ap_ldpc = padapter->mlmepriv.htpriv.ldpc_cap;
	out->ap_stbc = padapter->mlmepriv.htpriv.stbc_cap;
	out->sta_cap = psta->htpriv.ht_cap.cap_info;
	out->sta_ampdu_para = psta->htpriv.ht_cap.ampdu_params_info;
	out->op_present = psta->htpriv.op_present;
	out->ht_op_sta_width = GET_HT_OP_ELE_STA_CHL_WIDTH(psta->htpriv.ht_op);
	out->ht_40_intol = psta->ht_40mhz_intolerant;
	out->cur_bwmode = padapter->mlmeextpriv.cur_bwmode;
	out->cur_ch_offset = padapter->mlmeextpriv.cur_ch_offset;
}

u8 rtw_rust_apmode_sta_ht_option(struct sta_info *psta)
{
	return psta->htpriv.ht_option;
}

void rtw_rust_apmode_apply_ht(struct sta_info *psta, u8 ampdu_en, u8 min_sp, u8 bw,
			      u8 sgi20, u8 sgi40, u32 qos, u8 ch_off, u8 ldpc, u8 stbc)
{
	psta->htpriv.ampdu_enable = ampdu_en;
	psta->htpriv.rx_ampdu_min_spacing = min_sp;
	psta->cmn.bw_mode = bw;
	psta->htpriv.sgi_20m = sgi20;
	psta->htpriv.sgi_40m = sgi40;
	psta->qos_option = qos;
	psta->htpriv.ch_offset = ch_off;
	psta->htpriv.ldpc_cap = ldpc;
	psta->htpriv.stbc_cap = stbc;
}

void rtw_rust_apmode_clear_ht_no_option(struct sta_info *psta)
{
	psta->htpriv.ampdu_enable = _FALSE;
	psta->htpriv.sgi_20m = _FALSE;
	psta->htpriv.sgi_40m = _FALSE;
	psta->cmn.bw_mode = CHANNEL_WIDTH_20;
	psta->htpriv.ch_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
	psta->htpriv.ldpc_cap = 0;
	psta->htpriv.stbc_cap = 0;
}

u8 *rtw_rust_apmode_sta_mac(struct sta_info *psta)
{
	return psta->cmn.mac_addr;
}

void rtw_rust_apmode_reset_agg(struct sta_info *psta)
{
	psta->htpriv.agg_enable_bitmap = 0;
	psta->htpriv.candidate_tid_bitmap = 0;
}

void rtw_rust_apmode_set_ra_sgi(struct sta_info *psta, u8 sgi)
{
	psta->cmn.ra_info.is_support_sgi = sgi;
}

void rtw_rust_apmode_zero_stats(struct sta_info *psta)
{
	_rtw_memset((void *)&psta->sta_stats, 0, sizeof(struct stainfo_stats));
}

void rtw_rust_apmode_finish_assoc_state(_adapter *padapter, struct sta_info *psta,
					u32 dot11_auth, u8 is_mesh)
{
	_irqL irqL;

	(void)padapter;
	_enter_critical_bh(&psta->lock, &irqL);
	if (!is_mesh && dot11_auth == dot11AuthAlgrthm_8021X)
		psta->state |= WIFI_UNDER_KEY_HANDSHAKE;
	psta->state |= WIFI_ASOC_STATE;
	_exit_critical_bh(&psta->lock, &irqL);
}

void rtw_rust_apmode_set_odm_sta_info(_adapter *padapter, struct sta_info *psta)
{
	rtw_hal_set_odm_var(padapter, HAL_ODM_STA_INFO, psta, _TRUE);
}

#endif /* CONFIG_RUST_AP_STA_INFO_APMODE && !HOST_AP_STA_INFO_APMODE_TEST */
