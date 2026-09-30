// SPDX-License-Identifier: GPL-2.0
/* Trace + host stubs shared by C oracle and Rust L2 differential (W3-95 PR5). */
#include "host_sreset_adapter_types.h"

static struct host_sreset_adapter_trace g_trace;

struct host_sreset_adapter_trace *host_sreset_adapter_get_trace(void)
{
	return &g_trace;
}

void host_sreset_adapter_reset_trace(void)
{
	g_trace = (struct host_sreset_adapter_trace){0};
}

sint host_sreset_check_fwstate(struct mlme_priv *m, sint s)
{
	return (m->fw_state & (u32)s) != 0;
}

void host_sreset_rtw_netif_stop_queue(struct net_device *dev)
{
	(void)dev;
	g_trace.netif_stop++;
}

void host_sreset_rtw_netif_wake_queue(struct net_device *dev)
{
	(void)dev;
	g_trace.netif_wake++;
}

void host_sreset_rtw_cancel_all_timer(PADAPTER padapter)
{
	(void)padapter;
	g_trace.cancel_timers++;
}

void host_sreset_tasklet_kill(struct xmit_tasklet *t)
{
	(void)t;
	g_trace.tasklet_kill++;
}

void host_sreset_tasklet_hi_schedule(struct xmit_tasklet *t)
{
	(void)t;
	g_trace.tasklet_schedule++;
}

void host_sreset_rtw_scan_abort(PADAPTER padapter)
{
	(void)padapter;
	g_trace.scan_abort++;
}

void host_sreset_rtw_set_to_roam(PADAPTER padapter, u8 to_roam)
{
	(void)padapter;
	(void)to_roam;
	g_trace.set_to_roam++;
}

void host_sreset_rtw_join_timeout_handler(PADAPTER padapter)
{
	(void)padapter;
	g_trace.join_timeout++;
}

void host_sreset_restore_network_status(PADAPTER padapter)
{
	(void)padapter;
	g_trace.restore_network++;
}

void host_sreset_set_timer(struct timer_list *t, u32 ms)
{
	t->ms = ms;
	g_trace.dynamic_chk_timer_ms = ms;
}
