/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_TDLS_TYPES_H
#define HOST_TDLS_TYPES_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define BIT(x) (1U << (x))

#define TDLS_CH_SWITCH_ON_STATE BIT(16)
#define TDLS_PEER_AT_OFF_STATE BIT(17)
#define TDLS_PEER_SLEEP_STATE BIT(21)

int check_ap_tdls_prohibited(u8 *pframe, u8 pkt_len);
int check_ap_tdls_ch_switching_prohibited(u8 *pframe, u8 pkt_len);
u8 TDLS_check_ch_state(u32 state);

#endif /* HOST_TDLS_TYPES_H */
