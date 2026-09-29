/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_CMD_JOINBSS_TYPES_H
#define HOST_CMD_JOINBSS_TYPES_H

#include "host_types.h"

#define _SUCCESS 1
#define _FAIL 0
#define _TRUE 1
#define CMD_JOINBSS 0
#define Ndis802_11IBSS 0
#define Ndis802_11Infrastructure 1
#define WIFI_STATION_STATE 0x08
#define WIFI_ADHOC_STATE 0x20
#define ETH_ALEN 6
#define MAX_IE_SZ 768
typedef int sint;
typedef int NDIS_802_11_NETWORK_INFRASTRUCTURE;

typedef struct {
	u32 SsidLength;
	u8 Ssid[32];
} NDIS_802_11_SSID;

typedef struct {
	u32 DSConfig;
} NDIS_802_11_CONFIGURATION;

typedef struct {
	u8 MacAddress[ETH_ALEN];
	NDIS_802_11_SSID Ssid;
	NDIS_802_11_CONFIGURATION Configuration;
	NDIS_802_11_NETWORK_INFRASTRUCTURE InfrastructureMode;
	u32 IELength;
	u8 IEs[MAX_IE_SZ];
} WLAN_BSSID_EX;

struct wlan_network { WLAN_BSSID_EX network; };

struct mlme_priv {
	u32 fw_state;
	u8 assoc_by_bssid;
	u8 assoc_bssid[ETH_ALEN];
};

struct security_priv { u8 authenticator_ie[256]; };
struct registry_priv { u8 wmm_enable; };
struct cmd_obj { u16 cmdcode; u32 cmdsz; u8 *parmbuf; };
struct cmd_priv { int pad; };

struct _adapter {
	struct mlme_priv mlmepriv;
	struct security_priv securitypriv;
	struct registry_priv registrypriv;
	struct cmd_priv cmdpriv;
};

struct host_joinbss_trace {
	int enqueue_ok, cmd_code;
};

void host_joinbss_reset(void);
struct host_joinbss_trace *host_joinbss_get_trace(void);
void host_joinbss_set_malloc_fail(int n);
u8 rtw_joinbss_cmd(struct _adapter *a, struct wlan_network *n);

#endif
