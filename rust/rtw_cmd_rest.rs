// SPDX-License-Identifier: GPL-2.0
//! W3-60 cmd/evt priv init and teardown — Rust port of `core/rtw_cmd_priv.c`.

#![allow(
    dead_code,
    improper_ctypes,
    missing_docs,
    non_camel_case_types,
    non_snake_case,
    non_upper_case_globals,
    unreachable_pub,
    unused_unsafe
)]

#[cfg(all(
    not(host_cmd_priv_test),
    not(host_cmd_queue_test),
    not(host_cmd_thread_test),
    not(host_cmd_joinbss_test),
    not(host_cmd_drvextra_test),
    not(host_cmd_traffic_lps_test)
))]
use core::ffi::{c_int, c_void};
#[cfg(any(
    host_cmd_priv_test,
    host_cmd_queue_test,
    host_cmd_thread_test,
    host_cmd_joinbss_test,
    host_cmd_drvextra_test,
    host_cmd_traffic_lps_test
))]
use std::os::raw::{c_int, c_void};

type Sint = c_int;
const _SUCCESS: Sint = 1;
const _FAIL: Sint = 0;

const MAX_CMDSZ: u32 = 1536;
const CMDBUFF_ALIGN_SZ: usize = 512;
const MAX_RSPSZ: u32 = 512;

#[repr(C)]
struct List {
    next: *mut List,
    prev: *mut List,
}
#[repr(C)]
struct Queue {
    queue: List,
    lock: c_int,
}

#[repr(C)]
struct WorkItem {
    s: c_int,
}
#[repr(C)]
struct RtwCbuf {
    size: u32,
    write: u32,
    read: u32,
    bufs: *mut *mut c_void,
}

#[repr(C)]
pub struct EvtPriv {
    #[cfg(any(host_cmd_priv_test, event_thread_mode))]
    evt_notify: c_int,
    #[cfg(any(host_cmd_priv_test, event_thread_mode))]
    evt_queue: Queue,
    #[cfg(any(host_cmd_priv_test, c2h_wk))]
    c2h_wk: WorkItem,
    #[cfg(any(host_cmd_priv_test, c2h_wk))]
    c2h_wk_alive: bool,
    #[cfg(any(host_cmd_priv_test, c2h_wk))]
    c2h_queue: *mut RtwCbuf,
    event_seq: c_int,
    evt_buf: *mut u8,
    evt_allocated_buf: *mut u8,
    evt_done_cnt: u32,
}

#[repr(C)]
pub struct CmdPriv {
    cmd_queue_sema: c_int,
    start_cmdthread_sema: c_int,
    cmd_queue: Queue,
    cmd_seq: u8,
    cmd_buf: *mut u8,
    cmd_allocated_buf: *mut u8,
    rsp_buf: *mut u8,
    rsp_allocated_buf: *mut u8,
    cmd_issued_cnt: u32,
    cmd_done_cnt: u32,
    rsp_cnt: u32,
    cmdthd_running: c_int,
    padapter: *mut c_void,
    sctx_mutex: c_int,
}

#[cfg(not(host_cmd_priv_test))]
mod kernel_layout {
    use super::c_void;

    extern "C" {
        pub fn rtw_rust_cmd_priv_cmd_queue_sema(p: *mut c_void) -> *mut c_void;
        pub fn rtw_rust_cmd_priv_start_cmdthread_sema(p: *mut c_void) -> *mut c_void;
        pub fn rtw_rust_cmd_priv_cmd_seq(p: *mut c_void) -> *mut u8;
        pub fn rtw_rust_cmd_priv_cmd_buf(p: *mut c_void) -> *mut *mut u8;
        pub fn rtw_rust_cmd_priv_cmd_allocated_buf(p: *mut c_void) -> *mut *mut u8;
        pub fn rtw_rust_cmd_priv_rsp_buf(p: *mut c_void) -> *mut *mut u8;
        pub fn rtw_rust_cmd_priv_rsp_allocated_buf(p: *mut c_void) -> *mut *mut u8;
        pub fn rtw_rust_cmd_priv_cmd_issued_cnt(p: *mut c_void) -> *mut u32;
        pub fn rtw_rust_cmd_priv_cmd_done_cnt(p: *mut c_void) -> *mut u32;
        pub fn rtw_rust_cmd_priv_rsp_cnt(p: *mut c_void) -> *mut u32;
        pub fn rtw_rust_cmd_priv_sctx_mutex(p: *mut c_void) -> *mut c_void;
        pub fn rtw_rust_cmd_priv_init_queue(p: *mut c_void);
        pub fn rtw_rust_cmd_priv_spinlock_free(p: *mut c_void);
    }
}

extern "C" {
    #[cfg(not(host_cmd_priv_test))]
    fn _rtw_init_listhead(list: *mut List);
    #[cfg(not(host_cmd_priv_test))]
    fn _rtw_spinlock_init(lock: *mut c_int);
    fn _rtw_init_sema(sema: *mut c_int, init_val: c_int);
    fn _rtw_free_sema(sema: *mut c_int);
    fn _rtw_mutex_init(mutex: *mut c_int);
    fn _rtw_mutex_free(mutex: *mut c_int);
    fn _init_workitem(wk: *mut WorkItem, func: *mut c_void, cntx: *mut c_void);
    fn _cancel_workitem_sync(wk: *mut WorkItem);
    fn rtw_msleep_os(ms: c_int);
    fn c2h_wk_callback(wk: *mut WorkItem);
    fn rtw_cbuf_alloc(size: u32) -> *mut RtwCbuf;
    fn rtw_cbuf_free(cbuf: *mut RtwCbuf);
    fn rtw_cbuf_empty(cbuf: *mut RtwCbuf) -> bool;
    fn rtw_cbuf_pop(cbuf: *mut RtwCbuf) -> *mut c_void;
    #[cfg(any(host_cmd_priv_test, host_cmd_queue_test))]
    fn rtw_zmalloc(sz: u32) -> *mut u8;
    #[cfg(any(host_cmd_priv_test, host_cmd_queue_test))]
    fn rtw_mfree(p: *mut u8, sz: u32);
    #[cfg(not(host_cmd_priv_test))]
    fn _rtw_zmalloc(sz: u32) -> *mut u8;
    #[cfg(not(host_cmd_priv_test))]
    fn _rtw_mfree(p: *mut u8, sz: u32);
}

#[inline]
fn zmalloc(sz: u32) -> *mut u8 {
    unsafe {
        #[cfg(any(host_cmd_priv_test, host_cmd_queue_test))]
        {
            return rtw_zmalloc(sz);
        }
        #[cfg(all(not(host_cmd_priv_test), not(host_cmd_queue_test)))]
        {
            return _rtw_zmalloc(sz);
        }
    }
}

#[inline]
fn mfree(p: *mut u8, sz: u32) {
    if !p.is_null() {
        unsafe {
            #[cfg(any(host_cmd_priv_test, host_cmd_queue_test))]
            {
                rtw_mfree(p, sz);
            }
            #[cfg(all(not(host_cmd_priv_test), not(host_cmd_queue_test)))]
            {
                _rtw_mfree(p, sz);
            }
        }
    }
}

#[inline]
fn init_queue_host(q: *mut Queue) {
    unsafe {
        (*q).queue.next = &mut (*q).queue;
        (*q).queue.prev = &mut (*q).queue;
        (*q).lock = 0;
    }
}

#[inline]
fn init_queue(q: *mut Queue) {
    unsafe {
        #[cfg(host_cmd_priv_test)]
        {
            (*q).queue.next = &mut (*q).queue;
            (*q).queue.prev = &mut (*q).queue;
            (*q).lock = 0;
        }
        #[cfg(not(host_cmd_priv_test))]
        {
            _rtw_init_listhead(&mut (*q).queue);
            _rtw_spinlock_init(&mut (*q).lock);
        }
    }
}

#[inline]
fn aligned_buf(base: *mut u8, align: usize) -> *mut u8 {
    if base.is_null() {
        return core::ptr::null_mut();
    }
    let off = align - ((base as usize) & (align - 1));
    unsafe { base.add(off) }
}

#[cfg(any(host_cmd_priv_test, rust_cmd_priv))]
#[no_mangle]
pub extern "C" fn _rtw_init_cmd_priv(p: *mut c_void) -> Sint {
    if p.is_null() {
        return _FAIL;
    }
    unsafe {
        #[cfg(host_cmd_priv_test)]
        {
            let c = &mut *(p as *mut CmdPriv);
            _rtw_init_sema(&mut c.cmd_queue_sema, 0);
            _rtw_init_sema(&mut c.start_cmdthread_sema, 0);
            init_queue_host(&mut c.cmd_queue);
            c.cmd_seq = 1;
            c.cmd_allocated_buf = zmalloc(MAX_CMDSZ + CMDBUFF_ALIGN_SZ as u32);
            if c.cmd_allocated_buf.is_null() {
                return _FAIL;
            }
            c.cmd_buf = aligned_buf(c.cmd_allocated_buf, CMDBUFF_ALIGN_SZ);
            c.rsp_allocated_buf = zmalloc(MAX_RSPSZ + 4);
            if c.rsp_allocated_buf.is_null() {
                return _FAIL;
            }
            c.rsp_buf = aligned_buf(c.rsp_allocated_buf, 4);
            c.cmd_issued_cnt = 0;
            c.cmd_done_cnt = 0;
            c.rsp_cnt = 0;
            _rtw_mutex_init(&mut c.sctx_mutex);
        }
        #[cfg(not(host_cmd_priv_test))]
        {
            use kernel_layout::*;
            _rtw_init_sema(rtw_rust_cmd_priv_cmd_queue_sema(p).cast(), 0);
            _rtw_init_sema(rtw_rust_cmd_priv_start_cmdthread_sema(p).cast(), 0);
            rtw_rust_cmd_priv_init_queue(p);
            *rtw_rust_cmd_priv_cmd_seq(p) = 1;
            *rtw_rust_cmd_priv_cmd_allocated_buf(p) = zmalloc(MAX_CMDSZ + CMDBUFF_ALIGN_SZ as u32);
            if (*rtw_rust_cmd_priv_cmd_allocated_buf(p)).is_null() {
                return _FAIL;
            }
            *rtw_rust_cmd_priv_cmd_buf(p) =
                aligned_buf(*rtw_rust_cmd_priv_cmd_allocated_buf(p), CMDBUFF_ALIGN_SZ);
            *rtw_rust_cmd_priv_rsp_allocated_buf(p) = zmalloc(MAX_RSPSZ + 4);
            if (*rtw_rust_cmd_priv_rsp_allocated_buf(p)).is_null() {
                return _FAIL;
            }
            *rtw_rust_cmd_priv_rsp_buf(p) = aligned_buf(*rtw_rust_cmd_priv_rsp_allocated_buf(p), 4);
            *rtw_rust_cmd_priv_cmd_issued_cnt(p) = 0;
            *rtw_rust_cmd_priv_cmd_done_cnt(p) = 0;
            *rtw_rust_cmd_priv_rsp_cnt(p) = 0;
            _rtw_mutex_init(rtw_rust_cmd_priv_sctx_mutex(p).cast());
        }
    }
    _SUCCESS
}

#[cfg(any(host_cmd_priv_test, rust_cmd_priv))]
#[no_mangle]
pub extern "C" fn _rtw_free_cmd_priv(p: *mut c_void) {
    if p.is_null() {
        return;
    }
    unsafe {
        #[cfg(host_cmd_priv_test)]
        {
            let c = &mut *(p as *mut CmdPriv);
            _rtw_free_sema(&mut c.cmd_queue_sema);
            _rtw_free_sema(&mut c.start_cmdthread_sema);
            mfree(c.cmd_allocated_buf, MAX_CMDSZ + CMDBUFF_ALIGN_SZ as u32);
            mfree(c.rsp_allocated_buf, MAX_RSPSZ + 4);
            _rtw_mutex_free(&mut c.sctx_mutex);
        }
        #[cfg(not(host_cmd_priv_test))]
        {
            use kernel_layout::*;
            rtw_rust_cmd_priv_spinlock_free(p);
            _rtw_free_sema(rtw_rust_cmd_priv_cmd_queue_sema(p).cast());
            _rtw_free_sema(rtw_rust_cmd_priv_start_cmdthread_sema(p).cast());
            mfree(
                *rtw_rust_cmd_priv_cmd_allocated_buf(p),
                MAX_CMDSZ + CMDBUFF_ALIGN_SZ as u32,
            );
            mfree(*rtw_rust_cmd_priv_rsp_allocated_buf(p), MAX_RSPSZ + 4);
            _rtw_mutex_free(rtw_rust_cmd_priv_sctx_mutex(p).cast());
        }
    }
}

#[cfg(any(host_cmd_priv_test, rust_cmd_priv))]
#[no_mangle]
pub extern "C" fn _rtw_init_evt_priv(p: *mut EvtPriv) -> Sint {
    if p.is_null() {
        return _FAIL;
    }
    #[allow(unused_mut)]
    let mut res = _SUCCESS;
    unsafe {
        let e = &mut *p;
        e.event_seq = 0;
        e.evt_done_cnt = 0;
        #[cfg(any(host_cmd_priv_test, event_thread_mode))]
        {
            _rtw_init_sema(&mut e.evt_notify, 0);
            e.evt_allocated_buf = zmalloc(1024 + 4);
            if e.evt_allocated_buf.is_null() {
                res = _FAIL;
            } else {
                e.evt_buf = aligned_buf(e.evt_allocated_buf, 4);
                init_queue(&mut e.evt_queue);
            }
        }
        #[cfg(any(host_cmd_priv_test, c2h_wk))]
        {
            _init_workitem(
                &mut e.c2h_wk,
                c2h_wk_callback as *mut c_void,
                core::ptr::null_mut(),
            );
            e.c2h_wk_alive = false;
            e.c2h_queue = rtw_cbuf_alloc(11);
        }
    }
    res
}

#[cfg(any(host_cmd_priv_test, rust_cmd_priv))]
#[no_mangle]
pub extern "C" fn _rtw_free_evt_priv(p: *mut EvtPriv) {
    if p.is_null() {
        return;
    }
    unsafe {
        let e = &mut *p;
        #[cfg(any(host_cmd_priv_test, event_thread_mode))]
        {
            _rtw_free_sema(&mut e.evt_notify);
            mfree(e.evt_allocated_buf, 1024 + 4);
        }
        #[cfg(any(host_cmd_priv_test, c2h_wk))]
        {
            _cancel_workitem_sync(&mut e.c2h_wk);
            while e.c2h_wk_alive {
                rtw_msleep_os(10);
            }
            if !e.c2h_queue.is_null() {
                while !rtw_cbuf_empty(e.c2h_queue) {
                    let c2h = rtw_cbuf_pop(e.c2h_queue);
                    if !c2h.is_null() && c2h != p as *mut c_void {
                        mfree(c2h as *mut u8, 16);
                    }
                }
                rtw_cbuf_free(e.c2h_queue);
            }
        }
        let _ = e;
    }
}

// --- W3-61 cmd/evt queue enqueue/filter (core/rtw_cmd_queue.c) ---

#[cfg(any(host_cmd_queue_test, rust_cmd_queue))]
mod cmd_queue {
    use super::{c_int, c_void, List, Queue, Sint, MAX_CMDSZ, _FAIL, _SUCCESS};
    #[cfg(not(host_cmd_queue_test))]
    use super::{CmdPriv, EvtPriv};

    extern "C" {
        #[cfg(host_cmd_queue_test)]
        fn rtw_mfree(p: *mut u8, sz: u32);
        #[cfg(host_cmd_queue_test)]
        fn _rtw_up_sema(s: *mut c_int);
    }

    #[inline]
    fn qfree(p: *mut u8, sz: u32) {
        if !p.is_null() {
            unsafe {
                #[cfg(host_cmd_queue_test)]
                rtw_mfree(p, sz);
                #[cfg(not(host_cmd_queue_test))]
                kernel::_rtw_mfree(p, sz);
            }
        }
    }

    const CMD_SET_DRV_EXTRA: u16 = 12;
    const CMD_SET_CHANPLAN: u16 = 13;

    #[repr(C)]
    pub struct CmdObj {
        pub padapter: *mut c_void,
        pub cmdcode: u16,
        pub res: u8,
        pub parmbuf: *mut u8,
        pub cmdsz: u32,
        pub rsp: *mut u8,
        pub rspsz: u32,
        pub sctx: *mut c_void,
        pub no_io: u8,
        pub list: List,
    }

    #[repr(C)]
    pub struct EvtObj {
        pub evtcode: u16,
        pub res: u8,
        pub parmbuf: *mut u8,
        pub evtsz: u32,
        pub list: List,
    }

    #[cfg(host_cmd_queue_test)]
    #[repr(C)]
    pub struct QueueCmdPriv {
        pub cmd_queue_sema: c_int,
        pub start_cmdthread_sema: c_int,
        pub cmd_queue: Queue,
        pub cmdthd_running: c_int,
        pub padapter: *mut c_void,
    }

    #[cfg(host_cmd_queue_test)]
    #[repr(C)]
    pub struct QueueEvtPriv {
        pub evt_notify: c_int,
        pub evt_queue: Queue,
        pub evt_done_cnt: u32,
    }

    #[cfg(not(host_cmd_queue_test))]
    type QueueCmdPriv = CmdPriv;

    #[cfg(not(host_cmd_queue_test))]
    type QueueEvtPriv = EvtPriv;

    #[repr(C)]
    struct DrvextraCmdParm {
        ec_id: c_int,
        type_: c_int,
        size: c_int,
        pbuf: *mut u8,
    }

    #[cfg(not(host_cmd_queue_test))]
    mod kernel {
        use super::*;

        extern "C" {
            pub fn rtw_rust_hw_init_completed(adapter: *mut c_void) -> u8;
            pub fn rtw_rust_queue_enter_critical(lock: *mut c_int, irql: *mut c_ulong);
            pub fn rtw_rust_queue_exit_critical(lock: *mut c_int, irql: *mut c_ulong);
            pub fn rtw_rust_queue_enter_critical_bh(lock: *mut c_int, irql: *mut c_ulong);
            pub fn rtw_rust_queue_exit_critical_bh(lock: *mut c_int, irql: *mut c_ulong);
            pub fn _rtw_up_sema(sema: *mut c_int);
            pub fn _rtw_mfree(p: *mut u8, sz: u32);
            pub fn rtw_rust_cmd_priv_cmd_queue(p: *mut c_void) -> *mut Queue;
            pub fn rtw_rust_cmd_priv_cmd_queue_sema(p: *mut c_void) -> *mut c_void;
            pub fn rtw_rust_cmd_priv_for_enqueue(p: *mut c_void) -> *mut c_void;
            pub fn rtw_rust_cmd_priv_padapter(p: *mut c_void) -> *mut c_void;
            pub fn rtw_rust_cmd_priv_cmdthd_running(p: *mut c_void) -> c_int;
            pub fn rtw_rust_cmd_obj_no_io(pcmd: *mut c_void) -> u8;
            pub fn rtw_rust_cmd_obj_list(pcmd: *mut c_void) -> *mut List;
            pub fn rtw_rust_cmd_obj_from_list(plist: *mut List) -> *mut c_void;
            pub fn rtw_rust_cmd_obj_size() -> u32;
            #[cfg(event_thread_mode)]
            pub fn rtw_rust_evt_priv_evt_queue(p: *mut c_void) -> *mut Queue;
            #[cfg(event_thread_mode)]
            pub fn rtw_rust_evt_priv_evt_notify(p: *mut c_void) -> *mut c_void;
            #[cfg(event_thread_mode)]
            pub fn rtw_rust_evt_priv_evt_done_cnt(p: *mut c_void) -> *mut u32;
        }
    }

    #[cfg(not(host_cmd_queue_test))]
    use core::ffi::c_ulong;
    #[cfg(host_cmd_queue_test)]
    use std::os::raw::c_ulong;

    #[inline]
    fn list_empty(h: &List) -> bool {
        h.next == h as *const List as *mut List
    }

    #[inline]
    fn list_insert_tail(n: &mut List, head: &mut List) {
        unsafe {
            let prev = head.prev;
            n.next = head as *mut List;
            n.prev = prev;
            (*prev).next = n as *mut List;
            head.prev = n as *mut List;
        }
    }

    #[inline]
    fn list_insert_head(n: &mut List, head: &mut List) {
        unsafe {
            let next = head.next;
            n.next = next;
            n.prev = head as *mut List;
            (*next).prev = n as *mut List;
            head.next = n as *mut List;
        }
    }

    #[inline]
    fn list_delete(e: &mut List) {
        unsafe {
            (*e.prev).next = e.next;
            (*e.next).prev = e.prev;
            e.next = e as *mut List;
            e.prev = e as *mut List;
        }
    }

    #[inline]
    fn enter_critical(lock: &mut c_int, irql: &mut c_ulong) {
        #[cfg(not(host_cmd_queue_test))]
        unsafe {
            kernel::rtw_rust_queue_enter_critical(lock as *mut c_int, irql);
        }
        let _ = (lock, irql);
    }

    #[inline]
    fn exit_critical(lock: &mut c_int, irql: &mut c_ulong) {
        #[cfg(not(host_cmd_queue_test))]
        unsafe {
            kernel::rtw_rust_queue_exit_critical(lock as *mut c_int, irql);
        }
        let _ = (lock, irql);
    }

    #[inline]
    fn enter_critical_bh(lock: &mut c_int, irql: &mut c_ulong) {
        #[cfg(not(host_cmd_queue_test))]
        unsafe {
            kernel::rtw_rust_queue_enter_critical_bh(lock as *mut c_int, irql);
        }
        let _ = (lock, irql);
    }

    #[inline]
    fn exit_critical_bh(lock: &mut c_int, irql: &mut c_ulong) {
        #[cfg(not(host_cmd_queue_test))]
        unsafe {
            kernel::rtw_rust_queue_exit_critical_bh(lock as *mut c_int, irql);
        }
        let _ = (lock, irql);
    }

    /* cmd_obj field access. On the kernel path the offsets come from C
     * (core/rtw_cmd_queue.c) because struct cmd_obj puts no_io after sctx
     * and its size depends on the kernel headers; the host L2 fixture uses
     * the mirror directly. */
    #[inline]
    unsafe fn cmd_no_io(obj: *mut CmdObj) -> u8 {
        #[cfg(host_cmd_queue_test)]
        unsafe {
            (*obj).no_io
        }
        #[cfg(not(host_cmd_queue_test))]
        unsafe {
            kernel::rtw_rust_cmd_obj_no_io(obj as *mut c_void)
        }
    }

    #[inline]
    unsafe fn cmd_list(obj: *mut CmdObj) -> *mut List {
        #[cfg(host_cmd_queue_test)]
        unsafe {
            &mut (*obj).list as *mut List
        }
        #[cfg(not(host_cmd_queue_test))]
        unsafe {
            kernel::rtw_rust_cmd_obj_list(obj as *mut c_void)
        }
    }

    #[inline]
    unsafe fn cmd_from_list(ln: *mut List) -> *mut CmdObj {
        #[cfg(host_cmd_queue_test)]
        unsafe {
            (ln as *mut u8).offset(-(core::mem::offset_of!(CmdObj, list) as isize)) as *mut CmdObj
        }
        #[cfg(not(host_cmd_queue_test))]
        unsafe {
            kernel::rtw_rust_cmd_obj_from_list(ln) as *mut CmdObj
        }
    }

    #[inline]
    fn cmd_obj_size() -> u32 {
        #[cfg(host_cmd_queue_test)]
        {
            core::mem::size_of::<CmdObj>() as u32
        }
        #[cfg(not(host_cmd_queue_test))]
        unsafe {
            kernel::rtw_rust_cmd_obj_size()
        }
    }

    #[inline]
    fn hw_init_completed(adapter: *mut c_void) -> bool {
        if adapter.is_null() {
            return false;
        }
        #[cfg(host_cmd_queue_test)]
        unsafe {
            return *(adapter as *const u8) == 1;
        }
        #[cfg(not(host_cmd_queue_test))]
        unsafe {
            kernel::rtw_rust_hw_init_completed(adapter) == 1
        }
    }

    #[no_mangle]
    pub extern "C" fn _rtw_enqueue_cmd(queue: *mut Queue, obj: *mut CmdObj, to_head: bool) -> Sint {
        if queue.is_null() || obj.is_null() {
            return _SUCCESS;
        }
        unsafe {
            let q = &mut *queue;
            let mut irqL = 0usize as c_ulong;
            enter_critical(&mut q.lock, &mut irqL);
            let ln = cmd_list(obj);
            if to_head {
                list_insert_head(&mut *ln, &mut q.queue);
            } else {
                list_insert_tail(&mut *ln, &mut q.queue);
            }
            exit_critical(&mut q.lock, &mut irqL);
        }
        _SUCCESS
    }

    #[no_mangle]
    pub extern "C" fn _rtw_dequeue_cmd(queue: *mut Queue) -> *mut CmdObj {
        if queue.is_null() {
            return core::ptr::null_mut();
        }
        unsafe {
            let q = &mut *queue;
            let mut irqL = 0usize as c_ulong;
            enter_critical(&mut q.lock, &mut irqL);
            let obj = if list_empty(&q.queue) {
                core::ptr::null_mut()
            } else {
                let ln = q.queue.next;
                let obj = cmd_from_list(ln);
                list_delete(&mut *ln);
                obj
            };
            exit_critical(&mut q.lock, &mut irqL);
            obj
        }
    }

    #[no_mangle]
    pub extern "C" fn rtw_cmd_filter(pcmdpriv: *mut QueueCmdPriv, cmd_obj: *mut CmdObj) -> Sint {
        if pcmdpriv.is_null() || cmd_obj.is_null() {
            return _FAIL;
        }
        unsafe {
            let cmd = &*cmd_obj;
            let mut allow = 0u8;
            if cmd.cmdcode == CMD_SET_CHANPLAN {
                allow = 1;
            }
            if cmd_no_io(cmd_obj) != 0 {
                allow = 1;
            }
            #[cfg(host_cmd_queue_test)]
            let (adapter, thd_ok) = {
                let priv_ = &*pcmdpriv;
                (priv_.padapter, priv_.cmdthd_running != 0)
            };
            #[cfg(not(host_cmd_queue_test))]
            let (adapter, thd_ok) = {
                let p = pcmdpriv as *mut c_void;
                (
                    kernel::rtw_rust_cmd_priv_padapter(p),
                    kernel::rtw_rust_cmd_priv_cmdthd_running(p) != 0,
                )
            };
            let hw_ok = hw_init_completed(adapter);
            if (!hw_ok && allow == 0) || !thd_ok {
                return _FAIL;
            }
        }
        _SUCCESS
    }

    #[no_mangle]
    pub extern "C" fn rtw_enqueue_cmd(pcmdpriv: *mut QueueCmdPriv, cmd_obj: *mut CmdObj) -> u32 {
        if pcmdpriv.is_null() || cmd_obj.is_null() {
            return _FAIL as u32;
        }
        unsafe {
            #[cfg(host_cmd_queue_test)]
            {
                let priv_ = &mut *pcmdpriv;
                (*cmd_obj).padapter = priv_.padapter;
                if rtw_cmd_filter(pcmdpriv, cmd_obj) == _FAIL || (*cmd_obj).cmdsz > MAX_CMDSZ {
                    if (*cmd_obj).cmdcode == CMD_SET_DRV_EXTRA {
                        let extra = (*cmd_obj).parmbuf as *mut DrvextraCmdParm;
                        if !extra.is_null() && (*extra).size > 0 && !(*extra).pbuf.is_null() {
                            qfree((*extra).pbuf, (*extra).size as u32);
                        }
                    }
                    rtw_free_cmd_obj(cmd_obj);
                    return _FAIL as u32;
                }
                let res = _rtw_enqueue_cmd(&mut priv_.cmd_queue, cmd_obj, false);
                if res == _SUCCESS {
                    _rtw_up_sema(&mut priv_.cmd_queue_sema);
                }
                return res as u32;
            }
            #[cfg(not(host_cmd_queue_test))]
            {
                let p = pcmdpriv as *mut c_void;
                let padapter = kernel::rtw_rust_cmd_priv_padapter(p);
                (*cmd_obj).padapter = padapter;
                let priv_for_enqueue =
                    kernel::rtw_rust_cmd_priv_for_enqueue(p) as *mut QueueCmdPriv;
                if rtw_cmd_filter(priv_for_enqueue, cmd_obj) == _FAIL
                    || (*cmd_obj).cmdsz > MAX_CMDSZ
                {
                    if (*cmd_obj).cmdcode == CMD_SET_DRV_EXTRA {
                        let extra = (*cmd_obj).parmbuf as *mut DrvextraCmdParm;
                        if !extra.is_null() && (*extra).size > 0 && !(*extra).pbuf.is_null() {
                            qfree((*extra).pbuf, (*extra).size as u32);
                        }
                    }
                    rtw_free_cmd_obj(cmd_obj);
                    return _FAIL as u32;
                }
                let priv_p = priv_for_enqueue as *mut c_void;
                let q = kernel::rtw_rust_cmd_priv_cmd_queue(priv_p);
                let res = _rtw_enqueue_cmd(q, cmd_obj, false);
                if res == _SUCCESS {
                    kernel::_rtw_up_sema(
                        kernel::rtw_rust_cmd_priv_cmd_queue_sema(priv_p) as *mut c_int
                    );
                }
                return res as u32;
            }
        }
    }

    #[no_mangle]
    pub extern "C" fn rtw_dequeue_cmd(pcmdpriv: *mut QueueCmdPriv) -> *mut CmdObj {
        if pcmdpriv.is_null() {
            return core::ptr::null_mut();
        }
        unsafe {
            #[cfg(host_cmd_queue_test)]
            {
                _rtw_dequeue_cmd(&mut (*pcmdpriv).cmd_queue)
            }
            #[cfg(not(host_cmd_queue_test))]
            {
                _rtw_dequeue_cmd(kernel::rtw_rust_cmd_priv_cmd_queue(pcmdpriv as *mut c_void))
            }
        }
    }

    #[no_mangle]
    pub extern "C" fn rtw_free_cmd_obj(pcmd: *mut CmdObj) {
        if pcmd.is_null() {
            return;
        }
        unsafe {
            let p = &mut *pcmd;
            if !p.parmbuf.is_null() {
                qfree(p.parmbuf, p.cmdsz);
            }
            if !p.rsp.is_null() && p.rspsz != 0 {
                qfree(p.rsp, p.rspsz);
            }
            qfree(pcmd as *mut u8, cmd_obj_size());
        }
    }

    #[cfg(any(host_cmd_queue_test, event_thread_mode))]
    #[no_mangle]
    pub extern "C" fn rtw_enqueue_evt(pevtpriv: *mut QueueEvtPriv, obj: *mut EvtObj) -> u32 {
        if pevtpriv.is_null() || obj.is_null() {
            return _FAIL as u32;
        }
        unsafe {
            #[cfg(host_cmd_queue_test)]
            {
                let q = &mut (*pevtpriv).evt_queue;
                let mut irqL = 0usize as c_ulong;
                enter_critical(&mut q.lock, &mut irqL);
                list_insert_tail(&mut (*obj).list, &mut q.queue);
                exit_critical(&mut q.lock, &mut irqL);
            }
            #[cfg(not(host_cmd_queue_test))]
            {
                let q = kernel::rtw_rust_evt_priv_evt_queue(pevtpriv as *mut c_void);
                let mut irqL = 0usize as c_ulong;
                enter_critical_bh(&mut (*q).lock, &mut irqL);
                list_insert_tail(&mut (*obj).list, &mut (*q).queue);
                exit_critical_bh(&mut (*q).lock, &mut irqL);
            }
        }
        _SUCCESS as u32
    }

    #[cfg(any(host_cmd_queue_test, event_thread_mode))]
    #[no_mangle]
    pub extern "C" fn rtw_free_evt_obj(pevtobj: *mut EvtObj) {
        if pevtobj.is_null() {
            return;
        }
        unsafe {
            let p = &mut *pevtobj;
            if !p.parmbuf.is_null() {
                qfree(p.parmbuf, p.evtsz);
            }
            qfree(pevtobj as *mut u8, core::mem::size_of::<EvtObj>() as u32);
        }
    }

    #[cfg(any(host_cmd_queue_test, event_thread_mode))]
    #[no_mangle]
    pub extern "C" fn rtw_evt_notify_isr(pevtpriv: *mut QueueEvtPriv) {
        if pevtpriv.is_null() {
            return;
        }
        unsafe {
            #[cfg(host_cmd_queue_test)]
            {
                (*pevtpriv).evt_done_cnt += 1;
                _rtw_up_sema(&mut (*pevtpriv).evt_notify);
            }
            #[cfg(not(host_cmd_queue_test))]
            {
                let p = pevtpriv as *mut c_void;
                *kernel::rtw_rust_evt_priv_evt_done_cnt(p) += 1;
                kernel::_rtw_up_sema(kernel::rtw_rust_evt_priv_evt_notify(p) as *mut c_int);
            }
        }
    }
}

#[cfg(any(host_cmd_thread_test, rust_cmd_thread))]
mod cmd_thread {
    use super::{c_int, c_void, List, Queue, Sint, MAX_CMDSZ, _FAIL};
    #[cfg(all(rust_cmd_thread, not(host_cmd_thread_test)))]
    use core::ptr;
    #[cfg(host_cmd_thread_test)]
    use std::ptr;

    const H2C_SUCCESS: u8 = 0;
    const H2C_PARAMETERS_ERROR: u8 = 4;
    const H2C_DROPPED: u8 = 3;
    const RTW_SCTX_DONE_CMD_ERROR: c_int = 2;
    /// Host `HOST_CMD_WLANCMDS_SIZE`; kernel table length must match `wlancmds` link.
    const RTW_CMDTABLE_SIZE: usize = 4;
    const _TRUE: c_int = 1;

    type IrqL = usize;

    #[repr(C)]
    struct SubmitCtx {
        status: c_int,
        done: c_int,
    }

    #[repr(C)]
    struct CmdObj {
        padapter: *mut Adapter,
        cmdcode: u16,
        res: u8,
        parmbuf: *mut u8,
        cmdsz: u32,
        no_io: u8,
        sctx: *mut SubmitCtx,
        list: List,
    }

    #[repr(C)]
    struct CmdPriv {
        cmd_queue_sema: c_int,
        start_cmdthread_sema: c_int,
        cmd_queue: Queue,
        cmd_seq: u8,
        cmd_buf: *mut u8,
        cmd_issued_cnt: u32,
        cmd_done_cnt: u32,
        cmdthd_running: c_int,
        padapter: *mut Adapter,
        sctx_mutex: c_int,
    }

    #[repr(C)]
    pub struct Adapter {
        bDriverStopped: u8,
        bSurpriseRemoved: u8,
        cmdThread: *mut c_void,
        cmdpriv: CmdPriv,
    }

    type CmdHdl = extern "C" fn(*mut Adapter, *mut u8) -> u8;

    #[repr(C)]
    struct RtwCmd {
        cmd_hdl: Option<CmdHdl>,
        callback: *const c_void,
    }

    extern "C" {
        fn host_cmd_thread_loop_continue() -> c_int;
        fn _rtw_up_sema(s: *mut c_int);
        fn _rtw_down_sema(s: *mut c_int) -> Sint;
        fn rtw_thread_stop(th: *mut c_void) -> c_int;
        fn host_cmd_thread_enter_critical(l: *mut c_int, irql: *mut IrqL);
        fn host_cmd_thread_exit_critical(l: *mut c_int, irql: *mut IrqL);
        fn host_cmd_thread_is_list_empty(head: *mut List) -> c_int;
        fn host_cmd_thread_sctx_mutex_enter(m: *mut c_int);
        fn host_cmd_thread_sctx_mutex_exit(m: *mut c_int);
        fn host_cmd_thread_wait_stop();
        fn rtw_dequeue_cmd(p: *mut CmdPriv) -> *mut CmdObj;
        fn rtw_cmd_filter(p: *mut CmdPriv, obj: *mut CmdObj) -> c_int;
        fn rtw_free_cmd_obj(obj: *mut CmdObj);
        fn rtw_sctx_done(sctx: *mut *mut SubmitCtx);
        fn rtw_sctx_done_err(sctx: *mut *mut SubmitCtx, status: c_int);
        static mut wlancmds: [RtwCmd; RTW_CMDTABLE_SIZE];
    }

    #[no_mangle]
    pub extern "C" fn rtw_cmd_clr_isr(pcmdpriv: *mut CmdPriv) {
        if !pcmdpriv.is_null() {
            unsafe {
                (*pcmdpriv).cmd_done_cnt += 1;
            }
        }
    }

    #[no_mangle]
    pub extern "C" fn rtw_stop_cmd_thread(adapter: *mut Adapter) {
        if adapter.is_null() {
            return;
        }
        unsafe {
            if !(*adapter).cmdThread.is_null() {
                _rtw_up_sema(&mut (*adapter).cmdpriv.cmd_queue_sema);
                rtw_thread_stop((*adapter).cmdThread);
                (*adapter).cmdThread = core::ptr::null_mut();
            }
        }
    }

    #[no_mangle]
    pub extern "C" fn rtw_cmd_thread(context: *mut c_void) -> c_int {
        let padapter = context as *mut Adapter;
        if padapter.is_null() {
            return 0;
        }
        unsafe {
            let pcmdpriv = &mut (*padapter).cmdpriv;
            let pcmdbuf = pcmdpriv.cmd_buf;
            let mut exit_outer = false;

            (*pcmdpriv).cmdthd_running = _TRUE;
            _rtw_up_sema(&mut pcmdpriv.start_cmdthread_sema);

            'outer: while !exit_outer {
                if host_cmd_thread_loop_continue() == 0 {
                    break;
                }
                if _rtw_down_sema(&mut pcmdpriv.cmd_queue_sema) == _FAIL {
                    break;
                }
                if (*padapter).bDriverStopped != 0 || (*padapter).bSurpriseRemoved != 0 {
                    break;
                }
                let mut irqL: IrqL = 0;
                host_cmd_thread_enter_critical(
                    &mut pcmdpriv.cmd_queue.lock,
                    &mut irqL as *mut IrqL,
                );
                let queue_empty =
                    host_cmd_thread_is_list_empty(&mut pcmdpriv.cmd_queue.queue as *mut List) != 0;
                host_cmd_thread_exit_critical(&mut pcmdpriv.cmd_queue.lock, &mut irqL as *mut IrqL);
                if queue_empty {
                    continue;
                }

                'next: loop {
                    if host_cmd_thread_loop_continue() == 0 {
                        exit_outer = true;
                        break 'next;
                    }
                    if (*padapter).bDriverStopped != 0 || (*padapter).bSurpriseRemoved != 0 {
                        break 'outer;
                    }

                    let pcmd = rtw_dequeue_cmd(pcmdpriv);
                    if pcmd.is_null() {
                        continue 'outer;
                    }

                    (*pcmdpriv).cmd_issued_cnt += 1;
                    let idx = (*pcmd).cmdcode as usize;
                    let cmdsz = (*pcmd).cmdsz;
                    if cmdsz > MAX_CMDSZ || idx >= RTW_CMDTABLE_SIZE {
                        (*pcmd).res = H2C_PARAMETERS_ERROR;
                        post_process(pcmd);
                        continue 'next;
                    }
                    let cmd_hdl = wlancmds[idx].cmd_hdl;
                    if cmd_hdl.is_none() {
                        (*pcmd).res = H2C_PARAMETERS_ERROR;
                        post_process(pcmd);
                        continue 'next;
                    }
                    if rtw_cmd_filter(pcmdpriv, pcmd) == _FAIL {
                        (*pcmd).res = H2C_DROPPED;
                        post_process(pcmd);
                        continue 'next;
                    }
                    ptr::copy_nonoverlapping((*pcmd).parmbuf, pcmdbuf, (*pcmd).cmdsz as usize);
                    let ret = cmd_hdl.unwrap()((*pcmd).padapter, pcmdbuf);
                    (*pcmd).res = ret;
                    pcmdpriv.cmd_seq = pcmdpriv.cmd_seq.wrapping_add(1);
                    post_process(pcmd);
                }
            }

            (*pcmdpriv).cmdthd_running = 0;
            loop {
                let pcmd = rtw_dequeue_cmd(pcmdpriv);
                if pcmd.is_null() {
                    break;
                }
                rtw_free_cmd_obj(pcmd);
            }
            host_cmd_thread_wait_stop();
        }
        0
    }

    unsafe fn post_process(pcmd: *mut CmdObj) {
        let padapter = (*pcmd).padapter;
        let sctx_ptr = &mut (*pcmd).sctx as *mut *mut SubmitCtx;
        if !padapter.is_null() {
            host_cmd_thread_sctx_mutex_enter(&mut (*padapter).cmdpriv.sctx_mutex);
        }
        if !(*pcmd).sctx.is_null() {
            if (*pcmd).res == H2C_SUCCESS {
                rtw_sctx_done(sctx_ptr);
            } else {
                rtw_sctx_done_err(sctx_ptr, RTW_SCTX_DONE_CMD_ERROR);
            }
        }
        if !padapter.is_null() {
            host_cmd_thread_sctx_mutex_exit(&mut (*padapter).cmdpriv.sctx_mutex);
        }
        rtw_free_cmd_obj(pcmd);
    }
}

#[cfg(any(host_cmd_joinbss_test, rust_joinbss_cmd))]
mod joinbss_cmd {
    use super::{c_int, c_void, Sint, _FAIL, _SUCCESS};

    const _TRUE: c_int = 1;
    const CMD_JOINBSS: u16 = 0;
    const Ndis802_11IBSS: c_int = 0;
    const Ndis802_11Infrastructure: c_int = 1;
    const WIFI_STATION_STATE: u32 = 0x08;
    const WIFI_ADHOC_STATE: u32 = 0x20;
    const ETH_ALEN: usize = 6;

    #[repr(C)]
    struct Ndis80211Ssid {
        ssid_length: u32,
        ssid: [u8; 32],
    }

    #[repr(C)]
    struct Ndis80211Configuration {
        ds_config: u32,
    }

    #[repr(C)]
    struct WlanBssidEx {
        mac_address: [u8; ETH_ALEN],
        ssid: Ndis80211Ssid,
        configuration: Ndis80211Configuration,
        infrastructure_mode: c_int,
        ie_length: u32,
        ies: [u8; 768],
    }

    #[repr(C)]
    struct WlanNetwork {
        network: WlanBssidEx,
    }

    #[repr(C)]
    struct MlmePriv {
        fw_state: u32,
        assoc_by_bssid: u8,
        assoc_bssid: [u8; ETH_ALEN],
    }

    #[repr(C)]
    struct SecurityPriv {
        authenticator_ie: [u8; 256],
    }

    #[repr(C)]
    struct RegistryPriv {
        wmm_enable: u8,
    }

    #[repr(C)]
    struct CmdObj {
        cmdcode: u16,
        cmdsz: u32,
        parmbuf: *mut u8,
    }

    #[repr(C)]
    struct CmdPriv {
        _pad: c_int,
    }

    #[repr(C)]
    pub struct Adapter {
        mlmepriv: MlmePriv,
        securitypriv: SecurityPriv,
        registrypriv: RegistryPriv,
        cmdpriv: CmdPriv,
    }

    #[repr(C)]
    struct HostJoinbssTrace {
        enqueue_ok: c_int,
        cmd_code: c_int,
    }

    extern "C" {
        fn host_joinbss_zmalloc(sz: u32) -> *mut c_void;
        fn host_joinbss_get_trace() -> *mut HostJoinbssTrace;
        fn check_fwstate(m: *mut MlmePriv, s: Sint) -> c_int;
        fn set_fwstate(m: *mut MlmePriv, s: Sint);
        fn free(ptr: *mut c_void);
    }

    #[no_mangle]
    pub extern "C" fn rtw_joinbss_cmd(padapter: *mut Adapter, pnetwork: *mut WlanNetwork) -> u8 {
        if padapter.is_null() || pnetwork.is_null() {
            return _FAIL as u8;
        }
        unsafe {
            let padapter = &mut *padapter;
            let pnetwork = &*pnetwork;
            let ndis_mode = pnetwork.network.infrastructure_mode;

            let pcmd = host_joinbss_zmalloc(core::mem::size_of::<CmdObj>() as u32) as *mut CmdObj;
            if pcmd.is_null() {
                return _FAIL as u8;
            }
            if check_fwstate(
                &mut padapter.mlmepriv as *mut MlmePriv,
                WIFI_STATION_STATE as Sint | WIFI_ADHOC_STATE as Sint,
            ) != _TRUE
            {
                if ndis_mode == Ndis802_11IBSS {
                    set_fwstate(
                        &mut padapter.mlmepriv as *mut MlmePriv,
                        WIFI_ADHOC_STATE as Sint,
                    );
                } else if ndis_mode == Ndis802_11Infrastructure {
                    set_fwstate(
                        &mut padapter.mlmepriv as *mut MlmePriv,
                        WIFI_STATION_STATE as Sint,
                    );
                }
            }

            let psecnetwork = host_joinbss_zmalloc(core::mem::size_of::<WlanBssidEx>() as u32)
                as *mut WlanBssidEx;
            if psecnetwork.is_null() {
                free(pcmd as *mut c_void);
                return _FAIL as u8;
            }

            core::ptr::copy_nonoverlapping(&pnetwork.network, psecnetwork, 1);
            padapter.securitypriv.authenticator_ie[0] = (*psecnetwork).ie_length as u8;
            (*psecnetwork).ie_length = 12;
            if padapter.mlmepriv.assoc_by_bssid == 0 {
                core::ptr::copy_nonoverlapping(
                    pnetwork.network.mac_address.as_ptr(),
                    padapter.mlmepriv.assoc_bssid.as_mut_ptr(),
                    ETH_ALEN,
                );
            }

            (*pcmd).cmdsz = core::mem::size_of::<WlanBssidEx>() as u32;
            (*pcmd).cmdcode = CMD_JOINBSS;
            (*pcmd).parmbuf = psecnetwork as *mut u8;

            let tr = &mut *host_joinbss_get_trace();
            tr.enqueue_ok = 1;
            tr.cmd_code = CMD_JOINBSS as c_int;
            _SUCCESS as u8
        }
    }
}

#[cfg(any(host_cmd_drvextra_test, rust_drvextra_cmd))]
mod drvextra_cmd {
    use super::c_int;

    const H2C_SUCCESS: u8 = 0;
    const H2C_PARAMETERS_ERROR: u8 = 4;

    #[repr(C)]
    struct DrvextraCmdParm {
        ec_id: c_int,
        type_: c_int,
        size: c_int,
        pbuf: *mut u8,
    }

    #[repr(C)]
    pub struct Adapter {
        pad: u8,
    }

    extern "C" {
        fn rtw_dynamic_chk_wk_hdl(a: *mut Adapter);
        fn reset_securitypriv_hdl(a: *mut Adapter);
        fn free_assoc_resources_hdl(a: *mut Adapter, type_: u8);
        fn rtw_chk_hi_queue_hdl(a: *mut Adapter);
        fn rtw_mfree(p: *mut u8, sz: u32);
    }

    #[no_mangle]
    pub extern "C" fn rtw_drvextra_cmd_hdl(padapter: *mut Adapter, pbuf: *mut u8) -> u8 {
        if pbuf.is_null() {
            return H2C_PARAMETERS_ERROR;
        }
        unsafe {
            let pdrvextra_cmd = &*(pbuf as *mut DrvextraCmdParm);
            match pdrvextra_cmd.ec_id {
                2 => rtw_dynamic_chk_wk_hdl(padapter),
                10 => {
                    #[cfg(config_ap_mode)]
                    rtw_chk_hi_queue_hdl(padapter);
                }
                13 => reset_securitypriv_hdl(padapter),
                14 => free_assoc_resources_hdl(padapter, pdrvextra_cmd.type_ as u8),
                _ => {}
            }
            if !pdrvextra_cmd.pbuf.is_null() && pdrvextra_cmd.size > 0 {
                rtw_mfree(pdrvextra_cmd.pbuf, pdrvextra_cmd.size as u32);
            }
            H2C_SUCCESS
        }
    }
}

#[cfg(any(host_cmd_traffic_lps_test, rust_traffic_lps_cmd))]
mod traffic_lps_cmd {
    use super::c_int;

    const _TRUE: c_int = 1;
    const _FALSE: c_int = 0;
    const WIFI_ASOC_STATE: u32 = 0x01;
    const WIFI_STATION_STATE: u32 = 0x08;
    const WIFI_ADHOC_STATE: u32 = 0x20;
    const WIFI_ADHOC_MASTER_STATE: u32 = 0x40;
    const LPS_CTRL_CONNECT: u8 = 2;
    const LPS_CTRL_SPECIAL_PACKET: u8 = 4;
    const LPS_CTRL_LEAVE: u8 = 5;
    const LPS_CTRL_TRAFFIC_BUSY: u8 = 6;
    const LPS_CTRL_ENTER: u8 = 9;
    const HW_VAR_H2C_FW_JOINBSSRPT: c_int = 0;
    const LPS_DELAY_MS: c_int = 1000;
    const HW_PORT0: u8 = 0;
    const ETH_ALEN: usize = 6;

    #[repr(C)]
    struct RT_LINK_DETECT_T {
        NumRxOkInPeriod: u32,
        NumTxOkInPeriod: u32,
        NumRxUnicastOkInPeriod: u32,
        bBusyTraffic: u8,
        bTxBusyTraffic: u8,
        bRxBusyTraffic: u8,
        bHigherBusyTraffic: u8,
        bHigherBusyRxTraffic: u8,
        bHigherBusyTxTraffic: u8,
    }

    #[repr(C)]
    struct MlmePriv {
        fw_state: u32,
        LinkDetectInfo: RT_LINK_DETECT_T,
        assoc_bssid: [u8; ETH_ALEN],
    }

    #[repr(C)]
    struct PwrctrlPriv {
        lps_level: i8,
        LpsIdleCount: u8,
        bLeisurePs: u8,
        lps_chk_by_tp: u8,
        lps_bi_tp_th: c_int,
        lps_tx_tp_th: c_int,
        lps_rx_tp_th: c_int,
        lps_chk_cnt: c_int,
        lps_chk_cnt_th: c_int,
    }

    #[repr(C)]
    struct sta_stats {
        tx_bytes: u32,
        rx_bytes: u32,
        acc_tx_bytes: u32,
        acc_rx_bytes: u32,
        tx_tp_kbits: u32,
        rx_tp_kbits: u32,
    }

    #[repr(C)]
    struct sta_info {
        padapter: *mut Adapter,
        sta_stats: sta_stats,
    }

    #[repr(C)]
    struct sta_priv {
        sta: *mut sta_info,
    }

    #[repr(C)]
    pub struct Adapter {
        mlmepriv: MlmePriv,
        pwrctrlpriv: PwrctrlPriv,
        stapriv: sta_priv,
        hw_port: u8,
    }

    const _: () = assert!(core::mem::size_of::<RT_LINK_DETECT_T>() == 20);
    const _: () = assert!(core::mem::size_of::<MlmePriv>() == 32);
    const _: () = assert!(core::mem::offset_of!(Adapter, pwrctrlpriv) == 32);

    extern "C" {
        fn check_fwstate(m: *mut MlmePriv, s: c_int) -> c_int;
        fn LPS_Enter(a: *mut Adapter, reason: *const u8);
        fn LPS_Leave(a: *mut Adapter, reason: *const u8);
        fn rtw_hal_set_hwreg(a: *mut Adapter, id: c_int, val: *mut u8);
        fn rtw_set_lps_deny(a: *mut Adapter, ms: c_int);
        fn get_bssid(pmlmepriv: *mut MlmePriv) -> *mut u8;
        fn rtw_get_stainfo(pstapriv: *mut sta_priv, bssid: *mut u8) -> *mut sta_info;
        fn rtw_get_bcn_cnt(adapter: *mut Adapter) -> u8;
        fn rtw_lps_ctrl_wk_cmd(adapter: *mut Adapter, lps_ctrl_type: u8, flags: u8);
        fn rtw_mi_get_assoc_if_num(padapter: *mut Adapter) -> c_int;
        fn session_tracker_chk_cmd(padapter: *mut Adapter, parm: *mut core::ffi::c_void);
    }

    #[no_mangle]
    pub extern "C" fn lps_ctrl_wk_hdl(padapter: *mut Adapter, t: u8, buf: *mut u8) {
        if padapter.is_null() {
            return;
        }
        unsafe {
            let mlme = &mut (*padapter).mlmepriv;
            if check_fwstate(mlme as *mut MlmePriv, WIFI_ADHOC_MASTER_STATE as c_int) == _TRUE
                || check_fwstate(mlme as *mut MlmePriv, WIFI_ADHOC_STATE as c_int) == _TRUE
            {
                return;
            }
            let pwr = &mut (*padapter).pwrctrlpriv;
            match t {
                LPS_CTRL_CONNECT => {
                    let mut mstatus = 1u8;
                    pwr.LpsIdleCount = 0;
                    rtw_hal_set_hwreg(padapter, HW_VAR_H2C_FW_JOINBSSRPT, &mut mstatus);
                }
                LPS_CTRL_SPECIAL_PACKET => {
                    rtw_set_lps_deny(padapter, LPS_DELAY_MS);
                    LPS_Leave(padapter, b"LPS_CTRL_SPECIAL_PACKET\0".as_ptr());
                }
                LPS_CTRL_LEAVE => LPS_Leave(padapter, b"LPS_CTRL_LEAVE\0".as_ptr()),
                LPS_CTRL_ENTER => LPS_Enter(padapter, b"TRAFFIC_IDLE_1\0".as_ptr()),
                _ => {
                    let _ = buf;
                }
            }
        }
    }

    #[no_mangle]
    pub extern "C" fn _lps_chk_by_tp(adapter: *mut Adapter, from_timer: u8) -> u8 {
        if adapter.is_null() {
            return _FALSE as u8;
        }
        unsafe {
            let pmlmepriv = &mut (*adapter).mlmepriv;
            let psta = rtw_get_stainfo(
                &mut (*adapter).stapriv as *mut sta_priv,
                get_bssid(pmlmepriv as *mut MlmePriv),
            );
            if psta.is_null() {
                return _FALSE as u8;
            }
            let _ = rtw_get_bcn_cnt(adapter);
            (*psta).sta_stats.acc_tx_bytes = (*psta).sta_stats.tx_bytes;
            (*psta).sta_stats.acc_rx_bytes = (*psta).sta_stats.rx_bytes;
            let pwrpriv = &mut (*adapter).pwrctrlpriv;
            let tx_tp_mbits = (*psta).sta_stats.tx_tp_kbits >> 10;
            let rx_tp_mbits = (*psta).sta_stats.rx_tp_kbits >> 10;
            let bi_tp_mbits = tx_tp_mbits + rx_tp_mbits;
            let enter_ps = if bi_tp_mbits >= pwrpriv.lps_bi_tp_th as u32
                || tx_tp_mbits >= pwrpriv.lps_tx_tp_th as u32
                || rx_tp_mbits >= pwrpriv.lps_rx_tp_th as u32
            {
                pwrpriv.lps_chk_cnt = pwrpriv.lps_chk_cnt_th;
                _FALSE
            } else if pwrpriv.lps_chk_cnt != 0 && {
                pwrpriv.lps_chk_cnt -= 1;
                pwrpriv.lps_chk_cnt != 0
            } {
                _FALSE
            } else {
                _TRUE
            };
            if enter_ps == _TRUE {
                if from_timer == 0 {
                    LPS_Enter(adapter, b"TRAFFIC_IDLE\0".as_ptr());
                }
            } else if from_timer == 0 {
                LPS_Leave(adapter, b"TRAFFIC_BUSY\0".as_ptr());
            } else if (*adapter).hw_port == HW_PORT0 {
                rtw_lps_ctrl_wk_cmd(adapter, LPS_CTRL_TRAFFIC_BUSY, 0);
            }
            enter_ps as u8
        }
    }

    #[no_mangle]
    pub extern "C" fn _lps_chk_by_pkt_cnts(
        padapter: *mut Adapter,
        from_timer: u8,
        b_busy_traffic: u8,
    ) -> u8 {
        let _ = b_busy_traffic;
        if padapter.is_null() {
            return _FALSE as u8;
        }
        unsafe {
            let ld = &mut (*padapter).mlmepriv.LinkDetectInfo;
            let b_enter_ps = if (ld.NumRxUnicastOkInPeriod + ld.NumTxOkInPeriod > 8)
                || ld.NumRxUnicastOkInPeriod > 4
            {
                _FALSE
            } else {
                _TRUE
            };
            if b_enter_ps == _TRUE {
                if from_timer == 0 {
                    LPS_Enter(padapter, b"TRAFFIC_IDLE\0".as_ptr());
                }
            } else if from_timer == 0 {
                LPS_Leave(padapter, b"TRAFFIC_BUSY\0".as_ptr());
            } else if (*padapter).hw_port == HW_PORT0 {
                rtw_lps_ctrl_wk_cmd(padapter, LPS_CTRL_TRAFFIC_BUSY, 0);
            }
            b_enter_ps as u8
        }
    }

    #[no_mangle]
    pub extern "C" fn traffic_status_watchdog(padapter: *mut Adapter, from_timer: u8) -> u8 {
        if padapter.is_null() {
            return _FALSE as u8;
        }
        unsafe {
            let pmlmepriv = &mut (*padapter).mlmepriv;
            let pwrpriv = &mut (*padapter).pwrctrlpriv;
            let mut b_enter_ps = _FALSE;
            let mut busy_threshold: u16 = 100;
            let mut b_busy_traffic = _FALSE;
            let mut b_tx_busy_traffic = _FALSE;
            let mut b_rx_busy_traffic = _FALSE;
            let mut b_higher_busy_traffic = _FALSE;
            let mut b_higher_busy_rx_traffic = _FALSE;
            let mut b_higher_busy_tx_traffic = _FALSE;
            if check_fwstate(pmlmepriv as *mut MlmePriv, WIFI_ASOC_STATE as c_int) == _TRUE {
                if pmlmepriv.LinkDetectInfo.bBusyTraffic != 0 {
                    busy_threshold = 75;
                }
                if pmlmepriv.LinkDetectInfo.NumRxOkInPeriod > busy_threshold as u32
                    || pmlmepriv.LinkDetectInfo.NumTxOkInPeriod > busy_threshold as u32
                {
                    b_busy_traffic = _TRUE;
                    if pmlmepriv.LinkDetectInfo.NumRxOkInPeriod
                        > pmlmepriv.LinkDetectInfo.NumTxOkInPeriod
                    {
                        b_rx_busy_traffic = _TRUE;
                    } else {
                        b_tx_busy_traffic = _TRUE;
                    }
                }
                if pmlmepriv.LinkDetectInfo.NumRxOkInPeriod > 4000
                    || pmlmepriv.LinkDetectInfo.NumTxOkInPeriod > 4000
                {
                    b_higher_busy_traffic = _TRUE;
                    if pmlmepriv.LinkDetectInfo.NumRxOkInPeriod
                        > pmlmepriv.LinkDetectInfo.NumTxOkInPeriod
                    {
                        b_higher_busy_rx_traffic = _TRUE;
                    } else {
                        b_higher_busy_tx_traffic = _TRUE;
                    }
                }
                if pwrpriv.bLeisurePs != 0 && (pmlmepriv.fw_state & WIFI_STATION_STATE) != 0 {
                    b_enter_ps = if pwrpriv.lps_chk_by_tp != 0 {
                        _lps_chk_by_tp(padapter, from_timer) as c_int
                    } else {
                        _lps_chk_by_pkt_cnts(padapter, from_timer, b_busy_traffic as u8) as c_int
                    };
                }
            } else if from_timer == 0 && rtw_mi_get_assoc_if_num(padapter) == 0 {
                LPS_Leave(padapter, b"NON_LINKED\0".as_ptr());
            }
            session_tracker_chk_cmd(padapter, core::ptr::null_mut());
            pmlmepriv.LinkDetectInfo.NumRxOkInPeriod = 0;
            pmlmepriv.LinkDetectInfo.NumTxOkInPeriod = 0;
            pmlmepriv.LinkDetectInfo.NumRxUnicastOkInPeriod = 0;
            pmlmepriv.LinkDetectInfo.bBusyTraffic = b_busy_traffic as u8;
            pmlmepriv.LinkDetectInfo.bTxBusyTraffic = b_tx_busy_traffic as u8;
            pmlmepriv.LinkDetectInfo.bRxBusyTraffic = b_rx_busy_traffic as u8;
            pmlmepriv.LinkDetectInfo.bHigherBusyTraffic = b_higher_busy_traffic as u8;
            pmlmepriv.LinkDetectInfo.bHigherBusyRxTraffic = b_higher_busy_rx_traffic as u8;
            pmlmepriv.LinkDetectInfo.bHigherBusyTxTraffic = b_higher_busy_tx_traffic as u8;
            b_enter_ps as u8
        }
    }
}
