// SPDX-License-Identifier: GPL-2.0
/* Host C oracle — W3-102 PR1 prohibited + ch_state helpers. */

#include "host_tdls_types.h"

int check_ap_tdls_prohibited(u8 *pframe, u8 pkt_len)
{
	u8 tdls_prohibited_bit = 0x40;

	if (pkt_len < 5)
		return _FALSE;
	pframe += 4;
	if ((*pframe) & tdls_prohibited_bit)
		return _TRUE;
	return _FALSE;
}

int check_ap_tdls_ch_switching_prohibited(u8 *pframe, u8 pkt_len)
{
	u8 tdls_ch_swithcing_prohibited_bit = 0x80;

	if (pkt_len < 5)
		return _FALSE;
	pframe += 4;
	if ((*pframe) & tdls_ch_swithcing_prohibited_bit)
		return _TRUE;
	return _FALSE;
}

u8 TDLS_check_ch_state(u32 state)
{
	if (state & TDLS_CH_SWITCH_ON_STATE && state & TDLS_PEER_AT_OFF_STATE) {
		if (state & TDLS_PEER_SLEEP_STATE)
			return 2;
		return 1;
	}
	return 0;
}
