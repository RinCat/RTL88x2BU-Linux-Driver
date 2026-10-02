// SPDX-License-Identifier: GPL-2.0
#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(host_tdls_test)]
use std::ffi::c_void;

#[cfg(not(host_tdls_test))]
use core::ffi::c_void;

const _FALSE: u8 = 0;
const _TRUE: u8 = 1;
// Kernel osdep_service.h: _FAIL=0 (host harness init uses HOST_TDLS_INIT_OK=0).
const _FAIL: i32 = 0;
const HOST_TDLS_INIT_OK: i32 = 0;
const WIFI_FW_STATION_STATE: u32 = 0x02;
const WIFI_FW_ASSOC_SUCCESS: u32 = 0x00004000;
const TDLS_STATE_NONE: u32 = 0;
const TDLS_CH_SWITCH_ON_STATE: u32 = 1 << 16;
const TDLS_PEER_AT_OFF_STATE: u32 = 1 << 17;
const TDLS_PEER_SLEEP_STATE: u32 = 1 << 21;
const HAL_PRIME_CHNL_OFFSET_DONT_CARE: u8 = 0;

#[cfg(host_tdls_test)]
#[repr(C)]
pub struct RegistryPriv {
    pub en_tdls: u8,
    pub wifi_spec: u8,
}

#[cfg(host_tdls_test)]
#[repr(C)]
pub struct MlmePriv {
    pub _pad: u8,
}

#[cfg(host_tdls_test)]
#[repr(C)]
pub struct MlmeExtInfo {
    pub state: u32,
}

#[cfg(host_tdls_test)]
#[repr(C)]
pub struct MlmeExtPriv {
    pub mlmext_info: MlmeExtInfo,
}

#[cfg(host_tdls_test)]
#[repr(C)]
pub struct TdlsChSwitch {
    pub ch_sw_state: u32,
    pub chsw_on: i32,
    pub off_ch_num: u8,
    pub ch_offset: u8,
    pub cur_time: u32,
    pub delay_switch_back: u8,
    pub dump_stack: u8,
}

#[cfg(host_tdls_test)]
#[repr(C)]
pub struct TdlsInfo {
    pub ap_prohibited: u8,
    pub ch_switch_prohibited: u8,
    pub link_established: u8,
    pub sta_cnt: u8,
    pub sta_maximum: u8,
    pub chsw_info: TdlsChSwitch,
    pub ch_sensing: u8,
    pub watchdog_count: u8,
    pub dev_discovered: u8,
    pub driver_setup: u8,
    pub cmd_lock: i32,
    pub hdl_lock: i32,
    pub tdls_sctx: *mut c_void,
}

#[cfg(host_tdls_test)]
#[repr(C)]
pub struct Adapter {
    pub registrypriv: RegistryPriv,
    pub mlmepriv: MlmePriv,
    pub mlmeextpriv: MlmeExtPriv,
    pub tdlsinfo: TdlsInfo,
}

#[cfg(host_tdls_test)]
type Padapter = *mut Adapter;

#[cfg(not(host_tdls_test))]
type Padapter = *mut c_void;

#[cfg(host_tdls_test)]
fn adapter(p: Padapter) -> Option<&'static mut Adapter> {
    if p.is_null() {
        None
    } else {
        unsafe { Some(&mut *p) }
    }
}

#[cfg(host_tdls_test)]
#[no_mangle]
pub extern "C" fn rtw_mi_update_iface_status(_priv: *mut MlmePriv, _flags: i32) {}

#[cfg(host_tdls_test)]
fn tdls_set_link_established(a: &mut Adapter, en: u8) {
    a.tdlsinfo.link_established = en;
    rtw_mi_update_iface_status(&mut a.mlmepriv, 0);
}

#[no_mangle]
pub extern "C" fn check_ap_tdls_prohibited(pframe: *mut u8, pkt_len: u8) -> i32 {
    if pframe.is_null() || pkt_len < 5 {
        return 0;
    }
    unsafe { i32::from(*pframe.add(4) & 0x40 != 0) }
}

#[no_mangle]
pub extern "C" fn check_ap_tdls_ch_switching_prohibited(pframe: *mut u8, pkt_len: u8) -> i32 {
    if pframe.is_null() || pkt_len < 5 {
        return 0;
    }
    unsafe { i32::from(*pframe.add(4) & 0x80 != 0) }
}

#[no_mangle]
pub extern "C" fn TDLS_check_ch_state(state: u32) -> u8 {
    if state & TDLS_CH_SWITCH_ON_STATE != 0 && state & TDLS_PEER_AT_OFF_STATE != 0 {
        if state & TDLS_PEER_SLEEP_STATE != 0 {
            2
        } else {
            1
        }
    } else {
        0
    }
}

#[no_mangle]
pub extern "C" fn rtw_reset_tdls_info(padapter: Padapter) {
    let Some(a) = adapter(padapter) else {
        return;
    };
    a.tdlsinfo.ap_prohibited = _FALSE;
    a.tdlsinfo.ch_switch_prohibited = if a.registrypriv.wifi_spec == 1 {
        _FALSE
    } else {
        _TRUE
    };
    tdls_set_link_established(a, _FALSE);
    a.tdlsinfo.sta_cnt = 0;
    a.tdlsinfo.sta_maximum = _FALSE;
    a.tdlsinfo.chsw_info.ch_sw_state = TDLS_STATE_NONE;
    a.tdlsinfo.chsw_info.chsw_on = 0;
    a.tdlsinfo.chsw_info.off_ch_num = 0;
    a.tdlsinfo.chsw_info.ch_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
    a.tdlsinfo.chsw_info.cur_time = 0;
    a.tdlsinfo.chsw_info.delay_switch_back = _FALSE;
    a.tdlsinfo.chsw_info.dump_stack = _FALSE;
    a.tdlsinfo.ch_sensing = 0;
    a.tdlsinfo.watchdog_count = 0;
    a.tdlsinfo.dev_discovered = _FALSE;
    a.tdlsinfo.tdls_sctx = core::ptr::null_mut();
}

#[no_mangle]
pub extern "C" fn rtw_init_tdls_info(padapter: Padapter) -> i32 {
    let Some(a) = adapter(padapter) else {
        return _FAIL;
    };
    rtw_reset_tdls_info(padapter);
    a.tdlsinfo.driver_setup = _TRUE;
    a.tdlsinfo.cmd_lock = 0;
    a.tdlsinfo.hdl_lock = 0;
    HOST_TDLS_INIT_OK
}

#[no_mangle]
pub extern "C" fn rtw_free_tdls_info(ptdlsinfo: *mut TdlsInfo) {
    if !ptdlsinfo.is_null() {
        unsafe {
            core::ptr::write_bytes(ptdlsinfo, 0, 1);
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_is_tdls_enabled(padapter: Padapter) -> u8 {
    adapter(padapter)
        .map(|a| a.registrypriv.en_tdls)
        .unwrap_or(0)
}

#[no_mangle]
pub extern "C" fn rtw_set_tdls_enable(padapter: Padapter, enable: u8) {
    if let Some(a) = adapter(padapter) {
        a.registrypriv.en_tdls = enable;
    }
}
#[no_mangle]
pub extern "C" fn is_client_associated_to_ap(padapter: Padapter) -> i32 {
    let Some(a) = adapter(padapter) else {
        return _FAIL;
    };
    let st = a.mlmeextpriv.mlmext_info.state;
    if (st & WIFI_FW_ASSOC_SUCCESS != 0) && ((st & 0x03) == WIFI_FW_STATION_STATE) {
        _TRUE as i32
    } else {
        _FAIL
    }
}

#[no_mangle]
pub extern "C" fn rtw_tdls_is_setup_allowed(padapter: Padapter) -> u8 {
    let Some(a) = adapter(padapter) else {
        return _FALSE;
    };
    if is_client_associated_to_ap(padapter) == _FALSE as i32 {
        return _FALSE;
    }
    if a.tdlsinfo.ap_prohibited == _TRUE {
        return _FALSE;
    }
    _TRUE
}

#[no_mangle]
pub extern "C" fn rtw_tdls_is_chsw_allowed(padapter: Padapter) -> u8 {
    let Some(a) = adapter(padapter) else {
        return _FALSE;
    };
    if a.tdlsinfo.ch_switch_prohibited == _TRUE {
        return _FALSE;
    }
    if a.registrypriv.wifi_spec == 0 {
        return _FALSE;
    }
    _TRUE
}
