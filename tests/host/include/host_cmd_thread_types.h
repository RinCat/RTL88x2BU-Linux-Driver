/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_CMD_THREAD_TYPES_H
#define HOST_CMD_THREAD_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 1
#define _FAIL 0
#define MAX_CMDSZ 1536
#define H2C_SUCCESS 0
#define H2C_PARAMETERS_ERROR 4
#define H2C_DROPPED 3
#define RTW_SCTX_DONE_CMD_ERROR 2
#define CMD_SET_DRV_EXTRA 12
#define CMD_SET_CHANPLAN 13
#define HOST_CMD_WLANCMDS_SIZE 4

typedef int sint, ATOMIC_T, _sema, _mutex, _lock;
typedef unsigned long _irqL;
typedef u32 systime;

struct _list { struct _list *next, *prev; };
typedef struct _list _list;
struct __queue { _list queue; _lock lock; };
typedef struct __queue _queue;

struct submit_ctx { int status, done; };
struct drvextra_cmd_parm { int ec_id, type, size; u8 *pbuf; };

struct cmd_obj {
	struct _adapter *padapter;
	u16 cmdcode;
	u8 res;
	u8 *parmbuf;
	u32 cmdsz;
	u8 no_io;
	struct submit_ctx *sctx;
	_list list;
};

struct cmd_priv {
	_sema cmd_queue_sema, start_cmdthread_sema;
	_queue cmd_queue;
	u8 cmd_seq;
	u8 *cmd_buf;
	u32 cmd_issued_cnt, cmd_done_cnt;
	ATOMIC_T cmdthd_running;
	struct _adapter *padapter;
	_mutex sctx_mutex;
};

struct _adapter {
	u8 bDriverStopped, bSurpriseRemoved;
	void *cmdThread;
	struct cmd_priv cmdpriv;
};

typedef struct _adapter *PADAPTER;
typedef struct _adapter _adapter;
typedef int thread_return;
typedef void *thread_context;

struct rtw_cmd {
	u8 (*cmd_hdl)(PADAPTER, u8 *);
	void (*callback)(PADAPTER, struct cmd_obj *);
};

#define ATOMIC_SET(v, x) (*(v) = (x))
#define RTW_CANNOT_RUN(p) ((p)->bDriverStopped || (p)->bSurpriseRemoved)

static inline void _rtw_init_listhead(_list *l) { l->next = l->prev = l; }
static inline void _enter_critical(_lock *l, _irqL *i) { (void)l; (void)i; }
static inline void _exit_critical(_lock *l, _irqL *i) { (void)l; (void)i; }
static inline int rtw_is_list_empty(_list *h) { return h->next == h; }
static inline void _enter_critical_mutex(_mutex *m, void *f) { (void)m; (void)f; }
static inline void _exit_critical_mutex(_mutex *m, void *f) { (void)m; (void)f; }

void host_cmd_thread_reset(void);
void host_cmd_thread_set_sema_credits(int n);
int host_cmd_thread_sema_up_count(void);
int host_cmd_thread_loop_budget(int set);
int host_cmd_thread_cmd_hdl_calls(void);
int host_cmd_thread_stop_calls(void);
void host_cmd_thread_set_hw_init(int v);
int host_cmd_thread_loop_continue(void);

void _rtw_up_sema(_sema *s);
sint _rtw_down_sema(_sema *s);
int rtw_thread_stop(void *th);
#define thread_enter(n) ((void)(n))
#define flush_signals_thread() ((void)0)
static inline void rtw_thread_wait_stop(void) {}

void rtw_cmd_clr_isr(struct cmd_priv *p);
void rtw_stop_cmd_thread(struct _adapter *a);
thread_return rtw_cmd_thread(thread_context ctx);

sint _rtw_enqueue_cmd(_queue *q, struct cmd_obj *obj, bool to_head);
struct cmd_obj *rtw_dequeue_cmd(struct cmd_priv *p);
int rtw_cmd_filter(struct cmd_priv *p, struct cmd_obj *obj);
void rtw_free_cmd_obj(struct cmd_obj *pcmd);
void *rtw_zmalloc(u32 sz);
void rtw_mfree(u8 *p, u32 sz);
void rtw_sctx_done(struct submit_ctx **sctx);
void rtw_sctx_done_err(struct submit_ctx **sctx, int status);

extern struct rtw_cmd wlancmds[HOST_CMD_WLANCMDS_SIZE];

extern struct _adapter g_adapter;

#endif
