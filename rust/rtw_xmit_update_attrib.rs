// SPDX-License-Identifier: GPL-2.0
//! Host L2 oracle for `update_attrib_vcs_info`, `update_attrib_phy_info`, and
//! `update_attrib_sec_info_l2` (W3-86).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

use std::os::raw::c_void;

type U8 = u8;
type U16 = u16;
type U32 = u32;
type U64 = u64;

const NONE_VCS: U8 = 0;
const RTS_CTS: U8 = 1;
const CTS_TO_SELF: U8 = 2;
const DISABLE_VCS: U8 = 0;
const ENABLE_VCS: U8 = 1;
const AUTO_VCS: U8 = 2;
const WIRELESS_11_24N: U8 = 1 << 3;
const WIRELESS_11_5N: U8 = 1 << 4;
const HT_IOT_PEER_ATHEROS: U8 = 5;
const CHANNEL_WIDTH_20: U8 = 0;
const CHANNEL_WIDTH_40: U8 = 1;
const CHANNEL_WIDTH_80: U8 = 2;
const DRIVER_AMPDU_SPACING_DEFAULT: U8 = 0xFF;
const _AES_: U8 = 0x04;
const _TRUE: U8 = 1;
const _FALSE: U8 = 0;

#[inline]
fn is_supported_ht(net_type: U8) -> bool {
    (net_type & (WIRELESS_11_24N | WIRELESS_11_5N)) != 0
}

#[inline]
fn rtw_min_u8(a: U8, b: U8) -> U8 {
    if a > b {
        b
    } else {
        a
    }
}

fn validate_vcs(vrtl_carrier_sense: U8, vcs_type: U8, mode: U8) -> U8 {
    match vrtl_carrier_sense {
        DISABLE_VCS => NONE_VCS,
        ENABLE_VCS => vcs_type,
        AUTO_VCS => mode,
        _ => NONE_VCS,
    }
}

fn update_attrib_vcs_info_inner(
    cur_wireless_mode: U8,
    cur_bwmode: U8,
    assoc_ap_vendor: U8,
    ht_protection: U8,
    wifi_spec: U8,
    rts_thresh: U16,
    frag_len: U32,
    dot11_privacy: U8,
    vrtl_carrier_sense: U8,
    vcs_type: U8,
    driver_vcs_en: U8,
    driver_vcs_type: U8,
    is_hw_8812: bool,
    nr_frags: U8,
    last_txcmdsz: U32,
    rtsen: U8,
    cts2self: U8,
    ht_en: U8,
    ampdu_en: U8,
    ht_path_psta_rssi: Option<i8>,
) -> U8 {
    let sz = if nr_frags != 1 {
        frag_len
    } else {
        last_txcmdsz
    };

    let mut used_ht_branch = false;
    let mut vcs_mode = if cur_wireless_mode < WIRELESS_11_24N || wifi_spec != 0 {
        if sz > rts_thresh as U32 {
            RTS_CTS
        } else if rtsen != 0 {
            RTS_CTS
        } else if cts2self != 0 {
            CTS_TO_SELF
        } else {
            NONE_VCS
        }
    } else {
        used_ht_branch = true;
        let mode = 'ht: {
            if assoc_ap_vendor == HT_IOT_PEER_ATHEROS && ampdu_en == _TRUE && dot11_privacy == _AES_
            {
                break 'ht CTS_TO_SELF;
            }
            if rtsen != 0 || cts2self != 0 {
                break 'ht if rtsen != 0 { RTS_CTS } else { CTS_TO_SELF };
            }
            if ht_en != 0 {
                let ht_op = ht_protection;
                if (cur_bwmode != 0 && (ht_op == 2 || ht_op == 3))
                    || (cur_bwmode == 0 && ht_op == 3)
                {
                    break 'ht RTS_CTS;
                }
            }
            if sz > rts_thresh as U32 {
                break 'ht RTS_CTS;
            }
            if ampdu_en == _TRUE && !is_hw_8812 {
                break 'ht RTS_CTS;
            }
            NONE_VCS
        };
        mode
    };

    if used_ht_branch {
        if let Some(rssi) = ht_path_psta_rssi {
            if rssi < 18 && vcs_mode == RTS_CTS {
                vcs_mode = CTS_TO_SELF;
            }
        }
    }

    vcs_mode = validate_vcs(vrtl_carrier_sense, vcs_type, vcs_mode);
    if driver_vcs_en == 1 {
        vcs_mode = driver_vcs_type;
    }
    vcs_mode
}

#[repr(C)]
struct RaInfo {
    rate_id: U8,
}

#[repr(C)]
struct StaCmnInfo {
    ra_info: RaInfo,
    ldpc_en: U8,
    stbc_en: U8,
    bw_mode: U8,
}

#[repr(C)]
struct HtPriv {
    ht_option: U8,
    ch_offset: U8,
    ampdu_enable: U8,
    agg_enable_bitmap: U8,
    rx_ampdu_min_spacing: U8,
    tx_amsdu_enable: U8,
    sgi_20m: U8,
    sgi_40m: U8,
}

#[repr(C)]
struct VhtPriv {
    vht_option: U8,
    sgi_80m: U8,
}

#[repr(C)]
struct StaInfo {
    rtsen: U8,
    cts2self: U8,
    cmn: StaCmnInfo,
    htpriv: HtPriv,
    vhtpriv: VhtPriv,
}

#[repr(C)]
struct RegistryPriv {
    wifi_spec: U8,
    rts_thresh: U16,
    vrtl_carrier_sense: U8,
    vcs_type: U8,
    ht_enable: U8,
    wireless_mode: U8,
}

#[repr(C)]
struct SecurityPriv {
    dot11PrivacyAlgrthm: U8,
}

#[repr(C)]
struct MlmeExtInfo {
    assoc_ap_vendor: U8,
    ht_protection: U8,
}

#[repr(C)]
struct MlmeExtPriv {
    cur_wireless_mode: U8,
    cur_bwmode: U8,
    mlmext_info: MlmeExtInfo,
}

#[repr(C)]
struct XmitPriv {
    frag_len: U32,
}

#[repr(C)]
struct PktAttrib {
    nr_frags: U8,
    last_txcmdsz: U32,
    rtsen: U8,
    cts2self: U8,
    ht_en: U8,
    ampdu_en: U8,
    vcs_mode: U8,
    mdata: U8,
    eosp: U8,
    triggered: U8,
    ampdu_spacing: U8,
    raid: U8,
    bwmode: U8,
    sgi: U8,
    ldpc: U8,
    stbc: U8,
    ch_offset: U8,
    amsdu_ampdu_en: U8,
    priority: U8,
    retry_ctrl: U8,
}

#[repr(C)]
struct XmitFrame {
    attrib: PktAttrib,
}

#[repr(C)]
struct Adapter {
    mlmeextpriv: MlmeExtPriv,
    registrypriv: RegistryPriv,
    securitypriv: SecurityPriv,
    xmitpriv: XmitPriv,
    driver_vcs_en: U8,
    driver_vcs_type: U8,
    driver_ampdu_spacing: U8,
}

fn query_ra_short_gi(psta: &StaInfo, bw: U8) -> U8 {
    let sgi_20m = psta.htpriv.sgi_20m;
    let sgi_40m = psta.htpriv.sgi_40m;
    let sgi_80m = if psta.vhtpriv.vht_option != 0 {
        psta.vhtpriv.sgi_80m
    } else {
        _FALSE
    };
    match bw {
        CHANNEL_WIDTH_80 => sgi_80m,
        CHANNEL_WIDTH_40 => sgi_40m,
        _ => sgi_20m,
    }
}

fn rtw_get_tx_bw_mode(sta: &StaInfo) -> U8 {
    sta.cmn.bw_mode
}

fn update_attrib_phy_info_inner(adapter: &Adapter, pattrib: &mut PktAttrib, psta: &StaInfo) {
    let mlmeext = &adapter.mlmeextpriv;

    pattrib.rtsen = psta.rtsen;
    pattrib.cts2self = psta.cts2self;
    pattrib.mdata = 0;
    pattrib.eosp = 0;
    pattrib.triggered = 0;
    pattrib.ampdu_spacing = 0;

    pattrib.raid = psta.cmn.ra_info.rate_id;

    let bw = rtw_get_tx_bw_mode(psta);
    pattrib.bwmode = rtw_min_u8(bw, mlmeext.cur_bwmode);
    pattrib.sgi = query_ra_short_gi(psta, pattrib.bwmode);
    pattrib.ldpc = psta.cmn.ldpc_en;
    pattrib.stbc = psta.cmn.stbc_en;

    if adapter.registrypriv.ht_enable != 0 && is_supported_ht(adapter.registrypriv.wireless_mode) {
        pattrib.ht_en = psta.htpriv.ht_option;
        pattrib.ch_offset = psta.htpriv.ch_offset;
        pattrib.ampdu_en = _FALSE;

        pattrib.ampdu_spacing = if adapter.driver_ampdu_spacing != DRIVER_AMPDU_SPACING_DEFAULT {
            adapter.driver_ampdu_spacing
        } else {
            psta.htpriv.rx_ampdu_min_spacing
        };

        if pattrib.ht_en != 0 && psta.htpriv.ampdu_enable != 0 {
            let bit = 1_u8 << pattrib.priority;
            if (psta.htpriv.agg_enable_bitmap & bit) != 0 {
                pattrib.ampdu_en = _TRUE;
                pattrib.amsdu_ampdu_en = if psta.htpriv.tx_amsdu_enable == _TRUE {
                    _TRUE
                } else {
                    _FALSE
                };
            }
        }
    }

    pattrib.retry_ctrl = _FALSE;
}

#[no_mangle]
pub extern "C" fn update_attrib_vcs_info(padapter: *mut c_void, pxmitframe: *mut c_void) {
    if padapter.is_null() || pxmitframe.is_null() {
        return;
    }
    unsafe {
        let a = padapter as *mut Adapter;
        let f = pxmitframe as *mut XmitFrame;
        let att = &mut (*f).attrib;
        att.vcs_mode = update_attrib_vcs_info_inner(
            (*a).mlmeextpriv.cur_wireless_mode,
            (*a).mlmeextpriv.cur_bwmode,
            (*a).mlmeextpriv.mlmext_info.assoc_ap_vendor,
            (*a).mlmeextpriv.mlmext_info.ht_protection,
            (*a).registrypriv.wifi_spec,
            (*a).registrypriv.rts_thresh,
            (*a).xmitpriv.frag_len,
            (*a).securitypriv.dot11PrivacyAlgrthm,
            (*a).registrypriv.vrtl_carrier_sense,
            (*a).registrypriv.vcs_type,
            (*a).driver_vcs_en,
            (*a).driver_vcs_type,
            false,
            att.nr_frags,
            att.last_txcmdsz,
            att.rtsen,
            att.cts2self,
            att.ht_en,
            att.ampdu_en,
            None,
        );
    }
}

#[no_mangle]
pub extern "C" fn update_attrib_phy_info(
    padapter: *mut c_void,
    pattrib: *mut c_void,
    psta: *mut c_void,
) {
    if padapter.is_null() || pattrib.is_null() || psta.is_null() {
        return;
    }
    unsafe {
        let a = &*(padapter as *const Adapter);
        let att = &mut *(pattrib as *mut PktAttrib);
        let sta = &*(psta as *const StaInfo);
        update_attrib_phy_info_inner(a, att, sta);
    }
}

const DOT11_AUTH_OPEN: U8 = 0;
const DOT11_AUTH_8021X: U8 = 2;
const _NO_PRIVACY: U8 = 0;
const EAPOL_2_4: i32 = 10;
const EAPOL_4_4: i32 = 12;
const EAPOL_ETHERTYPE: U16 = 0x888e;
const SEC_L2_OK: i32 = 0;
const SEC_L2_FAIL: i32 = -1;

#[repr(C)]
struct KeyT {
    skey: [U8; 16],
}

#[repr(C)]
struct Dot11TxPn {
    val: U64,
}

#[repr(C)]
struct SecurityPrivSecTest {
    dot11AuthAlgrthm: U8,
    dot11PrivacyKeyIndex: U8,
    dot118021XGrpKeyid: U8,
    dot118021XGrpPrivacy: U8,
    busetkipkey: U8,
    sw_encrypt: U8,
    hw_decrypted: U8,
    dot118021x_bmc_cam_id: U8,
}

#[repr(C)]
struct StaInfoSecExt {
    base: StaInfo,
    mac_id: U8,
    ieee8021x_blocked: U8,
    dot118021XPrivacy: U8,
    dot11txpn: Dot11TxPn,
    dot118021x_UncstKey: KeyT,
    dot11tkiptxmickey: KeyT,
    resp_nonenc_eapol_key_starttime: U64,
}

#[repr(C)]
struct PktAttribSecExt {
    base: PktAttrib,
    ra: [U8; 6],
    encrypt: U8,
    key_idx: U8,
    iv: [U8; 32],
    iv_len: U8,
    icv_len: U8,
    bswenc: U8,
    bmc_camid: U8,
    ether_type: U16,
    mac_id: U8,
    dot118021x_UncstKey: KeyT,
    dot11tkiptxmickey: KeyT,
}

extern "C" {
    static mut host_xmit_sec_cfg: SecurityPrivSecTest;
    static mut host_xmit_sec_passing_ms: U32;
}

fn is_mcast(da: &[U8; 6]) -> bool {
    (da[0] & 0x01) != 0
}

fn host_get_encry_algo(
    base: &SecurityPriv,
    sec: &SecurityPrivSecTest,
    psta: &StaInfoSecExt,
    bmcast: bool,
) -> U8 {
    match sec.dot11AuthAlgrthm {
        DOT11_AUTH_OPEN => base.dot11PrivacyAlgrthm,
        DOT11_AUTH_8021X => {
            if bmcast {
                sec.dot118021XGrpPrivacy
            } else {
                psta.dot118021XPrivacy
            }
        }
        _ => _NO_PRIVACY,
    }
}

fn aes_iv(pattrib_iv: &mut [U8; 32], dot11txpn: &mut Dot11TxPn, keyidx: U8) {
    if dot11txpn.val == 0xffffffffffff {
        dot11txpn.val = 0;
    } else {
        dot11txpn.val += 1;
    }
    let val = dot11txpn.val;
    pattrib_iv[0] = val as U8;
    pattrib_iv[1] = (val >> 8) as U8;
    pattrib_iv[2] = 0;
    pattrib_iv[3] = (1 << 5) | ((keyidx & 0x3) << 6);
    pattrib_iv[4] = (val >> 16) as U8;
    pattrib_iv[5] = (val >> 24) as U8;
    pattrib_iv[6] = (val >> 32) as U8;
    pattrib_iv[7] = (val >> 40) as U8;
}

fn update_attrib_sec_info_l2_inner(
    padapter: &Adapter,
    pattrib: &mut PktAttribSecExt,
    psta: &mut StaInfoSecExt,
    eapol_type: i32,
    psec: &SecurityPrivSecTest,
    passing_ms: U32,
) -> i32 {
    let mut res = SEC_L2_OK;
    let bmcast = is_mcast(&pattrib.ra);

    pattrib.dot118021x_UncstKey.skey = [0; 16];
    pattrib.dot11tkiptxmickey.skey = [0; 16];
    pattrib.mac_id = psta.mac_id;

    if psta.ieee8021x_blocked == _TRUE
        || ((eapol_type == EAPOL_2_4 || eapol_type == EAPOL_4_4) && passing_ms <= 100)
    {
        pattrib.encrypt = 0;
        if pattrib.ether_type != EAPOL_ETHERTYPE {
            res = SEC_L2_FAIL;
        }
    } else {
        pattrib.encrypt = host_get_encry_algo(&padapter.securitypriv, psec, psta, bmcast);
        pattrib.key_idx = match psec.dot11AuthAlgrthm {
            DOT11_AUTH_OPEN => psec.dot11PrivacyKeyIndex,
            DOT11_AUTH_8021X => {
                if bmcast {
                    psec.dot118021XGrpKeyid
                } else {
                    0
                }
            }
            _ => 0,
        };
    }

    if res == SEC_L2_OK {
        match pattrib.encrypt {
            x if x == _AES_ => {
                pattrib.iv_len = 8;
                pattrib.icv_len = 8;
                if bmcast {
                    aes_iv(&mut pattrib.iv, &mut psta.dot11txpn, pattrib.key_idx);
                } else {
                    aes_iv(&mut pattrib.iv, &mut psta.dot11txpn, 0);
                }
            }
            _ => {
                pattrib.iv_len = 0;
                pattrib.icv_len = 0;
            }
        }
    }

    res
}

#[no_mangle]
pub extern "C" fn update_attrib_sec_info_l2(
    padapter: *mut c_void,
    pattrib: *mut c_void,
    psta: *mut c_void,
    eapol_type: i32,
) -> i32 {
    if padapter.is_null() || pattrib.is_null() || psta.is_null() {
        return SEC_L2_FAIL;
    }
    unsafe {
        let a = &*(padapter as *const Adapter);
        let att = &mut *(pattrib as *mut PktAttribSecExt);
        let sta = &mut *(psta as *mut StaInfoSecExt);
        let psec = &*core::ptr::addr_of!(host_xmit_sec_cfg);
        let passing_ms = core::ptr::read(core::ptr::addr_of!(host_xmit_sec_passing_ms));
        update_attrib_sec_info_l2_inner(a, att, sta, eapol_type, psec, passing_ms)
    }
}
