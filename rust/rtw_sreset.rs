// SPDX-License-Identifier: GPL-2.0
//! W3-95 sreset lifecycle — Rust port of `core/rtw_sreset.c` helpers (host L2 scope).

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
use core::ffi::c_uint;

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
    WIFI_STATUS_SUCCESS
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
}
