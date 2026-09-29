// SPDX-License-Identifier: GPL-2.0
#include <stdlib.h>
#include <string.h>
#include "host_cmd_joinbss_types.h"

static struct host_joinbss_trace g_trace;
static int g_malloc_fail;

void host_joinbss_reset(void) { g_malloc_fail = 0; g_trace = (struct host_joinbss_trace){0}; }
struct host_joinbss_trace *host_joinbss_get_trace(void) { return &g_trace; }
void host_joinbss_set_malloc_fail(int n) { g_malloc_fail = n; }

static void *zmalloc(u32 sz)
{
	if (g_malloc_fail-- > 0)
		return NULL;
	return calloc(1, sz);
}

sint check_fwstate(struct mlme_priv *m, sint s)
{
	return (!s && !m->fw_state) || (m->fw_state & (u32)s) ? _TRUE : 0;
}

void set_fwstate(struct mlme_priv *m, sint s) { m->fw_state |= (u32)s; }

u8 rtw_joinbss_cmd(struct _adapter *padapter, struct wlan_network *pnetwork)
{
	WLAN_BSSID_EX *psecnetwork;
	struct cmd_obj *pcmd;
	u8 res = _SUCCESS;
	NDIS_802_11_NETWORK_INFRASTRUCTURE ndis_mode = pnetwork->network.InfrastructureMode;

	pcmd = zmalloc(sizeof(*pcmd));
	if (!pcmd)
		return _FAIL;
	if (check_fwstate(&padapter->mlmepriv, WIFI_STATION_STATE | WIFI_ADHOC_STATE) != _TRUE) {
		if (ndis_mode == Ndis802_11IBSS)
			set_fwstate(&padapter->mlmepriv, WIFI_ADHOC_STATE);
		else if (ndis_mode == Ndis802_11Infrastructure)
			set_fwstate(&padapter->mlmepriv, WIFI_STATION_STATE);
	}
	psecnetwork = zmalloc(sizeof(WLAN_BSSID_EX));
	if (!psecnetwork) {
		free(pcmd);
		return _FAIL;
	}
	memcpy(psecnetwork, &pnetwork->network, sizeof(*psecnetwork));
	padapter->securitypriv.authenticator_ie[0] = (u8)psecnetwork->IELength;
	psecnetwork->IELength = 12;
	if (padapter->mlmepriv.assoc_by_bssid == 0)
		memcpy(padapter->mlmepriv.assoc_bssid, pnetwork->network.MacAddress, ETH_ALEN);
	pcmd->cmdsz = sizeof(WLAN_BSSID_EX);
	pcmd->cmdcode = CMD_JOINBSS;
	pcmd->parmbuf = (u8 *)psecnetwork;
	g_trace.enqueue_ok = 1;
	g_trace.cmd_code = pcmd->cmdcode;
	(void)res;
	return _SUCCESS;
}
