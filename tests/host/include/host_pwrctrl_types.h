/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal userspace types for host L2 pwrctrl tests (W3-94).
 */
#ifndef HOST_PWRCTRL_TYPES_H
#define HOST_PWRCTRL_TYPES_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0

typedef int _lock;
typedef _lock _pwrlock;
typedef u32 systime;

struct mlme_priv { u32 fw_state; };
struct xmit_priv { u16 free_xmitbuf_cnt, free_xmit_extbuf_cnt; };

struct pwrctrl_priv {
	_pwrlock lock;
	u32 ps_deny;
	u8 bpower_saving;
	systime ips_deny_time;
	u8 pwr_mode;
	u8 rf_pwrstate;
};

struct _adapter;
struct dvobj_priv { u8 iface_nums; struct _adapter *padapters[4]; };

struct _adapter {
	struct dvobj_priv *dvobj;
	struct mlme_priv mlmepriv;
	struct xmit_priv xmitpriv;
	struct pwrctrl_priv pwrctrlpriv;
	u8 bup;
	u8 bSurpriseRemoved;
};

typedef struct _adapter _adapter;
typedef _adapter *PADAPTER;

#define adapter_to_pwrctl(a) (&(a)->pwrctrlpriv)
#define adapter_to_dvobj(a) ((a)->dvobj)

typedef enum {
	PS_DENY_DRV_INITIAL = 0,
	PS_DENY_SCAN,
	PS_DENY_JOIN,
	PS_DENY_DISCONNECT,
	PS_DENY_SUSPEND,
	PS_DENY_IOCTL,
	PS_DENY_MGNT_TX,
	PS_DENY_MONITOR_MODE,
	PS_DENY_BEAMFORMING,
	PS_DENY_DRV_REMOVE = 30,
	PS_DENY_OTHERS = 31
} PS_DENY_REASON;

void _enter_pwrlock(_pwrlock *plock);
void _exit_pwrlock(_pwrlock *plock);

void rtw_ps_deny(PADAPTER padapter, PS_DENY_REASON reason);
void rtw_ps_deny_cancel(PADAPTER padapter, PS_DENY_REASON reason);
u32 rtw_ps_deny_get(PADAPTER padapter);

#endif /* HOST_PWRCTRL_TYPES_H */
