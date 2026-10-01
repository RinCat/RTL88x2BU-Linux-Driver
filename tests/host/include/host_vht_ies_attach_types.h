/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_VHT_IES_ATTACH_TYPES_H
#define HOST_VHT_IES_ATTACH_TYPES_H

#include "host_vht_build_types.h"

#define HOST_VHT_IES_ATTACH_MAX_IE_SZ 256

typedef struct _NDIS_802_11_CONFIGURATION {
	u32 Length;
	u32 BeaconPeriod;
	u32 ATIMWindow;
	u32 DSConfig;
} NDIS_802_11_CONFIGURATION;

typedef struct _WLAN_BSSID_EX {
	u32 Length;
	u8 MacAddress[6];
	u8 Reserved[2];
	u32 IELength;
	NDIS_802_11_CONFIGURATION Configuration;
	u8 IEs[HOST_VHT_IES_ATTACH_MAX_IE_SZ];
} WLAN_BSSID_EX;

void rtw_vht_ies_attach(_adapter *padapter, WLAN_BSSID_EX *pnetwork);

#endif /* HOST_VHT_IES_ATTACH_TYPES_H */
