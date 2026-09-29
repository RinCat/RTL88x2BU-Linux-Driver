// SPDX-License-Identifier: GPL-2.0
/*
 * Host oracle for rtw_pwr_unassociated_idle — sync with core/rtw_pwrctrl.c.
 * Omitted vs kernel: CONFIG_IOCTL_CFG80211 remain-on-channel / recent ROCH,
 * CONFIG_P2P listen/idle, and MP_DRIVER registrypriv.mp_mode early exits.
 */
#include "host_pwrctrl_lps_types.h"

static systime g_current_time;

void host_pwrctrl_lps_set_time(systime t) { g_current_time = t; }
systime rtw_get_current_time(void) { return g_current_time; }
int rtw_time_after(systime a, systime b) { return (s32)(a - b) > 0; }

sint check_fwstate(struct mlme_priv *m, sint s)
{
	if (!s && !m->fw_state)
		return _TRUE;
	return (m->fw_state & (u32)s) ? _TRUE : _FALSE;
}

u8 rtw_is_adapter_up(_adapter *iface)
{
	(void)iface;
	return _TRUE;
}

u8 rtw_pwr_unassociated_idle(_adapter *adapter)
{
	u8 i, ret = _FALSE;
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct xmit_priv *px = &adapter->xmitpriv;
	struct pwrctrl_priv *pwr = adapter_to_pwrctl(adapter);

	if (pwr->bpower_saving == _TRUE || rtw_time_after(pwr->ips_deny_time, rtw_get_current_time()))
		goto exit;

	for (i = 0; i < dvobj->iface_nums; i++) {
		_adapter *iface = dvobj->padapters[i];
		struct mlme_priv *mlme;

		if (!iface || !rtw_is_adapter_up(iface))
			continue;
		mlme = &iface->mlmepriv;
		if (check_fwstate(mlme, WIFI_ASOC_STATE | WIFI_UNDER_SURVEY)
		    || check_fwstate(mlme, WIFI_UNDER_LINKING | WIFI_UNDER_WPS)
		    || MLME_IS_AP(iface) || MLME_IS_MESH(iface)
		    || check_fwstate(mlme, WIFI_ADHOC_MASTER_STATE | WIFI_ADHOC_STATE))
			goto exit;
	}

	if (px->free_xmitbuf_cnt != NR_XMITBUFF || px->free_xmit_extbuf_cnt != NR_XMIT_EXTBUFF)
		goto exit;
	ret = _TRUE;
exit:
	return ret;
}
