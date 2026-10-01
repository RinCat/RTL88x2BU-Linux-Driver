// SPDX-License-Identifier: GPL-2.0
//! W3-94 pwrctrl — Rust port of `core/rtw_pwrctrl.c` ps deny + unassociated idle helpers.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub
)]

#[cfg(host_pwrctrl_test)]
use std::os::raw::c_uint;

#[cfg(not(host_pwrctrl_test))]
use core::ffi::c_uint;

#[cfg(host_pwrctrl_test)]
#[repr(C)]
pub struct PwrLock {
    pub _lock: i32,
}

#[cfg(host_pwrctrl_test)]
type Systime = u32;

#[cfg(host_pwrctrl_test)]
const _TRUE: u8 = 1;
#[cfg(host_pwrctrl_test)]
const _FALSE: u8 = 0;
#[cfg(host_pwrctrl_test)]
const NR_XMITBUFF: u16 = 4;
#[cfg(host_pwrctrl_test)]
const NR_XMIT_EXTBUFF: u16 = 32;
#[cfg(host_pwrctrl_test)]
const WIFI_ASOC_STATE: u32 = 0x0000_0001;
#[cfg(host_pwrctrl_test)]
const WIFI_UNDER_SURVEY: u32 = 0x0000_0800;
#[cfg(host_pwrctrl_test)]
const WIFI_UNDER_LINKING: u32 = 0x0000_0080;
#[cfg(host_pwrctrl_test)]
const WIFI_UNDER_WPS: u32 = 0x0000_0100;
#[cfg(host_pwrctrl_test)]
const WIFI_AP_STATE: u32 = 0x0000_0010;
#[cfg(host_pwrctrl_test)]
const WIFI_ADHOC_MASTER_STATE: u32 = 0x0000_0020;
#[cfg(host_pwrctrl_test)]
const WIFI_ADHOC_STATE: u32 = 0x0000_0040;

#[cfg(host_pwrctrl_test)]
#[repr(C)]
pub struct MlmePriv {
    pub fw_state: u32,
}

#[cfg(host_pwrctrl_test)]
#[repr(C)]
pub struct XmitPriv {
    pub free_xmitbuf_cnt: u16,
    pub free_xmit_extbuf_cnt: u16,
}

#[cfg(host_pwrctrl_test)]
#[repr(C)]
pub struct PwrctrlPriv {
    pub lock: PwrLock,
    pub ps_deny: u32,
    pub bpower_saving: u8,
    pub ips_deny_time: Systime,
    pub pwr_mode: u8,
    pub rf_pwrstate: u8,
}

#[cfg(host_pwrctrl_test)]
#[repr(C)]
pub struct DvobjPriv {
    pub iface_nums: u8,
    pub padapters: [*mut Adapter; 4],
}

#[cfg(host_pwrctrl_test)]
#[repr(C)]
pub struct Adapter {
    pub dvobj: *mut DvobjPriv,
    pub mlmepriv: MlmePriv,
    pub xmitpriv: XmitPriv,
    pub pwrctrlpriv: PwrctrlPriv,
    pub bup: u8,
    pub bSurpriseRemoved: u8,
}

#[cfg(host_pwrctrl_test)]
static mut G_CURRENT_TIME: Systime = 0;

#[cfg(host_pwrctrl_test)]
type Padapter = *mut Adapter;

#[cfg(not(host_pwrctrl_test))]
type Padapter = *mut core::ffi::c_void;

#[cfg(host_pwrctrl_test)]
type PsDenyReason = u32;

#[cfg(not(host_pwrctrl_test))]
type PsDenyReason = u32;

#[cfg(host_pwrctrl_test)]
fn adapter_to_pwrctl(adapter: Padapter) -> *mut PwrctrlPriv {
    unsafe { &mut (*adapter).pwrctrlpriv }
}

#[cfg(host_pwrctrl_test)]
#[no_mangle]
pub extern "C" fn host_pwrctrl_lps_set_time(t: Systime) {
    unsafe {
        G_CURRENT_TIME = t;
    }
}

#[cfg(host_pwrctrl_test)]
fn rtw_get_current_time() -> Systime {
    unsafe { G_CURRENT_TIME }
}

#[cfg(host_pwrctrl_test)]
fn rtw_time_after(a: Systime, b: Systime) -> bool {
    (a as i32).wrapping_sub(b as i32) > 0
}

#[cfg(host_pwrctrl_test)]
fn check_fwstate(m: &MlmePriv, mask: u32) -> bool {
    if mask == 0 && m.fw_state == 0 {
        return true;
    }
    (m.fw_state & mask) != 0
}

#[cfg(host_pwrctrl_test)]
fn rtw_is_adapter_up(_iface: Padapter) -> bool {
    true
}

#[cfg(host_pwrctrl_test)]
fn mlme_is_ap(iface: &Adapter) -> bool {
    (iface.mlmepriv.fw_state & WIFI_AP_STATE) != 0
}

#[cfg(host_pwrctrl_test)]
fn mlme_is_mesh(_iface: &Adapter) -> bool {
    false
}

#[cfg(host_pwrctrl_test)]
fn rtw_pwr_unassociated_idle_host(adapter: Padapter) -> u8 {
    if adapter.is_null() {
        return _FALSE;
    }
    unsafe {
        let adapter = &mut *adapter;
        let pwr = adapter_to_pwrctl(adapter);
        if (*pwr).bpower_saving == _TRUE {
            return _FALSE;
        }
        if rtw_time_after((*pwr).ips_deny_time, rtw_get_current_time()) {
            return _FALSE;
        }
        let dvobj = match adapter.dvobj.as_mut() {
            Some(d) => d,
            None => return _FALSE,
        };
        for i in 0..dvobj.iface_nums as usize {
            let iface_ptr = dvobj.padapters[i];
            if iface_ptr.is_null() || !rtw_is_adapter_up(iface_ptr) {
                continue;
            }
            let iface = &*iface_ptr;
            let mlme = &iface.mlmepriv;
            if check_fwstate(mlme, WIFI_ASOC_STATE | WIFI_UNDER_SURVEY)
                || check_fwstate(mlme, WIFI_UNDER_LINKING | WIFI_UNDER_WPS)
                || mlme_is_ap(iface)
                || mlme_is_mesh(iface)
                || check_fwstate(mlme, WIFI_ADHOC_MASTER_STATE | WIFI_ADHOC_STATE)
            {
                return _FALSE;
            }
        }
        let px = &adapter.xmitpriv;
        if px.free_xmitbuf_cnt != NR_XMITBUFF || px.free_xmit_extbuf_cnt != NR_XMIT_EXTBUFF {
            return _FALSE;
        }
        _TRUE
    }
}

#[cfg(host_pwrctrl_test)]
mod host {
    use super::*;

    pub fn enter_pwrlock(_lock: *mut PwrLock) {}
    pub fn exit_pwrlock(_lock: *mut PwrLock) {}
}

#[cfg(not(host_pwrctrl_test))]
mod kernel {
    extern "C" {
        fn rtw_rust_pwrctrl_enter_lock(pwr: *mut core::ffi::c_void);
        fn rtw_rust_pwrctrl_exit_lock(pwr: *mut core::ffi::c_void);
        fn rtw_rust_pwrctrl_ps_deny_ptr(pwr: *mut core::ffi::c_void) -> *mut u32;
    }

    pub fn enter_pwrlock(pwr: *mut core::ffi::c_void) {
        unsafe { rtw_rust_pwrctrl_enter_lock(pwr) };
    }

    pub fn exit_pwrlock(pwr: *mut core::ffi::c_void) {
        unsafe { rtw_rust_pwrctrl_exit_lock(pwr) };
    }

    pub fn ps_deny_mut(pwr: *mut core::ffi::c_void) -> *mut u32 {
        unsafe { rtw_rust_pwrctrl_ps_deny_ptr(pwr) }
    }
}

#[no_mangle]
pub extern "C" fn rtw_ps_deny(padapter: Padapter, reason: PsDenyReason) {
    if padapter.is_null() {
        return;
    }
    #[cfg(host_pwrctrl_test)]
    unsafe {
        let pwr = adapter_to_pwrctl(padapter);
        host::enter_pwrlock(&mut (*pwr).lock);
        (*pwr).ps_deny |= 1u32 << reason;
        host::exit_pwrlock(&mut (*pwr).lock);
    }
    #[cfg(not(host_pwrctrl_test))]
    unsafe {
        let pwr = padapter;
        kernel::enter_pwrlock(pwr);
        let deny = kernel::ps_deny_mut(pwr);
        if !deny.is_null() {
            *deny |= 1u32 << reason;
        }
        kernel::exit_pwrlock(pwr);
    }
}

#[no_mangle]
pub extern "C" fn rtw_ps_deny_cancel(padapter: Padapter, reason: PsDenyReason) {
    if padapter.is_null() {
        return;
    }
    #[cfg(host_pwrctrl_test)]
    unsafe {
        let pwr = adapter_to_pwrctl(padapter);
        host::enter_pwrlock(&mut (*pwr).lock);
        (*pwr).ps_deny &= !(1u32 << reason);
        host::exit_pwrlock(&mut (*pwr).lock);
    }
    #[cfg(not(host_pwrctrl_test))]
    unsafe {
        let pwr = padapter;
        kernel::enter_pwrlock(pwr);
        let deny = kernel::ps_deny_mut(pwr);
        if !deny.is_null() {
            *deny &= !(1u32 << reason);
        }
        kernel::exit_pwrlock(pwr);
    }
}

#[no_mangle]
pub extern "C" fn rtw_ps_deny_get(padapter: Padapter) -> c_uint {
    if padapter.is_null() {
        return 0;
    }
    #[cfg(host_pwrctrl_test)]
    unsafe {
        (*adapter_to_pwrctl(padapter)).ps_deny as c_uint
    }
    #[cfg(not(host_pwrctrl_test))]
    unsafe {
        let deny = kernel::ps_deny_mut(padapter);
        if deny.is_null() {
            0
        } else {
            *deny as c_uint
        }
    }
}

#[cfg(any(host_pwrctrl_test, rust_pwrctrl_unassociated_idle))]
#[no_mangle]
pub extern "C" fn rtw_pwr_unassociated_idle(padapter: Padapter) -> u8 {
    #[cfg(host_pwrctrl_test)]
    {
        return rtw_pwr_unassociated_idle_host(padapter);
    }
    #[cfg(all(not(host_pwrctrl_test), rust_pwrctrl_unassociated_idle))]
    {
        let _ = padapter;
        0
    }
}

#[cfg(host_pwrctrl_test)]
mod leave_all_ps {
    use super::*;

    const PS_MODE_ACTIVE: u8 = 0;
    const LPS_CTRL_LEAVE: u8 = 5;
    const RTW_CMDF_DIRECTLY: u8 = 1;
    const RF_OFF: u8 = 0;
    const IFACE_ID0: usize = 0;

    #[repr(C)]
    pub struct HostPwrctrlLeaveAllPsTrace {
        pub lps_ctrl_wk_cmd: i32,
        pub last_lps_ctrl_type: u8,
        pub last_lps_ctrl_flags: u8,
        pub ips_leave: i32,
    }

    static mut G_ASSOC_IF_NUM: i32 = 0;
    static mut G_MI_LINKED: u8 = 0;
    static mut G_TRACE: HostPwrctrlLeaveAllPsTrace = HostPwrctrlLeaveAllPsTrace {
        lps_ctrl_wk_cmd: 0,
        last_lps_ctrl_type: 0,
        last_lps_ctrl_flags: 0,
        ips_leave: 0,
    };

    fn lps_ctrl_wk_cmd(_adapter: Padapter, lps_ctrl_type: u8, flags: u8) {
        unsafe {
            G_TRACE.lps_ctrl_wk_cmd += 1;
            G_TRACE.last_lps_ctrl_type = lps_ctrl_type;
            G_TRACE.last_lps_ctrl_flags = flags;
        }
    }

    fn ips_leave(_a: Padapter) {
        unsafe {
            G_TRACE.ips_leave += 1;
        }
    }

    #[no_mangle]
    pub extern "C" fn host_pwrctrl_leave_all_ps_reset_trace() {
        unsafe {
            G_TRACE = HostPwrctrlLeaveAllPsTrace {
                lps_ctrl_wk_cmd: 0,
                last_lps_ctrl_type: 0,
                last_lps_ctrl_flags: 0,
                ips_leave: 0,
            };
        }
    }

    #[no_mangle]
    pub extern "C" fn host_pwrctrl_leave_all_ps_get_trace() -> *mut HostPwrctrlLeaveAllPsTrace {
        core::ptr::addr_of_mut!(G_TRACE)
    }

    #[no_mangle]
    pub extern "C" fn host_pwrctrl_leave_all_ps_set_assoc_if_num(n: i32) {
        unsafe {
            G_ASSOC_IF_NUM = n;
        }
    }

    #[no_mangle]
    pub extern "C" fn host_pwrctrl_leave_all_ps_set_mi_linked(linked: u8) {
        unsafe {
            G_MI_LINKED = linked;
        }
    }

    #[no_mangle]
    pub extern "C" fn LeaveAllPowerSaveModeDirect(adapter: Padapter) {
        if adapter.is_null() {
            return;
        }
        unsafe {
            let a = &*adapter;
            let dvobj = match (*adapter).dvobj.as_ref() {
                Some(d) => d,
                None => return,
            };
            let pri = dvobj.padapters[IFACE_ID0];
            let pwr = adapter_to_pwrctl(adapter);
            if a.bSurpriseRemoved != 0 {
                return;
            }
            if G_MI_LINKED != 0 {
                if (*pwr).pwr_mode == PS_MODE_ACTIVE {
                    return;
                }
                lps_ctrl_wk_cmd(pri, LPS_CTRL_LEAVE, RTW_CMDF_DIRECTLY);
            } else if (*pwr).rf_pwrstate == RF_OFF {
                // Host L2 shim omits IPS leave unless FWLPS/SWLPS/8188E cfgs are set.
            }
        }
    }

    #[no_mangle]
    pub extern "C" fn LeaveAllPowerSaveMode(adapter: Padapter) {
        if adapter.is_null() {
            return;
        }
        unsafe {
            let a = &*adapter;
            let pwr = adapter_to_pwrctl(adapter);
            if a.bup == 0 || a.bSurpriseRemoved != 0 {
                return;
            }
            if G_ASSOC_IF_NUM != 0 {
                // CONFIG_LPS_LCLK enqueue path (host shim): flags 0, not RTW_CMDF_DIRECTLY.
                lps_ctrl_wk_cmd(adapter, LPS_CTRL_LEAVE, 0);
            } else if (*pwr).rf_pwrstate == RF_OFF {
                // Host L2 shim omits IPS leave unless FWLPS/SWLPS/8188E cfgs are set.
            }
        }
    }
}

#[cfg(host_pwrctrl_test)]
mod lps_enter_host {
    #![allow(non_snake_case)]

    use std::os::raw::c_char;

    const PS_MODE_ACTIVE: u8 = 0;
    const HW_PORT0: u8 = 0;
    const WIFI_UNDER_SURVEY: u32 = 0x0000_0800;
    const WIFI_UNDER_LINKING: u32 = 0x0000_0080;
    const WIFI_UNDER_WPS: u32 = 0x0000_0100;
    const WIFI_AP_STATE: u32 = 0x0000_0010;
    const WIFI_ADHOC_MASTER_STATE: u32 = 0x0000_0020;
    const WIFI_ADHOC_STATE: u32 = 0x0000_0040;

    #[repr(C)]
    pub struct MlmePriv {
        pub fw_state: u32,
    }

    #[repr(C)]
    pub struct RegistryPriv {
        pub smart_ps: u8,
    }

    #[repr(C)]
    pub struct HalData {
        pub bFWReady: u8,
    }

    #[repr(C)]
    pub struct PwrctrlPriv {
        pub pwr_mode: u8,
        pub bpower_saving: u8,
        pub bLeisurePs: u8,
        pub LpsIdleCount: u8,
        pub power_mgnt: u8,
        pub bInSuspend: u8,
        pub lps_deny_time: u32,
    }

    #[repr(C)]
    pub struct DvobjPriv {
        pub iface_nums: u8,
        pub padapters: [*mut LpsEnterAdapter; 4],
    }

    #[repr(C)]
    pub struct LpsEnterAdapter {
        pub dvobj: *mut DvobjPriv,
        pub mlmepriv: MlmePriv,
        pub registrypriv: RegistryPriv,
        pub pwrctrlpriv: PwrctrlPriv,
        pub HalData: HalData,
        pub hw_port: u8,
    }

    #[repr(C)]
    pub struct HostPwrctrlLpsEnterTrace {
        pub set_ps_mode_calls: u32,
        pub last_ps_mode: u8,
    }

    static mut G_TIME: u32 = 0;
    static mut G_ASSOC: i32 = 1;
    static mut G_TRACE: HostPwrctrlLpsEnterTrace = HostPwrctrlLpsEnterTrace {
        set_ps_mode_calls: 0,
        last_ps_mode: 0,
    };

    fn check_fwstate(m: &MlmePriv, s: u32) -> bool {
        if s == 0 && m.fw_state == 0 {
            return true;
        }
        (m.fw_state & s) != 0
    }

    fn ps_rdy_check(a: &LpsEnterAdapter) -> bool {
        let p = &a.pwrctrlpriv;
        let m = &a.mlmepriv;
        if p.bInSuspend == 1 || (p.lps_deny_time as i32).wrapping_sub(unsafe { G_TIME } as i32) > 0
        {
            return false;
        }
        if check_fwstate(m, WIFI_UNDER_SURVEY)
            || check_fwstate(m, WIFI_UNDER_LINKING | WIFI_UNDER_WPS)
            || (m.fw_state & WIFI_AP_STATE) != 0
            || check_fwstate(m, WIFI_ADHOC_MASTER_STATE | WIFI_ADHOC_STATE)
        {
            return false;
        }
        true
    }

    fn record_ps_mode(mode: u8) {
        unsafe {
            G_TRACE.set_ps_mode_calls += 1;
            G_TRACE.last_ps_mode = mode;
        }
    }

    #[no_mangle]
    pub extern "C" fn host_pwrctrl_lps_enter_set_time(t: u32) {
        unsafe {
            G_TIME = t;
        }
    }

    #[no_mangle]
    pub extern "C" fn host_pwrctrl_lps_enter_set_assoc_if_num(n: i32) {
        unsafe {
            G_ASSOC = n;
        }
    }

    #[no_mangle]
    pub extern "C" fn host_pwrctrl_lps_enter_reset_trace() {
        unsafe {
            G_TRACE = HostPwrctrlLpsEnterTrace {
                set_ps_mode_calls: 0,
                last_ps_mode: 0,
            };
        }
    }

    #[no_mangle]
    pub extern "C" fn host_pwrctrl_lps_enter_get_trace() -> *mut HostPwrctrlLpsEnterTrace {
        core::ptr::addr_of_mut!(G_TRACE)
    }

    #[no_mangle]
    pub extern "C" fn LPS_Enter(a: *mut LpsEnterAdapter, _msg: *const c_char) {
        if a.is_null() {
            return;
        }
        unsafe {
            let a = &mut *a;
            let d = match a.dvobj.as_mut() {
                Some(d) => d,
                None => return,
            };
            if a.HalData.bFWReady == 0 || G_ASSOC != 1 || a.hw_port != HW_PORT0 {
                return;
            }
            for i in 0..d.iface_nums as usize {
                let iface = d.padapters[i];
                if iface.is_null() || !ps_rdy_check(&*iface) {
                    return;
                }
            }
            let p = &mut a.pwrctrlpriv;
            if p.bLeisurePs == 0 {
                return;
            }
            if p.LpsIdleCount >= 2 {
                if p.pwr_mode == PS_MODE_ACTIVE {
                    let mgnt = p.power_mgnt;
                    p.bpower_saving = 1;
                    p.pwr_mode = mgnt;
                    record_ps_mode(mgnt);
                }
            } else {
                p.LpsIdleCount += 1;
            }
        }
    }

    #[no_mangle]
    pub extern "C" fn LPS_Leave(a: *mut LpsEnterAdapter, _msg: *const c_char) {
        if a.is_null() {
            return;
        }
        unsafe {
            let a = &mut *a;
            let p = &mut a.pwrctrlpriv;
            if p.bLeisurePs != 0 && p.pwr_mode != PS_MODE_ACTIVE {
                p.pwr_mode = PS_MODE_ACTIVE;
                record_ps_mode(PS_MODE_ACTIVE);
            }
            p.bpower_saving = 0;
        }
    }
}
