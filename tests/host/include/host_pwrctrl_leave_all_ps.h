/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_PWRCTRL_LEAVE_ALL_PS_H
#define HOST_PWRCTRL_LEAVE_ALL_PS_H

#include "host_pwrctrl_types.h"

typedef s32 sint;

#define PS_MODE_ACTIVE 0
#define LPS_CTRL_LEAVE 5
#define RTW_CMDF_DIRECTLY 1
#define IFACE_ID0 0

enum _rfstate { rf_off, rf_on };

struct host_pwrctrl_leave_all_ps_trace {
	int lps_ctrl_wk_cmd;
	u8 last_lps_ctrl_type, last_lps_ctrl_flags;
	int ips_leave;
};

void host_pwrctrl_leave_all_ps_reset_trace(void);
struct host_pwrctrl_leave_all_ps_trace *host_pwrctrl_leave_all_ps_get_trace(void);
void host_pwrctrl_leave_all_ps_set_assoc_if_num(int n);
void host_pwrctrl_leave_all_ps_set_mi_linked(u8 linked);
void LeaveAllPowerSaveMode(PADAPTER Adapter);
void LeaveAllPowerSaveModeDirect(PADAPTER Adapter);

#endif
