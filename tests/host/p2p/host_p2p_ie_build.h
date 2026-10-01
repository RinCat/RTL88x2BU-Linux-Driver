/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_P2P_IE_BUILD_H
#define HOST_P2P_IE_BUILD_H
#include "host_types.h"
#define ETH_ALEN 6
#define MAX_P2P_IE_LEN 256
#define _VENDOR_SPECIFIC_IE_ 221
#define P2P_ATTR_CAPABILITY 0x02
#define P2P_ATTR_DEVICE_ID 0x03
#define P2P_DEVCAP_INVITATION_PROC (1 << 5)
#define P2P_DEVCAP_CLIENT_DISCOVERABILITY (1 << 1)
#define P2P_GRPCAP_GO (1 << 0)
#define P2P_GRPCAP_INTRABSS (1 << 3)
#define P2P_GRPCAP_GROUP_FORMATION (1 << 6)
#define P2P_STATE_PROVISIONING_ING 13
struct wifidirect_info {
	u8 role, p2p_state, device_addr[ETH_ALEN];
};
#define RTW_PUT_LE16(a, val) \
	do { \
		(a)[0] = (u8)((u16)(val) & 0xff); \
		(a)[1] = (u8)(((u16)(val) >> 8) & 0xff); \
	} while (0)
static inline int rtw_p2p_chk_state(struct wifidirect_info *w, int s)
{
	return w->p2p_state == (u8)s;
}
u32 build_beacon_p2p_ie(struct wifidirect_info *pwdinfo, u8 *pbuf);
#endif
