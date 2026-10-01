// SPDX-License-Identifier: GPL-2.0
/* Trace stubs for sreset_reset host L2 (W3-95 PR9). */
#include "host_sreset_reset_types.h"

static struct host_sreset_reset_trace g_trace;

struct host_sreset_reset_trace *host_sreset_reset_get_trace(void)
{
	return &g_trace;
}

void host_sreset_reset_reset_trace(void)
{
	g_trace = (struct host_sreset_reset_trace){0};
}

void host_sreset_reset_set_ps_mode(PADAPTER padapter)
{
	(void)padapter;
	g_trace.ps_mode_active++;
}

void host_sreset_reset_enter_pwrlock(PADAPTER padapter)
{
	(void)padapter;
	g_trace.pwrlock_enter++;
}

void host_sreset_reset_exit_pwrlock(PADAPTER padapter)
{
	(void)padapter;
	g_trace.pwrlock_exit++;
}

void host_sreset_reset_mi_adapter_hdl(PADAPTER padapter, u8 bstart)
{
	struct sreset_priv *psrtpriv;

	(void)padapter;
	if (bstart)
		g_trace.mi_start++;
	else
		g_trace.mi_stop++;
	psrtpriv = &GET_HAL_DATA(padapter)->srestpriv;
	if (!bstart)
		g_trace.inprogress_after_stop = psrtpriv->silent_reset_inprogress;
}

void host_sreset_reset_ips_enter(PADAPTER padapter)
{
	(void)padapter;
	g_trace.ips_enter++;
}

void host_sreset_reset_ips_leave(PADAPTER padapter)
{
	(void)padapter;
	g_trace.ips_leave++;
}

void host_sreset_reset_ap_info_restore(PADAPTER padapter)
{
	(void)padapter;
	g_trace.ap_restore++;
}
