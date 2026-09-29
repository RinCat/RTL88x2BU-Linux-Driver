// SPDX-License-Identifier: GPL-2.0
//! W3-94 ps deny gate — Rust port of `core/rtw_pwrctrl.c` ps_deny helpers.

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
#[repr(C)]
pub struct PwrctrlPriv {
    pub lock: PwrLock,
    pub ps_deny: u32,
}

#[cfg(host_pwrctrl_test)]
#[repr(C)]
pub struct Adapter {
    pub pwrctrlpriv: PwrctrlPriv,
}

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
