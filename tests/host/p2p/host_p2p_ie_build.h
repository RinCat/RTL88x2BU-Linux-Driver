/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_P2P_IE_BUILD_H
#define HOST_P2P_IE_BUILD_H
#include "host_types.h"
#define ETH_ALEN 6
#define WPS_MAX_DEVICE_NAME_LEN 32
#define MAX_P2P_IE_LEN 256
#define _VENDOR_SPECIFIC_IE_ 221
#define P2P_ATTR_STATUS 0x00
#define P2P_ATTR_CAPABILITY 0x02
#define P2P_ATTR_DEVICE_ID 0x03
#define P2P_ATTR_EX_LISTEN_TIMING 0x08
#define P2P_ATTR_DEVICE_INFO 0x0D
#define P2P_ATTR_GROUP_ID 0x0F
#define P2P_DEVCAP_SERVICE_DISCOVERY (1 << 0)
#define P2P_DEVCAP_CLIENT_DISCOVERABILITY (1 << 1)
#define P2P_DEVCAP_CONCURRENT_OPERATION (1 << 2)
#define P2P_DEVCAP_INVITATION_PROC (1 << 5)
#define P2P_GRPCAP_GO (1 << 0)
#define P2P_GRPCAP_INTRABSS (1 << 3)
#define P2P_GRPCAP_GROUP_FORMATION (1 << 6)
#define P2P_GRPCAP_PERSISTENT_GROUP (1 << 1)
#define DMP_P2P_DEVCAP_SUPPORT \
	(P2P_DEVCAP_SERVICE_DISCOVERY | P2P_DEVCAP_CLIENT_DISCOVERABILITY | \
	 P2P_DEVCAP_CONCURRENT_OPERATION | P2P_DEVCAP_INVITATION_PROC)
#define DMP_P2P_GRPCAP_SUPPORT P2P_GRPCAP_INTRABSS
#define P2P_ROLE_DEVICE 1
#define P2P_ROLE_CLIENT 2
#define P2P_ROLE_GO 3
#define P2P_STATE_NONE 0
#define P2P_STATE_PROVISIONING_ING 13
#define P2P_GOT_WPSINFO_PBC 3
#define WPS_ATTR_DEVICE_NAME 0x1011
#define WPS_CONFIG_METHOD_PBC 0x0080
#define WPS_CONFIG_METHOD_DISPLAY 0x0008
#define WPS_PDT_CID_MULIT_MEDIA 0x0008
#define WPS_PDT_SCID_MEDIA_SERVER 0x0005
#define WPSOUI 0x0050f204
struct wifidirect_info {
	u8 role, p2p_state, device_addr[ETH_ALEN];
	u8 device_name[WPS_MAX_DEVICE_NAME_LEN];
	u16 device_name_len;
	u8 persistent_supported;
	u8 ui_got_wps_info;
	u16 supported_wps_cm;
};
#define RTW_PUT_LE16(a, val) \
	do { \
		(a)[0] = (u8)((u16)(val) & 0xff); \
		(a)[1] = (u8)(((u16)(val) >> 8) & 0xff); \
	} while (0)
#define RTW_PUT_BE16(a, val) \
	do { \
		(a)[0] = (u8)(((u16)(val) >> 8) & 0xff); \
		(a)[1] = (u8)((u16)(val) & 0xff); \
	} while (0)
#define RTW_PUT_BE32(a, val) \
	do { \
		(a)[0] = (u8)(((u32)(val) >> 24) & 0xff); \
		(a)[1] = (u8)(((u32)(val) >> 16) & 0xff); \
		(a)[2] = (u8)(((u32)(val) >> 8) & 0xff); \
		(a)[3] = (u8)((u32)(val) & 0xff); \
	} while (0)
static inline int rtw_p2p_chk_state(struct wifidirect_info *w, int s)
{
	return w->p2p_state == (u8)s;
}
static inline int rtw_p2p_chk_role(struct wifidirect_info *w, int r)
{
	return w->role == (u8)r;
}
u8 *rtw_set_ie(u8 *pbuf, int index, u32 len, const u8 *source, u32 *frlen);
u32 rtw_set_p2p_attr_content(u8 *pbuf, u8 attr_id, u16 attr_len, u8 *pdata_attr);
u32 build_beacon_p2p_ie(struct wifidirect_info *pwdinfo, u8 *pbuf);
u32 build_assoc_resp_p2p_ie(struct wifidirect_info *pwdinfo, u8 *pbuf, u8 status_code);
u32 build_deauth_p2p_ie(struct wifidirect_info *pwdinfo, u8 *pbuf);
u32 build_probe_resp_p2p_ie(struct wifidirect_info *pwdinfo, u8 *pbuf);
u32 build_prov_disc_request_p2p_ie(struct wifidirect_info *pwdinfo, u8 *pbuf, u8 *pssid,
				   u8 ussidlen, u8 *pdev_raddr);
#endif
