/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_TDLS_TYPES_H
#define HOST_TDLS_TYPES_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 0

#define BIT(x) (1U << (x))
#define TDLS_STATE_NONE 0x00000000U
#define TDLS_CH_SWITCH_ON_STATE BIT(16)
#define TDLS_PEER_AT_OFF_STATE BIT(17)
#define TDLS_PEER_SLEEP_STATE BIT(21)
#define HAL_PRIME_CHNL_OFFSET_DONT_CARE 0

typedef int _lock;
typedef int ATOMIC_T;

struct registry_priv {
	u8 en_tdls;
	u8 wifi_spec;
};

struct mlme_priv {
	u8 _pad;
};

struct tdls_ch_switch {
	u32 ch_sw_state;
	ATOMIC_T chsw_on;
	u8 off_ch_num;
	u8 ch_offset;
	u32 cur_time;
	u8 delay_switch_back;
	u8 dump_stack;
};

struct tdls_info {
	u8 ap_prohibited;
	u8 ch_switch_prohibited;
	u8 link_established;
	u8 sta_cnt;
	u8 sta_maximum;
#ifdef CONFIG_TDLS_CH_SW
	struct tdls_ch_switch chsw_info;
#endif
	u8 ch_sensing;
	u8 watchdog_count;
	u8 dev_discovered;
	u8 driver_setup;
	_lock cmd_lock;
	_lock hdl_lock;
	void *tdls_sctx;
};

struct _adapter {
	struct registry_priv registrypriv;
	struct mlme_priv mlmepriv;
	struct tdls_info tdlsinfo;
};

typedef struct _adapter _adapter;
typedef _adapter *PADAPTER;

static inline void _rtw_spinlock_init(_lock *lock)
{
	(void)lock;
}

static inline void _rtw_spinlock_free(_lock *lock)
{
	(void)lock;
}

#define ATOMIC_SET(a, v) ((a) = (v))

int check_ap_tdls_prohibited(u8 *pframe, u8 pkt_len);
int check_ap_tdls_ch_switching_prohibited(u8 *pframe, u8 pkt_len);
u8 TDLS_check_ch_state(u32 state);
void rtw_reset_tdls_info(PADAPTER padapter);
int rtw_init_tdls_info(PADAPTER padapter);
void rtw_free_tdls_info(struct tdls_info *ptdlsinfo);
u8 rtw_is_tdls_enabled(PADAPTER padapter);
void rtw_set_tdls_enable(PADAPTER padapter, u8 enable);

#endif /* HOST_TDLS_TYPES_H */
