// SPDX-License-Identifier: GPL-2.0
//! W3-98 P2P channel/negotiation leaf helpers (host L2 Rust oracle).
#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    unreachable_pub
)]

use std::os::raw::{c_int, c_uint};
use std::ptr;

const _TRUE: u8 = 1;
const _FALSE: u8 = 0;
const _BE: u32 = 12;
const P2P_ATTR_MANAGEABILITY: u8 = 0x0a;
const P2P_ATTR_NOA: u8 = 0x0c;
const P2P_STATE_NONE: u8 = 0;
const P2P_PS_NONE: u32 = 0;
const P2P_PS_CTWINDOW: u32 = 1;
const P2P_PS_NOA: u32 = 2;
const P2P_WK_DISABLE: u8 = 0;
const P2P_WK_ENABLE: u8 = 1;
const P2P_OUI: [u8; 4] = [0x50, 0x6F, 0x9A, 0x09];

#[repr(C)]
pub struct WifidirectInfo {
    pub p2p_state: u8,
    pub noa_index: u8,
    pub opp_ps: u8,
    pub ctwindow: u8,
    pub noa_num: u8,
    pub noa_count: [u8; 2],
    _wdinfo_pad: u8,
    pub p2p_ps_mode: u32,
    pub noa_duration: [u32; 2],
    pub noa_interval: [u32; 2],
    pub noa_start_time: [u32; 2],
}

#[repr(C)]
pub struct HostRfChan {
    pub ChannelNum: u8,
}

#[repr(C)]
pub struct RfCtl {
    pub max_chan_nums: u8,
    pub channel_set: [HostRfChan; 16],
}

#[repr(C)]
pub struct PwrctrlPriv {
    pub bFwCurrentInPSMode: u8,
}

#[repr(C)]
pub struct Adapter {
    pub wdinfo: WifidirectInfo,
    pub rfctl: RfCtl,
    pub pwrctrlpriv: PwrctrlPriv,
}

type Padapter = *mut Adapter;

extern "C" {
    fn p2p_ps_wk_cmd(a: Padapter, cmd: u8, en: u8);
}

fn le16(x: &[u8]) -> u16 {
    (x[1] as u16) << 8 | x[0] as u16
}

fn rd32(s: &[u8], off: usize) -> u32 {
    u32::from_le_bytes([s[off], s[off + 1], s[off + 2], s[off + 3]])
}

unsafe fn p2p_ie(in_ie: *mut u8, in_len: c_int, ielen: *mut c_uint) -> *mut u8 {
    if !ielen.is_null() {
        *ielen = 0;
    }
    if in_ie.is_null() || in_len <= 0 {
        return ptr::null_mut();
    }
    let s = std::slice::from_raw_parts(in_ie, in_len as usize);
    let mut c = 0;
    while c + 5 < s.len() {
        if s[c] == 221 && s[c + 2..c + 6] == P2P_OUI {
            if !ielen.is_null() {
                *ielen = s[c + 1] as c_uint + 2;
            }
            return in_ie.add(c);
        }
        c += s[c + 1] as usize + 2;
    }
    ptr::null_mut()
}

unsafe fn p2p_attr_content(
    ie: *mut u8,
    ilen: c_uint,
    id: u8,
    buf: *mut u8,
    len: *mut c_uint,
) -> *mut u8 {
    if ie.is_null() || ilen <= 6 {
        return ptr::null_mut();
    }
    let s = std::slice::from_raw_parts(ie, ilen as usize);
    if s[0] != 221 || s[2..6] != P2P_OUI {
        return ptr::null_mut();
    }
    let mut off = 6usize;
    while off + 3 <= s.len() {
        let alen = le16(&s[off + 1..]) as usize + 3;
        if off + alen > s.len() {
            break;
        }
        if s[off] == id {
            let cl = alen - 3;
            if !len.is_null() {
                let l = &mut *len;
                if buf.is_null() || *l > cl as c_uint {
                    *l = cl as c_uint;
                }
            }
            if !buf.is_null() && !len.is_null() {
                ptr::copy_nonoverlapping(ie.add(off + 3), buf, (*len) as usize);
            }
            return ie.add(off + 3);
        }
        off += alen;
    }
    ptr::null_mut()
}

#[no_mangle]
pub extern "C" fn rtw_p2p_is_channel_list_ok(
    desired_ch: u8,
    ch_list: *mut u8,
    ch_cnt: u8,
) -> c_int {
    if ch_list.is_null() {
        return 0;
    }
    let list = unsafe { std::slice::from_raw_parts(ch_list, ch_cnt as usize) };
    i32::from(list.iter().any(|&c| c == desired_ch))
}

#[no_mangle]
pub extern "C" fn rtw_p2p_get_peer_ch_list(
    pwdinfo: *mut WifidirectInfo,
    ch_content: *mut u8,
    ch_cnt: u8,
    peer_ch_list: *mut u8,
) -> u8 {
    let _ = pwdinfo;
    if ch_content.is_null() || peer_ch_list.is_null() {
        return 0;
    }
    let mut content = unsafe { std::slice::from_raw_parts_mut(ch_content, ch_cnt as usize) };
    let out = unsafe { std::slice::from_raw_parts_mut(peer_ch_list, 128) };
    if content.len() < 3 {
        return 0;
    }
    content = &mut content[3..];
    let (mut j, mut ch_no) = (0usize, 0u8);
    while !content.is_empty() && content.len() >= 2 {
        content = &mut content[1..];
        let temp = content[0] as usize;
        if content.len() < temp + 1 {
            break;
        }
        for i in 0..temp {
            out[j] = content[i + 1];
            j += 1;
        }
        content = &mut content[temp + 1..];
        ch_no += temp as u8;
    }
    ch_no
}

#[no_mangle]
pub extern "C" fn rtw_p2p_ch_inclusion(
    adapter: Padapter,
    peer_ch_list: *mut u8,
    peer_ch_num: u8,
    ch_list_inclusioned: *mut u8,
) -> u8 {
    if adapter.is_null() || peer_ch_list.is_null() || ch_list_inclusioned.is_null() {
        return 0;
    }
    let rf = unsafe { &(*adapter).rfctl };
    let peer = unsafe { std::slice::from_raw_parts(peer_ch_list, peer_ch_num as usize) };
    let out = unsafe { std::slice::from_raw_parts_mut(ch_list_inclusioned, peer_ch_num as usize) };
    let (mut ch_no, mut temp) = (0u8, 0usize);
    for &p in peer {
        for j in temp..rf.max_chan_nums as usize {
            if p == rf.channel_set[j].ChannelNum {
                out[ch_no as usize] = p;
                ch_no += 1;
                temp = j;
                break;
            }
        }
    }
    ch_no
}

#[no_mangle]
pub extern "C" fn rtw_p2p_nego_intent_compare(req: u8, resp: u8) -> u8 {
    if req >> 1 == resp >> 1 {
        return if req & 1 != 0 { _TRUE } else { _FALSE };
    }
    if req >> 1 > resp >> 1 {
        _TRUE
    } else {
        _FALSE
    }
}

#[no_mangle]
pub extern "C" fn process_p2p_cross_connect_ie(a: Padapter, ies: *mut u8, len: c_uint) -> c_int {
    let _ = a;
    if len <= _BE {
        return _TRUE as c_int;
    }
    let base = unsafe { ies.add(_BE as usize) };
    let mut rem = len - _BE;
    let mut pl = 0u32;
    let mut pie = unsafe { p2p_ie(base, rem as c_int, &mut pl) };
    let mut ret = _TRUE as c_int;
    while !pie.is_null() {
        let mut attr = [0u8; 32];
        let mut al = attr.len() as c_uint;
        if !unsafe { p2p_attr_content(pie, pl, P2P_ATTR_MANAGEABILITY, attr.as_mut_ptr(), &mut al) }
            .is_null()
        {
            if (attr[0] & 0x03) == 0x01 {
                ret = _FALSE as c_int;
            }
            break;
        }
        let used = unsafe { pie.offset_from(base) as c_uint } + pl;
        rem = len - _BE - used;
        pie = unsafe { p2p_ie(pie.add(pl as usize), rem as c_int, &mut pl) };
    }
    ret
}

#[no_mangle]
pub extern "C" fn process_p2p_ps_ie(a: Padapter, ies: *mut u8, len: c_uint) {
    if a.is_null() || len <= _BE {
        return;
    }
    let w = unsafe { &mut (*a).wdinfo };
    if w.p2p_state == P2P_STATE_NONE {
        return;
    }
    let base = unsafe { ies.add(_BE as usize) };
    let mut rem = len - _BE;
    let mut pl = 0u32;
    let mut pie = unsafe { p2p_ie(base, rem as c_int, &mut pl) };
    let (mut fp, mut fps) = (false, false);
    while !pie.is_null() {
        fp = true;
        let mut al = 0u32;
        let noa = unsafe { p2p_attr_content(pie, pl, P2P_ATTR_NOA, ptr::null_mut(), &mut al) };
        if !noa.is_null() {
            fps = true;
            let ns = unsafe { std::slice::from_raw_parts(noa, al as usize) };
            let idx = ns[0];
            if w.p2p_ps_mode == P2P_PS_NONE || idx != w.noa_index {
                w.noa_index = idx;
                w.opp_ps = ns[1] >> 7;
                w.ctwindow = if w.opp_ps != 0 { ns[1] & 0x7f } else { 0 };
                let (mut off, mut num) = (2usize, 0u8);
                if al > 2 && (al - 2) % 13 == 0 {
                    while (off as u32) < al && (num as usize) < 2 {
                        w.noa_count[num as usize] = ns[off];
                        off += 1;
                        w.noa_duration[num as usize] = rd32(ns, off);
                        off += 4;
                        w.noa_interval[num as usize] = rd32(ns, off);
                        off += 4;
                        w.noa_start_time[num as usize] = rd32(ns, off);
                        off += 4;
                        num += 1;
                    }
                }
                w.noa_num = num;
                if w.opp_ps == 1 {
                    w.p2p_ps_mode = P2P_PS_CTWINDOW;
                    if unsafe { (*a).pwrctrlpriv.bFwCurrentInPSMode } == _TRUE {
                        unsafe { p2p_ps_wk_cmd(a, P2P_WK_ENABLE, 1) };
                    }
                } else if w.noa_num > 0 {
                    w.p2p_ps_mode = P2P_PS_NOA;
                    unsafe { p2p_ps_wk_cmd(a, P2P_WK_ENABLE, 1) };
                } else if w.p2p_ps_mode > P2P_PS_NONE {
                    unsafe { p2p_ps_wk_cmd(a, P2P_WK_DISABLE, 1) };
                }
            }
            break;
        }
        let used = unsafe { pie.offset_from(base) as c_uint } + pl;
        rem = len - _BE - used;
        pie = unsafe { p2p_ie(pie.add(pl as usize), rem as c_int, &mut pl) };
    }
    if fp && w.p2p_ps_mode > P2P_PS_NONE && !fps {
        unsafe { p2p_ps_wk_cmd(a, P2P_WK_DISABLE, 1) };
    }
}

#[cfg(host_p2p_ie_build)]
mod ie_build {
    use std::os::raw::c_uchar;
    use std::ptr;

    const VS_IE: u8 = 221;
    const P2P_ATTR_STATUS: u8 = 0x00;
    const P2P_ATTR_CAPABILITY: u8 = 0x02;
    const P2P_ATTR_DEVICE_ID: u8 = 0x03;
    const P2P_ATTR_EX_LISTEN_TIMING: u8 = 0x08;
    const P2P_ATTR_DEVICE_INFO: u8 = 0x0d;
    const P2P_ATTR_GROUP_ID: u8 = 0x0f;
    const P2P_OUI_IE: [u8; 4] = [0x50, 0x6F, 0x9A, 0x09];

    #[repr(C)]
    pub struct WifidirectInfoIe {
        pub role: u8,
        pub p2p_state: u8,
        pub device_addr: [u8; 6],
        pub device_name: [u8; 32],
        pub device_name_len: u16,
        pub persistent_supported: u8,
        pub ui_got_wps_info: u8,
        pub supported_wps_cm: u16,
    }

    fn put_le16(b: &mut [u8], v: u16) {
        b[0] = (v & 0xff) as u8;
        b[1] = (v >> 8) as u8;
    }
    fn put_be16(b: &mut [u8], v: u16) {
        b[0] = (v >> 8) as u8;
        b[1] = (v & 0xff) as u8;
    }
    fn put_be32(b: &mut [u8], v: u32) {
        b[0] = (v >> 24) as u8;
        b[1] = (v >> 16) as u8;
        b[2] = (v >> 8) as u8;
        b[3] = v as u8;
    }

    unsafe fn set_ie(pbuf: *mut u8, len: u32, src: &[u8], frlen: *mut u32) -> *mut u8 {
        if pbuf.is_null() {
            return ptr::null_mut();
        }
        let p = std::slice::from_raw_parts_mut(pbuf, (len + 2) as usize);
        p[0] = VS_IE;
        p[1] = len as u8;
        p[2..2 + len as usize].copy_from_slice(src);
        if !frlen.is_null() {
            *frlen += len + 2;
        }
        pbuf.add(len as usize + 2)
    }

    fn set_p2p_attr(p: &mut [u8], id: u8, data: &[u8]) -> u32 {
        p[0] = id;
        put_le16(&mut p[1..3], data.len() as u16);
        p[3..3 + data.len()].copy_from_slice(data);
        (data.len() + 3) as u32
    }

    #[no_mangle]
    pub extern "C" fn build_beacon_p2p_ie(
        pwdinfo: *mut WifidirectInfoIe,
        pbuf: *mut c_uchar,
    ) -> u32 {
        if pwdinfo.is_null() || pbuf.is_null() {
            return 0;
        }
        let w = unsafe { &*pwdinfo };
        let mut p2p = [0u8; 256];
        let mut off = 4usize;
        p2p[0..4].copy_from_slice(&P2P_OUI_IE);
        let mut cap: u16 = (1 << 5) | (1 << 1);
        cap |= ((1 | 8) << 8) as u16;
        if w.p2p_state == 13 {
            cap |= (1 << 6) << 8;
        }
        let mut cap_b = [0u8; 2];
        put_le16(&mut cap_b, cap);
        off += set_p2p_attr(&mut p2p[off..], P2P_ATTR_CAPABILITY, &cap_b) as usize;
        off += set_p2p_attr(&mut p2p[off..], P2P_ATTR_DEVICE_ID, &w.device_addr) as usize;
        let mut total = 0u32;
        unsafe {
            set_ie(pbuf, off as u32, &p2p[..off], &mut total);
        }
        total
    }

    #[no_mangle]
    pub extern "C" fn build_assoc_resp_p2p_ie(
        pwdinfo: *mut WifidirectInfoIe,
        pbuf: *mut c_uchar,
        status_code: u8,
    ) -> u32 {
        let _ = pwdinfo;
        if pbuf.is_null() {
            return 0;
        }
        let mut p2p = [0u8; 32];
        p2p[0..4].copy_from_slice(&P2P_OUI_IE);
        let mut off = 4usize;
        off += set_p2p_attr(&mut p2p[off..], P2P_ATTR_STATUS, &[status_code]) as usize;
        let mut total = 0u32;
        unsafe {
            set_ie(pbuf, off as u32, &p2p[..off], &mut total);
        }
        total
    }

    #[no_mangle]
    pub extern "C" fn build_deauth_p2p_ie(
        _pwdinfo: *mut WifidirectInfoIe,
        _pbuf: *mut c_uchar,
    ) -> u32 {
        0
    }
}

#[cfg(all(host_p2p_ie_build, host_p2p_ie_build_probe))]
mod ie_build_probe {
    use std::os::raw::c_uchar;

    const VS_IE: u8 = 221;
    const P2P_ATTR_CAPABILITY: u8 = 0x02;
    const P2P_ATTR_EX_LISTEN_TIMING: u8 = 0x08;
    const P2P_ATTR_DEVICE_INFO: u8 = 0x0d;
    const P2P_ATTR_GROUP_ID: u8 = 0x0f;
    const P2P_OUI_IE: [u8; 4] = [0x50, 0x6F, 0x9A, 0x09];

    #[repr(C)]
    pub struct WifidirectInfoIe {
        pub role: u8,
        pub p2p_state: u8,
        pub device_addr: [u8; 6],
        pub device_name: [u8; 32],
        pub device_name_len: u16,
        pub persistent_supported: u8,
        pub ui_got_wps_info: u8,
        pub supported_wps_cm: u16,
    }

    fn put_le16(b: &mut [u8], v: u16) {
        b[0] = (v & 0xff) as u8;
        b[1] = (v >> 8) as u8;
    }
    fn put_be16(b: &mut [u8], v: u16) {
        b[0] = (v >> 8) as u8;
        b[1] = (v & 0xff) as u8;
    }
    fn put_be32(b: &mut [u8], v: u32) {
        b[0] = (v >> 24) as u8;
        b[1] = (v >> 16) as u8;
        b[2] = (v >> 8) as u8;
        b[3] = v as u8;
    }

    unsafe fn set_ie(pbuf: *mut u8, len: u32, src: &[u8], frlen: *mut u32) {
        if pbuf.is_null() {
            return;
        }
        let p = std::slice::from_raw_parts_mut(pbuf, (len + 2) as usize);
        p[0] = VS_IE;
        p[1] = len as u8;
        p[2..2 + len as usize].copy_from_slice(src);
        if !frlen.is_null() {
            *frlen += len + 2;
        }
    }

    fn append_dev_info(p2p: &mut [u8], mut off: usize, w: &WifidirectInfoIe) -> usize {
        p2p[off] = P2P_ATTR_DEVICE_INFO;
        off += 1;
        put_le16(&mut p2p[off..], 21 + w.device_name_len);
        off += 2;
        p2p[off..off + 6].copy_from_slice(&w.device_addr);
        off += 6;
        put_be16(&mut p2p[off..], w.supported_wps_cm);
        off += 2;
        put_be16(&mut p2p[off..], 0x0008);
        off += 2;
        put_be32(&mut p2p[off..], 0x0050f204);
        off += 4;
        put_be16(&mut p2p[off..], 0x0005);
        off += 2;
        p2p[off] = 0;
        off += 1;
        put_be16(&mut p2p[off..], 0x1011);
        off += 2;
        put_be16(&mut p2p[off..], w.device_name_len);
        off += 2;
        let dn = w.device_name_len as usize;
        p2p[off..off + dn].copy_from_slice(&w.device_name[..dn]);
        off + dn
    }

    #[no_mangle]
    pub extern "C" fn build_probe_resp_p2p_ie(
        pwdinfo: *mut WifidirectInfoIe,
        pbuf: *mut c_uchar,
    ) -> u32 {
        if pwdinfo.is_null() || pbuf.is_null() {
            return 0;
        }
        let w = unsafe { &*pwdinfo };
        let mut p2p = [0u8; 256];
        let mut off = 4usize;
        p2p[0..4].copy_from_slice(&P2P_OUI_IE);
        p2p[off] = P2P_ATTR_CAPABILITY;
        off += 1;
        put_le16(&mut p2p[off..], 2);
        off += 2;
        p2p[off] = 0x27;
        off += 1;
        if w.role == 3 {
            p2p[off] = 1 | 8;
            if w.p2p_state == 13 {
                p2p[off] |= 1 << 6;
            }
            off += 1;
        } else if w.role == 1 {
            p2p[off] = if w.persistent_supported != 0 {
                2 | 8
            } else {
                8
            };
            off += 1;
        }
        p2p[off] = P2P_ATTR_EX_LISTEN_TIMING;
        off += 1;
        put_le16(&mut p2p[off..], 4);
        off += 2;
        put_le16(&mut p2p[off..], 0xffff);
        off += 2;
        put_le16(&mut p2p[off..], 0xffff);
        off += 2;
        off = append_dev_info(&mut p2p, off, w);
        let mut total = 0u32;
        unsafe {
            set_ie(pbuf, off as u32, &p2p[..off], &mut total);
        }
        total
    }

    #[no_mangle]
    pub extern "C" fn build_prov_disc_request_p2p_ie(
        pwdinfo: *mut WifidirectInfoIe,
        pbuf: *mut c_uchar,
        pssid: *mut c_uchar,
        ussidlen: u8,
        pdev_raddr: *mut c_uchar,
    ) -> u32 {
        if pwdinfo.is_null() || pbuf.is_null() {
            return 0;
        }
        let w = unsafe { &*pwdinfo };
        let mut p2p = [0u8; 256];
        let mut off = 4usize;
        p2p[0..4].copy_from_slice(&P2P_OUI_IE);
        p2p[off] = P2P_ATTR_CAPABILITY;
        off += 1;
        put_le16(&mut p2p[off..], 2);
        off += 2;
        p2p[off] = 0x27;
        off += 1;
        p2p[off] = if w.persistent_supported != 0 {
            2 | 8
        } else {
            8
        };
        off += 1;
        p2p[off] = P2P_ATTR_DEVICE_INFO;
        off += 1;
        put_le16(&mut p2p[off..], 21 + w.device_name_len);
        off += 2;
        p2p[off..off + 6].copy_from_slice(&w.device_addr);
        off += 6;
        let cm = if w.ui_got_wps_info == 3 {
            0x0080u16
        } else {
            0x0008u16
        };
        put_be16(&mut p2p[off..], cm);
        off += 2;
        put_be16(&mut p2p[off..], 0x0008);
        off += 2;
        put_be32(&mut p2p[off..], 0x0050f204);
        off += 4;
        put_be16(&mut p2p[off..], 0x0005);
        off += 2;
        p2p[off] = 0;
        off += 1;
        put_be16(&mut p2p[off..], 0x1011);
        off += 2;
        put_be16(&mut p2p[off..], w.device_name_len);
        off += 2;
        let dn = w.device_name_len as usize;
        p2p[off..off + dn].copy_from_slice(&w.device_name[..dn]);
        off += dn;
        if w.role == 2 && !pssid.is_null() && !pdev_raddr.is_null() {
            p2p[off] = P2P_ATTR_GROUP_ID;
            off += 1;
            put_le16(&mut p2p[off..], 6 + ussidlen as u16);
            off += 2;
            p2p[off..off + 6].copy_from_slice(unsafe { std::slice::from_raw_parts(pdev_raddr, 6) });
            off += 6;
            p2p[off..off + ussidlen as usize]
                .copy_from_slice(unsafe { std::slice::from_raw_parts(pssid, ussidlen as usize) });
            off += ussidlen as usize;
        }
        let mut total = 0u32;
        unsafe {
            set_ie(pbuf, off as u32, &p2p[..off], &mut total);
        }
        total
    }
}
