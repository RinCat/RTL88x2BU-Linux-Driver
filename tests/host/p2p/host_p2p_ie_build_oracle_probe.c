// SPDX-License-Identifier: GPL-2.0
#include <string.h>
#include "host_p2p_ie_build.h"

static u32 go_add_group_info_attr(struct wifidirect_info *pwdinfo, u8 *pbuf)
{
	(void)pwdinfo;
	(void)pbuf;
	return 0;
}

static u32 append_device_info(u8 *p2pie, u32 off, struct wifidirect_info *w)
{
	p2pie[off++] = P2P_ATTR_DEVICE_INFO;
	RTW_PUT_LE16(p2pie + off, 21 + w->device_name_len);
	off += 2;
	memcpy(p2pie + off, w->device_addr, ETH_ALEN);
	off += ETH_ALEN;
	RTW_PUT_BE16(p2pie + off, w->supported_wps_cm);
	off += 2;
	RTW_PUT_BE16(p2pie + off, WPS_PDT_CID_MULIT_MEDIA);
	off += 2;
	RTW_PUT_BE32(p2pie + off, WPSOUI);
	off += 4;
	RTW_PUT_BE16(p2pie + off, WPS_PDT_SCID_MEDIA_SERVER);
	off += 2;
	p2pie[off++] = 0;
	RTW_PUT_BE16(p2pie + off, WPS_ATTR_DEVICE_NAME);
	off += 2;
	RTW_PUT_BE16(p2pie + off, w->device_name_len);
	off += 2;
	memcpy(p2pie + off, w->device_name, w->device_name_len);
	return off + w->device_name_len;
}

u32 build_probe_resp_p2p_ie(struct wifidirect_info *pwdinfo, u8 *pbuf)
{
	u8 p2pie[MAX_P2P_IE_LEN] = {0};
	u32 len = 0, off = 0;

	p2pie[off++] = 0x50;
	p2pie[off++] = 0x6F;
	p2pie[off++] = 0x9A;
	p2pie[off++] = 0x09;
	p2pie[off++] = P2P_ATTR_CAPABILITY;
	RTW_PUT_LE16(p2pie + off, 0x0002);
	off += 2;
	p2pie[off++] = DMP_P2P_DEVCAP_SUPPORT;
	if (rtw_p2p_chk_role(pwdinfo, P2P_ROLE_GO)) {
		p2pie[off] = P2P_GRPCAP_GO | P2P_GRPCAP_INTRABSS;
		if (rtw_p2p_chk_state(pwdinfo, P2P_STATE_PROVISIONING_ING))
			p2pie[off] |= P2P_GRPCAP_GROUP_FORMATION;
		off++;
	} else if (rtw_p2p_chk_role(pwdinfo, P2P_ROLE_DEVICE)) {
		p2pie[off++] = pwdinfo->persistent_supported ?
					P2P_GRPCAP_PERSISTENT_GROUP | DMP_P2P_GRPCAP_SUPPORT :
					DMP_P2P_GRPCAP_SUPPORT;
	}
	p2pie[off++] = P2P_ATTR_EX_LISTEN_TIMING;
	RTW_PUT_LE16(p2pie + off, 0x0004);
	off += 2;
	RTW_PUT_LE16(p2pie + off, 0xFFFF);
	off += 2;
	RTW_PUT_LE16(p2pie + off, 0xFFFF);
	off += 2;
	off = append_device_info(p2pie, off, pwdinfo);
	if (rtw_p2p_chk_role(pwdinfo, P2P_ROLE_GO))
		off += go_add_group_info_attr(pwdinfo, p2pie + off);
	pbuf = rtw_set_ie(pbuf, _VENDOR_SPECIFIC_IE_, off, p2pie, &len);
	return len;
}

u32 build_prov_disc_request_p2p_ie(struct wifidirect_info *pwdinfo, u8 *pbuf, u8 *pssid,
				    u8 ussidlen, u8 *pdev_raddr)
{
	u8 p2pie[MAX_P2P_IE_LEN] = {0};
	u32 len = 0, off = 0;
	u16 cm;

	p2pie[off++] = 0x50;
	p2pie[off++] = 0x6F;
	p2pie[off++] = 0x9A;
	p2pie[off++] = 0x09;
	p2pie[off++] = P2P_ATTR_CAPABILITY;
	RTW_PUT_LE16(p2pie + off, 0x0002);
	off += 2;
	p2pie[off++] = DMP_P2P_DEVCAP_SUPPORT;
	p2pie[off++] = pwdinfo->persistent_supported ?
				P2P_GRPCAP_PERSISTENT_GROUP | DMP_P2P_GRPCAP_SUPPORT :
				DMP_P2P_GRPCAP_SUPPORT;
	p2pie[off++] = P2P_ATTR_DEVICE_INFO;
	RTW_PUT_LE16(p2pie + off, 21 + pwdinfo->device_name_len);
	off += 2;
	memcpy(p2pie + off, pwdinfo->device_addr, ETH_ALEN);
	off += ETH_ALEN;
	cm = pwdinfo->ui_got_wps_info == P2P_GOT_WPSINFO_PBC ? WPS_CONFIG_METHOD_PBC :
							       WPS_CONFIG_METHOD_DISPLAY;
	RTW_PUT_BE16(p2pie + off, cm);
	off += 2;
	RTW_PUT_BE16(p2pie + off, WPS_PDT_CID_MULIT_MEDIA);
	off += 2;
	RTW_PUT_BE32(p2pie + off, WPSOUI);
	off += 4;
	RTW_PUT_BE16(p2pie + off, WPS_PDT_SCID_MEDIA_SERVER);
	off += 2;
	p2pie[off++] = 0;
	RTW_PUT_BE16(p2pie + off, WPS_ATTR_DEVICE_NAME);
	off += 2;
	RTW_PUT_BE16(p2pie + off, pwdinfo->device_name_len);
	off += 2;
	memcpy(p2pie + off, pwdinfo->device_name, pwdinfo->device_name_len);
	off += pwdinfo->device_name_len;
	if (rtw_p2p_chk_role(pwdinfo, P2P_ROLE_CLIENT)) {
		p2pie[off++] = P2P_ATTR_GROUP_ID;
		RTW_PUT_LE16(p2pie + off, ETH_ALEN + ussidlen);
		off += 2;
		memcpy(p2pie + off, pdev_raddr, ETH_ALEN);
		off += ETH_ALEN;
		memcpy(p2pie + off, pssid, ussidlen);
		off += ussidlen;
	}
	pbuf = rtw_set_ie(pbuf, _VENDOR_SPECIFIC_IE_, off, p2pie, &len);
	return len;
}
