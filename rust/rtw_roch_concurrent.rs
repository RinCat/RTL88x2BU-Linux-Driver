// SPDX-License-Identifier: GPL-2.0
//! W3-97 concurrent roch helpers (host L2 Rust oracle).
#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    unreachable_pub
)]

use std::os::raw::{c_int, c_void};

const _TRUE: u8 = 1;
const _FALSE: u8 = 0;
const H2C_SUCCESS: c_int = 0;
const MI_LINKED: c_int = 1;
const ROCH_AP_ROCH_CH_SWITCH_PROCESS_WK: c_int = 2;
const RTW_CMDF_DIRECTLY: u8 = 1;
const HAL_PRIME_CHNL_OFFSET_DONT_CARE: u8 = 0;
const CHANNEL_WIDTH_20: u8 = 0;

#[repr(C)]
pub struct Ieee80211Channel {
    pub center_freq: c_int,
}

#[repr(C)]
pub struct RochInfo {
    pub remain_on_ch_channel: Ieee80211Channel,
    pub min_home_dur: u32,
    pub max_away_dur: u32,
    pub ap_timer_inited: c_int,
    pub ro_ch_timer_inited: c_int,
}

#[repr(C)]
pub struct RtwWdevPriv {
    pub switch_ch_to: c_int,
}

#[repr(C)]
pub struct Adapter {
    pub rochinfo: RochInfo,
    pub wdev: RtwWdevPriv,
    pub oper_ch: u8,
    pub mi_linked: c_int,
    pub union_ch: u8,
    pub union_bw: u8,
    pub union_offset: u8,
    pub cfg80211_is_roch: u8,
}

type Padapter = *mut Adapter;

#[repr(C)]
pub struct HostRochConcurrentTrace {
    pub set_channel: c_int,
    pub leave_opch: c_int,
    pub back_opch: c_int,
    pub set_timer: c_int,
    pub wk_cmd: c_int,
    pub wk_cmd_type: c_int,
    pub set_ch: u8,
    pub set_timer_ms: u32,
}

extern "C" {
    fn host_roch_concurrent_trace() -> *mut HostRochConcurrentTrace;
}

fn freq_to_ch(f: c_int) -> u8 {
    if f == 0 {
        0
    } else {
        ((f - 2407) / 5) as u8
    }
}

unsafe fn get_remain_ch(padapter: Padapter) -> u8 {
    freq_to_ch((*padapter).rochinfo.remain_on_ch_channel.center_freq)
}

unsafe fn chk_need_stay_in_cur_chan(_padapter: Padapter) -> u8 {
    _FALSE
}

unsafe fn set_channel_bwmode(a: Padapter, ch: u8) {
    let tr = host_roch_concurrent_trace();
    (*tr).set_channel = 1;
    (*tr).set_ch = ch;
    (*a).oper_ch = ch;
}

unsafe fn set_timer_ms(ms: u32) {
    let tr = host_roch_concurrent_trace();
    (*tr).set_timer = 1;
    (*tr).set_timer_ms = ms;
}

unsafe fn rtw_mi_check_status(a: Padapter, _st: c_int) -> u8 {
    if (*a).mi_linked != 0 {
        _TRUE
    } else {
        _FALSE
    }
}

#[no_mangle]
pub extern "C" fn rtw_init_roch_info(padapter: Padapter) {
    if padapter.is_null() {
        return;
    }
    unsafe {
        (*padapter).rochinfo = std::mem::zeroed();
        (*padapter).rochinfo.ap_timer_inited = 1;
        (*padapter).rochinfo.min_home_dur = 1500;
        (*padapter).rochinfo.max_away_dur = 250;
        (*padapter).rochinfo.ro_ch_timer_inited = 1;
    }
}

unsafe fn rtw_roch_wk_hdl(padapter: Padapter, cmd: c_int) -> c_int {
    if cmd == ROCH_AP_ROCH_CH_SWITCH_PROCESS_WK {
        rtw_concurrent_handler(padapter);
    }
    H2C_SUCCESS
}

#[no_mangle]
pub extern "C" fn rtw_roch_wk_cmd(
    padapter: Padapter,
    cmd: c_int,
    _parm: *mut c_void,
    flags: u8,
) -> u8 {
    unsafe {
        let tr = host_roch_concurrent_trace();
        (*tr).wk_cmd = 1;
        (*tr).wk_cmd_type = cmd;
        if flags & RTW_CMDF_DIRECTLY != 0 && rtw_roch_wk_hdl(padapter, cmd) != H2C_SUCCESS {
            return _FALSE;
        }
    }
    _TRUE
}

#[no_mangle]
pub extern "C" fn rtw_ap_roch_ch_switch_timer_process(ctx: *mut c_void) {
    let adapter = ctx as Padapter;
    if adapter.is_null() {
        return;
    }
    unsafe {
        (*adapter).wdev.switch_ch_to = 1;
        rtw_roch_wk_cmd(
            adapter,
            ROCH_AP_ROCH_CH_SWITCH_PROCESS_WK,
            std::ptr::null_mut(),
            0,
        );
    }
}

#[no_mangle]
pub extern "C" fn rtw_concurrent_handler(padapter: Padapter) {
    if padapter.is_null() {
        return;
    }
    unsafe {
        let prochinfo = &mut (*padapter).rochinfo;
        let remain_ch = get_remain_ch(padapter);

        if (*padapter).cfg80211_is_roch == _FALSE {
            return;
        }

        if rtw_mi_check_status(padapter, MI_LINKED) == _TRUE {
            let union_ch = (*padapter).union_ch;
            let duration = if (*padapter).oper_ch != union_ch {
                set_channel_bwmode(padapter, union_ch);
                (*host_roch_concurrent_trace()).back_opch = 1;
                prochinfo.min_home_dur
            } else {
                (*host_roch_concurrent_trace()).leave_opch = 1;
                set_channel_bwmode(padapter, remain_ch);
                prochinfo.max_away_dur
            };
            (*padapter).wdev.switch_ch_to = 0;
            set_timer_ms(duration);
        } else if chk_need_stay_in_cur_chan(padapter) == _FALSE {
            set_channel_bwmode(padapter, remain_ch);
        }
    }
}
