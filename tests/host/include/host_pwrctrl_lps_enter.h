/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_PWRCTRL_LPS_ENTER_H
#define HOST_PWRCTRL_LPS_ENTER_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define PS_MODE_ACTIVE 0
#define HW_PORT0 0
#define WIFI_UNDER_SURVEY 0x00000800
#define WIFI_UNDER_LINKING 0x00000080
#define WIFI_UNDER_WPS 0x00000100
#define WIFI_AP_STATE 0x00000010
#define WIFI_ADHOC_STATE 0x00000020
#define WIFI_ADHOC_MASTER_STATE 0x00000040

typedef u32 systime;
typedef s32 sint;

struct mlme_priv { u32 fw_state; };
struct registry_priv { u8 smart_ps; };
struct hal_data { u8 bFWReady; };
struct pwrctrl_priv {
	u8 pwr_mode, bpower_saving, bLeisurePs, LpsIdleCount, power_mgnt, bInSuspend;
	systime lps_deny_time;
};
struct dvobj_priv { u8 iface_nums; struct _adapter *padapters[4]; };
struct _adapter {
	struct dvobj_priv *dvobj;
	struct mlme_priv mlmepriv;
	struct registry_priv registrypriv;
	struct pwrctrl_priv pwrctrlpriv;
	struct hal_data HalData;
	u8 hw_port;
};
typedef struct _adapter _adapter;

struct host_pwrctrl_lps_enter_trace {
	u32 set_ps_mode_calls;
	u8 last_ps_mode;
};

void host_pwrctrl_lps_enter_set_time(systime t);
void host_pwrctrl_lps_enter_set_assoc_if_num(int n);
void host_pwrctrl_lps_enter_reset_trace(void);
struct host_pwrctrl_lps_enter_trace *host_pwrctrl_lps_enter_get_trace(void);
void LPS_Enter(_adapter *a, const char *msg);
void LPS_Leave(_adapter *a, const char *msg);

#endif
