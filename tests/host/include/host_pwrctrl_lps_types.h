/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_PWRCTRL_LPS_TYPES_H
#define HOST_PWRCTRL_LPS_TYPES_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define NR_XMITBUFF 4
#define NR_XMIT_EXTBUFF 32
#define WIFI_ASOC_STATE 0x00000001
#define WIFI_UNDER_SURVEY 0x00000800
#define WIFI_UNDER_LINKING 0x00000080
#define WIFI_UNDER_WPS 0x00000100
#define WIFI_AP_STATE 0x00000010
#define WIFI_ADHOC_MASTER_STATE 0x00000020
#define WIFI_ADHOC_STATE 0x00000040

typedef u32 systime;
typedef s32 sint;

struct mlme_priv { u32 fw_state; };
struct xmit_priv { u16 free_xmitbuf_cnt, free_xmit_extbuf_cnt; };
struct pwrctrl_priv { u8 bpower_saving; systime ips_deny_time; };
struct _adapter;
struct dvobj_priv { u8 iface_nums; struct _adapter *padapters[4]; };
struct _adapter {
	struct dvobj_priv *dvobj;
	struct mlme_priv mlmepriv;
	struct xmit_priv xmitpriv;
	struct pwrctrl_priv pwrctrlpriv;
};

typedef struct _adapter _adapter;

#define adapter_to_pwrctl(a) (&(a)->pwrctrlpriv)
#define adapter_to_dvobj(a) ((a)->dvobj)
#define MLME_IS_AP(a) (((a)->mlmepriv.fw_state & WIFI_AP_STATE) != 0)
#define MLME_IS_MESH(a) _FALSE

sint check_fwstate(struct mlme_priv *m, sint s);
u8 rtw_is_adapter_up(_adapter *iface);
void host_pwrctrl_lps_set_time(systime t);
u8 rtw_pwr_unassociated_idle(_adapter *adapter);

#endif
