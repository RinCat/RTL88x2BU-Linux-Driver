// SPDX-License-Identifier: GPL-2.0
//! W3-94 pwrctrl — Rust port of `core/rtw_pwrctrl.c` ps deny + unassociated idle helpers.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(host_pwrctrl_test)]
use std::os::raw::c_uint;

#[cfg(not(host_pwrctrl_test))]
use core::ffi::c_uint;

#[cfg(host_pwrctrl_test)]
#[repr(C)]
pub struct PwrLock {
    pub _lock: i32,
}

#[cfg(host_pwrctrl_test)]
type Systime = u32;

#[cfg(host_pwrctrl_test)]
const _TRUE: u8 = 1;
#[cfg(host_pwrctrl_test)]
const _FALSE: u8 = 0;
#[cfg(host_pwrctrl_test)]
const NR_XMITBUFF: u16 = 4;
#[cfg(host_pwrctrl_test)]
const NR_XMIT_EXTBUFF: u16 = 32;
#[cfg(host_pwrctrl_test)]
const WIFI_ASOC_STATE: u32 = 0x0000_0001;
#[cfg(host_pwrctrl_test)]
const WIFI_UNDER_SURVEY: u32 = 0x0000_0800;
#[cfg(host_pwrctrl_test)]
const WIFI_UNDER_LINKING: u32 = 0x0000_0080;
#[cfg(host_pwrctrl_test)]
const WIFI_UNDER_WPS: u32 = 0x0000_0100;
#[cfg(host_pwrctrl_test)]
const WIFI_AP_STATE: u32 = 0x0000_0010;
#[cfg(host_pwrctrl_test)]
const WIFI_ADHOC_MASTER_STATE: u32 = 0x0000_0020;
#[cfg(host_pwrctrl_test)]
const WIFI_ADHOC_STATE: u32 = 0x0000_0040;

#[cfg(host_pwrctrl_test)]
#[repr(C)]
pub struct MlmePriv {
    pub fw_state: u32,
}

#[cfg(host_pwrctrl_test)]
#[repr(C)]
pub struct XmitPriv {
    pub free_xmitbuf_cnt: u16,
    pub free_xmit_extbuf_cnt: u16,
}

#[cfg(host_pwrctrl_test)]
#[repr(C)]
pub struct PwrctrlPriv {
    pub lock: PwrLock,
    pub ps_deny: u32,
    pub bpower_saving: u8,
    pub ips_deny_time: Systime,
}

#[cfg(host_pwrctrl_test)]
#[repr(C)]
pub struct DvobjPriv {
    pub iface_nums: u8,
    pub padapters: [*mut Adapter; 4],
}

#[cfg(host_pwrctrl_test)]
#[repr(C)]
pub struct Adapter {
    pub dvobj: *mut DvobjPriv,
    pub mlmepriv: MlmePriv,
    pub xmitpriv: XmitPriv,
    pub pwrctrlpriv: PwrctrlPriv,
}

#[cfg(host_pwrctrl_test)]
static mut G_CURRENT_TIME: Systime = 0;

#[cfg(host_pwrctrl_test)]
type Padapter = *mut Adapter;

#[cfg(not(host_pwrctrl_test))]
type Padapter = *mut core::ffi::c_void;

#[cfg(host_pwrctrl_test)]
type PsDenyReason = u32;

#[cfg(not(host_pwrctrl_test))]
type PsDenyReason = u32;

#[cfg(host_pwrctrl_test)]
fn adapter_to_pwrctl(adapter: Padapter) -> *mut PwrctrlPriv {
    unsafe { &mut (*adapter).pwrctrlpriv }
}

#[cfg(host_pwrctrl_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_lps_set_time(t: Systime) {
    unsafe {
        G_CURRENT_TIME = t;
    }
}

#[cfg(host_pwrctrl_test)]
fn rtw_get_current_time() -> Systime {
    unsafe { G_CURRENT_TIME }
}

#[cfg(host_pwrctrl_test)]
fn rtw_time_after(a: Systime, b: Systime) -> bool {
    (a as i32).wrapping_sub(b as i32) > 0
}

#[cfg(host_pwrctrl_test)]
fn check_fwstate(m: &MlmePriv, mask: u32) -> bool {
    if mask == 0 && m.fw_state == 0 {
        return true;
    }
    (m.fw_state & mask) != 0
}

#[cfg(host_pwrctrl_test)]
fn rtw_is_adapter_up(_iface: Padapter) -> bool {
    true
}

#[cfg(host_pwrctrl_test)]
fn mlme_is_ap(iface: &Adapter) -> bool {
    (iface.mlmepriv.fw_state & WIFI_AP_STATE) != 0
}

#[cfg(host_pwrctrl_test)]
fn mlme_is_mesh(_iface: &Adapter) -> bool {
    false
}

#[cfg(host_pwrctrl_test)]
fn rtw_pwr_unassociated_idle_host(adapter: Padapter) -> u8 {
    if adapter.is_null() {
        return _FALSE;
    }
    unsafe {
        let adapter = &mut *adapter;
        let pwr = adapter_to_pwrctl(adapter);
        if (*pwr).bpower_saving == _TRUE {
            return _FALSE;
        }
        if rtw_time_after((*pwr).ips_deny_time, rtw_get_current_time()) {
            return _FALSE;
        }
        let dvobj = match adapter.dvobj.as_mut() {
            Some(d) => d,
            None => return _FALSE,
        };
        for i in 0..dvobj.iface_nums as usize {
            let iface_ptr = dvobj.padapters[i];
            if iface_ptr.is_null() || !rtw_is_adapter_up(iface_ptr) {
                continue;
            }
            let iface = &*iface_ptr;
            let mlme = &iface.mlmepriv;
            if check_fwstate(mlme, WIFI_ASOC_STATE | WIFI_UNDER_SURVEY)
                || check_fwstate(mlme, WIFI_UNDER_LINKING | WIFI_UNDER_WPS)
                || mlme_is_ap(iface)
                || mlme_is_mesh(iface)
                || check_fwstate(mlme, WIFI_ADHOC_MASTER_STATE | WIFI_ADHOC_STATE)
            {
                return _FALSE;
            }
        }
        let px = &adapter.xmitpriv;
        if px.free_xmitbuf_cnt != NR_XMITBUFF || px.free_xmit_extbuf_cnt != NR_XMIT_EXTBUFF {
            return _FALSE;
        }
        _TRUE
    }
}

#[cfg(host_pwrctrl_test)]
mod host {
    use super::*;

    pub fn enter_pwrlock(_lock: *mut PwrLock) {}
    pub fn exit_pwrlock(_lock: *mut PwrLock) {}
}

#[cfg(not(host_pwrctrl_test))]
mod kernel {
    use super::*;

    extern "C" {
        fn rtw_rust_pwrctrl_enter_lock(pwr: *mut core::ffi::c_void);
        fn rtw_rust_pwrctrl_exit_lock(pwr: *mut core::ffi::c_void);
        fn rtw_rust_pwrctrl_ps_deny_ptr(pwr: *mut core::ffi::c_void) -> *mut u32;
    }

    pub fn enter_pwrlock(pwr: *mut core::ffi::c_void) {
        unsafe { rtw_rust_pwrctrl_enter_lock(pwr) };
    }

    pub fn exit_pwrlock(pwr: *mut core::ffi::c_void) {
        unsafe { rtw_rust_pwrctrl_exit_lock(pwr) };
    }

    pub fn ps_deny_mut(pwr: *mut core::ffi::c_void) -> *mut u32 {
        unsafe { rtw_rust_pwrctrl_ps_deny_ptr(pwr) }
    }
}

#[no_mangle]
pub extern "C" fn rtw_ps_deny(padapter: Padapter, reason: PsDenyReason) {
    if padapter.is_null() {
        return;
    }
    #[cfg(host_pwrctrl_test)]
    unsafe {
        let pwr = adapter_to_pwrctl(padapter);
        host::enter_pwrlock(&mut (*pwr).lock);
        (*pwr).ps_deny |= 1u32 << reason;
        host::exit_pwrlock(&mut (*pwr).lock);
    }
    #[cfg(not(host_pwrctrl_test))]
    unsafe {
        let pwr = padapter;
        kernel::enter_pwrlock(pwr);
        let deny = kernel::ps_deny_mut(pwr);
        if !deny.is_null() {
            *deny |= 1u32 << reason;
        }
        kernel::exit_pwrlock(pwr);
    }
}

#[no_mangle]
pub extern "C" fn rtw_ps_deny_cancel(padapter: Padapter, reason: PsDenyReason) {
    if padapter.is_null() {
        return;
    }
    #[cfg(host_pwrctrl_test)]
    unsafe {
        let pwr = adapter_to_pwrctl(padapter);
        host::enter_pwrlock(&mut (*pwr).lock);
        (*pwr).ps_deny &= !(1u32 << reason);
        host::exit_pwrlock(&mut (*pwr).lock);
    }
    #[cfg(not(host_pwrctrl_test))]
    unsafe {
        let pwr = padapter;
        kernel::enter_pwrlock(pwr);
        let deny = kernel::ps_deny_mut(pwr);
        if !deny.is_null() {
            *deny &= !(1u32 << reason);
        }
        kernel::exit_pwrlock(pwr);
    }
}

#[no_mangle]
pub extern "C" fn rtw_ps_deny_get(padapter: Padapter) -> c_uint {
    if padapter.is_null() {
        return 0;
    }
    #[cfg(host_pwrctrl_test)]
    unsafe {
        (*adapter_to_pwrctl(padapter)).ps_deny as c_uint
    }
    #[cfg(not(host_pwrctrl_test))]
    unsafe {
        let deny = kernel::ps_deny_mut(padapter);
        if deny.is_null() {
            0
        } else {
            *deny as c_uint
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_pwr_unassociated_idle(padapter: Padapter) -> u8 {
    #[cfg(host_pwrctrl_test)]
    {
        return rtw_pwr_unassociated_idle_host(padapter);
    }
    #[cfg(not(host_pwrctrl_test))]
    {
        let _ = padapter;
        0
    }
}
