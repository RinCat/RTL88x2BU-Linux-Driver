// SPDX-License-Identifier: GPL-2.0
//! W3-84 VHT IE build helpers — Rust port of `core/rtw_vht_build.c` (host L2 slice).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(not(host_vht_build_test))]
use core::ffi::c_void;
#[cfg(host_vht_build_test)]
use std::os::raw::c_void;

const EID_VHTOperation: u8 = 192;
const CHANNEL_WIDTH_80: u8 = 2;
const BW_CAP_80M: u8 = 1 << 4;
const BW_CAP_160M: u8 = 1 << 5;
const HAL_PRIME_CHNL_OFFSET_LOWER: u8 = 1;

#[repr(C)]
pub struct RegistryPriv {
    pub bw_mode: u8,
    pub ampdu_factor: u8,
}

#[repr(C)]
pub struct VhtPriv {
    pub vht_cap: [u8; 32],
    pub vht_mcs_map: [u8; 2],
    pub ldpc_cap: u8,
    pub stbc_cap: u8,
    pub sgi_80m: u8,
    pub vht_highest_rate: u8,
}

#[repr(C)]
pub struct MlmePriv {
    pub vhtpriv: VhtPriv,
}

#[repr(C)]
pub struct HostVhtBuildFixture {
    pub rx_packet_offset: u32,
    pub max_recvbuf_sz: u32,
    pub rx_stbc_nss: u8,
    pub hal_max_bw: u8,
    pub hal_bw_support: [u8; 5],
}

#[repr(C)]
pub struct MlmeExtInfo {
    pub assoc_AP_vendor: u8,
}

#[repr(C)]
pub struct MlmeExtPriv {
    pub mlmext_info: MlmeExtInfo,
}

#[repr(C)]
pub struct Adapter {
    pub registrypriv: RegistryPriv,
    pub mlmepriv: MlmePriv,
    pub mlmeextpriv: MlmeExtPriv,
    pub host_fixture: HostVhtBuildFixture,
}

fn regsty_bw_5g(reg: &RegistryPriv) -> u8 {
    reg.bw_mode >> 4
}

#[cfg(host_vht_build_test)]
mod host {
    use super::*;

    extern "C" {
        fn hal_chk_bw_cap(adapter: *mut Adapter, cap: u8) -> bool;
        fn rtw_get_center_ch(ch: u8, bw: u8, offset: u8) -> u8;
        fn rtw_set_ie(
            pbuf: *mut u8,
            index: i32,
            len: u32,
            source: *const u8,
            frlen: *mut u32,
        ) -> *mut u8;
    }

    fn set_vht_operation_chl_width(p: &mut [u8; 5], v: u8) {
        p[0] = v;
    }

    fn set_vht_operation_center1(p: &mut [u8; 5], v: u8) {
        p[1] = v;
    }

    fn set_vht_operation_center2(p: &mut [u8; 5], v: u8) {
        p[2] = v;
    }

    pub fn build_vht_operation_ie(padapter: *mut Adapter, pbuf: *mut u8, channel: u8) -> u32 {
        if padapter.is_null() || pbuf.is_null() {
            return 0;
        }
        unsafe {
            let padapter_ref = &*padapter;
            let pvhtpriv = &padapter_ref.mlmepriv.vhtpriv;
            let mut operation = [0u8; 5];
            let bw_mode = regsty_bw_5g(&padapter_ref.registrypriv);
            let (chnl_width, center_freq) = if hal_chk_bw_cap(padapter, BW_CAP_80M | BW_CAP_160M)
                && regsty_bw_5g(&padapter_ref.registrypriv) >= CHANNEL_WIDTH_80
            {
                (
                    1u8,
                    rtw_get_center_ch(channel, bw_mode, HAL_PRIME_CHNL_OFFSET_LOWER),
                )
            } else {
                (0u8, 0u8)
            };

            set_vht_operation_chl_width(&mut operation, chnl_width);
            set_vht_operation_center1(&mut operation, center_freq);
            set_vht_operation_center2(&mut operation, 0);
            operation[3..5].copy_from_slice(&pvhtpriv.vht_mcs_map);

            let mut len: u32 = 0;
            rtw_set_ie(
                pbuf,
                EID_VHTOperation as i32,
                5,
                operation.as_ptr(),
                &mut len,
            );
            len
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_build_vht_operation_ie(
    padapter: *mut c_void,
    pbuf: *mut u8,
    channel: u8,
) -> u32 {
    #[cfg(host_vht_build_test)]
    {
        host::build_vht_operation_ie(padapter as *mut Adapter, pbuf, channel)
    }
    #[cfg(not(host_vht_build_test))]
    {
        let _ = (padapter, pbuf, channel);
        0
    }
}
