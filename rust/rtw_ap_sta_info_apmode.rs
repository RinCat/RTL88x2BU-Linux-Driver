// SPDX-License-Identifier: GPL-2.0
//! W3-83 `update_sta_info_apmode` — host L2 oracle (kernel wiring in PR14).

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

type U8 = u8;
type U16 = u16;
type U32 = u32;
type Adapter = c_void;
type StaInfo = c_void;

const _TRUE: U8 = 1;
const _FALSE: U8 = 0;
const dot11AuthAlgrthm_8021X: U32 = 2;
const WIFI_ASOC_STATE: U32 = 0x0000_0001;
const WIFI_UNDER_KEY_HANDSHAKE: U32 = 0x0100_0000;
const CHANNEL_WIDTH_20: U8 = 0;
const CHANNEL_WIDTH_40: U8 = 1;
const HAL_PRIME_CHNL_OFFSET_DONT_CARE: U8 = 0;
const IEEE80211_HT_CAP_SUP_WIDTH: U16 = 0x0002;
const IEEE80211_HT_CAP_SGI_20: U16 = 0x0020;
const IEEE80211_HT_CAP_SGI_40: U16 = 0x0040;
const IEEE80211_HT_CAP_AMPDU_DENSITY: U16 = 0x001c;
const LDPC_HT_ENABLE_TX: U8 = 1 << 1;
const LDPC_HT_CAP_TX: U8 = 1 << 3;
const STBC_HT_ENABLE_TX: U8 = 1 << 1;
const STBC_HT_CAP_TX: U8 = 1 << 3;

#[repr(C)]
struct HostApmodeHtIn {
    ap_ampdu_en: U8,
    ap_cap: U16,
    ap_ldpc: U8,
    ap_stbc: U8,
    sta_ht_cap: [U8; 26],
    sta_cap: U16,
    sta_ampdu_para: U8,
    op_present: U8,
    ht_op_sta_width: U8,
    ht_40_intol: U8,
    cur_bwmode: U8,
    cur_ch_offset: U8,
}

extern "C" {
    fn VCS_update(padapter: *mut Adapter, psta: *mut StaInfo);
    fn send_delba(padapter: *mut Adapter, initiator: i32, addr: *mut U8);
    fn query_ra_short_GI(psta: *mut StaInfo, bw: U8) -> U8;
    fn rtw_get_tx_bw_mode(padapter: *mut Adapter, psta: *mut StaInfo) -> U8;
    fn update_ldpc_stbc_cap(psta: *mut StaInfo);
    fn update_sta_vht_info_apmode(padapter: *mut Adapter, psta: *mut StaInfo);
    fn rtw_hal_set_odm_var(padapter: *mut Adapter, variable: i32, psta: *mut StaInfo, val: U8);
    fn host_rust_apmode_dot11_auth(padapter: *mut Adapter) -> U32;
    fn host_rust_apmode_is_mesh(padapter: *mut Adapter) -> U8;
    fn host_rust_apmode_set_8021x_blocked(psta: *mut StaInfo, blocked: U32);
    fn host_rust_apmode_read_ht_inputs(
        padapter: *mut Adapter,
        psta: *mut StaInfo,
        out: *mut HostApmodeHtIn,
    );
    fn host_rust_apmode_sta_ht_option(psta: *mut StaInfo) -> U8;
    fn host_rust_apmode_apply_ht(
        psta: *mut StaInfo,
        ampdu_en: U8,
        min_sp: U8,
        bw: U8,
        sgi20: U8,
        sgi40: U8,
        qos: U32,
        ch_off: U8,
        ldpc: U8,
        stbc: U8,
    );
    fn host_rust_apmode_clear_ht_no_option(psta: *mut StaInfo);
    fn host_rust_apmode_sta_mac(psta: *mut StaInfo) -> *mut U8;
    fn host_rust_apmode_reset_agg(psta: *mut StaInfo);
    fn host_rust_apmode_set_ra_sgi(psta: *mut StaInfo, sgi: U8);
    fn host_rust_apmode_zero_stats(psta: *mut StaInfo);
    fn host_rust_apmode_or_state(psta: *mut StaInfo, bits: U32);
}

#[inline]
fn test_flag(flag: U8, test: U8) -> bool {
    (flag & test) != 0
}

#[inline]
fn set_flag(flag: &mut U8, set: U8) {
    *flag |= set;
}

#[inline]
fn get_ht_cap_ele_ldpc_cap(ht_cap: &[U8; 26]) -> U8 {
    ht_cap[0] & 1
}

#[inline]
fn get_ht_cap_ele_rx_stbc(ht_cap: &[U8; 26]) -> U8 {
    ht_cap[1] & 3
}

fn ht_assoc_update(in_: &HostApmodeHtIn) -> (U8, U8, U8, U8, U8, U32, U8, U8, U8) {
    let mut bw_mode = CHANNEL_WIDTH_20;
    let cap_and = in_.sta_cap & in_.ap_cap;
    if (cap_and & IEEE80211_HT_CAP_SUP_WIDTH) != 0 {
        bw_mode = CHANNEL_WIDTH_40;
    }
    if in_.op_present != 0 && in_.ht_op_sta_width == 0 {
        bw_mode = CHANNEL_WIDTH_20;
    }
    if in_.ht_40_intol != 0 {
        bw_mode = CHANNEL_WIDTH_20;
    }
    if in_.cur_bwmode < bw_mode {
        bw_mode = in_.cur_bwmode;
    }
    let min_sp = (in_.sta_ampdu_para & IEEE80211_HT_CAP_AMPDU_DENSITY as U8) >> 2;
    let mut sgi_20m = _FALSE;
    if (cap_and & IEEE80211_HT_CAP_SGI_20) != 0 {
        sgi_20m = _TRUE;
    }
    let mut sgi_40m = _FALSE;
    if (cap_and & IEEE80211_HT_CAP_SGI_40) != 0 && bw_mode == CHANNEL_WIDTH_40 {
        sgi_40m = _TRUE;
    }
    let mut ldpc: U8 = 0;
    let mut stbc: U8 = 0;
    if test_flag(in_.ap_ldpc, LDPC_HT_ENABLE_TX) && get_ht_cap_ele_ldpc_cap(&in_.sta_ht_cap) != 0 {
        set_flag(&mut ldpc, LDPC_HT_ENABLE_TX | LDPC_HT_CAP_TX);
    }
    if test_flag(in_.ap_stbc, STBC_HT_ENABLE_TX) && get_ht_cap_ele_rx_stbc(&in_.sta_ht_cap) != 0 {
        set_flag(&mut stbc, STBC_HT_ENABLE_TX | STBC_HT_CAP_TX);
    }
    (
        in_.ap_ampdu_en,
        min_sp,
        bw_mode,
        sgi_20m,
        sgi_40m,
        _TRUE as U32,
        in_.cur_ch_offset,
        ldpc,
        stbc,
    )
}

#[no_mangle]
pub extern "C" fn update_sta_info_apmode(padapter: *mut Adapter, psta: *mut StaInfo) {
    if padapter.is_null() || psta.is_null() {
        return;
    }
    unsafe {
        let dot11_auth = host_rust_apmode_dot11_auth(padapter);
        let is_mesh = host_rust_apmode_is_mesh(padapter) != 0;
        let blocked = if !is_mesh && dot11_auth == dot11AuthAlgrthm_8021X {
            _TRUE as U32
        } else {
            _FALSE as U32
        };
        host_rust_apmode_set_8021x_blocked(psta, blocked);
        VCS_update(padapter, psta);
        if host_rust_apmode_sta_ht_option(psta) != 0 {
            let mut inputs = HostApmodeHtIn {
                ap_ampdu_en: 0,
                ap_cap: 0,
                ap_ldpc: 0,
                ap_stbc: 0,
                sta_ht_cap: [0; 26],
                sta_cap: 0,
                sta_ampdu_para: 0,
                op_present: 0,
                ht_op_sta_width: 0,
                ht_40_intol: 0,
                cur_bwmode: 0,
                cur_ch_offset: 0,
            };
            host_rust_apmode_read_ht_inputs(padapter, psta, &mut inputs);
            let (ampdu_en, min_sp, bw, sgi20, sgi40, qos, ch_off, ldpc, stbc) =
                ht_assoc_update(&inputs);
            host_rust_apmode_apply_ht(
                psta, ampdu_en, min_sp, bw, sgi20, sgi40, qos, ch_off, ldpc, stbc,
            );
        } else {
            host_rust_apmode_clear_ht_no_option(psta);
        }
        let mac = host_rust_apmode_sta_mac(psta);
        send_delba(padapter, 0, mac);
        send_delba(padapter, 1, mac);
        host_rust_apmode_reset_agg(psta);
        update_sta_vht_info_apmode(padapter, psta);
        let tx_bw = rtw_get_tx_bw_mode(padapter, psta);
        host_rust_apmode_set_ra_sgi(psta, query_ra_short_GI(psta, tx_bw));
        update_ldpc_stbc_cap(psta);
        host_rust_apmode_zero_stats(psta);
        rtw_hal_set_odm_var(padapter, 0, psta, _TRUE);
        if !is_mesh && dot11_auth == dot11AuthAlgrthm_8021X {
            host_rust_apmode_or_state(psta, WIFI_UNDER_KEY_HANDSHAKE);
        }
        host_rust_apmode_or_state(psta, WIFI_ASOC_STATE);
    }
}
