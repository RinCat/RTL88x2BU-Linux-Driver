// SPDX-License-Identifier: GPL-2.0
//! W3-81 beacon HT/WPS/ERP kernel object.

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
type U16 = u16;
type U32 = u32;
type Sint = i32;
type Adapter = *mut c_void;
type NetPtr = *mut c_void;
type Pndis80211VariableIes = *mut c_void;

const _BEACON_IE_OFFSET_: usize = 12;
const _FIXED_IE_LENGTH_: U32 = 12;
const _ERPINFO_IE_: Sint = 42;
const _HT_ADD_INFO_IE_: Sint = 61;
const MAX_IE_SZ: U32 = 257;
const _TRUE: U8 = 1;
const _FALSE: U8 = 0;
const CHANNEL_WIDTH_40: U8 = 2;
const HAL_PRIME_CHNL_OFFSET_LOWER: U8 = 1;
const HT_INFO_HT_PARAM_SECONDARY_CHNL_ABOVE: U8 = 1;
const HT_INFO_HT_PARAM_SECONDARY_CHNL_BELOW: U8 = 3;
const RTW_ERP_INFO_NON_ERP_PRESENT: U8 = 1 << 0;
const RTW_ERP_INFO_USE_PROTECTION: U8 = 1 << 1;
const RTW_ERP_INFO_BARKER_PREAMBLE_MODE: U8 = 1 << 2;

extern "C" {
    fn rtw_get_ie(pbuf: *const U8, index: Sint, len: *mut Sint, limit: Sint) -> *mut U8;
    fn _rtw_malloc(sz: U32) -> *mut c_void;
    fn _rtw_mfree(p: *mut c_void, sz: U32);
    fn _rtw_memcpy(dst: *mut U8, src: *const U8, sz: U32);
    fn rtw_get_wps_ie(
        in_ie: *const U8,
        in_len: U32,
        wps_ie: *mut U8,
        wps_ielen: *mut U32,
    ) -> *mut U8;
    fn ERP_IE_handler(padapter: Adapter, pie: Pndis80211VariableIes);

    fn rtw_rust_bcn_update_network(adapter: Adapter) -> NetPtr;
    fn rtw_rust_ap_bcn_net_ie_len(net: NetPtr) -> *mut U32;
    fn rtw_rust_ap_bcn_net_ies(net: NetPtr) -> *mut U8;
    fn rtw_rust_bcn_update_erp_enable(adapter: Adapter) -> U8;
    fn rtw_rust_bcn_update_ht_info_enable(adapter: Adapter) -> U8;
    fn rtw_rust_bcn_update_num_sta_non_erp(adapter: Adapter) -> Sint;
    fn rtw_rust_bcn_update_num_sta_no_short_preamble(adapter: Adapter) -> Sint;
    fn rtw_rust_bcn_update_ht_option(adapter: Adapter) -> U8;
    fn rtw_rust_bcn_update_ht_op_mode(adapter: Adapter) -> U16;
    fn rtw_rust_bcn_update_wps_beacon_ie(adapter: Adapter) -> *mut U8;
    fn rtw_rust_bcn_update_sw_to_20mhz(adapter: Adapter) -> U8;
    fn rtw_rust_bcn_update_set_sw_to_20mhz(adapter: Adapter, v: U8);
    fn rtw_rust_bcn_update_num_sta_40mhz_intolerant(adapter: Adapter) -> Sint;
    fn rtw_rust_bcn_update_ht_20mhz_width_req(adapter: Adapter) -> U8;
    fn rtw_rust_bcn_update_ht_intolerant_ch_reported(adapter: Adapter) -> U8;
    fn rtw_rust_bcn_update_olbc_is_true(adapter: Adapter) -> U8;
    fn rtw_rust_bcn_update_cur_channel(adapter: Adapter) -> U8;
    fn rtw_rust_bcn_update_cur_bwmode(adapter: Adapter) -> U8;
    fn rtw_rust_bcn_update_cur_ch_offset(adapter: Adapter) -> U8;
    #[cfg(any(config_interrupt_based_txbcn, config_pci_hci))]
    fn rtw_rust_bcn_update_wps_fwstate(adapter: Adapter, pwps_ie_src: *mut U8, wps_ielen: U32);
    fn _rtw_memcmp(a: *const c_void, b: *const c_void, n: U32) -> Sint;
    static RTW_WPA_OUI: [U8; 4];
    static WMM_OUI: [U8; 4];
    static WPS_OUI: [U8; 4];
    static P2P_OUI: [U8; 4];
}

fn net(adapter: Adapter) -> NetPtr {
    unsafe { rtw_rust_bcn_update_network(adapter) }
}

fn set_ht_op_ele_2nd_chl_offset(infos: *mut U8, v: U8) {
    unsafe {
        *infos = (*infos & !0x3) | (v & 0x3);
    }
}

fn set_ht_op_ele_sta_chl_width(infos: *mut U8, v: U8) {
    unsafe {
        *infos = (*infos & !0x4) | ((v & 1) << 2);
    }
}

#[no_mangle]
pub extern "C" fn update_bcn_erpinfo_ie(padapter: Adapter) {
    if padapter.is_null() {
        return;
    }
    unsafe {
        if rtw_rust_bcn_update_erp_enable(padapter) == 0 {
            return;
        }
        let pnetwork = net(padapter);
        let ie = rtw_rust_ap_bcn_net_ies(pnetwork);
        let mut len: Sint = 0;
        let p = rtw_get_ie(
            ie.add(_BEACON_IE_OFFSET_),
            _ERPINFO_IE_,
            &mut len,
            (*rtw_rust_ap_bcn_net_ie_len(pnetwork) as Sint) - _BEACON_IE_OFFSET_ as Sint,
        );
        if p.is_null() || len <= 0 {
            return;
        }
        let pie = p as Pndis80211VariableIes;
        let data = p.add(2);
        if rtw_rust_bcn_update_num_sta_non_erp(padapter) == 1 {
            *data |= RTW_ERP_INFO_NON_ERP_PRESENT | RTW_ERP_INFO_USE_PROTECTION;
        } else {
            *data &= !(RTW_ERP_INFO_NON_ERP_PRESENT | RTW_ERP_INFO_USE_PROTECTION);
        }
        if rtw_rust_bcn_update_num_sta_no_short_preamble(padapter) > 0 {
            *data |= RTW_ERP_INFO_BARKER_PREAMBLE_MODE;
        } else {
            *data &= !RTW_ERP_INFO_BARKER_PREAMBLE_MODE;
        }
        ERP_IE_handler(padapter, pie);
    }
}

#[no_mangle]
pub extern "C" fn update_bcn_htinfo_ie(padapter: Adapter) {
    if padapter.is_null() {
        return;
    }
    unsafe {
        if rtw_rust_bcn_update_ht_option(padapter) == _FALSE {
            return;
        }
        if rtw_rust_bcn_update_ht_info_enable(padapter) != 1 {
            return;
        }
        let pnetwork = net(padapter);
        let ie = rtw_rust_ap_bcn_net_ies(pnetwork);
        let mut len: Sint = 0;
        let p = rtw_get_ie(
            ie.add(_BEACON_IE_OFFSET_),
            _HT_ADD_INFO_IE_,
            &mut len,
            (*rtw_rust_ap_bcn_net_ie_len(pnetwork) as Sint) - _BEACON_IE_OFFSET_ as Sint,
        );
        if p.is_null() || len <= 0 {
            return;
        }
        let infos = p.add(2 + 1);

        if rtw_rust_bcn_update_sw_to_20mhz(padapter) == 0
            && rtw_rust_bcn_update_cur_channel(padapter) <= 14
        {
            if rtw_rust_bcn_update_num_sta_40mhz_intolerant(padapter) > 0
                || rtw_rust_bcn_update_ht_20mhz_width_req(padapter) == _TRUE
                || rtw_rust_bcn_update_ht_intolerant_ch_reported(padapter) == _TRUE
                || rtw_rust_bcn_update_olbc_is_true(padapter) != 0
            {
                set_ht_op_ele_2nd_chl_offset(infos, 0);
                set_ht_op_ele_sta_chl_width(infos, 0);
                rtw_rust_bcn_update_set_sw_to_20mhz(padapter, 1);
            }
        } else if rtw_rust_bcn_update_num_sta_40mhz_intolerant(padapter) == 0
            && rtw_rust_bcn_update_ht_20mhz_width_req(padapter) == _FALSE
            && rtw_rust_bcn_update_ht_intolerant_ch_reported(padapter) == _FALSE
            && rtw_rust_bcn_update_olbc_is_true(padapter) == 0
        {
            if rtw_rust_bcn_update_cur_bwmode(padapter) >= CHANNEL_WIDTH_40 {
                set_ht_op_ele_sta_chl_width(infos, 1);
                let sec =
                    if rtw_rust_bcn_update_cur_ch_offset(padapter) == HAL_PRIME_CHNL_OFFSET_LOWER {
                        HT_INFO_HT_PARAM_SECONDARY_CHNL_ABOVE
                    } else {
                        HT_INFO_HT_PARAM_SECONDARY_CHNL_BELOW
                    };
                set_ht_op_ele_2nd_chl_offset(infos, sec);
                rtw_rust_bcn_update_set_sw_to_20mhz(padapter, 0);
            }
        }

        let le = rtw_rust_bcn_update_ht_op_mode(padapter).to_le();
        *infos.add(1) = (le & 0xff) as U8;
        *infos.add(2) = (le >> 8) as U8;
    }
}

#[no_mangle]
pub extern "C" fn update_bcn_wps_ie(padapter: Adapter) {
    if padapter.is_null() {
        return;
    }
    unsafe {
        let pnetwork = net(padapter);
        let ie = rtw_rust_ap_bcn_net_ies(pnetwork);
        let ielen = *rtw_rust_ap_bcn_net_ie_len(pnetwork);
        let mut wps_ielen: U32 = 0;

        let pwps_ie = rtw_get_wps_ie(
            ie.add(_FIXED_IE_LENGTH_ as usize),
            ielen - _FIXED_IE_LENGTH_,
            core::ptr::null_mut(),
            &mut wps_ielen,
        );
        if pwps_ie.is_null() || wps_ielen == 0 {
            return;
        }

        let pwps_ie_src = rtw_rust_bcn_update_wps_beacon_ie(padapter);
        if pwps_ie_src.is_null() {
            return;
        }

        let wps_offset = pwps_ie.offset_from(ie) as U32;
        let premainder_ie = pwps_ie.add(wps_ielen as usize);
        let remainder_ielen = ielen - wps_offset - wps_ielen;

        let mut pbackup_remainder_ie: *mut U8 = core::ptr::null_mut();
        if remainder_ielen > 0 {
            pbackup_remainder_ie = _rtw_malloc(remainder_ielen) as *mut U8;
            if !pbackup_remainder_ie.is_null() {
                _rtw_memcpy(pbackup_remainder_ie, premainder_ie, remainder_ielen);
            }
        }

        wps_ielen = *pwps_ie_src.add(1) as U32;
        if wps_offset + wps_ielen + 2 + remainder_ielen <= MAX_IE_SZ {
            _rtw_memcpy(pwps_ie, pwps_ie_src, wps_ielen + 2);
            if !pbackup_remainder_ie.is_null() {
                _rtw_memcpy(
                    pwps_ie.add((wps_ielen + 2) as usize),
                    pbackup_remainder_ie,
                    remainder_ielen,
                );
            }
            *rtw_rust_ap_bcn_net_ie_len(pnetwork) = wps_offset + (wps_ielen + 2) + remainder_ielen;
        }

        if !pbackup_remainder_ie.is_null() {
            _rtw_mfree(pbackup_remainder_ie as *mut c_void, remainder_ielen);
        }

        #[cfg(any(config_interrupt_based_txbcn, config_pci_hci))]
        rtw_rust_bcn_update_wps_fwstate(padapter, pwps_ie_src, wps_ielen);
    }
}

#[no_mangle]
pub extern "C" fn update_bcn_vendor_spec_ie(padapter: Adapter, oui: *mut U8) {
    if padapter.is_null() || oui.is_null() {
        return;
    }
    unsafe {
        let oui_c = oui as *const c_void;
        if _rtw_memcmp(RTW_WPA_OUI.as_ptr() as *const c_void, oui_c, 4) != 0 {
            /* update_bcn_wpa_ie: log-only in C */
        } else if _rtw_memcmp(WMM_OUI.as_ptr() as *const c_void, oui_c, 4) != 0 {
            /* update_bcn_wmm_ie: log-only in C */
        } else if _rtw_memcmp(WPS_OUI.as_ptr() as *const c_void, oui_c, 4) != 0 {
            update_bcn_wps_ie(padapter);
        } else if _rtw_memcmp(P2P_OUI.as_ptr() as *const c_void, oui_c, 4) != 0 {
            /* update_bcn_p2p_ie: no-op in C */
        }
    }
}
