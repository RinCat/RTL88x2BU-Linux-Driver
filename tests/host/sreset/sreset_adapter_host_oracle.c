// SPDX-License-Identifier: GPL-2.0
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
static sint check_fwstate(struct mlme_priv *m, sint s)
{
	return (m->fw_state & (u32)s) != 0;
}
static void rtw_netif_stop_queue(struct net_device *dev)
{
	(void)dev;
	g_trace.netif_stop++;
}
static void rtw_netif_wake_queue(struct net_device *dev)
{
	(void)dev;
	g_trace.netif_wake++;
}
static void rtw_cancel_all_timer(PADAPTER padapter)
{
	(void)padapter;
	g_trace.cancel_timers++;
}

static void tasklet_kill(struct xmit_tasklet *t)
{
	(void)t;
	g_trace.tasklet_kill++;
}

static void tasklet_hi_schedule(struct xmit_tasklet *t)
{
	(void)t;
	g_trace.tasklet_schedule++;
}

static void rtw_scan_abort(PADAPTER padapter)
{
	(void)padapter;
	g_trace.scan_abort++;
}

static void rtw_set_to_roam(PADAPTER padapter, u8 to_roam)
{
	(void)padapter;
	(void)to_roam;
	g_trace.set_to_roam++;
}

static void rtw_join_timeout_handler(PADAPTER padapter)
{
	(void)padapter;
	g_trace.join_timeout++;
}

static void sreset_restore_network_status(PADAPTER padapter)
{
	(void)padapter;
	g_trace.restore_network++;
}

static void _set_timer(struct timer_list *t, u32 ms)
{
	t->ms = ms;
	g_trace.dynamic_chk_timer_ms = ms;
}

void sreset_stop_adapter(PADAPTER padapter)
{
	struct mlme_priv *pmlmepriv;
	struct xmit_priv *pxmitpriv;

	if (!padapter)
		return;
	pmlmepriv = &padapter->mlmepriv;
	pxmitpriv = &padapter->xmitpriv;
	rtw_netif_stop_queue(padapter->pnetdev);
	rtw_cancel_all_timer(padapter);
	tasklet_kill(&pxmitpriv->xmit_tasklet);
	if (check_fwstate(pmlmepriv, WIFI_UNDER_SURVEY))
		rtw_scan_abort(padapter);
	if (check_fwstate(pmlmepriv, WIFI_UNDER_LINKING)) {
		rtw_set_to_roam(padapter, 0);
		rtw_join_timeout_handler(padapter);
	}
}

void sreset_start_adapter(PADAPTER padapter)
{
	struct mlme_priv *pmlmepriv;
	struct xmit_priv *pxmitpriv;

	if (!padapter)
		return;
	pmlmepriv = &padapter->mlmepriv;
	pxmitpriv = &padapter->xmitpriv;
	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE))
		sreset_restore_network_status(padapter);
	tasklet_hi_schedule(&pxmitpriv->xmit_tasklet);
	if (is_primary_adapter(padapter))
		_set_timer(&adapter_to_dvobj(padapter)->dynamic_chk_timer, 2000);
	rtw_netif_wake_queue(padapter->pnetdev);
}
