/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_SRESET_RESET_TYPES_H
#define HOST_SRESET_RESET_TYPES_H

#include "host_types.h"

#define WIFI_STATUS_SUCCESS 0u
#define rf_off 2

struct sreset_priv {
	u8 silent_reset_inprogress;
	u8 Wifi_Error_Status;
	u8 self_dect_fw;
	u8 rx_cnt;
};

struct hal_data_type {
	struct sreset_priv srestpriv;
};

struct pwrctrl_priv {
	int lock;
	int change_rfpwrstate;
};

struct debug_priv {
	u32 dbg_sreset_cnt;
};

struct dvobj_priv {
	struct debug_priv drv_dbg;
};

struct _adapter {
	struct dvobj_priv *dvobj;
	struct hal_data_type *HalData;
	struct pwrctrl_priv pwrctl_priv;
};

typedef struct _adapter _adapter;
typedef _adapter *PADAPTER;

#define GET_HAL_DATA(a) ((a)->HalData)
#define adapter_to_pwrctl(a) (&(a)->pwrctl_priv)
#define adapter_to_dvobj(a) ((a)->dvobj)

struct host_sreset_reset_trace {
	unsigned ps_mode_active;
	unsigned pwrlock_enter;
	unsigned pwrlock_exit;
	unsigned mi_stop;
	unsigned mi_start;
	unsigned ips_enter;
	unsigned ips_leave;
	unsigned ap_restore;
	u8 inprogress_after_stop;
};

struct host_sreset_reset_trace *host_sreset_reset_get_trace(void);
void host_sreset_reset_reset_trace(void);

void host_sreset_reset_set_ps_mode(PADAPTER padapter);
void host_sreset_reset_enter_pwrlock(PADAPTER padapter);
void host_sreset_reset_exit_pwrlock(PADAPTER padapter);
void host_sreset_reset_mi_adapter_hdl(PADAPTER padapter, u8 bstart);
void host_sreset_reset_ips_enter(PADAPTER padapter);
void host_sreset_reset_ips_leave(PADAPTER padapter);
void host_sreset_reset_ap_info_restore(PADAPTER padapter);

void sreset_reset(PADAPTER padapter);

#endif
