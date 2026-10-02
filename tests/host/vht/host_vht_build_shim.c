// SPDX-License-Identifier: GPL-2.0
#include "host_vht_build_types.h"

#include <stdbool.h>

_adapter host_vht_build_adapter;
u8 host_vht_build_hal_bw_cap;

static bool hal_is_bw_support(_adapter *adapter, u8 bw)
{
	if (bw >= sizeof(adapter->host_fixture.hal_bw_support))
		return false;
	return adapter->host_fixture.hal_bw_support[bw] != 0;
}

u8 *rtw_set_ie(u8 *pbuf, int index, uint len, const u8 *source, uint *frlen)
{
	*pbuf = (u8)index;
	*(pbuf + 1) = (u8)len;
	if (len)
		_rtw_memcpy(pbuf + 2, source, len);
	if (frlen)
		*frlen += len + 2;
	return pbuf + len + 2;
}

bool hal_chk_bw_cap(_adapter *adapter, u8 cap)
{
	(void)adapter;
	return (host_vht_build_hal_bw_cap & cap) != 0;
}

u8 hal_largest_bw(_adapter *padapter, u8 in_bw)
{
	for (; in_bw > CHANNEL_WIDTH_20; in_bw--) {
		if (hal_is_bw_support(padapter, in_bw))
			break;
	}
	if (!hal_is_bw_support(padapter, in_bw))
		in_bw = CHANNEL_WIDTH_20;
	return in_bw;
}

u8 rtw_get_center_ch(u8 ch, u8 bw, u8 offset)
{
	if (bw == CHANNEL_WIDTH_80 && ch >= 36 && ch <= 48 && ch % 4 == 0)
		return 42;
	if (bw == CHANNEL_WIDTH_80 && ch == 149)
		return 155;
	if (offset == HAL_PRIME_CHNL_OFFSET_LOWER && bw != CHANNEL_WIDTH_20)
		return ch + 2;
	return ch;
}

void rtw_hal_get_def_var(_adapter *padapter, HAL_DEF_VARIABLE variable, void *value)
{
	switch (variable) {
	case HAL_DEF_RX_PACKET_OFFSET:
		*(u32 *)value = padapter->host_fixture.rx_packet_offset;
		break;
	case HAL_DEF_MAX_RECVBUF_SZ:
		*(u32 *)value = padapter->host_fixture.max_recvbuf_sz;
		break;
	case HAL_DEF_RX_STBC:
		*(u8 *)value = padapter->host_fixture.rx_stbc_nss;
		break;
	case HAL_DEF_BEAMFORMER_CAP:
		*(u8 *)value = padapter->host_fixture.beamformer_rf_num;
		break;
	case HAL_DEF_BEAMFORMEE_CAP:
		*(u8 *)value = padapter->host_fixture.beamformee_rf_num;
		break;
	default:
		break;
	}
}
