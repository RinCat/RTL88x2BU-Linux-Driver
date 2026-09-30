/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_SRESET_ADAPTER_TYPES_H
#define HOST_SRESET_ADAPTER_TYPES_H

#include "host_types.h"

#define WIFI_ASOC_STATE 0x00000001u
#define WIFI_UNDER_SURVEY 0x00000800u
#define WIFI_UNDER_LINKING 0x00000080u

typedef u32 systime;
typedef s32 sint;

struct host_sreset_adapter_trace {
	unsigned netif_stop, netif_wake, cancel_timers, tasklet_kill;
	unsigned tasklet_schedule, scan_abort, set_to_roam, join_timeout;
	unsigned restore_network;
	unsigned dynamic_chk_timer_ms;
};

struct mlme_priv { u32 fw_state; };
struct xmit_tasklet { int dummy; };
struct xmit_priv { struct xmit_tasklet xmit_tasklet; };
struct timer_list { u32 ms; };
struct dvobj_priv { struct timer_list dynamic_chk_timer; };
struct net_device { int dummy; };

#ifdef CONFIG_CONCURRENT_MODE
enum host_adapter_type {
	HOST_PRIMARY_ADAPTER,
	HOST_VIRTUAL_ADAPTER,
};
#define PRIMARY_ADAPTER HOST_PRIMARY_ADAPTER
#define VIRTUAL_ADAPTER HOST_VIRTUAL_ADAPTER
#define is_primary_adapter(adapter) ((adapter)->adapter_type == PRIMARY_ADAPTER)
#else
#define is_primary_adapter(adapter) (1)
#endif

struct _adapter {
	struct dvobj_priv dvobj;
	struct mlme_priv mlmepriv;
	struct xmit_priv xmitpriv;
	struct net_device *pnetdev;
#ifdef CONFIG_CONCURRENT_MODE
	u8 adapter_type;
#endif
};
typedef struct _adapter _adapter;
typedef _adapter *PADAPTER;
#define adapter_to_dvobj(a) (&(a)->dvobj)

struct host_sreset_adapter_trace *host_sreset_adapter_get_trace(void);
void host_sreset_adapter_reset_trace(void);
void sreset_stop_adapter(PADAPTER padapter);
void sreset_start_adapter(PADAPTER padapter);

#endif
