// SPDX-License-Identifier: GPL-2.0
//! W3-85: `rtw_sta_rx_data_validate_hdr` — host L2 + kernel object.

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
type U16 = u16;
type U32 = u32;
type Sint = i32;

const ETH_ALEN: usize = 6;
const _SUCCESS: Sint = 1;
const _FAIL: Sint = 0;
const RTW_RX_HANDLED: Sint = 2;
const WIFI_ASOC_STATE: U32 = 0x0000_0001;
const WIFI_UNDER_LINKING: U32 = 0x0000_0080;

const ETH_RA: i32 = 0;
const ETH_TA: i32 = 1;
const ETH_DST: i32 = 2;
const ETH_SRC: i32 = 3;
const ETH_BSSID: i32 = 4;

fn is_mcast(da: &[U8; ETH_ALEN]) -> bool {
    (da[0] & 0x01) != 0
}

fn whdr_off(whdr: *const U8, off: usize) -> *mut U8 {
    unsafe { whdr.add(off) as *mut U8 }
}

fn qos_data(whdr: *const U8) -> bool {
    !whdr.is_null() && (unsafe { *(whdr as *const u16) }.to_le() & 0x00fc) & (1 << 6) != 0
}

#[cfg(host_recv_sta_test)]
mod host {
    use super::*;

    #[repr(C)]
    pub struct StaInfoHost {
        pub sta_stats: [U8; 256],
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
        pub mlmepriv_fw_state: U32,
        pub mlmepriv_bssid: [U8; ETH_ALEN],
        pub mlmepriv_link_detect: [U8; 8],
        pub recvpriv_rx_bytes: u64,
        pub stapriv: StaPrivHost,
        pub rfctl: RfCtlHost,
    }

    extern "C" {
        fn rtw_get_stainfo(stapriv: *mut StaPrivHost, hwaddr: *mut U8) -> *mut StaInfoHost;
        fn rtw_get_current_time() -> U32;
        fn rtw_get_passing_time_ms(start: U32) -> i32;
        fn issue_deauth(adapter: *mut AdapterHost, mac: *mut U8, reason: U16);
        fn count_rx_stats(
            adapter: *mut AdapterHost,
            rframe: *mut RecvFrameHost,
            sta: *mut StaInfoHost,
        );
    }

    fn mem_eq(a: *const U8, b: *const U8, n: usize) -> bool {
        if a.is_null() || b.is_null() {
            return false;
        }
        unsafe { core::slice::from_raw_parts(a, n) == core::slice::from_raw_parts(b, n) }
    }

    fn ethcpy(dst: &mut [U8; ETH_ALEN], src: *const U8) {
        if !src.is_null() {
            unsafe { core::ptr::copy_nonoverlapping(src, dst.as_mut_ptr(), ETH_ALEN) };
        }
    }

    pub fn validate_hdr_impl(
        adapter: *mut AdapterHost,
        rframe: *mut RecvFrameHost,
        sta: *mut *mut StaInfoHost,
    ) -> Sint {
        if adapter.is_null() || rframe.is_null() || sta.is_null() {
            return _FAIL;
        }
        let adapter = unsafe { &mut *adapter };
        let rframe = unsafe { &mut *rframe };
        let whdr = rframe.hdr.rx_data;
        let att = &mut rframe.hdr.attrib;
        unsafe {
            *sta = core::ptr::null_mut();
        }

        let mut ra = [0u8; ETH_ALEN];
        if !whdr.is_null() {
            ethcpy(&mut ra, whdr_off(whdr, 4));
        }
        let bmc = is_mcast(&ra);

        if att.to_fr_ds == 0 {
            ethcpy(&mut att.ra, whdr_off(whdr, 4));
            ethcpy(&mut att.ta, whdr_off(whdr, 10));
            ethcpy(&mut att.dst, whdr_off(whdr, 4));
            ethcpy(&mut att.src, whdr_off(whdr, 10));
            ethcpy(&mut att.bssid, whdr_off(whdr, 16));
            if !mem_eq(att.bssid.as_ptr(), att.src.as_ptr(), ETH_ALEN) {
                return _FAIL;
            }
            let p = unsafe { rtw_get_stainfo(&mut adapter.stapriv, whdr_off(whdr, 10)) };
            unsafe {
                *sta = p;
            }
            return if p.is_null() { _FAIL } else { _SUCCESS };
        }

        let fw = adapter.mlmepriv_fw_state;
        if (fw & (WIFI_ASOC_STATE | WIFI_UNDER_LINKING)) == 0 {
            if !bmc {
                static mut TS: U32 = 0;
                let now = unsafe { rtw_get_current_time() };
                if unsafe { rtw_get_passing_time_ms(TS) > 10000 || TS == 0 } {
                    unsafe {
                        TS = now;
                        issue_deauth(adapter, whdr_off(whdr, 10), 7);
                    }
                }
            }
            return _FAIL;
        }

        ethcpy(&mut att.ra, whdr_off(whdr, 4));
        ethcpy(&mut att.ta, whdr_off(whdr, 10));
        match att.to_fr_ds {
            2 => {
                ethcpy(&mut att.dst, whdr_off(whdr, 4));
                ethcpy(&mut att.src, whdr_off(whdr, 16));
                ethcpy(&mut att.bssid, whdr_off(whdr, 10));
            }
            3 => {
                ethcpy(&mut att.dst, whdr_off(whdr, 16));
                ethcpy(&mut att.src, whdr_off(whdr, 24));
                ethcpy(&mut att.bssid, whdr_off(whdr, 10));
            }
            _ => return RTW_RX_HANDLED,
        }

        if att.amsdu == 0 && mem_eq(att.src.as_ptr(), adapter.mac_addr.as_ptr(), ETH_ALEN) {
            return _FAIL;
        }

        let p = unsafe { rtw_get_stainfo(&mut adapter.stapriv, att.ta.as_mut_ptr()) };
        unsafe {
            *sta = p;
        }
        if p.is_null() {
            if !bmc && adapter.rfctl.radar_detected == 0 {
                unsafe {
                    issue_deauth(adapter, att.ta.as_mut_ptr(), 7);
                }
            }
            return _FAIL;
        }
        if qos_data(whdr) {
            unsafe {
                count_rx_stats(adapter, rframe, p);
            }
            return RTW_RX_HANDLED;
        }
        _SUCCESS
    }
}

#[cfg(not(host_recv_sta_test))]
mod kernel {
    use super::*;
    use core::ffi::c_void;

    type Adapter = *mut c_void;
    type RecvFrame = *mut c_void;
    type StaInfo = *mut c_void;
    type StaPriv = *mut c_void;

    extern "C" {
        fn rtw_get_stainfo(stapriv: StaPriv, hwaddr: *mut U8) -> StaInfo;
        fn rtw_rust_recv_sta_get_current_time() -> U32;
        fn rtw_rust_recv_sta_get_passing_time_ms(start: U32) -> i32;
        fn issue_deauth(adapter: Adapter, mac: *mut U8, reason: U16);
        fn count_rx_stats(adapter: Adapter, rframe: RecvFrame, sta: StaInfo);
        fn rtw_rust_recv_sta_frame_rx_data(rframe: RecvFrame) -> *mut U8;
        fn rtw_rust_recv_sta_attrib_to_fr_ds(rframe: RecvFrame) -> U8;
        fn rtw_rust_recv_sta_attrib_amsdu(rframe: RecvFrame) -> U8;
        fn rtw_rust_recv_sta_attrib_set_eth(rframe: RecvFrame, field: i32, mac: *const U8);
        fn rtw_rust_recv_sta_attrib_eth_eq(rframe: RecvFrame, field_a: i32, field_b: i32) -> i32;
        fn rtw_rust_recv_sta_attrib_get_eth(rframe: RecvFrame, field: i32) -> *const U8;
        fn rtw_rust_recv_sta_mlme_fw_state(adapter: Adapter) -> U32;
        fn rtw_rust_recv_sta_adapter_mac(adapter: Adapter) -> *const U8;
        fn rtw_rust_recv_sta_stapriv(adapter: Adapter) -> StaPriv;
        fn rtw_rust_recv_sta_radar_detected(adapter: Adapter) -> U8;
        fn rtw_rust_recv_sta_out_sta(out: *mut StaInfo, psta: StaInfo);
    }

    fn mem_eq_mac(a: *const U8, b: *const U8) -> bool {
        if a.is_null() || b.is_null() {
            return false;
        }
        unsafe {
            core::slice::from_raw_parts(a, ETH_ALEN) == core::slice::from_raw_parts(b, ETH_ALEN)
        }
    }

    pub fn validate_hdr_impl(adapter: Adapter, rframe: RecvFrame, sta: *mut *mut c_void) -> Sint {
        if adapter.is_null() || rframe.is_null() || sta.is_null() {
            return _FAIL;
        }
        let whdr = unsafe { rtw_rust_recv_sta_frame_rx_data(rframe) };
        unsafe {
            rtw_rust_recv_sta_out_sta(sta, core::ptr::null_mut());
        }

        let mut ra = [0u8; ETH_ALEN];
        if !whdr.is_null() {
            unsafe {
                core::ptr::copy_nonoverlapping(whdr_off(whdr, 4), ra.as_mut_ptr(), ETH_ALEN);
            }
        }
        let bmc = is_mcast(&ra);

        if unsafe { rtw_rust_recv_sta_attrib_to_fr_ds(rframe) } == 0 {
            unsafe {
                rtw_rust_recv_sta_attrib_set_eth(rframe, ETH_RA, whdr_off(whdr, 4));
                rtw_rust_recv_sta_attrib_set_eth(rframe, ETH_TA, whdr_off(whdr, 10));
                rtw_rust_recv_sta_attrib_set_eth(rframe, ETH_DST, whdr_off(whdr, 4));
                rtw_rust_recv_sta_attrib_set_eth(rframe, ETH_SRC, whdr_off(whdr, 10));
                rtw_rust_recv_sta_attrib_set_eth(rframe, ETH_BSSID, whdr_off(whdr, 16));
            }
            if unsafe { rtw_rust_recv_sta_attrib_eth_eq(rframe, ETH_BSSID, ETH_SRC) } == 0 {
                return _FAIL;
            }
            let p =
                unsafe { rtw_get_stainfo(rtw_rust_recv_sta_stapriv(adapter), whdr_off(whdr, 10)) };
            unsafe {
                rtw_rust_recv_sta_out_sta(sta, p);
            }
            return if p.is_null() { _FAIL } else { _SUCCESS };
        }

        let fw = unsafe { rtw_rust_recv_sta_mlme_fw_state(adapter) };
        if (fw & (WIFI_ASOC_STATE | WIFI_UNDER_LINKING)) == 0 {
            if !bmc {
                static mut TS: U32 = 0;
                let now = unsafe { rtw_rust_recv_sta_get_current_time() };
                if unsafe { rtw_rust_recv_sta_get_passing_time_ms(TS) > 10000 || TS == 0 } {
                    unsafe {
                        TS = now;
                        issue_deauth(adapter, whdr_off(whdr, 10), 7);
                    }
                }
            }
            return _FAIL;
        }

        unsafe {
            rtw_rust_recv_sta_attrib_set_eth(rframe, ETH_RA, whdr_off(whdr, 4));
            rtw_rust_recv_sta_attrib_set_eth(rframe, ETH_TA, whdr_off(whdr, 10));
        }
        match unsafe { rtw_rust_recv_sta_attrib_to_fr_ds(rframe) } {
            2 => unsafe {
                rtw_rust_recv_sta_attrib_set_eth(rframe, ETH_DST, whdr_off(whdr, 4));
                rtw_rust_recv_sta_attrib_set_eth(rframe, ETH_SRC, whdr_off(whdr, 16));
                rtw_rust_recv_sta_attrib_set_eth(rframe, ETH_BSSID, whdr_off(whdr, 10));
            },
            3 => unsafe {
                rtw_rust_recv_sta_attrib_set_eth(rframe, ETH_DST, whdr_off(whdr, 16));
                rtw_rust_recv_sta_attrib_set_eth(rframe, ETH_SRC, whdr_off(whdr, 24));
                rtw_rust_recv_sta_attrib_set_eth(rframe, ETH_BSSID, whdr_off(whdr, 10));
            },
            _ => return RTW_RX_HANDLED,
        }

        if unsafe { rtw_rust_recv_sta_attrib_amsdu(rframe) } == 0 {
            let src = unsafe { rtw_rust_recv_sta_attrib_get_eth(rframe, ETH_SRC) };
            let mac = unsafe { rtw_rust_recv_sta_adapter_mac(adapter) };
            if mem_eq_mac(src, mac) {
                return _FAIL;
            }
        }

        let ta = unsafe { rtw_rust_recv_sta_attrib_get_eth(rframe, ETH_TA) };
        let p = unsafe { rtw_get_stainfo(rtw_rust_recv_sta_stapriv(adapter), ta as *mut U8) };
        unsafe {
            rtw_rust_recv_sta_out_sta(sta, p);
        }
        if p.is_null() {
            if !bmc && unsafe { rtw_rust_recv_sta_radar_detected(adapter) } == 0 {
                unsafe {
                    issue_deauth(adapter, ta as *mut U8, 7);
                }
            }
            return _FAIL;
        }
        if qos_data(whdr) {
            unsafe {
                count_rx_stats(adapter, rframe, p);
            }
            return RTW_RX_HANDLED;
        }
        _SUCCESS
    }
}

#[no_mangle]
pub extern "C" fn rtw_sta_rx_data_validate_hdr(
    adapter: *mut c_void,
    rframe: *mut c_void,
    sta: *mut *mut c_void,
) -> Sint {
    #[cfg(host_recv_sta_test)]
    return host::validate_hdr_impl(
        adapter as *mut host::AdapterHost,
        rframe as *mut host::RecvFrameHost,
        sta as *mut *mut host::StaInfoHost,
    );
    #[cfg(not(host_recv_sta_test))]
    return kernel::validate_hdr_impl(adapter, rframe, sta);
}
