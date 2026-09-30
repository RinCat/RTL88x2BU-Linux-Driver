// SPDX-License-Identifier: GPL-2.0
//! W3-96 remain-on-channel handlers (host L2 Rust oracle).
#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    unreachable_pub
)]

#[cfg(host_roch_test)]
use std::os::raw::{c_int, c_void};

const _TRUE: u8 = 1;
const _FALSE: u8 = 0;
const H2C_SUCCESS: c_int = 0;
const RTW_CMDF_DIRECTLY: u8 = 1;
const ROCH_RO_CH_WK: c_int = 0;
const ROCH_CANCEL_RO_CH_WK: c_int = 1;
const WIFI_UNDER_LINKING: i32 = 0x80;
const WIFI_ASOC_STATE: i32 = 0x1;

#[cfg(host_roch_test)]
#[repr(C)]
pub struct MlmePriv {
    pub fwstate: i32,
}
#[cfg(host_roch_test)]
#[repr(C)]
pub struct RochInfo {
    pub restore_channel: u8,
    pub is_roch: u8,
}
#[cfg(host_roch_test)]
#[repr(C)]
pub struct DvobjPriv {
    pub iface_nums: u8,
    pub union_ch: u8,
    pub padapters: [*mut Adapter; 2],
}
#[cfg(host_roch_test)]
#[repr(C)]
pub struct Adapter {
    pub mlmepriv: MlmePriv,
    pub rochinfo: RochInfo,
    pub oper_ch: u8,
    pub dvobj: *mut DvobjPriv,
}
#[cfg(host_roch_test)]
#[repr(C)]
pub struct Ieee80211Channel {
    pub center_freq: c_int,
}
#[cfg(host_roch_test)]
#[repr(C)]
pub struct RochParm {
    pub ch: Ieee80211Channel,
    pub duration: u32,
}
#[cfg(host_roch_test)]
#[repr(C)]
pub struct HostRochTrace {
    pub set_channel: c_int,
    pub set_timer: c_int,
    pub cancel_timer: c_int,
    pub roch_expired: c_int,
    pub wk_cmd_direct: c_int,
    pub mfree: c_int,
    pub set_channel_ch: u8,
    pub set_timer_ms: u32,
}

#[cfg(host_roch_test)]
extern "C" {
    fn host_roch_trace() -> *mut HostRochTrace;
    fn free(p: *mut c_void);
}

#[cfg(host_roch_test)]
type Padapter = *mut Adapter;

#[cfg(host_roch_test)]
fn chk(m: &MlmePriv, st: i32) -> u8 {
    if (m.fwstate & st) != 0 {
        _TRUE
    } else {
        _FALSE
    }
}

#[cfg(host_roch_test)]
fn freq_to_ch(f: c_int) -> u8 {
    if f == 0 {
        0
    } else {
        ((f - 2407) / 5) as u8
    }
}

#[cfg(host_roch_test)]
unsafe fn union_chan(a: Padapter) -> u8 {
    let dv = (*a).dvobj;
    if !dv.is_null() && (*dv).union_ch != 0 {
        (*dv).union_ch
    } else {
        6
    }
}

#[no_mangle]
pub extern "C" fn rtw_roch_stay_in_cur_chan(padapter: Padapter) -> u8 {
    if padapter.is_null() {
        return _FALSE;
    }
    #[cfg(host_roch_test)]
    unsafe {
        let dv = (*padapter).dvobj;
        if dv.is_null() {
            return _FALSE;
        }
        for i in 0..(*dv).iface_nums {
            let iface = (*dv).padapters[i as usize];
            if !iface.is_null() && chk(&(*iface).mlmepriv, WIFI_UNDER_LINKING) == _TRUE {
                return _TRUE;
            }
        }
    }
    _FALSE
}

#[cfg(host_roch_test)]
unsafe fn ro_ch(a: Padapter, p: *mut RochParm) -> c_int {
    let tr = host_roch_trace();
    let mut remain = freq_to_ch((*p).ch.center_freq);
    if (*a).rochinfo.is_roch != _TRUE {
        return H2C_SUCCESS;
    }
    if rtw_roch_stay_in_cur_chan(a) == _TRUE {
        remain = union_chan(a);
    }
    if remain != (*a).oper_ch && chk(&(*a).mlmepriv, WIFI_ASOC_STATE) == _FALSE {
        (*tr).set_channel = 1;
        (*tr).set_channel_ch = remain;
        (*a).oper_ch = remain;
    }
    (*tr).set_timer = 1;
    (*tr).set_timer_ms = (*p).duration;
    H2C_SUCCESS
}

#[cfg(host_roch_test)]
unsafe fn cancel_ro(a: Padapter) -> c_int {
    let tr = host_roch_trace();
    if (*a).rochinfo.is_roch != _TRUE {
        return H2C_SUCCESS;
    }
    (*tr).cancel_timer = 1;
    (*tr).set_channel = 1;
    (*tr).set_channel_ch = (*a).rochinfo.restore_channel;
    (*a).oper_ch = (*a).rochinfo.restore_channel;
    (*a).rochinfo.is_roch = _FALSE;
    (*tr).roch_expired = 1;
    H2C_SUCCESS
}

#[no_mangle]
pub extern "C" fn rtw_roch_wk_hdl(padapter: Padapter, cmd: c_int, buf: *mut u8) -> c_int {
    if padapter.is_null() {
        return H2C_SUCCESS;
    }
    #[cfg(host_roch_test)]
    unsafe {
        if cmd == ROCH_RO_CH_WK {
            return ro_ch(padapter, buf as *mut RochParm);
        }
        if cmd == ROCH_CANCEL_RO_CH_WK {
            return cancel_ro(padapter);
        }
    }
    H2C_SUCCESS
}

#[no_mangle]
pub extern "C" fn rtw_roch_wk_cmd(padapter: Padapter, cmd: c_int, p: *mut c_void, flags: u8) -> u8 {
    #[cfg(host_roch_test)]
    unsafe {
        if flags & RTW_CMDF_DIRECTLY != 0 {
            (*host_roch_trace()).wk_cmd_direct += 1;
            if rtw_roch_wk_hdl(padapter, cmd, p as *mut u8) != H2C_SUCCESS {
                return _FALSE;
            }
            if !p.is_null() {
                (*host_roch_trace()).mfree += 1;
                free(p);
            }
        }
    }
    _TRUE
}
