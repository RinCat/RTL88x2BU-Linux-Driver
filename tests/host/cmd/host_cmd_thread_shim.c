// SPDX-License-Identifier: GPL-2.0
#include <stdlib.h>
#include <stddef.h>
#include "host_cmd_thread_types.h"

static int g_sema_credits, g_sema_up, g_stop_calls;
static struct { u8 hw_init_completed; } g_hal;

struct rtw_cmd wlancmds[HOST_CMD_WLANCMDS_SIZE];

void host_cmd_thread_reset(void)
{
	g_sema_credits = g_sema_up = g_stop_calls = 0;
}

void host_cmd_thread_set_sema_credits(int n) { g_sema_credits = n; }
int host_cmd_thread_sema_up_count(void) { return g_sema_up; }
int host_cmd_thread_stop_calls(void) { return g_stop_calls; }
void host_cmd_thread_set_hw_init(int v) { g_hal.hw_init_completed = (u8)v; }
int host_cmd_thread_loop_budget(int set) { (void)set; return 0; }
int host_cmd_thread_cmd_hdl_calls(void) { return 0; }
int host_cmd_thread_loop_continue(void) { return 0; }

sint _rtw_down_sema(_sema *s) { (void)s; return g_sema_credits-- > 0 ? _SUCCESS : _FAIL; }
void _rtw_up_sema(_sema *s) { (void)s; g_sema_up++; }
int rtw_thread_stop(void *th) { (void)th; g_stop_calls++; return 1; }
