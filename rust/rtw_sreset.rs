// SPDX-License-Identifier: GPL-2.0
//! W3-95 sreset lifecycle — Rust port of `core/rtw_sreset.c` helpers.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(host_sreset_test)]
use std::os::raw::c_uint;

#[cfg(not(host_sreset_test))]
use core::ffi::{c_uint, c_ulong, c_void};

const _TRUE: u8 = 1;
const _FALSE: u8 = 0;
const REG_TXDMA_STATUS: u32 = 0x0210;
const WIFI_STATUS_SUCCESS: u8 = 0;
const USB_READ_PORT_FAIL: u8 = 2;
const USB_WRITE_PORT_FAIL: u8 = 4;
const WIFI_MAC_TXDMA_ERROR: u8 = 8;
const WIFI_IF_NOT_EXIST: u8 = 64;

#[cfg(host_sreset_test)]
type Systime = u32;

#[cfg(not(host_sreset_test))]
type Systime = c_ulong;

#[cfg(host_sreset_test)]
#[repr(C)]
pub struct SresetPriv {
    pub silent_reset_inprogress: u8,
    pub wifi_error_status: u8,
    pub last_tx_time: Systime,
    pub last_tx_complete_time: Systime,
}

#[cfg(host_sreset_test)]
#[repr(C)]
pub struct HalData {
    pub srestpriv: SresetPriv,
}

#[cfg(host_sreset_test)]
#[repr(C)]
pub struct Adapter {
    pub HalData: HalData,
}

#[cfg(host_sreset_test)]
type Padapter = *mut Adapter;

#[cfg(not(host_sreset_test))]
type Padapter = *mut c_void;

#[cfg(all(host_sreset_test, host_sreset_adapter_test))]
const WIFI_ASOC_STATE: u32 = 0x0000_0001;
#[cfg(all(host_sreset_test, host_sreset_adapter_test))]
const WIFI_UNDER_SURVEY: u32 = 0x0000_0800;
#[cfg(all(host_sreset_test, host_sreset_adapter_test))]
const WIFI_UNDER_LINKING: u32 = 0x0000_0080;

#[cfg(all(host_sreset_test, host_sreset_adapter_test))]
#[repr(C)]
pub struct MlmePriv {
    pub fw_state: u32,
}

#[cfg(all(host_sreset_test, host_sreset_adapter_test))]
#[repr(C)]
pub struct XmitTasklet {
    pub dummy: i32,
}

#[cfg(all(host_sreset_test, host_sreset_adapter_test))]
#[repr(C)]
pub struct XmitPriv {
    pub xmit_tasklet: XmitTasklet,
}

#[cfg(all(host_sreset_test, host_sreset_adapter_test))]
#[repr(C)]
pub struct TimerList {
    pub ms: u32,
}

#[cfg(all(host_sreset_test, host_sreset_adapter_test))]
#[repr(C)]
pub struct DvobjPriv {
    pub dynamic_chk_timer: TimerList,
}

#[cfg(all(host_sreset_test, host_sreset_adapter_test))]
#[repr(C)]
pub struct NetDevice {
    pub dummy: i32,
}

#[cfg(all(host_sreset_test, host_sreset_adapter_test))]
#[repr(C)]
pub struct HostAdapter {
    pub dvobj: DvobjPriv,
    pub mlmepriv: MlmePriv,
    pub xmitpriv: XmitPriv,
    pub pnetdev: *mut NetDevice,
    #[cfg(CONFIG_CONCURRENT_MODE)]
    pub adapter_type: u8,
}

#[cfg(all(host_sreset_test, host_sreset_adapter_test))]
type HostPadapter = *mut HostAdapter;

#[cfg(all(host_sreset_test, host_sreset_adapter_test))]
const HOST_PRIMARY_ADAPTER: u8 = 0;
#[cfg(all(host_sreset_test, host_sreset_adapter_test))]
const HOST_VIRTUAL_ADAPTER: u8 = 1;

#[cfg(all(host_sreset_test, host_sreset_adapter_test))]
fn host_is_primary_adapter(adapter: &HostAdapter) -> bool {
    #[cfg(CONFIG_CONCURRENT_MODE)]
    {
        adapter.adapter_type == HOST_PRIMARY_ADAPTER
    }
    #[cfg(not(CONFIG_CONCURRENT_MODE))]
    {
        let _ = adapter;
        true
    }
}

#[cfg(all(host_sreset_test, host_sreset_adapter_test))]
mod host_adapter {
    use super::*;

    extern "C" {
        fn host_sreset_check_fwstate(m: *mut MlmePriv, s: i32) -> i32;
        fn host_sreset_rtw_netif_stop_queue(dev: *mut NetDevice);
        fn host_sreset_rtw_netif_wake_queue(dev: *mut NetDevice);
        fn host_sreset_rtw_cancel_all_timer(padapter: HostPadapter);
        fn host_sreset_tasklet_kill(t: *mut XmitTasklet);
        fn host_sreset_tasklet_hi_schedule(t: *mut XmitTasklet);
        fn host_sreset_rtw_scan_abort(padapter: HostPadapter);
        fn host_sreset_rtw_set_to_roam(padapter: HostPadapter, to_roam: u8);
        fn host_sreset_rtw_join_timeout_handler(padapter: HostPadapter);
        fn host_sreset_restore_network_status(padapter: HostPadapter);
        fn host_sreset_set_timer(t: *mut TimerList, ms: u32);
    }

    fn check_fwstate(m: &MlmePriv, mask: u32) -> bool {
        unsafe { host_sreset_check_fwstate(m as *const _ as *mut MlmePriv, mask as i32) != 0 }
    }

    pub fn stop_adapter(padapter: HostPadapter) {
        if padapter.is_null() {
            return;
        }
        unsafe {
            let pnetdev = (*padapter).pnetdev;
            host_sreset_rtw_netif_stop_queue(pnetdev);
            host_sreset_rtw_cancel_all_timer(padapter);
            host_sreset_tasklet_kill(&mut (*padapter).xmitpriv.xmit_tasklet);
            if check_fwstate(&(*padapter).mlmepriv, WIFI_UNDER_SURVEY) {
                host_sreset_rtw_scan_abort(padapter);
            }
            if check_fwstate(&(*padapter).mlmepriv, WIFI_UNDER_LINKING) {
                host_sreset_rtw_set_to_roam(padapter, 0);
                host_sreset_rtw_join_timeout_handler(padapter);
            }
        }
    }

    pub fn start_adapter(padapter: HostPadapter) {
        if padapter.is_null() {
            return;
        }
        unsafe {
            let pnetdev = (*padapter).pnetdev;
            if check_fwstate(&(*padapter).mlmepriv, WIFI_ASOC_STATE) {
                host_sreset_restore_network_status(padapter);
            }
            host_sreset_tasklet_hi_schedule(&mut (*padapter).xmitpriv.xmit_tasklet);
            if host_is_primary_adapter(&*padapter) {
                host_sreset_set_timer(&mut (*padapter).dvobj.dynamic_chk_timer, 2000);
            }
            host_sreset_rtw_netif_wake_queue(pnetdev);
        }
    }
}

#[cfg(host_sreset_test)]
static mut G_REG_TXDMA: u32 = 0;

#[cfg(host_sreset_test)]
#[no_mangle]
pub extern "C" fn host_sreset_set_reg_read(addr: u32, val: u32) {
    if addr == REG_TXDMA_STATUS {
        unsafe { G_REG_TXDMA = val };
    }
}

#[cfg(host_sreset_test)]
#[no_mangle]
pub extern "C" fn host_sreset_clear_reg_reads() {
    unsafe { G_REG_TXDMA = 0 };
}

#[cfg(host_sreset_test)]
fn hal_data(adapter: Padapter) -> *mut HalData {
    unsafe { &mut (*adapter).HalData }
}

#[cfg(not(host_sreset_test))]
mod kernel {
    use super::*;

    extern "C" {
        fn rtw_rust_sreset_mutex_init(padapter: Padapter);
        fn rtw_rust_sreset_silent_inprogress_ptr(padapter: Padapter) -> *mut u8;
        fn rtw_rust_sreset_wifi_error_status_ptr(padapter: Padapter) -> *mut u8;
        fn rtw_rust_sreset_last_tx_time_ptr(padapter: Padapter) -> *mut Systime;
        fn rtw_rust_sreset_last_tx_complete_time_ptr(padapter: Padapter) -> *mut Systime;
        fn rtw_rust_sreset_read32(padapter: Padapter, addr: u32) -> u32;
    }

    pub fn mutex_init(padapter: Padapter) {
        unsafe { rtw_rust_sreset_mutex_init(padapter) };
    }

    pub fn silent_inprogress(padapter: Padapter) -> *mut u8 {
        unsafe { rtw_rust_sreset_silent_inprogress_ptr(padapter) }
    }

    pub fn wifi_error_status(padapter: Padapter) -> *mut u8 {
        unsafe { rtw_rust_sreset_wifi_error_status_ptr(padapter) }
    }

    pub fn last_tx_time(padapter: Padapter) -> *mut Systime {
        unsafe { rtw_rust_sreset_last_tx_time_ptr(padapter) }
    }

    pub fn last_tx_complete_time(padapter: Padapter) -> *mut Systime {
        unsafe { rtw_rust_sreset_last_tx_complete_time_ptr(padapter) }
    }

    pub fn read32(padapter: Padapter, addr: u32) -> u32 {
        unsafe { rtw_rust_sreset_read32(padapter, addr) }
    }
}

#[cfg(host_sreset_test)]
fn read32(_adapter: Padapter, addr: u32) -> u32 {
    if addr == REG_TXDMA_STATUS {
        unsafe { G_REG_TXDMA }
    } else {
        0
    }
}

#[no_mangle]
pub extern "C" fn sreset_init_value(padapter: Padapter) {
    if padapter.is_null() {
        return;
    }
    #[cfg(host_sreset_test)]
    unsafe {
        let p = &mut (*hal_data(padapter)).srestpriv;
        p.silent_reset_inprogress = _FALSE;
        p.wifi_error_status = WIFI_STATUS_SUCCESS;
        p.last_tx_time = 0;
        p.last_tx_complete_time = 0;
    }
    #[cfg(not(host_sreset_test))]
    unsafe {
        kernel::mutex_init(padapter);
        let inprog = kernel::silent_inprogress(padapter);
        let err = kernel::wifi_error_status(padapter);
        let tx = kernel::last_tx_time(padapter);
        let txc = kernel::last_tx_complete_time(padapter);
        if !inprog.is_null() {
            *inprog = _FALSE;
        }
        if !err.is_null() {
            *err = WIFI_STATUS_SUCCESS;
        }
        if !tx.is_null() {
            *tx = 0;
        }
        if !txc.is_null() {
            *txc = 0;
        }
    }
}

#[no_mangle]
pub extern "C" fn sreset_reset_value(padapter: Padapter) {
    if padapter.is_null() {
        return;
    }
    #[cfg(host_sreset_test)]
    unsafe {
        let p = &mut (*hal_data(padapter)).srestpriv;
        p.wifi_error_status = WIFI_STATUS_SUCCESS;
        p.last_tx_time = 0;
        p.last_tx_complete_time = 0;
    }
    #[cfg(not(host_sreset_test))]
    unsafe {
        let err = kernel::wifi_error_status(padapter);
        let tx = kernel::last_tx_time(padapter);
        let txc = kernel::last_tx_complete_time(padapter);
        if !err.is_null() {
            *err = WIFI_STATUS_SUCCESS;
        }
        if !tx.is_null() {
            *tx = 0;
        }
        if !txc.is_null() {
            *txc = 0;
        }
    }
}

#[no_mangle]
pub extern "C" fn sreset_get_wifi_status(padapter: Padapter) -> u8 {
    if padapter.is_null() {
        return WIFI_STATUS_SUCCESS;
    }
    #[cfg(host_sreset_test)]
    unsafe {
        let p = &mut (*hal_data(padapter)).srestpriv;
        let mut status = WIFI_STATUS_SUCCESS;
        if p.silent_reset_inprogress == _TRUE {
            return status;
        }
        let val32 = read32(padapter, REG_TXDMA_STATUS);
        if val32 == 0xeaeaeaea {
            p.wifi_error_status = WIFI_IF_NOT_EXIST;
        } else if val32 != 0 {
            p.wifi_error_status = WIFI_MAC_TXDMA_ERROR;
        }
        if p.wifi_error_status != WIFI_STATUS_SUCCESS {
            status = p.wifi_error_status & !(USB_READ_PORT_FAIL | USB_WRITE_PORT_FAIL);
        }
        p.wifi_error_status = WIFI_STATUS_SUCCESS;
        status
    }
    #[cfg(not(host_sreset_test))]
    unsafe {
        let mut status = WIFI_STATUS_SUCCESS;
        let inprog = kernel::silent_inprogress(padapter);
        let err = kernel::wifi_error_status(padapter);
        if inprog.is_null() || err.is_null() {
            return WIFI_STATUS_SUCCESS;
        }
        if *inprog == _TRUE {
            return status;
        }
        let val32 = kernel::read32(padapter, REG_TXDMA_STATUS);
        if val32 == 0xeaeaeaea {
            *err = WIFI_IF_NOT_EXIST;
        } else if val32 != 0 {
            *err = WIFI_MAC_TXDMA_ERROR;
        }
        if *err != WIFI_STATUS_SUCCESS {
            status = *err & !(USB_READ_PORT_FAIL | USB_WRITE_PORT_FAIL);
        }
        *err = WIFI_STATUS_SUCCESS;
        status
    }
}

#[no_mangle]
pub extern "C" fn sreset_set_wifi_error_status(padapter: Padapter, status: c_uint) {
    if padapter.is_null() {
        return;
    }
    #[cfg(host_sreset_test)]
    unsafe {
        (*hal_data(padapter)).srestpriv.wifi_error_status = status as u8;
    }
    #[cfg(not(host_sreset_test))]
    unsafe {
        let err = kernel::wifi_error_status(padapter);
        if !err.is_null() {
            *err = status as u8;
        }
    }
}

#[cfg(all(host_sreset_test, host_sreset_adapter_test))]
#[no_mangle]
pub extern "C" fn sreset_stop_adapter(padapter: HostPadapter) {
    host_adapter::stop_adapter(padapter);
}

#[cfg(all(host_sreset_test, host_sreset_adapter_test))]
#[no_mangle]
pub extern "C" fn sreset_start_adapter(padapter: HostPadapter) {
    host_adapter::start_adapter(padapter);
}

#[no_mangle]
pub extern "C" fn sreset_inprogress(padapter: Padapter) -> u8 {
    if padapter.is_null() {
        return _FALSE;
    }
    #[cfg(host_sreset_test)]
    unsafe {
        (*hal_data(padapter)).srestpriv.silent_reset_inprogress
    }
    #[cfg(not(host_sreset_test))]
    unsafe {
        let inprog = kernel::silent_inprogress(padapter);
        if inprog.is_null() {
            _FALSE
        } else {
            *inprog
        }
    }
}
