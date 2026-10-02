/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_P2P_WFD_BUILD_H
#define HOST_P2P_WFD_BUILD_H
#include "host_types.h"
#define _TRUE 1
#define _FALSE 0
#define ETH_ALEN 6
#define MAX_WFD_IE_LEN 128
#define _VENDOR_SPECIFIC_IE_ 221
#define P2P_ROLE_CLIENT 2
#define P2P_ROLE_GO 3
#define WIFI_ASOC_STATE 0x00000001U
#define WL_FUNC_MIRACAST 2U
#define WFD_ATTR_DEVICE_INFO 0x00
#define WFD_ATTR_ASSOC_BSSID 0x01
#define WFD_ATTR_COUPLED_SINK_INFO 0x06
#define WFD_DEVINFO_SESSION_AVAIL 0x0010
#define WFD_DEVINFO_WSD 0x0040
#define WFD_DEVINFO_PC_TDLS 0x0080
#define RTW_PUT_BE16(a, val) \
	do { \
		(a)[0] = (u8)(((u16)(val) >> 8) & 0xff); \
		(a)[1] = (u8)((u16)(val) & 0xff); \
	} while (0)
struct wifi_display_info { u16 rtsp_ctrlport; u8 wfd_device_type; };
struct mlme_priv { u8 assoc_bssid[ETH_ALEN]; u32 fwstate; };
struct sta_priv { int asoc_list_cnt; };
struct _adapter;
struct wifidirect_info {
	struct _adapter *padapter;
	u8 role, wfd_tdls_enable;
	struct wifi_display_info *wfd_info;
};
struct _adapter {
	struct wifidirect_info wdinfo;
	struct wifi_display_info wfd_info;
	struct mlme_priv mlmepriv;
	struct sta_priv stapriv;
	u8 miracast_enabled;
};
static inline int check_fwstate(struct mlme_priv *p, u32 s)
{
	return (p->fwstate & s) ? _TRUE : _FALSE;
}
u8 *rtw_set_ie(u8 *pbuf, int index, u32 len, const u8 *source, u32 *frlen);
u32 build_beacon_wfd_ie(struct wifidirect_info *pwdinfo, u8 *pbuf);
#ifdef HOST_P2P_WFD_PROBE
u32 build_probe_req_wfd_ie(struct wifidirect_info *pwdinfo, u8 *pbuf);
#endif
#endif
