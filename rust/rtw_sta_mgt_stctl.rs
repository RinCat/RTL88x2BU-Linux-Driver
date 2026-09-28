// SPDX-License-Identifier: GPL-2.0
//! W3-55 st_ctl + stainfo_offset — Rust port of `core/rtw_sta_mgt_stctl.c`.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(host_sta_mgt_test)]
use std::os::raw::c_int;

#[cfg(not(host_sta_mgt_test))]
use core::ffi::c_int;

const SESSION_TRACKER_REG_ID_NUM: usize = 1;

type RuleFn = extern "C" fn(*mut Adapter, *mut u8, *mut u8, *mut u8, *mut u8) -> bool;

// C leaves st_register.rule NULL before registration and again after
// rtw_st_ctl_unregister(), so it must be modelled as a nullable pointer. A bare
// `extern "C" fn` is non-nullable, which lets rustc mark it nonnull and fold the
// NULL test away; Option<RuleFn> is layout-identical under the null pointer
// optimization and keeps the check.
type RuleFnOpt = Option<RuleFn>;

#[repr(C)]
pub struct StRegister {
    pub s_proto: u8,
    pub rule: RuleFnOpt,
}

#[cfg(host_sta_mgt_test)]
mod layout {
    use super::*;

    #[repr(C)]
    pub struct List {
        pub next: *mut List,
        pub prev: *mut List,
    }

    #[repr(C)]
    pub struct Queue {
        pub queue: List,
        pub lock: c_int,
    }

    #[repr(C)]
    pub struct SessionTracker {
        pub list: List,
        pub local_naddr: u32,
        pub local_port: u16,
        pub remote_naddr: u32,
        pub remote_port: u16,
        pub set_time: usize,
        pub status: u8,
    }

    #[repr(C)]
    pub struct StCtl {
        pub reg: [StRegister; SESSION_TRACKER_REG_ID_NUM],
        pub tracker_q: Queue,
    }

    #[repr(C)]
    pub struct StaInfo {
        pub _pad: [u8; 16],
    }

    #[repr(C)]
    pub struct StaPriv {
        pub pstainfo_buf: *mut u8,
    }

    #[repr(C)]
    pub struct Adapter {
        pub stapriv: StaPriv,
    }
}

#[cfg(host_sta_mgt_test)]
use layout::{Adapter, Queue, SessionTracker, StCtl, StaInfo, StaPriv};

#[cfg(not(host_sta_mgt_test))]
pub type Adapter = core::ffi::c_void;
#[cfg(not(host_sta_mgt_test))]
pub type StCtl = core::ffi::c_void;
#[cfg(not(host_sta_mgt_test))]
pub type StaInfo = core::ffi::c_void;
#[cfg(not(host_sta_mgt_test))]
pub type StaPriv = core::ffi::c_void;

#[cfg(host_sta_mgt_test)]
extern "C" {
    fn host_sta_mgt_stainfo_offset(stapriv: *mut StaPriv, sta: *mut StaInfo) -> c_int;
    fn host_sta_mgt_stctl_clear(st_ctl: *mut StCtl);
}

#[cfg(not(host_sta_mgt_test))]
mod kernel {
    use super::*;

    extern "C" {
        pub fn rtw_rust_stctl_memzero_reg(st_ctl: *mut u8);
        pub fn rtw_rust_stctl_queue_init(st_ctl: *mut u8);
        pub fn rtw_rust_stctl_queue_deinit(st_ctl: *mut u8);
        pub fn rtw_rust_stctl_clear_tracker_q(st_ctl: *mut u8);
        pub fn rtw_rust_stainfo_buf(stapriv: *mut u8) -> *mut u8;
        pub fn rtw_rust_sta_info_size() -> u32;
        pub fn rtw_rust_stainfo_offset_valid(offset: c_int) -> u8;
        pub fn rtw_rust_stctl_reg_s_proto(st_ctl: *mut u8, idx: u8) -> u8;
        pub fn rtw_rust_stctl_reg_rule(st_ctl: *mut u8, idx: u8) -> RuleFnOpt;
        pub fn rtw_rust_stctl_reg_set(st_ctl: *mut u8, idx: u8, s_proto: u8, rule: RuleFnOpt);
        pub fn rtw_rust_stctl_reg_clear(st_ctl: *mut u8, idx: u8);
        pub fn rtw_rust_stctl_any_reg(st_ctl: *mut u8) -> u8;
        pub fn rtw_rust_stctl_warn_on(cond: c_int);
        pub fn rtw_rust_stainfo_offset_invalid_log(func: *const u8, offset: c_int);
    }
}

#[cfg(host_sta_mgt_test)]
fn init_queue(q: &mut Queue) {
    q.queue.next = &mut q.queue;
    q.queue.prev = &mut q.queue;
}

#[cfg(host_sta_mgt_test)]
fn clear_tracker_q(st_ctl: &mut StCtl) {
    unsafe {
        host_sta_mgt_stctl_clear(st_ctl);
    }
}

#[no_mangle]
pub extern "C" fn rtw_st_ctl_init(st_ctl: *mut StCtl) {
    if st_ctl.is_null() {
        return;
    }
    #[cfg(host_sta_mgt_test)]
    unsafe {
        let st_ctl = &mut *st_ctl;
        for r in &mut st_ctl.reg {
            r.s_proto = 0;
            r.rule = core::mem::transmute(0usize);
        }
        init_queue(&mut st_ctl.tracker_q);
    }
    #[cfg(not(host_sta_mgt_test))]
    unsafe {
        kernel::rtw_rust_stctl_memzero_reg(st_ctl.cast());
        kernel::rtw_rust_stctl_queue_init(st_ctl.cast());
    }
}

#[no_mangle]
pub extern "C" fn rtw_st_ctl_clear_tracker_q(st_ctl: *mut StCtl) {
    if st_ctl.is_null() {
        return;
    }
    #[cfg(host_sta_mgt_test)]
    unsafe {
        clear_tracker_q(&mut *st_ctl);
    }
    #[cfg(not(host_sta_mgt_test))]
    unsafe {
        kernel::rtw_rust_stctl_clear_tracker_q(st_ctl.cast());
    }
}

#[no_mangle]
pub extern "C" fn rtw_st_ctl_deinit(st_ctl: *mut StCtl) {
    if st_ctl.is_null() {
        return;
    }
    #[cfg(host_sta_mgt_test)]
    unsafe {
        let st_ctl = &mut *st_ctl;
        clear_tracker_q(st_ctl);
    }
    #[cfg(not(host_sta_mgt_test))]
    unsafe {
        kernel::rtw_rust_stctl_clear_tracker_q(st_ctl.cast());
        kernel::rtw_rust_stctl_queue_deinit(st_ctl.cast());
    }
}

#[no_mangle]
pub extern "C" fn rtw_st_ctl_register(st_ctl: *mut StCtl, st_reg_id: u8, reg: *mut StRegister) {
    if st_reg_id as usize >= SESSION_TRACKER_REG_ID_NUM {
        #[cfg(not(host_sta_mgt_test))]
        unsafe {
            kernel::rtw_rust_stctl_warn_on(1);
        }
        return;
    }
    if st_ctl.is_null() || reg.is_null() {
        return;
    }
    #[cfg(host_sta_mgt_test)]
    unsafe {
        let st_ctl = &mut *st_ctl;
        let reg = &*reg;
        st_ctl.reg[st_reg_id as usize].s_proto = reg.s_proto;
        st_ctl.reg[st_reg_id as usize].rule = reg.rule;
    }
    #[cfg(not(host_sta_mgt_test))]
    unsafe {
        let reg = &*reg;
        kernel::rtw_rust_stctl_reg_set(st_ctl.cast(), st_reg_id, reg.s_proto, reg.rule);
    }
}

#[no_mangle]
pub extern "C" fn rtw_st_ctl_unregister(st_ctl: *mut StCtl, st_reg_id: u8) {
    if st_reg_id as usize >= SESSION_TRACKER_REG_ID_NUM {
        #[cfg(not(host_sta_mgt_test))]
        unsafe {
            kernel::rtw_rust_stctl_warn_on(1);
        }
        return;
    }
    if st_ctl.is_null() {
        return;
    }
    #[cfg(host_sta_mgt_test)]
    unsafe {
        let st_ctl = &mut *st_ctl;
        st_ctl.reg[st_reg_id as usize].s_proto = 0;
        st_ctl.reg[st_reg_id as usize].rule = core::mem::transmute(0usize);
        if !st_ctl.reg.iter().any(|r| r.s_proto != 0) {
            clear_tracker_q(st_ctl);
        }
    }
    #[cfg(not(host_sta_mgt_test))]
    unsafe {
        kernel::rtw_rust_stctl_reg_clear(st_ctl.cast(), st_reg_id);
        if kernel::rtw_rust_stctl_any_reg(st_ctl.cast()) == 0 {
            kernel::rtw_rust_stctl_clear_tracker_q(st_ctl.cast());
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_st_ctl_chk_reg_s_proto(st_ctl: *mut StCtl, s_proto: u8) -> bool {
    if st_ctl.is_null() {
        return false;
    }
    #[cfg(host_sta_mgt_test)]
    unsafe {
        return (*st_ctl).reg.iter().any(|r| r.s_proto == s_proto);
    }
    #[cfg(not(host_sta_mgt_test))]
    unsafe {
        for i in 0..SESSION_TRACKER_REG_ID_NUM {
            if kernel::rtw_rust_stctl_reg_s_proto(st_ctl.cast(), i as u8) == s_proto {
                return true;
            }
        }
        false
    }
}

#[no_mangle]
pub extern "C" fn rtw_st_ctl_chk_reg_rule(
    st_ctl: *mut StCtl,
    adapter: *mut Adapter,
    local_naddr: *mut u8,
    local_port: *mut u8,
    remote_naddr: *mut u8,
    remote_port: *mut u8,
) -> bool {
    if st_ctl.is_null() {
        return false;
    }
    #[cfg(host_sta_mgt_test)]
    unsafe {
        for r in &(*st_ctl).reg {
            if let Some(rule) = r.rule {
                if rule(adapter, local_naddr, local_port, remote_naddr, remote_port) {
                    return true;
                }
            }
        }
        return false;
    }
    #[cfg(not(host_sta_mgt_test))]
    unsafe {
        for i in 0..SESSION_TRACKER_REG_ID_NUM {
            if let Some(rule) = kernel::rtw_rust_stctl_reg_rule(st_ctl.cast(), i as u8) {
                if rule(adapter, local_naddr, local_port, remote_naddr, remote_port) {
                    return true;
                }
            }
        }
        false
    }
}

#[no_mangle]
pub extern "C" fn rtw_stainfo_offset(stapriv: *mut StaPriv, sta: *mut StaInfo) -> c_int {
    if stapriv.is_null() || sta.is_null() {
        return 0;
    }
    #[cfg(host_sta_mgt_test)]
    unsafe {
        return host_sta_mgt_stainfo_offset(stapriv, sta);
    }
    #[cfg(not(host_sta_mgt_test))]
    unsafe {
        let buf = kernel::rtw_rust_stainfo_buf(stapriv.cast());
        if buf.is_null() {
            return 0;
        }
        let size = kernel::rtw_rust_sta_info_size() as isize;
        if size <= 0 {
            return 0;
        }
        let offset = sta.cast::<u8>().offset_from(buf) / size;
        if kernel::rtw_rust_stainfo_offset_valid(offset as c_int) == 0 {
            kernel::rtw_rust_stainfo_offset_invalid_log(
                b"rtw_stainfo_offset\0".as_ptr(),
                offset as c_int,
            );
        }
        offset as c_int
    }
}

#[cfg(host_sta_mgt_test)]
#[no_mangle]
pub extern "C" fn test_st_match_rule(
    _adapter: *mut Adapter,
    _local_naddr: *mut u8,
    local_port: *mut u8,
    _remote_naddr: *mut u8,
    remote_port: *mut u8,
) -> bool {
    fn port_is_5001(port: *mut u8) -> bool {
        if port.is_null() {
            return false;
        }
        let b = unsafe { core::slice::from_raw_parts(port, 2) };
        u16::from_be_bytes([b[0], b[1]]) == 5001
    }
    port_is_5001(local_port) || port_is_5001(remote_port)
}

#[cfg(host_sta_mgt_test)]
#[no_mangle]
pub static mut test_st_reg: StRegister = StRegister {
    s_proto: 0x06,
    rule: Some(test_st_match_rule),
};
