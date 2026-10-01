// SPDX-License-Identifier: GPL-2.0
#include <string.h>
#include "host_p2p_ie_build.h"

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

u32 rtw_set_p2p_attr_content(u8 *pbuf, u8 attr_id, u16 attr_len, u8 *pdata_attr)
{
	*pbuf++ = attr_id;
	RTW_PUT_LE16(pbuf, attr_len);
	pbuf += 2;
	memcpy(pbuf, pdata_attr, attr_len);
	return attr_len + 3;
}

u32 build_beacon_p2p_ie(struct wifidirect_info *pwdinfo, u8 *pbuf)
{
	u8 p2pie[MAX_P2P_IE_LEN] = {0}, cap_le[2];
	u16 capability = 0;
	u32 len = 0, p2pielen = 0;

	p2pie[p2pielen++] = 0x50;
	p2pie[p2pielen++] = 0x6F;
	p2pie[p2pielen++] = 0x9A;
	p2pie[p2pielen++] = 0x09;
	capability = P2P_DEVCAP_INVITATION_PROC | P2P_DEVCAP_CLIENT_DISCOVERABILITY;
	capability |= (P2P_GRPCAP_GO | P2P_GRPCAP_INTRABSS) << 8;
	if (rtw_p2p_chk_state(pwdinfo, P2P_STATE_PROVISIONING_ING))
		capability |= P2P_GRPCAP_GROUP_FORMATION << 8;
	RTW_PUT_LE16(cap_le, capability);
	p2pielen += rtw_set_p2p_attr_content(&p2pie[p2pielen], P2P_ATTR_CAPABILITY, 2, cap_le);
	p2pielen += rtw_set_p2p_attr_content(&p2pie[p2pielen], P2P_ATTR_DEVICE_ID, ETH_ALEN,
					     pwdinfo->device_addr);
	pbuf = rtw_set_ie(pbuf, _VENDOR_SPECIFIC_IE_, p2pielen, p2pie, &len);
	return len;
}
