// SPDX-License-Identifier: GPL-2.0
/* Host L2 C oracle: sitesurvey_cmd_hdl (W3-89 enter/process + W3-90 backop/complete). */
#include "host_mlme_ext_sitesurvey_cmd_types.h"
#include <string.h>

static void sitesurvey_res_reset(_adapter *adapter, struct sitesurvey_parm *parm)
{
	struct ss_res *ss = &adapter->mlmeextpriv.sitesurvey_res;
	RT_CHANNEL_INFO *chset = adapter_to_chset(adapter);
	int i;

	ss->bss_cnt = 0;
	ss->activate_ch_cnt = 0;
	ss->channel_idx = 0;
	ss->force_ssid_scan = 0;
	ss->igi_scan = 0;
	ss->igi_before_scan = 0;
	ss->scan_cnt = 0;
	ss->ssid_num = 0;
	for (i = 0; i < RTW_SSID_SCAN_AMOUNT; i++) {
		if (parm->ssid[i].SsidLength) {
			memcpy(ss->ssid[i].Ssid, parm->ssid[i].Ssid, 32);
			ss->ssid[i].SsidLength = parm->ssid[i].SsidLength;
			ss->ssid_num++;
		} else {
			ss->ssid[i].SsidLength = 0;
		}
	}
	ss->ch_num = (u8)rtw_scan_ch_decision(adapter, ss->ch, RTW_CHANNEL_SCAN_AMOUNT,
					      parm->ch, parm->ch_num, parm->acs, parm->reason);
	for (i = 0; i < MAX_CHANNEL_NUM; i++)
		chset[i].hidden_bss_cnt = 0;
	ss->bw = parm->bw;
	ss->igi = parm->igi;
	ss->token = parm->token;
	ss->duration = parm->duration;
	ss->scan_mode = (u8)parm->scan_mode;
	ss->acs = parm->acs;
}

u8 sitesurvey_cmd_hdl(_adapter *padapter, u8 *pbuf)
{
	struct sitesurvey_parm *pparm = (struct sitesurvey_parm *)pbuf;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct ss_res *ss = &pmlmeext->sitesurvey_res;
	u8 val8;

	if (mlmeext_chk_scan_state(pmlmeext, SCAN_PROCESS))
		ss->channel_idx++;

	if (mlmeext_scan_state(pmlmeext) != mlmeext_scan_next_state(pmlmeext))
		mlmeext_set_scan_state(pmlmeext, mlmeext_scan_next_state(pmlmeext));

operation_by_state:
	switch (mlmeext_scan_state(pmlmeext)) {
	case SCAN_DISABLE:
		sitesurvey_res_reset(padapter, pparm);
		mlmeext_set_scan_state(pmlmeext, SCAN_START);
		goto operation_by_state;
	case SCAN_START:
		if (ss->rx_ampdu_accept != RX_AMPDU_ACCEPT_INVALID ||
		    ss->rx_ampdu_size != RX_AMPDU_SIZE_INVALID)
			rtw_rx_ampdu_apply(padapter);
		rtw_hal_set_hwreg(padapter, HW_VAR_CHECK_TXBUF, 0);
		rtw_hal_macid_sleep_all_used(padapter);
		if (rtw_ps_annc(padapter, 1)) {
			mlmeext_set_scan_state(pmlmeext, SCAN_PS_ANNC_WAIT);
			mlmeext_set_scan_next_state(pmlmeext, SCAN_ENTER);
			set_survey_timer(pmlmeext, 50);
		} else {
			mlmeext_set_scan_state(pmlmeext, SCAN_ENTER);
			goto operation_by_state;
		}
		break;
	case SCAN_ENTER:
		rtw_phydm_ability_backup(padapter);
		sitesurvey_set_igi(padapter);
		rtw_phydm_func_for_offchannel(padapter);
		sitesurvey_set_msr(padapter, _TRUE);
		val8 = 1;
		rtw_hal_set_hwreg(padapter, HW_VAR_MLME_SITESURVEY, &val8);
		mlmeext_set_scan_state(pmlmeext, SCAN_PROCESS);
		goto operation_by_state;
	case SCAN_PROCESS: {
		u8 scan_ch;
		RT_SCAN_TYPE scan_type;
		u8 next_state;
		u32 scan_ms;

		next_state = sitesurvey_pick_ch_behavior(padapter, &scan_ch, &scan_type);
		if (next_state != SCAN_PROCESS) {
			mlmeext_set_scan_state(pmlmeext, next_state);
			goto operation_by_state;
		}
		site_survey(padapter, scan_ch, scan_type);
		scan_ms = ss->scan_ch_ms;
		set_survey_timer(pmlmeext, scan_ms);
		break;
	}
	case SCAN_BACKING_OP: {
		u8 back_ch = 0, back_bw = 0, back_ch_offset = 0;

		if (rtw_mi_get_ch_setting_union(padapter, &back_ch, &back_bw,
						&back_ch_offset) == 0) {
			back_ch = pmlmeext->cur_channel;
			back_bw = pmlmeext->cur_bwmode;
			back_ch_offset = pmlmeext->cur_ch_offset;
		}
		set_channel_bwmode(padapter, back_ch, back_ch_offset, back_bw);
		sitesurvey_set_msr(padapter, _FALSE);
		val8 = 0;
		rtw_hal_set_hwreg(padapter, HW_VAR_MLME_SITESURVEY, &val8);
		if (mlmeext_chk_scan_backop_flags(pmlmeext, SS_BACKOP_PS_ANNC)) {
			sitesurvey_set_igi(padapter);
			rtw_hal_macid_wakeup_all_used(padapter);
			rtw_ps_annc(padapter, 0);
		}
		mlmeext_set_scan_state(pmlmeext, SCAN_BACK_OP);
		ss->backop_time = rtw_get_current_time();
		if (mlmeext_chk_scan_backop_flags(pmlmeext, SS_BACKOP_TX_RESUME))
			rtw_mi_os_xmit_schedule(padapter);
		goto operation_by_state;
	}
	case SCAN_BACK_OP:
		if (rtw_get_passing_time_ms(ss->backop_time) >= ss->backop_ms ||
		    pmlmeext->scan_abort) {
			mlmeext_set_scan_state(pmlmeext, SCAN_LEAVING_OP);
			goto operation_by_state;
		}
		set_survey_timer(pmlmeext, 50);
		break;
	case SCAN_LEAVING_OP:
		rtw_hal_set_hwreg(padapter, HW_VAR_CHECK_TXBUF, 0);
		rtw_hal_macid_sleep_all_used(padapter);
		if (mlmeext_chk_scan_backop_flags(pmlmeext, SS_BACKOP_PS_ANNC) &&
		    rtw_ps_annc(padapter, 1)) {
			mlmeext_set_scan_state(pmlmeext, SCAN_PS_ANNC_WAIT);
			mlmeext_set_scan_next_state(pmlmeext, SCAN_LEAVE_OP);
			set_survey_timer(pmlmeext, 50);
		} else {
			mlmeext_set_scan_state(pmlmeext, SCAN_LEAVE_OP);
			goto operation_by_state;
		}
		break;
	case SCAN_LEAVE_OP:
		if (mlmeext_chk_scan_backop_flags(pmlmeext, SS_BACKOP_PS_ANNC))
			sitesurvey_set_igi(padapter);
		sitesurvey_set_msr(padapter, _TRUE);
		val8 = 1;
		rtw_hal_set_hwreg(padapter, HW_VAR_MLME_SITESURVEY, &val8);
		mlmeext_set_scan_state(pmlmeext, SCAN_PROCESS);
		goto operation_by_state;
	case SCAN_COMPLETE:
		survey_done_set_ch_bw(padapter);
		sitesurvey_set_msr(padapter, _FALSE);
		val8 = 0;
		rtw_hal_set_hwreg(padapter, HW_VAR_MLME_SITESURVEY, &val8);
		rtw_phydm_ability_restore(padapter);
		sitesurvey_set_igi(padapter);
		rtw_hal_macid_wakeup_all_used(padapter);
		rtw_ps_annc(padapter, 0);
		rtw_rx_ampdu_apply(padapter);
		mlmeext_set_scan_state(pmlmeext, SCAN_DISABLE);
		break;
	default:
		break;
	}
	return 0;
}
