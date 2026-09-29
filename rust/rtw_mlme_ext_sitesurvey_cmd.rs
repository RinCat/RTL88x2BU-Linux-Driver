// SPDX-License-Identifier: GPL-2.0
//! sitesurvey_cmd_hdl — host L2 oracle (W3-89 enter/process, W3-90 backop/complete).

#![allow(
    dead_code,
    improper_ctypes,
    non_snake_case,
    non_camel_case_types,
    non_upper_case_globals,
    private_interfaces,
    missing_docs
)]

use std::mem::{offset_of, size_of};
use std::os::raw::c_int;

type U8 = u8;
type U32 = u32;
type ScanType = c_int;

const SCAN_DISABLE: U8 = 0;
const SCAN_START: U8 = 1;
const SCAN_PS_ANNC_WAIT: U8 = 2;
const SCAN_ENTER: U8 = 3;
const SCAN_PROCESS: U8 = 4;
const SCAN_BACKING_OP: U8 = 5;
const SCAN_BACK_OP: U8 = 6;
const SCAN_LEAVING_OP: U8 = 7;
const SCAN_LEAVE_OP: U8 = 8;
const SCAN_COMPLETE: U8 = 12;

const SS_BACKOP_PS_ANNC: U8 = 1 << 4;
const SS_BACKOP_TX_RESUME: U8 = 1 << 5;

const RX_AMPDU_ACCEPT_INVALID: U8 = 0xff;
const RX_AMPDU_SIZE_INVALID: U8 = 0xff;
const HW_VAR_CHECK_TXBUF: c_int = 0;
const HW_VAR_MLME_SITESURVEY: c_int = 1;

const HOST_SS_RES_SIZE: usize = 248;
const HOST_SS_RES_TOKEN_OFF: usize = 20;
const HOST_SS_RES_BACKOP_FLAGS_OFF: usize = 36;
const HOST_MLMEEXT_CUR_CHANNEL_OFF: usize = 252;
const HOST_MLMEEXT_SCAN_ABORT_OFF: usize = 255;

#[repr(C)]
struct SsRes {
    state: U8,
    next_state: U8,
    bss_cnt: u16,
    activate_ch_cnt: u16,
    scan_ch_ms: u16,
    scan_mode: U8,
    force_ssid_scan: U8,
    ssid_num: U8,
    ch_num: U8,
    channel_idx: U8,
    scan_cnt: U8,
    scan_cnt_max: U8,
    igi: U8,
    igi_scan: U8,
    igi_before_scan: U8,
    _pad_token: [U8; 2],
    token: U32,
    duration: u16,
    bw: U8,
    acs: U8,
    rx_ampdu_accept: U8,
    rx_ampdu_size: U8,
    backop_ms: u16,
    backop_time: U32,
    backop_flags: U8,
    _pad_after_backop: [U8; 3],
    _tail: [U8; 208],
}

#[repr(C)]
struct MlmeExtPriv {
    _info: [U8; 4],
    sitesurvey_res: SsRes,
    cur_channel: U8,
    cur_bwmode: U8,
    cur_ch_offset: U8,
    scan_abort: U8,
}

#[repr(C)]
struct Adapter {
    _pad: [U8; 8],
    mlmeextpriv: MlmeExtPriv,
    _rfctl: [U8; 177],
}

const _: () = {
    assert!(size_of::<SsRes>() == HOST_SS_RES_SIZE);
    assert!(offset_of!(SsRes, token) == HOST_SS_RES_TOKEN_OFF);
    assert!(offset_of!(SsRes, backop_flags) == HOST_SS_RES_BACKOP_FLAGS_OFF);
    assert!(offset_of!(MlmeExtPriv, cur_channel) == HOST_MLMEEXT_CUR_CHANNEL_OFF);
    assert!(offset_of!(MlmeExtPriv, scan_abort) == HOST_MLMEEXT_SCAN_ABORT_OFF);
};

extern "C" {
    fn host_sitesurvey_res_reset(a: *mut Adapter, parm: *mut U8);
    fn rtw_ps_annc(a: *mut Adapter, ps: bool) -> U8;
    fn rtw_phydm_ability_backup(a: *mut Adapter);
    fn rtw_phydm_ability_restore(a: *mut Adapter);
    fn rtw_phydm_func_for_offchannel(a: *mut Adapter);
    fn sitesurvey_set_igi(a: *mut Adapter);
    fn sitesurvey_set_msr(a: *mut Adapter, enter: bool);
    fn site_survey(a: *mut Adapter, ch: U8, scan_type: ScanType);
    fn set_survey_timer(e: *mut MlmeExtPriv, ms: U32);
    fn rtw_hal_set_hwreg(a: *mut Adapter, id: c_int, val: *mut U8);
    fn rtw_hal_macid_sleep_all_used(a: *mut Adapter);
    fn rtw_hal_macid_wakeup_all_used(a: *mut Adapter);
    fn rtw_rx_ampdu_apply(a: *mut Adapter);
    fn sitesurvey_pick_ch_behavior(a: *mut Adapter, ch: *mut U8, scan_type: *mut ScanType) -> U8;
    fn set_channel_bwmode(a: *mut Adapter, ch: U8, offset: U8, bw: U8);
    fn rtw_mi_get_ch_setting_union(
        a: *mut Adapter,
        ch: *mut U8,
        bw: *mut U8,
        offset: *mut U8,
    ) -> c_int;
    fn survey_done_set_ch_bw(a: *mut Adapter);
    fn rtw_mi_os_xmit_schedule(a: *mut Adapter);
    fn rtw_get_current_time() -> U32;
    fn rtw_get_passing_time_ms(start: U32) -> U32;
}

#[no_mangle]
pub extern "C" fn sitesurvey_cmd_hdl(padapter: *mut Adapter, pbuf: *mut U8) -> U8 {
    if padapter.is_null() || pbuf.is_null() {
        return 0;
    }
    unsafe {
        let pparm = pbuf;
        let ext = &mut (*padapter).mlmeextpriv;
        if ext.sitesurvey_res.state == SCAN_PROCESS {
            ext.sitesurvey_res.channel_idx = ext.sitesurvey_res.channel_idx.wrapping_add(1);
        }
        if ext.sitesurvey_res.state != ext.sitesurvey_res.next_state {
            ext.sitesurvey_res.state = ext.sitesurvey_res.next_state;
        }
        loop {
            match ext.sitesurvey_res.state {
                SCAN_DISABLE => {
                    host_sitesurvey_res_reset(padapter, pparm);
                    ext.sitesurvey_res.state = SCAN_START;
                    ext.sitesurvey_res.next_state = SCAN_START;
                }
                SCAN_START => {
                    let ss = &ext.sitesurvey_res;
                    if ss.rx_ampdu_accept != RX_AMPDU_ACCEPT_INVALID
                        || ss.rx_ampdu_size != RX_AMPDU_SIZE_INVALID
                    {
                        rtw_rx_ampdu_apply(padapter);
                    }
                    rtw_hal_set_hwreg(padapter, HW_VAR_CHECK_TXBUF, std::ptr::null_mut());
                    rtw_hal_macid_sleep_all_used(padapter);
                    if rtw_ps_annc(padapter, true) != 0 {
                        ext.sitesurvey_res.state = SCAN_PS_ANNC_WAIT;
                        ext.sitesurvey_res.next_state = SCAN_ENTER;
                        set_survey_timer(ext, 50);
                        break;
                    }
                    ext.sitesurvey_res.state = SCAN_ENTER;
                    ext.sitesurvey_res.next_state = SCAN_ENTER;
                }
                SCAN_ENTER => {
                    rtw_phydm_ability_backup(padapter);
                    sitesurvey_set_igi(padapter);
                    rtw_phydm_func_for_offchannel(padapter);
                    sitesurvey_set_msr(padapter, true);
                    let mut on: U8 = 1;
                    rtw_hal_set_hwreg(padapter, HW_VAR_MLME_SITESURVEY, &mut on);
                    ext.sitesurvey_res.state = SCAN_PROCESS;
                    ext.sitesurvey_res.next_state = SCAN_PROCESS;
                }
                SCAN_PROCESS => {
                    let mut scan_ch: U8 = 0;
                    let mut scan_type: ScanType = 0;
                    let next = sitesurvey_pick_ch_behavior(padapter, &mut scan_ch, &mut scan_type);
                    if next != SCAN_PROCESS {
                        ext.sitesurvey_res.state = next;
                        ext.sitesurvey_res.next_state = next;
                    } else {
                        site_survey(padapter, scan_ch, scan_type);
                        let ms = ext.sitesurvey_res.scan_ch_ms;
                        set_survey_timer(ext, ms as U32);
                        break;
                    }
                }
                SCAN_BACKING_OP => {
                    let mut back_ch: U8 = 0;
                    let mut back_bw: U8 = 0;
                    let mut back_ch_offset: U8 = 0;
                    if rtw_mi_get_ch_setting_union(
                        padapter,
                        &mut back_ch,
                        &mut back_bw,
                        &mut back_ch_offset,
                    ) == 0
                    {
                        back_ch = ext.cur_channel;
                        back_bw = ext.cur_bwmode;
                        back_ch_offset = ext.cur_ch_offset;
                    }
                    set_channel_bwmode(padapter, back_ch, back_ch_offset, back_bw);
                    sitesurvey_set_msr(padapter, false);
                    let mut off: U8 = 0;
                    rtw_hal_set_hwreg(padapter, HW_VAR_MLME_SITESURVEY, &mut off);
                    let flags = ext.sitesurvey_res.backop_flags;
                    if (flags & SS_BACKOP_PS_ANNC) == SS_BACKOP_PS_ANNC {
                        sitesurvey_set_igi(padapter);
                        rtw_hal_macid_wakeup_all_used(padapter);
                        rtw_ps_annc(padapter, false);
                    }
                    ext.sitesurvey_res.state = SCAN_BACK_OP;
                    ext.sitesurvey_res.next_state = SCAN_BACK_OP;
                    ext.sitesurvey_res.backop_time = rtw_get_current_time();
                    if (flags & SS_BACKOP_TX_RESUME) == SS_BACKOP_TX_RESUME {
                        rtw_mi_os_xmit_schedule(padapter);
                    }
                }
                SCAN_BACK_OP => {
                    let elapsed = rtw_get_passing_time_ms(ext.sitesurvey_res.backop_time);
                    if elapsed >= ext.sitesurvey_res.backop_ms as U32 || ext.scan_abort != 0 {
                        ext.sitesurvey_res.state = SCAN_LEAVING_OP;
                        ext.sitesurvey_res.next_state = SCAN_LEAVING_OP;
                    } else {
                        set_survey_timer(ext, 50);
                        break;
                    }
                }
                SCAN_LEAVING_OP => {
                    rtw_hal_set_hwreg(padapter, HW_VAR_CHECK_TXBUF, std::ptr::null_mut());
                    rtw_hal_macid_sleep_all_used(padapter);
                    let flags = ext.sitesurvey_res.backop_flags;
                    if (flags & SS_BACKOP_PS_ANNC) == SS_BACKOP_PS_ANNC
                        && rtw_ps_annc(padapter, true) != 0
                    {
                        ext.sitesurvey_res.state = SCAN_PS_ANNC_WAIT;
                        ext.sitesurvey_res.next_state = SCAN_LEAVE_OP;
                        set_survey_timer(ext, 50);
                        break;
                    }
                    ext.sitesurvey_res.state = SCAN_LEAVE_OP;
                    ext.sitesurvey_res.next_state = SCAN_LEAVE_OP;
                }
                SCAN_LEAVE_OP => {
                    let flags = ext.sitesurvey_res.backop_flags;
                    if (flags & SS_BACKOP_PS_ANNC) == SS_BACKOP_PS_ANNC {
                        sitesurvey_set_igi(padapter);
                    }
                    sitesurvey_set_msr(padapter, true);
                    let mut on: U8 = 1;
                    rtw_hal_set_hwreg(padapter, HW_VAR_MLME_SITESURVEY, &mut on);
                    ext.sitesurvey_res.state = SCAN_PROCESS;
                    ext.sitesurvey_res.next_state = SCAN_PROCESS;
                }
                SCAN_COMPLETE => {
                    survey_done_set_ch_bw(padapter);
                    sitesurvey_set_msr(padapter, false);
                    let mut off: U8 = 0;
                    rtw_hal_set_hwreg(padapter, HW_VAR_MLME_SITESURVEY, &mut off);
                    rtw_phydm_ability_restore(padapter);
                    sitesurvey_set_igi(padapter);
                    rtw_hal_macid_wakeup_all_used(padapter);
                    rtw_ps_annc(padapter, false);
                    rtw_rx_ampdu_apply(padapter);
                    ext.sitesurvey_res.state = SCAN_DISABLE;
                    ext.sitesurvey_res.next_state = SCAN_DISABLE;
                    break;
                }
                _ => break,
            }
        }
        0
    }
}
