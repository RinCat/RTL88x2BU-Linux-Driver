// SPDX-License-Identifier: GPL-2.0
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include "host_cmd_thread_types.h"

static int g_sema_credits, g_sema_up, g_stop_calls, g_loop_budget = 100, g_cmd_hdl_calls;
static struct { u8 hw_init_completed; } g_hal;

struct rtw_cmd wlancmds[HOST_CMD_WLANCMDS_SIZE];

static u8 default_cmd_hdl(PADAPTER a, u8 *b)
{
	(void)a;
	(void)b;
	g_cmd_hdl_calls++;
	return H2C_SUCCESS;
}

void host_cmd_thread_reset(void)
{
	g_sema_credits = g_sema_up = g_stop_calls = g_cmd_hdl_calls = 0;
	g_loop_budget = 100;
	wlancmds[0].cmd_hdl = default_cmd_hdl;
}

void host_cmd_thread_set_sema_credits(int n) { g_sema_credits = n; }
int host_cmd_thread_sema_up_count(void) { return g_sema_up; }
int host_cmd_thread_stop_calls(void) { return g_stop_calls; }
void host_cmd_thread_set_hw_init(int v) { g_hal.hw_init_completed = (u8)v; }
int host_cmd_thread_loop_budget(int set) { if (set >= 0) g_loop_budget = set; return g_loop_budget; }
int host_cmd_thread_cmd_hdl_calls(void) { return g_cmd_hdl_calls; }
int host_cmd_thread_loop_continue(void)
{
	if (g_loop_budget <= 0)
		return 0;
	g_loop_budget--;
	return 1;
}

sint _rtw_down_sema(_sema *s) { (void)s; return g_sema_credits-- > 0 ? _SUCCESS : _FAIL; }
void _rtw_up_sema(_sema *s) { (void)s; g_sema_up++; }
int rtw_thread_stop(void *th) { (void)th; g_stop_calls++; return 1; }
sint _rtw_enqueue_cmd(_queue *q, struct cmd_obj *obj, bool to_head)
{
	_irqL irqL;

	_enter_critical(&q->lock, &irqL);
	if (to_head) {
		obj->list.next = q->queue.next;
		obj->list.prev = &q->queue;
		q->queue.next->prev = &obj->list;
		q->queue.next = &obj->list;
	} else {
		obj->list.next = &q->queue;
		obj->list.prev = q->queue.prev;
		q->queue.prev->next = &obj->list;
		q->queue.prev = &obj->list;
	}
	_exit_critical(&q->lock, &irqL);
	return _SUCCESS;
}

struct cmd_obj *rtw_dequeue_cmd(struct cmd_priv *p)
{
	_irqL irqL;
	struct cmd_obj *obj = NULL;

	_enter_critical(&p->cmd_queue.lock, &irqL);
	if (p->cmd_queue.queue.next != &p->cmd_queue.queue) {
		_list *ln = p->cmd_queue.queue.next;
		ln->prev->next = ln->next;
		ln->next->prev = ln->prev;
		obj = (struct cmd_obj *)((char *)ln - offsetof(struct cmd_obj, list));
	}
	_exit_critical(&p->cmd_queue.lock, &irqL);
	return obj;
}

int rtw_cmd_filter(struct cmd_priv *p, struct cmd_obj *obj)
{
	u8 allow = obj->cmdcode == CMD_SET_CHANPLAN || obj->no_io;

	if (!p->cmdthd_running || (!g_hal.hw_init_completed && !allow))
		return _FAIL;
	return _SUCCESS;
}

void rtw_free_cmd_obj(struct cmd_obj *pcmd)
{
	if (pcmd->parmbuf)
		rtw_mfree(pcmd->parmbuf, pcmd->cmdsz);
	rtw_mfree((u8 *)pcmd, sizeof(*pcmd));
}

void *rtw_zmalloc(u32 sz) { return calloc(1, sz); }
void rtw_mfree(u8 *p, u32 sz) { (void)sz; free(p); }

void rtw_sctx_done(struct submit_ctx **sctx)
{
	if (sctx && *sctx)
		(*sctx)->done = 1;
}

void rtw_sctx_done_err(struct submit_ctx **sctx, int status)
{
	if (sctx && *sctx) {
		(*sctx)->done = 1;
		(*sctx)->status = status;
	}
}
