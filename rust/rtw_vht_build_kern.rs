// SPDX-License-Identifier: GPL-2.0
//! W3-84 PR8: kernel `rtw_build_vht_cap_ie` (operation IE in PR8b).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

use core::ffi::c_void;

type Adapter = *mut c_void;
type U8 = u8;
type U32 = u32;

const EID_VHTCapability: i32 = 191;
const CHANNEL_WIDTH_160: U8 = 3;
const CHANNEL_WIDTH_80_80: U8 = 4;
const BW_CAP_160M: U8 = 1 << 5;
const BW_CAP_80_80M: U8 = 1 << 6;
const LDPC_VHT_ENABLE_RX: U8 = 1;
const STBC_VHT_ENABLE_RX: U8 = 1;
const STBC_VHT_ENABLE_TX: U8 = 1 << 1;
const MGN_VHT1SS_MCS0: U8 = 0xA0;

#[repr(i32)]
enum HalDefVariable {
    MaxRecvbufSz = 3,
    RxPacketOffset = 4,
    RxStbc = 15,
    BeamformerCap = 20,
    BeamformeeCap = 21,
}

#[cfg(config_beamforming)]
const BEAMFORMING_VHT_BEAMFORMER_ENABLE: u16 = 1;
#[cfg(config_beamforming)]
const BEAMFORMING_VHT_BEAMFORMEE_ENABLE: u16 = 2;
#[cfg(config_beamforming)]
const BEAMFORMING_VHT_MU_MIMO_AP_ENABLE: u16 = 4;
#[cfg(config_beamforming)]
const BEAMFORMING_VHT_MU_MIMO_STA_ENABLE: u16 = 8;

#[cfg(all(config_beamforming, config_80211ac_vht))]
const HT_IOT_PEER_BROADCOM: U8 = 3;

extern "C" {
    static VHT_MCS_DATA_RATE: [[[u16; 40]; 2]; 3];
    fn hal_chk_bw_cap(adapter: Adapter, cap: U8) -> bool;
    fn hal_largest_bw(adapter: Adapter, in_bw: U8) -> U8;
    fn rtw_hal_get_def_var(adapter: Adapter, variable: HalDefVariable, value: *mut c_void);
    fn rtw_set_ie(
        pbuf: *mut U8,
        index: i32,
        len: U32,
        source: *const U8,
        frlen: *mut U32,
    ) -> *mut U8;
    fn rtw_rust_vht_build_regsty_bw5g(adapter: Adapter) -> U8;
    fn rtw_rust_vht_build_regsty_is_bw5g_support(adapter: Adapter, bw: U8) -> U8;
    fn rtw_rust_vht_build_regsty_ampdu_factor(adapter: Adapter) -> U8;
    fn rtw_rust_vht_build_vht_cap(adapter: Adapter) -> *mut U8;
    fn rtw_rust_vht_build_vht_mcs_map(adapter: Adapter) -> *mut U8;
    fn rtw_rust_vht_build_ldpc_cap(adapter: Adapter) -> U8;
    fn rtw_rust_vht_build_stbc_cap(adapter: Adapter) -> U8;
    fn rtw_rust_vht_build_sgi_80m(adapter: Adapter) -> U8;
    fn rtw_rust_vht_build_vht_highest_rate(adapter: Adapter) -> U8;
    fn rtw_rust_vht_build_beamform_cap(adapter: Adapter) -> u16;
    fn rtw_rust_vht_build_ap_bf_is_mu_bfer(adapter: Adapter) -> U8;
    fn rtw_rust_vht_build_ap_bf_su_sound_dim(adapter: Adapter) -> U8;
    fn rtw_rust_vht_build_assoc_ap_vendor(adapter: Adapter) -> U8;
}

fn test_flag(v: U8, f: U8) -> bool {
    (v & f) != 0
}

#[cfg(config_beamforming)]
fn test_flag_u16(v: u16, f: u16) -> bool {
    (v & f) != 0
}

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

fn vht_mcs_to_data_rate(bw: U8, short_gi: U8, vht_mcs_rate: U8) -> u16 {
    let idx = ((vht_mcs_rate.wrapping_sub(MGN_VHT1SS_MCS0)) & 0x3f) as usize;
    unsafe { VHT_MCS_DATA_RATE[bw as usize][short_gi as usize][idx] }
}

#[no_mangle]
pub extern "C" fn rtw_build_vht_cap_ie(adapter: Adapter, pbuf: *mut U8) -> U32 {
    if adapter.is_null() || pbuf.is_null() {
        return 0;
    }
    unsafe {
        let pcap = core::slice::from_raw_parts_mut(rtw_rust_vht_build_vht_cap(adapter), 32);
        pcap.fill(0);

        let mut rx_packet_offset: U32 = 0;
        let mut max_recvbuf_sz: U32 = 0;
        let mut rx_stbc_nss: U8 = 0;
        rtw_hal_get_def_var(
            adapter,
            HalDefVariable::RxPacketOffset,
            &mut rx_packet_offset as *mut U32 as *mut c_void,
        );
        rtw_hal_get_def_var(
            adapter,
            HalDefVariable::MaxRecvbufSz,
            &mut max_recvbuf_sz as *mut U32 as *mut c_void,
        );

        let avail = max_recvbuf_sz.saturating_sub(rx_packet_offset);
        if avail >= 11454 {
            set_bits_le_1byte(&mut pcap[0..1], 0, 2, 2);
        } else if avail >= 7991 {
            set_bits_le_1byte(&mut pcap[0..1], 0, 2, 1);
        } else if avail >= 3895 {
            set_bits_le_1byte(&mut pcap[0..1], 0, 2, 0);
        }

        if hal_chk_bw_cap(adapter, BW_CAP_160M)
            && rtw_rust_vht_build_regsty_is_bw5g_support(adapter, CHANNEL_WIDTH_160) != 0
        {
            if hal_chk_bw_cap(adapter, BW_CAP_80_80M)
                && rtw_rust_vht_build_regsty_is_bw5g_support(adapter, CHANNEL_WIDTH_80_80) != 0
            {
                set_bits_le_1byte(&mut pcap[0..1], 2, 2, 2);
            } else {
                set_bits_le_1byte(&mut pcap[0..1], 2, 2, 1);
            }
        } else {
            set_bits_le_1byte(&mut pcap[0..1], 2, 2, 0);
        }

        if test_flag(rtw_rust_vht_build_ldpc_cap(adapter), LDPC_VHT_ENABLE_RX) {
            set_bits_le_1byte(&mut pcap[0..1], 4, 1, 1);
        }

        let sgi = rtw_rust_vht_build_sgi_80m(adapter);
        set_bits_le_1byte(&mut pcap[0..1], 5, 1, if sgi != 0 { 1 } else { 0 });

        if test_flag(rtw_rust_vht_build_stbc_cap(adapter), STBC_VHT_ENABLE_TX) {
            set_bits_le_1byte(&mut pcap[0..1], 7, 1, 1);
        }

        if test_flag(rtw_rust_vht_build_stbc_cap(adapter), STBC_VHT_ENABLE_RX) {
            rtw_hal_get_def_var(
                adapter,
                HalDefVariable::RxStbc,
                &mut rx_stbc_nss as *mut U8 as *mut c_void,
            );
            set_bits_le_1byte(&mut pcap[1..2], 0, 3, rx_stbc_nss);
        }

        #[cfg(config_beamforming)]
        {
            let beamform_cap = rtw_rust_vht_build_beamform_cap(adapter);
            let mut rf_num: U8 = 0;
            if test_flag_u16(beamform_cap, BEAMFORMING_VHT_BEAMFORMER_ENABLE) {
                set_bits_le_1byte(&mut pcap[1..2], 3, 1, 1);
                rtw_hal_get_def_var(
                    adapter,
                    HalDefVariable::BeamformerCap,
                    &mut rf_num as *mut U8 as *mut c_void,
                );
                set_bits_le_1byte(&mut pcap[2..3], 0, 3, rf_num);
                if test_flag_u16(beamform_cap, BEAMFORMING_VHT_MU_MIMO_AP_ENABLE) {
                    set_bits_le_1byte(&mut pcap[2..3], 3, 1, 1);
                }
            }
            if test_flag_u16(beamform_cap, BEAMFORMING_VHT_BEAMFORMEE_ENABLE) {
                set_bits_le_1byte(&mut pcap[1..2], 4, 1, 1);
                rtw_hal_get_def_var(
                    adapter,
                    HalDefVariable::BeamformeeCap,
                    &mut rf_num as *mut U8 as *mut c_void,
                );
                #[cfg(config_80211ac_vht)]
                if rtw_rust_vht_build_assoc_ap_vendor(adapter) == HT_IOT_PEER_BROADCOM
                    && rtw_rust_vht_build_ap_bf_is_mu_bfer(adapter) == 0
                    && rtw_rust_vht_build_ap_bf_su_sound_dim(adapter) == 2
                    && rf_num >= 2
                {
                    rf_num = 2;
                }
                set_bits_le_1byte(&mut pcap[1..2], 5, 3, rf_num);
                if test_flag_u16(beamform_cap, BEAMFORMING_VHT_MU_MIMO_STA_ENABLE) {
                    set_bits_le_1byte(&mut pcap[2..3], 4, 1, 1);
                }
            }
        }

        set_bits_le_1byte(&mut pcap[2..3], 5, 1, 0);
        set_bits_le_1byte(&mut pcap[2..3], 6, 1, 1);

        set_bits_le_2byte(&mut pcap[2..4], 7, 3, {
            let f = rtw_rust_vht_build_regsty_ampdu_factor(adapter);
            if f != 0xFE {
                f as u16
            } else {
                7
            }
        });
        set_bits_le_1byte(&mut pcap[3..4], 2, 2, 0);

        let mcs_map = core::slice::from_raw_parts(rtw_rust_vht_build_vht_mcs_map(adapter), 2);
        pcap[4..6].copy_from_slice(mcs_map);
        pcap[8..10].copy_from_slice(mcs_map);

        let bw = hal_largest_bw(adapter, rtw_rust_vht_build_regsty_bw5g(adapter));
        let sgi_idx = if sgi != 0 { 1u8 } else { 0u8 };
        let mut highest =
            vht_mcs_to_data_rate(bw, sgi_idx, rtw_rust_vht_build_vht_highest_rate(adapter));
        highest = (highest + 1) >> 1;

        set_bits_le_2byte(&mut pcap[6..8], 0, 13, highest);
        set_bits_le_2byte(&mut pcap[10..12], 0, 13, highest);

        let mut len: U32 = 0;
        rtw_set_ie(pbuf, EID_VHTCapability, 12, pcap.as_ptr(), &mut len);
        len
    }
}
