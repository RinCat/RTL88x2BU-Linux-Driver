// SPDX-License-Identifier: GPL-2.0
//! W3-81 `_update_beacon` dispatcher (kernel).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    unreachable_pub
)]

use core::ffi::c_void;

type U8 = u8;
type U32 = u32;
type IrqL = u32;
type Adapter = *mut c_void;

const _TRUE: U8 = 1;
const _FALSE: U8 = 0;
const _TIM_IE_: U8 = 5;
const _ERPINFO_IE_: U8 = 42;
const _HT_CAPABILITY_IE_: U8 = 45;
const _RSN_IE_2_: U8 = 48;
const _HT_ADD_INFO_IE_: U8 = 61;
const _EXT_CAP_IE_: U8 = 127;
const _VENDOR_SPECIFIC_IE_: U8 = 221;
const RTW_CMDF_WAIT_ACK: U8 = 1 << 1;

extern "C" {
    fn update_BCNTIM(padapter: Adapter);
    fn update_bcn_erpinfo_ie(padapter: Adapter);
    fn update_bcn_htinfo_ie(padapter: Adapter);
    fn update_bcn_vendor_spec_ie(padapter: Adapter, oui: *mut U8);
    fn set_tx_beacon_cmd(padapter: Adapter, flags: U8) -> U8;
    fn rtw_rust_bcn_dispatch_update_ext_capab_ie(padapter: Adapter);

    fn rtw_rust_bcn_dispatch_bstart_bss(adapter: Adapter) -> U8;
    fn rtw_rust_bcn_dispatch_bcn_lock(adapter: Adapter) -> *mut c_void;
    fn rtw_rust_bcn_dispatch_set_update_bcn(adapter: Adapter, v: U8);
    fn rtw_rust_bcn_dispatch_enter_critical(lock: *mut c_void, irqL: *mut IrqL);
    fn rtw_rust_bcn_dispatch_exit_critical(lock: *mut c_void, irqL: *mut IrqL);

    #[cfg(config_rtw_mesh)]
    fn rtw_rust_bcn_dispatch_mesh_config(padapter: Adapter) -> U8;
}

#[no_mangle]
pub extern "C" fn _update_beacon(
    padapter: Adapter,
    ie_id: U8,
    oui: *mut U8,
    tx: U8,
    flags: U8,
    tag: *const u8,
) {
    let _ = tag;
    if padapter.is_null() {
        return;
    }
    unsafe {
        if rtw_rust_bcn_dispatch_bstart_bss(padapter) == _FALSE {
            return;
        }

        let mut irqL: IrqL = 0;
        let lock = rtw_rust_bcn_dispatch_bcn_lock(padapter);
        rtw_rust_bcn_dispatch_enter_critical(lock, &mut irqL);

        let updated = match ie_id {
            x if x == _TIM_IE_ => {
                update_BCNTIM(padapter);
                true
            }
            x if x == _ERPINFO_IE_ => {
                update_bcn_erpinfo_ie(padapter);
                true
            }
            x if x == _HT_CAPABILITY_IE_ => true,
            x if x == _RSN_IE_2_ => true,
            x if x == _HT_ADD_INFO_IE_ => {
                update_bcn_htinfo_ie(padapter);
                true
            }
            x if x == _EXT_CAP_IE_ => {
                rtw_rust_bcn_dispatch_update_ext_capab_ie(padapter);
                true
            }
            #[cfg(config_rtw_mesh)]
            x if x == 113 => rtw_rust_bcn_dispatch_mesh_config(padapter) != 0,
            x if x == _VENDOR_SPECIFIC_IE_ => {
                update_bcn_vendor_spec_ie(padapter, oui);
                true
            }
            _ => true,
        };

        if updated {
            rtw_rust_bcn_dispatch_set_update_bcn(padapter, _TRUE);
        }

        rtw_rust_bcn_dispatch_exit_critical(lock, &mut irqL);

        #[cfg(not(config_interrupt_based_txbcn))]
        #[cfg(any(
            config_usb_hci,
            config_sdio_hci,
            config_gspi_hci,
            config_pci_bcn_polling
        ))]
        if tx != 0 && updated {
            if flags == RTW_CMDF_WAIT_ACK {
                set_tx_beacon_cmd(padapter, RTW_CMDF_WAIT_ACK);
            } else {
                set_tx_beacon_cmd(padapter, 0);
            }
        }
    }
}
