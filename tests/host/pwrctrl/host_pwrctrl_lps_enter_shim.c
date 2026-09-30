// SPDX-License-Identifier: GPL-2.0
#include <string.h>
#include "host_pwrctrl_lps_enter.h"

static systime g_time;
static int g_assoc = 1;
static struct host_pwrctrl_lps_enter_trace g_trace;

void host_pwrctrl_lps_enter_set_time(systime t) { g_time = t; }
void host_pwrctrl_lps_enter_set_assoc_if_num(int n) { g_assoc = n; }
void host_pwrctrl_lps_enter_reset_trace(void) { memset(&g_trace, 0, sizeof(g_trace)); }
struct host_pwrctrl_lps_enter_trace *host_pwrctrl_lps_enter_get_trace(void) { return &g_trace; }

static sint check_fwstate(struct mlme_priv *m, sint s)
{
	return (!s && !m->fw_state) || (m->fw_state & (u32)s) ? _TRUE : _FALSE;
}

static u8 ps_rdy_check(_adapter *a)
{
	struct pwrctrl_priv *p = &a->pwrctrlpriv;
	struct mlme_priv *m = &a->mlmepriv;

	if (p->bInSuspend || (s32)(p->lps_deny_time - g_time) > 0)
		return _FALSE;
	if (check_fwstate(m, WIFI_UNDER_SURVEY)
	    || check_fwstate(m, WIFI_UNDER_LINKING | WIFI_UNDER_WPS)
	    || (m->fw_state & WIFI_AP_STATE)
	    || check_fwstate(m, WIFI_ADHOC_MASTER_STATE | WIFI_ADHOC_STATE))
		return _FALSE;
	return _TRUE;
}

static void set_ps_mode(_adapter *a, u8 mode, u8 smart, const char *msg)
{
	(void)smart;
	(void)msg;
	g_trace.set_ps_mode_calls++;
	g_trace.last_ps_mode = mode;
	a->pwrctrlpriv.pwr_mode = mode;
}

void LPS_Enter(_adapter *a, const char *msg)
{
	struct dvobj_priv *d = a->dvobj;
	struct pwrctrl_priv *p = &a->pwrctrlpriv;
	int i;

	(void)msg;
	if (!a->HalData.bFWReady || g_assoc != 1 || a->hw_port != HW_PORT0)
		return;
	for (i = 0; i < d->iface_nums; i++)
		if (!ps_rdy_check(d->padapters[i]))
			return;
	if (!p->bLeisurePs)
		return;
	if (p->LpsIdleCount >= 2) {
		if (p->pwr_mode == PS_MODE_ACTIVE) {
			p->bpower_saving = _TRUE;
			set_ps_mode(a, p->power_mgnt, a->registrypriv.smart_ps, msg);
		}
	} else
		p->LpsIdleCount++;
}

void LPS_Leave(_adapter *a, const char *msg)
{
	struct pwrctrl_priv *p = &a->pwrctrlpriv;

	(void)msg;
	if (p->bLeisurePs) {
		if (p->pwr_mode != PS_MODE_ACTIVE)
			set_ps_mode(a, PS_MODE_ACTIVE, 0, msg);
	}
	p->bpower_saving = _FALSE;
}
