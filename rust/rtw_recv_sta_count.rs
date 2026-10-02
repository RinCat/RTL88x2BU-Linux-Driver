// SPDX-License-Identifier: GPL-2.0
//! W3-85: `count_rx_stats` — host L2 oracle + kernel object (`CONFIG_RUST_RECV_STA`).

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(not(host_recv_sta_test))]
use core::ffi::c_void;
#[cfg(host_recv_sta_test)]
use std::ffi::c_void;

type U8 = u8;
type U32 = u32;
type U64 = u64;
type Systime = U32;

const ETH_ALEN: usize = 6;
const TID_NUM: usize = 16;

fn mac_addr_is_bcst(addr: &[U8; ETH_ALEN]) -> bool {
    addr.iter().all(|&b| b == 0xff)
}

fn is_mcast(da: &[U8; ETH_ALEN]) -> bool {
    (da[0] & 0x01) != 0
}

fn is_broadcast_mac_addr(addr: &[U8; ETH_ALEN]) -> bool {
    mac_addr_is_bcst(addr)
}

#[cfg(host_recv_sta_test)]
mod host {
    use super::*;

    #[repr(C)]
    pub struct StainfoStatsHost {
        pub last_rx_time: Systime,
        pub rx_data_pkts: U64,
        pub rx_data_bc_pkts: U64,
        pub rx_data_mc_pkts: U64,
        pub rx_data_qos_pkts: [U64; TID_NUM],
        pub rx_bytes: U64,
        pub rx_bc_bytes: U64,
        pub rx_mc_bytes: U64,
        pub rxratecnt: [U32; 128],
    }

    #[repr(C)]
    pub struct StaInfoHost {
        pub sta_stats: StainfoStatsHost,
    }

    #[repr(C)]
    pub struct LinkDetectHost {
        pub num_rx_ok_in_period: U32,
        pub num_rx_unicast_ok_in_period: U32,
    }

    #[repr(C)]
    pub struct WlanBssidExHost {
        pub mac_address: [U8; ETH_ALEN],
    }

    #[repr(C)]
    pub struct WlanNetworkHost {
        pub network: WlanBssidExHost,
    }

    #[repr(C)]
    pub struct MlmePrivHost {
        pub fw_state: U32,
        pub cur_network: WlanNetworkHost,
        pub link_detect_info: LinkDetectHost,
    }

    #[repr(C)]
    pub struct RecvPrivHost {
        pub rx_bytes: U64,
    }

    #[repr(C)]
    pub struct StaPrivHost {
        pub _pad: U8,
    }

    #[repr(C)]
    pub struct RfCtlHost {
        pub radar_detected: U8,
    }

    #[repr(C)]
    pub struct RxPktAttribHost {
        pub len: u32,
        pub to_fr_ds: U8,
        pub amsdu: U8,
        pub priority: U8,
        pub data_rate: U8,
        pub dst: [U8; ETH_ALEN],
        pub src: [U8; ETH_ALEN],
        pub ta: [U8; ETH_ALEN],
        pub ra: [U8; ETH_ALEN],
        pub bssid: [U8; ETH_ALEN],
    }

    #[repr(C)]
    pub struct RecvFrameHdrHost {
        pub len: u32,
        pub rx_data: *mut U8,
        pub rx_tail: *mut U8,
        pub attrib: RxPktAttribHost,
        pub psta: *mut StaInfoHost,
    }

    #[repr(C)]
    pub struct RecvFrameHost {
        pub hdr: RecvFrameHdrHost,
    }

    #[repr(C)]
    pub struct AdapterHost {
        pub mac_addr: [U8; ETH_ALEN],
        pub mlmepriv: MlmePrivHost,
        pub recvpriv: RecvPrivHost,
        pub stapriv: StaPrivHost,
        pub rfctl: RfCtlHost,
    }

    extern "C" {
        fn rtw_get_current_time() -> Systime;
    }

    pub fn count_rx_stats_impl(
        adapter: *mut AdapterHost,
        rframe: *mut RecvFrameHost,
        sta: *mut StaInfoHost,
    ) {
        if adapter.is_null() || rframe.is_null() {
            return;
        }
        let adapter = unsafe { &mut *adapter };
        let rframe = unsafe { &mut *rframe };
        let sz = rframe.hdr.len as i32;
        adapter.recvpriv.rx_bytes += sz as u64;
        adapter.mlmepriv.link_detect_info.num_rx_ok_in_period += 1;

        let dst = rframe.hdr.attrib.dst;
        if !mac_addr_is_bcst(&dst) && !is_mcast(&dst) {
            adapter
                .mlmepriv
                .link_detect_info
                .num_rx_unicast_ok_in_period += 1;
        }

        let psta = if !sta.is_null() { sta } else { rframe.hdr.psta };
        if psta.is_null() {
            return;
        }
        let psta = unsafe { &mut *psta };
        let pstats = &mut psta.sta_stats;
        let ra = rframe.hdr.attrib.ra;
        let is_ra_bmc = is_mcast(&ra);

        pstats.last_rx_time = unsafe { rtw_get_current_time() };
        pstats.rx_data_pkts += 1;
        pstats.rx_bytes += sz as u64;
        if is_broadcast_mac_addr(&ra) {
            pstats.rx_data_bc_pkts += 1;
            pstats.rx_bc_bytes += sz as u64;
        } else if is_ra_bmc {
            pstats.rx_data_mc_pkts += 1;
            pstats.rx_mc_bytes += sz as u64;
        }

        if !is_ra_bmc {
            let rate = rframe.hdr.attrib.data_rate as usize;
            if rate < pstats.rxratecnt.len() {
                pstats.rxratecnt[rate] += 1;
            }
            let pri = rframe.hdr.attrib.priority as usize;
            if pri < TID_NUM {
                pstats.rx_data_qos_pkts[pri] += 1;
            }
        }
    }
}

#[cfg(not(host_recv_sta_test))]
mod kernel {
    use super::*;
    use core::ffi::c_void;

    type Adapter = *mut c_void;
    type RecvFrame = *mut c_void;
    type StaInfo = *mut c_void;

    extern "C" {
        fn rtw_rust_recv_sta_get_current_time() -> Systime;
        fn rtw_rust_recv_sta_frame_len(rframe: RecvFrame) -> i32;
        fn rtw_rust_recv_sta_recvpriv_rx_bytes(adapter: Adapter) -> *mut U64;
        fn rtw_rust_recv_sta_link_rx_ok(adapter: Adapter) -> *mut U32;
        fn rtw_rust_recv_sta_link_rx_unicast_ok(adapter: Adapter) -> *mut U32;
        fn rtw_rust_recv_sta_attrib_dst(rframe: RecvFrame) -> *const U8;
        fn rtw_rust_recv_sta_attrib_ra(rframe: RecvFrame) -> *const U8;
        fn rtw_rust_recv_sta_attrib_priority(rframe: RecvFrame) -> U8;
        fn rtw_rust_recv_sta_attrib_data_rate(rframe: RecvFrame) -> U8;
        fn rtw_rust_recv_sta_frame_psta(rframe: RecvFrame) -> StaInfo;
        fn rtw_rust_recv_sta_stat_last_rx_time(psta: StaInfo) -> *mut Systime;
        fn rtw_rust_recv_sta_stat_rx_data_pkts(psta: StaInfo) -> *mut U64;
        fn rtw_rust_recv_sta_stat_rx_bytes(psta: StaInfo) -> *mut U64;
        fn rtw_rust_recv_sta_stat_rx_data_bc_pkts(psta: StaInfo) -> *mut U64;
        fn rtw_rust_recv_sta_stat_rx_bc_bytes(psta: StaInfo) -> *mut U64;
        fn rtw_rust_recv_sta_stat_rx_data_mc_pkts(psta: StaInfo) -> *mut U64;
        fn rtw_rust_recv_sta_stat_rx_mc_bytes(psta: StaInfo) -> *mut U64;
        fn rtw_rust_recv_sta_stat_rxratecnt(psta: StaInfo, rate: U8) -> *mut U32;
        fn rtw_rust_recv_sta_stat_rx_data_qos_pkts(psta: StaInfo, tid: U8) -> *mut U64;
    }

    pub fn count_rx_stats_impl(adapter: Adapter, rframe: RecvFrame, sta: StaInfo) {
        if adapter.is_null() || rframe.is_null() {
            return;
        }
        let sz = unsafe { rtw_rust_recv_sta_frame_len(rframe) };
        unsafe {
            *rtw_rust_recv_sta_recvpriv_rx_bytes(adapter) += sz as U64;
            *rtw_rust_recv_sta_link_rx_ok(adapter) += 1;
        }

        let dst_ptr = unsafe { rtw_rust_recv_sta_attrib_dst(rframe) };
        if !dst_ptr.is_null() {
            let dst = unsafe { core::slice::from_raw_parts(dst_ptr, ETH_ALEN) };
            let dst_arr: [U8; ETH_ALEN] = dst.try_into().unwrap_or([0; ETH_ALEN]);
            if !mac_addr_is_bcst(&dst_arr) && !is_mcast(&dst_arr) {
                unsafe {
                    *rtw_rust_recv_sta_link_rx_unicast_ok(adapter) += 1;
                }
            }
        }

        let psta = if !sta.is_null() {
            sta
        } else {
            unsafe { rtw_rust_recv_sta_frame_psta(rframe) }
        };
        if psta.is_null() {
            return;
        }

        let ra_ptr = unsafe { rtw_rust_recv_sta_attrib_ra(rframe) };
        if ra_ptr.is_null() {
            return;
        }
        let ra = unsafe { core::slice::from_raw_parts(ra_ptr, ETH_ALEN) };
        let ra_arr: [U8; ETH_ALEN] = ra.try_into().unwrap_or([0; ETH_ALEN]);
        let is_ra_bmc = is_mcast(&ra_arr);

        unsafe {
            *rtw_rust_recv_sta_stat_last_rx_time(psta) = rtw_rust_recv_sta_get_current_time();
            *rtw_rust_recv_sta_stat_rx_data_pkts(psta) += 1;
            *rtw_rust_recv_sta_stat_rx_bytes(psta) += sz as U64;
        }
        if is_broadcast_mac_addr(&ra_arr) {
            unsafe {
                *rtw_rust_recv_sta_stat_rx_data_bc_pkts(psta) += 1;
                *rtw_rust_recv_sta_stat_rx_bc_bytes(psta) += sz as U64;
            }
        } else if is_ra_bmc {
            unsafe {
                *rtw_rust_recv_sta_stat_rx_data_mc_pkts(psta) += 1;
                *rtw_rust_recv_sta_stat_rx_mc_bytes(psta) += sz as U64;
            }
        }

        if !is_ra_bmc {
            let rate = unsafe { rtw_rust_recv_sta_attrib_data_rate(rframe) };
            let cnt = unsafe { rtw_rust_recv_sta_stat_rxratecnt(psta, rate) };
            if !cnt.is_null() {
                unsafe {
                    *cnt += 1;
                }
            }
            let pri = unsafe { rtw_rust_recv_sta_attrib_priority(rframe) };
            let qos = unsafe { rtw_rust_recv_sta_stat_rx_data_qos_pkts(psta, pri) };
            if !qos.is_null() {
                unsafe {
                    *qos += 1;
                }
            }
        }
    }
}

#[no_mangle]
pub extern "C" fn count_rx_stats(adapter: *mut c_void, rframe: *mut c_void, sta: *mut c_void) {
    #[cfg(host_recv_sta_test)]
    host::count_rx_stats_impl(
        adapter as *mut host::AdapterHost,
        rframe as *mut host::RecvFrameHost,
        sta as *mut host::StaInfoHost,
    );
    #[cfg(not(host_recv_sta_test))]
    kernel::count_rx_stats_impl(adapter, rframe, sta);
}
