// SPDX-License-Identifier: GPL-2.0
//! W3-89 sitesurvey_cmd_hdl enter/process — host L2 oracle.

#![allow(
    dead_code,
    improper_ctypes,
    non_snake_case,
    non_camel_case_types,
    non_upper_case_globals,
    private_interfaces,
    missing_docs
)]

use std::os::raw::c_int;

type U8 = u8;
type U32 = u32;
type ScanType = c_int;

const SCAN_DISABLE: U8 = 0;
const SCAN_START: U8 = 1;
const SCAN_PS_ANNC_WAIT: U8 = 2;
const SCAN_ENTER: U8 = 3;
const SCAN_PROCESS: U8 = 4;
const RX_AMPDU_ACCEPT_INVALID: U8 = 0xff;
const RX_AMPDU_SIZE_INVALID: U8 = 0xff;
const HW_VAR_CHECK_TXBUF: c_int = 0;
const HW_VAR_MLME_SITESURVEY: c_int = 1;

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
    _tail: [U8; 210],
}

#[repr(C)]
struct MlmeExtPriv {
    _info: [U8; 4],
    sitesurvey_res: SsRes,
    cur_channel: U8,
    cur_bwmode: U8,
    cur_ch_offset: U8,
}

#[repr(C)]
struct Adapter {
    _pad: [U8; 8],
    mlmeextpriv: MlmeExtPriv,
    _rfctl: [U8; 177],
}

extern "C" {
    fn host_sitesurvey_res_reset(a: *mut Adapter, parm: *mut U8);
    fn rtw_ps_annc(a: *mut Adapter, ps: bool) -> U8;
    fn rtw_phydm_ability_backup(a: *mut Adapter);
    fn rtw_phydm_func_for_offchannel(a: *mut Adapter);
    fn sitesurvey_set_igi(a: *mut Adapter);
    fn sitesurvey_set_msr(a: *mut Adapter, enter: bool);
    fn site_survey(a: *mut Adapter, ch: U8, scan_type: ScanType);
    fn set_survey_timer(e: *mut MlmeExtPriv, ms: U32);
    fn rtw_hal_set_hwreg(a: *mut Adapter, id: c_int, val: *mut U8);
    fn rtw_hal_macid_sleep_all_used(a: *mut Adapter);
    fn rtw_rx_ampdu_apply(a: *mut Adapter);
    fn sitesurvey_pick_ch_behavior(a: *mut Adapter, ch: *mut U8, scan_type: *mut ScanType) -> U8;
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
            ext.sitesurvey_res.channel_idx = ext.sitesurvey_res.channel_idx.saturating_add(1);
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
                _ => break,
            }
        }
        0
    }
}
