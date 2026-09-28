// SPDX-License-Identifier: GPL-2.0
#include "host_ap_sta_info_apmode_types.h"

static u8 g_vcs_calls;
static u8 g_delba_calls;
static u8 g_odm_calls;
static u8 g_ra_sgi;
static u8 g_tx_bw;

void host_apmode_reset(void)
{
	g_vcs_calls = 0;
	g_delba_calls = 0;
	g_odm_calls = 0;
	g_ra_sgi = 0;
	g_tx_bw = CHANNEL_WIDTH_20;
}

u8 host_apmode_vcs_calls(void)
{
	return g_vcs_calls;
}

u8 host_apmode_delba_calls(void)
{
	return g_delba_calls;
}

u8 host_apmode_odm_calls(void)
{
	return g_odm_calls;
}

void host_apmode_set_ra_sgi(u8 sgi)
{
	g_ra_sgi = sgi;
}

void host_apmode_set_tx_bw(u8 bw)
{
	g_tx_bw = bw;
}

void VCS_update(_adapter *padapter, struct sta_info *psta)
{
	(void)padapter;
	(void)psta;
	g_vcs_calls++;
}

void send_delba(_adapter *padapter, int initiator, u8 *addr)
{
	(void)padapter;
	(void)initiator;
	(void)addr;
	g_delba_calls++;
}

u8 query_ra_short_GI(struct sta_info *psta, u8 bw)
{
	(void)psta;
	(void)bw;
	return g_ra_sgi;
}

u8 rtw_get_tx_bw_mode(_adapter *padapter, struct sta_info *psta)
{
	(void)padapter;
	(void)psta;
	return g_tx_bw;
}

void update_ldpc_stbc_cap(struct sta_info *psta)
{
	(void)psta;
}

void update_sta_vht_info_apmode(_adapter *padapter, void *psta)
{
	(void)padapter;
	(void)psta;
}

void rtw_hal_set_odm_var(_adapter *padapter, enum hal_odm_var variable, struct sta_info *psta, u8 val)
{
	(void)padapter;
	(void)variable;
	(void)psta;
	(void)val;
	g_odm_calls++;
}

struct host_apmode_ht_in {
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

u32 host_rust_apmode_dot11_auth(_adapter *padapter)
{
	return padapter->securitypriv.dot11AuthAlgrthm;
}

void host_rust_apmode_set_8021x_blocked(struct sta_info *psta, u32 blocked)
{
	psta->ieee8021x_blocked = blocked;
}

void host_rust_apmode_read_ht_inputs(_adapter *padapter, struct sta_info *psta,
				     struct host_apmode_ht_in *out)
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

u8 host_rust_apmode_sta_ht_option(struct sta_info *psta)
{
	return psta->htpriv.ht_option;
}

void host_rust_apmode_apply_ht(struct sta_info *psta, u8 ampdu_en, u8 min_sp, u8 bw,
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

void host_rust_apmode_clear_ht_no_option(struct sta_info *psta)
{
	psta->htpriv.ampdu_enable = _FALSE;
	psta->htpriv.sgi_20m = _FALSE;
	psta->htpriv.sgi_40m = _FALSE;
	psta->cmn.bw_mode = CHANNEL_WIDTH_20;
	psta->htpriv.ch_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
}

u8 *host_rust_apmode_sta_mac(struct sta_info *psta)
{
	return psta->cmn.mac_addr;
}

void host_rust_apmode_reset_agg(struct sta_info *psta)
{
	psta->htpriv.agg_enable_bitmap = 0;
	psta->htpriv.candidate_tid_bitmap = 0;
}

void host_rust_apmode_set_ra_sgi(struct sta_info *psta, u8 sgi)
{
	psta->cmn.ra_info.is_support_sgi = sgi;
}

void host_rust_apmode_zero_stats(struct sta_info *psta)
{
	memset(&psta->sta_stats, 0, sizeof(psta->sta_stats));
}

void host_rust_apmode_or_state(struct sta_info *psta, u32 bits)
{
	psta->state |= bits;
}
