// SPDX-License-Identifier: GPL-2.0
//! W3-98 P2P channel/negotiation pure helpers (host L2 Rust oracle, PR4).
#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    unreachable_pub
)]

use std::os::raw::c_int;

const _TRUE: u8 = 1;
const _FALSE: u8 = 0;

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
