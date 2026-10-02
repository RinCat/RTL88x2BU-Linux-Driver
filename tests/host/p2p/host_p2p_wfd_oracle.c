// SPDX-License-Identifier: GPL-2.0
#include <string.h>
#include "host_p2p_wfd_build.h"

static int hal_chk_wl_func(struct _adapter *a, u32 f)
{
	(void)f;
	return a->miracast_enabled ? _TRUE : _FALSE;
}

static int is_any_client_associated(struct _adapter *a)
{
	return a->stapriv.asoc_list_cnt ? _TRUE : _FALSE;
}

u8 *rtw_set_ie(u8 *pbuf, int index, u32 len, const u8 *source, u32 *frlen)
{
	*pbuf = (u8)index;
	*(pbuf + 1) = (u8)len;
	if (len)
		memcpy(pbuf + 2, source, len);
	if (frlen)
		*frlen += len + 2;
	return pbuf + len + 2;
}

u32 build_beacon_wfd_ie(struct wifidirect_info *pwdinfo, u8 *pbuf)
{
	u8 wfdie[MAX_WFD_IE_LEN] = {0};
	u16 val16 = 0;
	u32 len = 0, wfdielen = 0;
	struct _adapter *padapter = pwdinfo->padapter;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct wifi_display_info *pwfd_info = padapter->wdinfo.wfd_info;

	if (!hal_chk_wl_func(padapter, WL_FUNC_MIRACAST))
		return 0;

	wfdie[wfdielen++] = 0x50;
	wfdie[wfdielen++] = 0x6F;
	wfdie[wfdielen++] = 0x9A;
	wfdie[wfdielen++] = 0x0A;
	wfdie[wfdielen++] = WFD_ATTR_DEVICE_INFO;
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	if (P2P_ROLE_GO == pwdinfo->role) {
		if (is_any_client_associated(padapter))
			val16 = pwfd_info->wfd_device_type | WFD_DEVINFO_WSD;
		else
			val16 = pwfd_info->wfd_device_type | WFD_DEVINFO_SESSION_AVAIL | WFD_DEVINFO_WSD;
	} else {
		val16 = pwfd_info->wfd_device_type | WFD_DEVINFO_SESSION_AVAIL | WFD_DEVINFO_WSD;
	}
	RTW_PUT_BE16(wfdie + wfdielen, val16);
	wfdielen += 2;
	RTW_PUT_BE16(wfdie + wfdielen, pwfd_info->rtsp_ctrlport);
	wfdielen += 2;
	RTW_PUT_BE16(wfdie + wfdielen, 300);
	wfdielen += 2;

	wfdie[wfdielen++] = WFD_ATTR_ASSOC_BSSID;
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;
	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)
		memcpy(wfdie + wfdielen, pmlmepriv->assoc_bssid, ETH_ALEN);
	else
		memset(wfdie + wfdielen, 0, ETH_ALEN);
	wfdielen += ETH_ALEN;

	wfdie[wfdielen++] = WFD_ATTR_COUPLED_SINK_INFO;
	RTW_PUT_BE16(wfdie + wfdielen, 0x0007);
	wfdielen += 2;
	memset(wfdie + wfdielen, 0, 7);
	wfdielen += 7;

	rtw_set_ie(pbuf, _VENDOR_SPECIFIC_IE_, wfdielen, wfdie, &len);
	return len;
}

#ifdef HOST_P2P_WFD_PROBE
u32 build_probe_req_wfd_ie(struct wifidirect_info *pwdinfo, u8 *pbuf)
{
	u8 wfdie[MAX_WFD_IE_LEN] = {0};
	u16 val16 = 0;
	u32 len = 0, wfdielen = 0;
	struct _adapter *padapter = pwdinfo->padapter;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct wifi_display_info *pwfd_info = padapter->wdinfo.wfd_info;

	if (!hal_chk_wl_func(padapter, WL_FUNC_MIRACAST))
		return 0;

	wfdie[wfdielen++] = 0x50;
	wfdie[wfdielen++] = 0x6F;
	wfdie[wfdielen++] = 0x9A;
	wfdie[wfdielen++] = 0x0A;
	wfdie[wfdielen++] = WFD_ATTR_DEVICE_INFO;
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;

	if (pwdinfo->wfd_tdls_enable == 1)
		val16 = pwfd_info->wfd_device_type | WFD_DEVINFO_SESSION_AVAIL | WFD_DEVINFO_WSD |
			WFD_DEVINFO_PC_TDLS;
	else
		val16 = pwfd_info->wfd_device_type | WFD_DEVINFO_SESSION_AVAIL | WFD_DEVINFO_WSD;
	RTW_PUT_BE16(wfdie + wfdielen, val16);
	wfdielen += 2;
	RTW_PUT_BE16(wfdie + wfdielen, pwfd_info->rtsp_ctrlport);
	wfdielen += 2;
	RTW_PUT_BE16(wfdie + wfdielen, 300);
	wfdielen += 2;

	wfdie[wfdielen++] = WFD_ATTR_ASSOC_BSSID;
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;
	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)
		memcpy(wfdie + wfdielen, pmlmepriv->assoc_bssid, ETH_ALEN);
	else
		memset(wfdie + wfdielen, 0, ETH_ALEN);
	wfdielen += ETH_ALEN;

	wfdie[wfdielen++] = WFD_ATTR_COUPLED_SINK_INFO;
	RTW_PUT_BE16(wfdie + wfdielen, 0x0007);
	wfdielen += 2;
	memset(wfdie + wfdielen, 0, 7);
	wfdielen += 7;

	rtw_set_ie(pbuf, _VENDOR_SPECIFIC_IE_, wfdielen, wfdie, &len);
	return len;
}
#endif

#ifdef HOST_P2P_WFD_PROBE_ASSOC
static u16 probe_resp_devinfo(struct wifidirect_info *pwdinfo, struct wifi_display_info *wfd,
			      struct _adapter *a)
{
	u16 v16;

	if (pwdinfo->session_available) {
		if (P2P_ROLE_GO == pwdinfo->role) {
			if (is_any_client_associated(a)) {
				if (pwdinfo->wfd_tdls_enable)
					v16 = wfd->wfd_device_type | WFD_DEVINFO_WSD |
					      WFD_DEVINFO_PC_TDLS | WFD_DEVINFO_HDCP_SUPPORT;
				else
					v16 = wfd->wfd_device_type | WFD_DEVINFO_WSD |
					      WFD_DEVINFO_HDCP_SUPPORT;
			} else if (pwdinfo->wfd_tdls_enable) {
				v16 = wfd->wfd_device_type | WFD_DEVINFO_SESSION_AVAIL |
				      WFD_DEVINFO_WSD | WFD_DEVINFO_PC_TDLS | WFD_DEVINFO_HDCP_SUPPORT;
			} else {
				v16 = wfd->wfd_device_type | WFD_DEVINFO_SESSION_AVAIL |
				      WFD_DEVINFO_WSD | WFD_DEVINFO_HDCP_SUPPORT;
			}
		} else if (pwdinfo->wfd_tdls_enable) {
			v16 = wfd->wfd_device_type | WFD_DEVINFO_SESSION_AVAIL | WFD_DEVINFO_WSD |
			      WFD_DEVINFO_PC_TDLS | WFD_DEVINFO_HDCP_SUPPORT;
		} else {
			v16 = wfd->wfd_device_type | WFD_DEVINFO_SESSION_AVAIL | WFD_DEVINFO_WSD |
			      WFD_DEVINFO_HDCP_SUPPORT;
		}
	} else if (pwdinfo->wfd_tdls_enable) {
		v16 = wfd->wfd_device_type | WFD_DEVINFO_WSD | WFD_DEVINFO_PC_TDLS |
		      WFD_DEVINFO_HDCP_SUPPORT;
	} else {
		v16 = wfd->wfd_device_type | WFD_DEVINFO_WSD | WFD_DEVINFO_HDCP_SUPPORT;
	}
	return v16;
}

static u32 append_tail_attrs(struct mlme_priv *m, u8 *wfdie, u32 wfdielen, u8 go_session)
{
	wfdie[wfdielen++] = WFD_ATTR_ASSOC_BSSID;
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;
	if (check_fwstate(m, WIFI_ASOC_STATE) == _TRUE)
		memcpy(wfdie + wfdielen, m->assoc_bssid, ETH_ALEN);
	else
		memset(wfdie + wfdielen, 0, ETH_ALEN);
	wfdielen += ETH_ALEN;
	wfdie[wfdielen++] = WFD_ATTR_COUPLED_SINK_INFO;
	RTW_PUT_BE16(wfdie + wfdielen, 0x0007);
	wfdielen += 2;
	memset(wfdie + wfdielen, 0, 7);
	wfdielen += 7;
	if (go_session) {
		wfdie[wfdielen++] = WFD_ATTR_SESSION_INFO;
		RTW_PUT_BE16(wfdie + wfdielen, 0x0000);
		wfdielen += 2;
	}
	return wfdielen;
}

u32 build_probe_resp_wfd_ie(struct wifidirect_info *pwdinfo, u8 *pbuf, u8 tunneled)
{
	u8 wfdie[MAX_WFD_IE_LEN] = {0};
	u32 len = 0, wfdielen = 0;
	struct _adapter *a = pwdinfo->padapter;
	struct wifi_display_info *wfd = pwdinfo->wfd_info;
	u16 v16;

	/* Host stub: no dvobj/ifaces — driver may append WFD_ATTR_ALTER_MAC when
	 * CONFIG_CONCURRENT_MODE && CONFIG_TDLS && tunneled==0; parity target for
	 * Rust PR2 when those Kconfig paths are modeled. */
	(void)tunneled;
	if (!hal_chk_wl_func(a, WL_FUNC_MIRACAST))
		return 0;
	wfdie[wfdielen++] = 0x50;
	wfdie[wfdielen++] = 0x6F;
	wfdie[wfdielen++] = 0x9A;
	wfdie[wfdielen++] = 0x0A;
	wfdie[wfdielen++] = WFD_ATTR_DEVICE_INFO;
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;
	v16 = probe_resp_devinfo(pwdinfo, wfd, a);
	RTW_PUT_BE16(wfdie + wfdielen, v16);
	wfdielen += 2;
	RTW_PUT_BE16(wfdie + wfdielen, wfd->rtsp_ctrlport);
	wfdielen += 2;
	RTW_PUT_BE16(wfdie + wfdielen, 300);
	wfdielen += 2;
	wfdielen = append_tail_attrs(&a->mlmepriv, wfdie, wfdielen,
				     pwdinfo->role == P2P_ROLE_GO ? 1 : 0);
	rtw_set_ie(pbuf, _VENDOR_SPECIFIC_IE_, wfdielen, wfdie, &len);
	return len;
}

u32 build_assoc_req_wfd_ie(struct wifidirect_info *pwdinfo, u8 *pbuf)
{
	u8 wfdie[MAX_WFD_IE_LEN] = {0};
	u16 val16;
	u32 len = 0, wfdielen = 0;
	struct _adapter *a = pwdinfo->padapter;
	struct wifi_display_info *wfd = pwdinfo->wfd_info;

	if (!hal_chk_wl_func(a, WL_FUNC_MIRACAST))
		return 0;
	if (rtw_p2p_chk_state(pwdinfo, P2P_STATE_NONE) ||
	    rtw_p2p_chk_state(pwdinfo, P2P_STATE_IDLE))
		return 0;
	wfdie[wfdielen++] = 0x50;
	wfdie[wfdielen++] = 0x6F;
	wfdie[wfdielen++] = 0x9A;
	wfdie[wfdielen++] = 0x0A;
	wfdie[wfdielen++] = WFD_ATTR_DEVICE_INFO;
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;
	val16 = wfd->wfd_device_type | WFD_DEVINFO_SESSION_AVAIL | WFD_DEVINFO_WSD;
	RTW_PUT_BE16(wfdie + wfdielen, val16);
	wfdielen += 2;
	RTW_PUT_BE16(wfdie + wfdielen, wfd->rtsp_ctrlport);
	wfdielen += 2;
	RTW_PUT_BE16(wfdie + wfdielen, 300);
	wfdielen += 2;
	wfdielen = append_tail_attrs(&a->mlmepriv, wfdie, wfdielen, 0);
	rtw_set_ie(pbuf, _VENDOR_SPECIFIC_IE_, wfdielen, wfdie, &len);
	return len;
}

u32 build_assoc_resp_wfd_ie(struct wifidirect_info *pwdinfo, u8 *pbuf)
{
	u8 wfdie[MAX_WFD_IE_LEN] = {0};
	u16 val16;
	u32 len = 0, wfdielen = 0;
	struct _adapter *a = pwdinfo->padapter;
	struct wifi_display_info *wfd = pwdinfo->wfd_info;

	if (!hal_chk_wl_func(a, WL_FUNC_MIRACAST))
		return 0;
	wfdie[wfdielen++] = 0x50;
	wfdie[wfdielen++] = 0x6F;
	wfdie[wfdielen++] = 0x9A;
	wfdie[wfdielen++] = 0x0A;
	wfdie[wfdielen++] = WFD_ATTR_DEVICE_INFO;
	RTW_PUT_BE16(wfdie + wfdielen, 0x0006);
	wfdielen += 2;
	val16 = wfd->wfd_device_type | WFD_DEVINFO_SESSION_AVAIL | WFD_DEVINFO_WSD;
	RTW_PUT_BE16(wfdie + wfdielen, val16);
	wfdielen += 2;
	RTW_PUT_BE16(wfdie + wfdielen, wfd->rtsp_ctrlport);
	wfdielen += 2;
	RTW_PUT_BE16(wfdie + wfdielen, 300);
	wfdielen += 2;
	wfdielen = append_tail_attrs(&a->mlmepriv, wfdie, wfdielen, 0);
	rtw_set_ie(pbuf, _VENDOR_SPECIFIC_IE_, wfdielen, wfdie, &len);
	return len;
}
#endif
