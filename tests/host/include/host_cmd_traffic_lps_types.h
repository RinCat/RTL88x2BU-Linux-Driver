/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_CMD_TRAFFIC_LPS_TYPES_H
#define HOST_CMD_TRAFFIC_LPS_TYPES_H
#include "host_types.h"
#define _TRUE 1
#define _FALSE 0
#define WIFI_ASOC_STATE 0x04
#define WIFI_ADHOC_STATE 0x20
#define WIFI_ADHOC_MASTER_STATE 0x10
#define LPS_CTRL_LEAVE 5
#define LPS_CTRL_ENTER 9
#define LPS_CTRL_CONNECT 2
#define LPS_CTRL_SPECIAL_PACKET 4
#define HW_VAR_H2C_FW_JOINBSSRPT 0
#define LPS_DELAY_MS 1000
typedef int sint;
struct mlme_priv { u32 fw_state; };
struct pwrctrl_priv { s8 lps_level; u8 LpsIdleCount; };
struct _adapter { struct mlme_priv mlmepriv; struct pwrctrl_priv pwrctrlpriv; };
typedef struct _adapter _adapter;
struct lps_ctrl_wk_parm { s8 lps_level; };
struct host_traffic_lps_trace {
	int lps_enter, lps_leave, hw_joinbss_rpt, set_lps_deny;
	u8 hw_joinbss_val;
};
#define adapter_to_pwrctl(a) (&(a)->pwrctrlpriv)
sint check_fwstate(struct mlme_priv *m, sint s);
void LPS_Enter(_adapter *a, const char *reason);
void LPS_Leave(_adapter *a, const char *reason);
void rtw_hal_set_hwreg(_adapter *a, int id, u8 *val);
void rtw_set_lps_deny(_adapter *a, int ms);
void host_traffic_lps_reset(void);
struct host_traffic_lps_trace *host_traffic_lps_get_trace(void);
void lps_ctrl_wk_hdl(_adapter *padapter, u8 lps_ctrl_type, u8 *buf);
#endif
