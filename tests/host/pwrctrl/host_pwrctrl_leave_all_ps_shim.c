// SPDX-License-Identifier: GPL-2.0
/* Host oracle for LeaveAllPowerSaveMode* (core/rtw_pwrctrl.c policy gates). */
#include <string.h>
#include "host_pwrctrl_leave_all_ps.h"

#define CONFIG_LPS 1
#define CONFIG_LPS_LCLK 1

static struct host_pwrctrl_leave_all_ps_trace g_trace;
static int g_assoc_if_num;
static u8 g_mi_linked;

void host_pwrctrl_leave_all_ps_reset_trace(void) { memset(&g_trace, 0, sizeof(g_trace)); }
struct host_pwrctrl_leave_all_ps_trace *host_pwrctrl_leave_all_ps_get_trace(void) { return &g_trace; }
void host_pwrctrl_leave_all_ps_set_assoc_if_num(int n) { g_assoc_if_num = n; }
void host_pwrctrl_leave_all_ps_set_mi_linked(u8 linked) { g_mi_linked = linked; }

static void rtw_lps_ctrl_wk_cmd(PADAPTER adapter, u8 t, u8 flags)
{
	(void)adapter;
	g_trace.lps_ctrl_wk_cmd++;
	g_trace.last_lps_ctrl_type = t;
	g_trace.last_lps_ctrl_flags = flags;
}

#if defined(CONFIG_FWLPS_IN_IPS) || defined(CONFIG_SWLPS_IN_IPS) || \
	defined(CONFIG_RTL8188E) || \
	(defined(CONFIG_PLATFORM_SPRD) && defined(CONFIG_RTL8188E))
static u8 ips_leave(PADAPTER a)
{
	(void)a;
	g_trace.ips_leave++;
	return _TRUE;
}
#endif

void LeaveAllPowerSaveModeDirect(PADAPTER Adapter)
{
	PADAPTER pri = Adapter->dvobj->padapters[IFACE_ID0];
	struct pwrctrl_priv *pwr = adapter_to_pwrctl(Adapter);

	if (Adapter->bSurpriseRemoved)
		return;
	if (g_mi_linked) {
		if (pwr->pwr_mode == PS_MODE_ACTIVE)
			return;
#ifdef CONFIG_LPS
		rtw_lps_ctrl_wk_cmd(pri, LPS_CTRL_LEAVE, RTW_CMDF_DIRECTLY);
#endif
	} else if (pwr->rf_pwrstate == rf_off) {
#if defined(CONFIG_FWLPS_IN_IPS) || defined(CONFIG_SWLPS_IN_IPS) || defined(CONFIG_RTL8188E)
		ips_leave(pri);
#endif
	}
}

void LeaveAllPowerSaveMode(PADAPTER Adapter)
{
	struct pwrctrl_priv *pwr = adapter_to_pwrctl(Adapter);
	u8 enqueue = 0;

	if (!Adapter->bup || Adapter->bSurpriseRemoved)
		return;
	if (g_assoc_if_num) {
#ifdef CONFIG_LPS_LCLK
		enqueue = 1;
#endif
#ifdef CONFIG_LPS
		rtw_lps_ctrl_wk_cmd(Adapter, LPS_CTRL_LEAVE,
				    enqueue ? 0 : RTW_CMDF_DIRECTLY);
#endif
	} else if (pwr->rf_pwrstate == rf_off) {
#if defined(CONFIG_FWLPS_IN_IPS) || defined(CONFIG_SWLPS_IN_IPS) || \
	(defined(CONFIG_PLATFORM_SPRD) && defined(CONFIG_RTL8188E))
		ips_leave(Adapter);
#endif
	}
}
