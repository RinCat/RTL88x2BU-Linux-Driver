// SPDX-License-Identifier: GPL-2.0
//! W3-81 beacon HT/WPS/ERP refresh — Rust port of `core/rtw_ap_bcn_update.c` (host L2).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    unreachable_pub
)]
#![cfg(host_ap_bcn_update_test)]

use core::ffi::c_void;

type U8 = u8;
type U16 = u16;
type U32 = u32;
type Sint = i32;

const _BEACON_IE_OFFSET_: usize = 12;
const _ERPINFO_IE_: Sint = 42;
const _HT_ADD_INFO_IE_: Sint = 61;
const _TRUE: U8 = 1;
const _FALSE: U8 = 0;
const CHANNEL_WIDTH_40: U8 = 2;
const HAL_PRIME_CHNL_OFFSET_LOWER: U8 = 1;
const HT_INFO_HT_PARAM_SECONDARY_CHNL_ABOVE: U8 = 1;
const HT_INFO_HT_PARAM_SECONDARY_CHNL_BELOW: U8 = 3;

const RTW_ERP_INFO_NON_ERP_PRESENT: U8 = 1 << 0;
const RTW_ERP_INFO_USE_PROTECTION: U8 = 1 << 1;
const RTW_ERP_INFO_BARKER_PREAMBLE_MODE: U8 = 1 << 2;

#[repr(C)]
struct Ndis80211VariableIes {
    element_id: U8,
    length: U8,
    data: [U8; 1],
}

type Pndis80211VariableIes = *mut Ndis80211VariableIes;

#[repr(C)]
struct WlanBssidEx {
    ie_length: U32,
    ies: [U8; 256],
}

#[repr(C)]
struct HtPriv {
    ht_option: U8,
}

#[repr(C)]
struct MlmeExtInfo {
    erp_enable: U8,
    ht_info_enable: U8,
    network: WlanBssidEx,
}

#[repr(C)]
struct MlmeExtPriv {
    mlmext_info: MlmeExtInfo,
    cur_channel: U8,
    cur_bwmode: U8,
    cur_ch_offset: U8,
}

#[repr(C)]
struct MlmePriv {
    num_sta_non_erp: i32,
    num_sta_no_short_preamble: i32,
    wps_beacon_ie: *mut U8,
    htpriv: HtPriv,
    ht_op_mode: U16,
    num_sta_40mhz_intolerant: i32,
    ht_20mhz_width_req: U8,
    ht_intolerant_ch_reported: U8,
    olbc: i32,
    sw_to_20mhz: U8,
}

#[repr(C)]
struct Adapter {
    mlmepriv: MlmePriv,
    mlmeextpriv: MlmeExtPriv,
}

#[repr(C)]
struct HtInfoElement {
    primary_channel: U8,
    infos: [U8; 5],
    mcs_rate: [U8; 16],
}

extern "C" {
    fn rtw_get_ie(pbuf: *const U8, index: Sint, len: *mut Sint, limit: Sint) -> *mut U8;
    static mut host_bcn_update_last_erp_byte: U8;
    static mut host_bcn_update_last_ht_op_mode: U16;
    static mut host_bcn_update_last_ht_info_byte: U8;
}

fn cpu_to_le16(x: U16) -> U16 {
    x.to_le()
}

fn set_ht_op_ele_2nd_chl_offset(p: *mut HtInfoElement, v: U8) {
    unsafe {
        p.as_mut().unwrap().infos[0] = (p.as_ref().unwrap().infos[0] & !0x3) | (v & 0x3);
    }
}

fn set_ht_op_ele_sta_chl_width(p: *mut HtInfoElement, v: U8) {
    unsafe {
        p.as_mut().unwrap().infos[0] = (p.as_ref().unwrap().infos[0] & !0x4) | ((v & 1) << 2);
    }
}

#[no_mangle]
pub extern "C" fn ERP_IE_handler(_padapter: *mut c_void, pie: Pndis80211VariableIes) {
    unsafe {
        if !pie.is_null() && (*pie).length > 0 {
            host_bcn_update_last_erp_byte = (*pie).data[0];
        }
    }
}

#[no_mangle]
pub extern "C" fn update_bcn_erpinfo_ie(padapter: *mut c_void) {
    if padapter.is_null() {
        return;
    }
    unsafe {
        let padapter = padapter as *mut Adapter;
        let pmlmepriv = &mut (*padapter).mlmepriv;
        let pmlmeext = &mut (*padapter).mlmeextpriv;
        let pmlmeinfo = &mut pmlmeext.mlmext_info;
        let pnetwork = &mut pmlmeinfo.network;
        let ie = pnetwork.ies.as_mut_ptr();
        let mut len: Sint = 0;

        if pmlmeinfo.erp_enable == 0 {
            return;
        }

        let p = rtw_get_ie(
            ie.add(_BEACON_IE_OFFSET_),
            _ERPINFO_IE_,
            &mut len,
            (pnetwork.ie_length as Sint) - _BEACON_IE_OFFSET_ as Sint,
        );
        if p.is_null() || len <= 0 {
            return;
        }
        let pie = p as Pndis80211VariableIes;
        if pmlmepriv.num_sta_non_erp == 1 {
            (*pie).data[0] |= RTW_ERP_INFO_NON_ERP_PRESENT | RTW_ERP_INFO_USE_PROTECTION;
        } else {
            (*pie).data[0] &= !(RTW_ERP_INFO_NON_ERP_PRESENT | RTW_ERP_INFO_USE_PROTECTION);
        }
        if pmlmepriv.num_sta_no_short_preamble > 0 {
            (*pie).data[0] |= RTW_ERP_INFO_BARKER_PREAMBLE_MODE;
        } else {
            (*pie).data[0] &= !RTW_ERP_INFO_BARKER_PREAMBLE_MODE;
        }
        ERP_IE_handler(padapter.cast(), pie);
    }
}

#[no_mangle]
pub extern "C" fn update_bcn_htinfo_ie(padapter: *mut c_void) {
    if padapter.is_null() {
        return;
    }
    unsafe {
        let padapter = padapter as *mut Adapter;
        let pmlmepriv = &mut (*padapter).mlmepriv;
        let pmlmeext = &mut (*padapter).mlmeextpriv;
        let pmlmeinfo = &mut pmlmeext.mlmext_info;
        let pnetwork = &mut pmlmeinfo.network;
        let ie = pnetwork.ies.as_mut_ptr();
        let mut len: Sint = 0;

        if pmlmepriv.htpriv.ht_option == _FALSE {
            return;
        }
        if pmlmeinfo.ht_info_enable != 1 {
            return;
        }

        let p = rtw_get_ie(
            ie.add(_BEACON_IE_OFFSET_),
            _HT_ADD_INFO_IE_,
            &mut len,
            (pnetwork.ie_length as Sint) - _BEACON_IE_OFFSET_ as Sint,
        );
        if p.is_null() || len <= 0 {
            return;
        }
        let pht_info = p.add(2) as *mut HtInfoElement;

        if pmlmepriv.sw_to_20mhz == 0 && pmlmeext.cur_channel <= 14 {
            if pmlmepriv.num_sta_40mhz_intolerant > 0
                || pmlmepriv.ht_20mhz_width_req == _TRUE
                || pmlmepriv.ht_intolerant_ch_reported == _TRUE
                || pmlmepriv.olbc > 0
            {
                set_ht_op_ele_2nd_chl_offset(pht_info, 0);
                set_ht_op_ele_sta_chl_width(pht_info, 0);
                pmlmepriv.sw_to_20mhz = 1;
            }
        } else if pmlmepriv.num_sta_40mhz_intolerant == 0
            && pmlmepriv.ht_20mhz_width_req == _FALSE
            && pmlmepriv.ht_intolerant_ch_reported == _FALSE
            && pmlmepriv.olbc == 0
        {
            if pmlmeext.cur_bwmode >= CHANNEL_WIDTH_40 {
                set_ht_op_ele_sta_chl_width(pht_info, 1);
                let sec = if pmlmeext.cur_ch_offset == HAL_PRIME_CHNL_OFFSET_LOWER {
                    HT_INFO_HT_PARAM_SECONDARY_CHNL_ABOVE
                } else {
                    HT_INFO_HT_PARAM_SECONDARY_CHNL_BELOW
                };
                set_ht_op_ele_2nd_chl_offset(pht_info, sec);
                pmlmepriv.sw_to_20mhz = 0;
            }
        }

        let le = cpu_to_le16(pmlmepriv.ht_op_mode);
        (*pht_info).infos[1] = (le & 0xff) as U8;
        (*pht_info).infos[2] = (le >> 8) as U8;
        host_bcn_update_last_ht_info_byte = (*pht_info).infos[0];
        host_bcn_update_last_ht_op_mode =
            (*pht_info).infos[1] as U16 | ((*pht_info).infos[2] as U16) << 8;
    }
}
