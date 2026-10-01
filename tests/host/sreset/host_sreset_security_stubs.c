// SPDX-License-Identifier: GPL-2.0
/* Trace stubs for restore_security L2 differential (W3-95 PR7+). */
#include "host_sreset_security_types.h"

static struct host_sreset_security_trace g_trace;
static u8 g_sta_present;

struct host_sreset_security_trace *host_sreset_security_get_trace(void)
{
	return &g_trace;
}

void host_sreset_security_reset_trace(void)
{
	g_trace = (struct host_sreset_security_trace){0};
}

void host_sreset_security_set_sta_present(u8 present)
{
	g_sta_present = present;
}

u8 *get_bssid(struct mlme_priv *pmlmepriv)
{
	return pmlmepriv->cur_network.network.MacAddress;
}

struct sta_info *rtw_get_stainfo(struct sta_priv *pstapriv, u8 *hwaddr)
{
	(void)hwaddr;
	if (!g_sta_present)
		return NULL;
	return &pstapriv->stub_sta;
}

void rtw_hal_set_hwreg(PADAPTER padapter, u32 variable, u8 *val)
{
	(void)padapter;
	if (variable == HW_VAR_SEC_CFG && val)
		g_trace.last_sec_cfg = *val;
}

void rtw_setstakey_cmd(PADAPTER padapter, struct sta_info *psta, int keytype,
		       u8 enqueue)
{
	(void)padapter;
	(void)psta;
	(void)keytype;
	(void)enqueue;
	g_trace.setstakey_calls++;
}

sint rtw_set_key(PADAPTER padapter, struct security_priv *psecuritypriv,
		 sint keyid, u8 set_tx, bool enqueue)
{
	(void)padapter;
	(void)set_tx;
	(void)enqueue;
	g_trace.set_key_calls++;
	(void)psecuritypriv;
	(void)keyid;
	return 0;
}
