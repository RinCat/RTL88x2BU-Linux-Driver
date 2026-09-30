/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_AP_BCN_UPDATE_TYPES_H
#define HOST_AP_BCN_UPDATE_TYPES_H

#include "host_types.h"

#define _BEACON_IE_OFFSET_ 12
#define _FIXED_IE_LENGTH_ _BEACON_IE_OFFSET_
#define _ERPINFO_IE_ 42
#define _HT_ADD_INFO_IE_ 61
#define WLAN_EID_VENDOR_SPECIFIC 221
#define MAX_IE_SZ 257
#define _TRUE 1
#define _FALSE 0
#define CHANNEL_WIDTH_40 2
#define HAL_PRIME_CHNL_OFFSET_LOWER 1
#define HT_INFO_HT_PARAM_SECONDARY_CHNL_ABOVE 1
#define HT_INFO_HT_PARAM_SECONDARY_CHNL_BELOW 3

#define RTW_ERP_INFO_NON_ERP_PRESENT (1U << 0)
#define RTW_ERP_INFO_USE_PROTECTION (1U << 1)
#define RTW_ERP_INFO_BARKER_PREAMBLE_MODE (1U << 2)

typedef int sint;
typedef struct _NDIS_802_11_VARIABLE_IEs {
	u8 ElementID;
	u8 Length;
	u8 data[1];
} NDIS_802_11_VARIABLE_IEs, *PNDIS_802_11_VARIABLE_IEs;

typedef struct _WLAN_BSSID_EX {
	u32 IELength;
	u8 IEs[256];
} WLAN_BSSID_EX;

struct ht_priv {
	u8 ht_option;
};

struct mlme_ext_info {
	u8 ERP_enable;
	u8 HT_info_enable;
	WLAN_BSSID_EX network;
};

struct mlme_ext_priv {
	struct mlme_ext_info mlmext_info;
	u8 cur_channel;
	u8 cur_bwmode;
	u8 cur_ch_offset;
};

struct mlme_priv {
	int num_sta_non_erp;
	int num_sta_no_short_preamble;
	u8 *wps_beacon_ie;
	struct ht_priv htpriv;
	u16 ht_op_mode;
	int num_sta_40mhz_intolerant;
	u8 ht_20mhz_width_req;
	u8 ht_intolerant_ch_reported;
	int olbc;
	u8 sw_to_20mhz;
};

typedef struct _adapter {
	struct mlme_priv mlmepriv;
	struct mlme_ext_priv mlmeextpriv;
} _adapter;

struct HT_info_element {
	u8 primary_channel;
	u8 infos[5];
	u8 MCS_rate[16];
} __attribute__((packed));

u8 *rtw_get_ie(const u8 *pbuf, sint index, sint *len, sint limit);
void ERP_IE_handler(_adapter *padapter, PNDIS_802_11_VARIABLE_IEs pIE);
void update_bcn_erpinfo_ie(_adapter *padapter);
void update_bcn_htinfo_ie(_adapter *padapter);
void update_bcn_wps_ie(_adapter *padapter);
void update_bcn_vendor_spec_ie(_adapter *padapter, u8 *oui);

extern u8 host_bcn_update_last_vendor_kind;

void *rtw_malloc(size_t sz);
void rtw_mfree(void *p, size_t sz);
u8 *rtw_get_wps_ie(const u8 *in_ie, u32 in_len, u8 *wps_ie, u32 *wps_ielen);

/* Match include/rtw_ht.h: bit fields live in infos[0] (byte after primary_channel). */
#define SET_HT_OP_ELE_2ND_CHL_OFFSET(_p, _v) \
	((_p)->infos[0] = ((_p)->infos[0] & ~0x3) | ((_v) & 0x3))
#define SET_HT_OP_ELE_STA_CHL_WIDTH(_p, _v) \
	((_p)->infos[0] = ((_p)->infos[0] & ~0x4) | (((_v) & 1) << 2))

static inline u16 cpu_to_le16(u16 x)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	return x;
#else
	return (u16)((x >> 8) | (x << 8));
#endif
}

#endif /* HOST_AP_BCN_UPDATE_TYPES_H */
