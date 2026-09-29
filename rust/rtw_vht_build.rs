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

const EID_VHTCapability: u8 = 191;
const EID_VHTOperation: u8 = 192;
const CHANNEL_WIDTH_80: u8 = 2;
const CHANNEL_WIDTH_160: u8 = 3;
const CHANNEL_WIDTH_80_80: u8 = 4;
const BW_CAP_80M: u8 = 1 << 4;
const BW_CAP_160M: u8 = 1 << 5;
const BW_CAP_80_80M: u8 = 1 << 6;
const HAL_PRIME_CHNL_OFFSET_LOWER: u8 = 1;
const LDPC_VHT_ENABLE_RX: u8 = 1;
const STBC_VHT_ENABLE_RX: u8 = 1;
const STBC_VHT_ENABLE_TX: u8 = 1 << 1;
const MGN_VHT1SS_MCS0: u8 = 0xA0;

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

fn regsty_is_bw_5g_support(reg: &RegistryPriv, bw: u8) -> bool {
    regsty_bw_5g(reg) >= bw
}

fn test_flag(v: u8, f: u8) -> bool {
    (v & f) != 0
}

#[cfg(host_vht_build_test)]
mod host {
    use super::*;

    #[repr(i32)]
    enum HalDefVariable {
        MaxRecvbufSz = 3,
        RxPacketOffset = 4,
        RxStbc = 15,
    }

    extern "C" {
        fn hal_chk_bw_cap(adapter: *mut Adapter, cap: u8) -> bool;
        fn hal_largest_bw(adapter: *mut Adapter, in_bw: u8) -> u8;
        fn rtw_get_center_ch(ch: u8, bw: u8, offset: u8) -> u8;
        fn rtw_hal_get_def_var(
            adapter: *mut Adapter,
            variable: HalDefVariable,
            value: *mut core::ffi::c_void,
        );
        fn rtw_set_ie(
            pbuf: *mut u8,
            index: i32,
            len: u32,
            source: *const u8,
            frlen: *mut u32,
        ) -> *mut u8;
    }

    static VHT_MCS_DATA_RATE: [[&[u16; 40]; 2]; 3] = [
        [
            &[
                13, 26, 39, 52, 78, 104, 117, 130, 156, 156, 26, 52, 78, 104, 156, 208, 234, 260,
                312, 312, 39, 78, 117, 156, 234, 312, 351, 390, 468, 520, 52, 104, 156, 208, 312,
                416, 468, 520, 624, 624,
            ],
            &[
                14, 29, 43, 58, 87, 116, 130, 144, 173, 173, 29, 58, 87, 116, 173, 231, 260, 289,
                347, 347, 43, 87, 130, 173, 260, 347, 390, 433, 520, 578, 58, 116, 173, 231, 347,
                462, 520, 578, 693, 693,
            ],
        ],
        [
            &[
                27, 54, 81, 108, 162, 216, 243, 270, 324, 360, 54, 108, 162, 216, 324, 432, 486,
                540, 648, 720, 81, 162, 243, 324, 486, 648, 729, 810, 972, 1080, 108, 216, 324,
                432, 648, 864, 972, 1080, 1296, 1440,
            ],
            &[
                30, 60, 90, 120, 180, 240, 270, 300, 360, 400, 60, 120, 180, 240, 360, 480, 540,
                600, 720, 800, 90, 180, 270, 360, 540, 720, 810, 900, 1080, 1200, 120, 240, 360,
                480, 720, 960, 1080, 1200, 1440, 1600,
            ],
        ],
        [
            &[
                59, 117, 176, 234, 351, 468, 527, 585, 702, 780, 117, 234, 351, 468, 702, 936,
                1053, 1170, 1404, 1560, 176, 351, 527, 702, 1053, 1404, 1580, 1755, 2106, 2340,
                234, 468, 702, 936, 1404, 1872, 2106, 2340, 2808, 3120,
            ],
            &[
                65, 130, 195, 260, 390, 520, 585, 650, 780, 867, 130, 260, 390, 520, 780, 1040,
                1170, 1300, 1560, 1734, 195, 390, 585, 780, 1170, 1560, 1755, 1950, 2340, 2600,
                260, 520, 780, 1040, 1560, 2080, 2340, 2600, 3120, 3467,
            ],
        ],
    ];

    fn set_bits_le_1byte(p: &mut [u8], offset: u32, length: u32, value: u8) {
        let mask = (((1u32 << length) - 1) << offset) as u8;
        p[0] = (p[0] & !mask) | ((value & ((1u32 << length) - 1) as u8) << offset);
    }

    fn set_bits_le_2byte(p: &mut [u8], offset: u32, length: u32, value: u16) {
        let cur = u16::from(p[0]) | (u16::from(p[1]) << 8);
        let mask = (((1u32 << length) - 1) << offset) as u16;
        let new = (cur & !mask) | (((value as u32) & ((1u32 << length) - 1)) as u16) << offset;
        p[0] = (new & 0xff) as u8;
        p[1] = (new >> 8) as u8;
    }

    fn vht_mcs_to_data_rate(bw: u8, short_gi: u8, vht_mcs_rate: u8) -> u16 {
        let idx = ((vht_mcs_rate.wrapping_sub(MGN_VHT1SS_MCS0)) & 0x3f) as usize;
        VHT_MCS_DATA_RATE[bw as usize][short_gi as usize][idx]
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

    pub fn build_vht_cap_ie(padapter: *mut Adapter, pbuf: *mut u8) -> u32 {
        if padapter.is_null() || pbuf.is_null() {
            return 0;
        }
        unsafe {
            let padapter_ref = &mut *padapter;
            let pregistrypriv = &padapter_ref.registrypriv;
            let pvhtpriv = &mut padapter_ref.mlmepriv.vhtpriv;
            let mut rx_packet_offset: u32 = 0;
            let mut max_recvbuf_sz: u32 = 0;
            let mut rx_stbc_nss: u8 = 0;

            pvhtpriv.vht_cap.fill(0);

            rtw_hal_get_def_var(
                padapter,
                HalDefVariable::RxPacketOffset,
                &mut rx_packet_offset as *mut u32 as *mut core::ffi::c_void,
            );
            rtw_hal_get_def_var(
                padapter,
                HalDefVariable::MaxRecvbufSz,
                &mut max_recvbuf_sz as *mut u32 as *mut core::ffi::c_void,
            );

            let avail = max_recvbuf_sz.saturating_sub(rx_packet_offset);
            let pcap = &mut pvhtpriv.vht_cap;
            if avail >= 11454 {
                set_bits_le_1byte(&mut pcap[0..1], 0, 2, 2);
            } else if avail >= 7991 {
                set_bits_le_1byte(&mut pcap[0..1], 0, 2, 1);
            } else if avail >= 3895 {
                set_bits_le_1byte(&mut pcap[0..1], 0, 2, 0);
            }

            if hal_chk_bw_cap(padapter, BW_CAP_160M)
                && regsty_is_bw_5g_support(pregistrypriv, CHANNEL_WIDTH_160)
            {
                if hal_chk_bw_cap(padapter, BW_CAP_80_80M)
                    && regsty_is_bw_5g_support(pregistrypriv, CHANNEL_WIDTH_80_80)
                {
                    set_bits_le_1byte(&mut pcap[0..1], 2, 2, 2);
                } else {
                    set_bits_le_1byte(&mut pcap[0..1], 2, 2, 1);
                }
            } else {
                set_bits_le_1byte(&mut pcap[0..1], 2, 2, 0);
            }

            if test_flag(pvhtpriv.ldpc_cap, LDPC_VHT_ENABLE_RX) {
                set_bits_le_1byte(&mut pcap[0..1], 4, 1, 1);
            }

            set_bits_le_1byte(
                &mut pcap[0..1],
                5,
                1,
                if pvhtpriv.sgi_80m != 0 { 1 } else { 0 },
            );

            if test_flag(pvhtpriv.stbc_cap, STBC_VHT_ENABLE_TX) {
                set_bits_le_1byte(&mut pcap[0..1], 7, 1, 1);
            }

            if test_flag(pvhtpriv.stbc_cap, STBC_VHT_ENABLE_RX) {
                rtw_hal_get_def_var(
                    padapter,
                    HalDefVariable::RxStbc,
                    &mut rx_stbc_nss as *mut u8 as *mut core::ffi::c_void,
                );
                set_bits_le_1byte(&mut pcap[1..2], 0, 3, rx_stbc_nss);
            }

            set_bits_le_1byte(&mut pcap[2..3], 5, 1, 0);
            set_bits_le_1byte(&mut pcap[2..3], 6, 1, 1);

            let ampdu = if pregistrypriv.ampdu_factor != 0xFE {
                pregistrypriv.ampdu_factor
            } else {
                7
            };
            set_bits_le_2byte(&mut pcap[2..4], 7, 3, ampdu as u16);
            set_bits_le_1byte(&mut pcap[3..4], 2, 2, 0);

            pcap[4..6].copy_from_slice(&pvhtpriv.vht_mcs_map);
            pcap[8..10].copy_from_slice(&pvhtpriv.vht_mcs_map);

            let bw = hal_largest_bw(padapter, regsty_bw_5g(pregistrypriv));
            let sgi = if pvhtpriv.sgi_80m != 0 { 1u8 } else { 0u8 };
            let mut highest = vht_mcs_to_data_rate(bw, sgi, pvhtpriv.vht_highest_rate);
            highest = (highest + 1) >> 1;

            set_bits_le_2byte(&mut pcap[6..8], 0, 13, highest);
            set_bits_le_2byte(&mut pcap[10..12], 0, 13, highest);

            let mut len: u32 = 0;
            rtw_set_ie(pbuf, EID_VHTCapability as i32, 12, pcap.as_ptr(), &mut len);
            len
        }
    }
}

#[no_mangle]
pub extern "C" fn rtw_build_vht_cap_ie(padapter: *mut c_void, pbuf: *mut u8) -> u32 {
    #[cfg(host_vht_build_test)]
    {
        host::build_vht_cap_ie(padapter as *mut Adapter, pbuf)
    }
    #[cfg(not(host_vht_build_test))]
    {
        let _ = (padapter, pbuf);
        0
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
