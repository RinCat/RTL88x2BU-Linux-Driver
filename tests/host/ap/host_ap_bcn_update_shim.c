// SPDX-License-Identifier: GPL-2.0
/* Host shims for tests/host/ap L2 harness (rtw_get_ie + observability). */
#include <stdlib.h>
#include <string.h>
#include "host_ap_bcn_update_types.h"

u8 host_bcn_update_last_erp_byte;
u16 host_bcn_update_last_ht_op_mode;
u8 host_bcn_update_last_ht_info_byte;
u32 host_bcn_update_last_ielen;
u8 host_bcn_update_last_vendor_kind;

u8 *rtw_get_ie(const u8 *pbuf, sint index, sint *len, sint limit)
{
	sint tmp;

	if (limit < 1)
		return NULL;
	for (tmp = 0; tmp < limit;) {
		u8 id = pbuf[tmp];

		if (id == index) {
			*len = pbuf[tmp + 1];
			return (u8 *)(pbuf + tmp);
		}
		tmp += pbuf[tmp + 1] + 2;
		if (tmp >= limit)
			break;
	}
	*len = 0;
	return NULL;
}

void *rtw_malloc(size_t sz)
{
	return malloc(sz);
}

void rtw_mfree(void *p, size_t sz)
{
	(void)sz;
	free(p);
}

u8 *rtw_get_wps_ie(const u8 *in_ie, u32 in_len, u8 *wps_ie, u32 *wps_ielen)
{
	u32 cnt = 0;
	const u8 wps_oui[4] = {0x00, 0x50, 0xf2, 0x04};

	if (wps_ielen)
		*wps_ielen = 0;
	if (!in_ie || in_len <= 0)
		return NULL;

	while (cnt + 1 + 4 < in_len) {
		u8 eid = in_ie[cnt];

		if (eid == WLAN_EID_VENDOR_SPECIFIC &&
		    memcmp(&in_ie[cnt + 2], wps_oui, 4) == 0) {
			if (wps_ielen)
				*wps_ielen = in_ie[cnt + 1] + 2;
			if (wps_ie)
				_rtw_memcpy(wps_ie, &in_ie[cnt], in_ie[cnt + 1] + 2);
			return (u8 *)(in_ie + cnt);
		}
		cnt += in_ie[cnt + 1] + 2;
	}
	return NULL;
}
