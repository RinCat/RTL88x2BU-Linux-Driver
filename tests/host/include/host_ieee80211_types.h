/* SPDX-License-Identifier: GPL-2.0 */
/*
 * IEEE 802.11 types/constants for host ieee80211_rest L2 tests (W3-26).
 */
#ifndef HOST_IEEE80211_TYPES_H
#define HOST_IEEE80211_TYPES_H

#include "host_types.h"
#include "host_wlan_util_types.h"

#include <stdbool.h>
#include <stddef.h>

#define _TRUE 1
#define _FALSE 0
#define _FAIL 0
#define _SUCCESS 1

#define ETH_ALEN 6
#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_ARG(x) \
	((u8 *)(x))[0], ((u8 *)(x))[1], ((u8 *)(x))[2], ((u8 *)(x))[3], \
	((u8 *)(x))[4], ((u8 *)(x))[5]

#define RTW_ERR(...) do { } while (0)
#define RTW_INFO(...) do { } while (0)
#define RTW_DBG(...) do { } while (0)
#define RTW_DBGDUMP ((void *)0)

static inline void dump_ies(void *sel, const u8 *ies, unsigned int len)
{
	(void)sel;
	(void)ies;
	(void)len;
}

#define rtw_warn_on(cond) ((void)(cond))

#define BIT(n) (1U << (n))
#define BIT0 BIT(0)
#define BIT1 BIT(1)
#define BIT2 BIT(2)
#define BIT3 BIT(3)
#define BIT4 BIT(4)
#define BIT5 BIT(5)
#define BIT6 BIT(6)

#define WIRELESS_INVALID 0
#define WIRELESS_11B BIT0
#define WIRELESS_11G BIT1
#define WIRELESS_11A BIT2
#define WIRELESS_11_24N BIT3
#define WIRELESS_11_5N BIT4
#define WIRELESS_11AC BIT6
#define WIRELESS_11BG (WIRELESS_11B | WIRELESS_11G)
#define WIRELESS_11G_24N (WIRELESS_11G | WIRELESS_11_24N)
#define WIRELESS_11BG_24N (WIRELESS_11B | WIRELESS_11G | WIRELESS_11_24N)
#define WIRELESS_11_5AC (WIRELESS_11A | WIRELESS_11AC)
#define WIRELESS_11A_5N (WIRELESS_11A | WIRELESS_11_5N)

#define NDIS_802_11_LENGTH_RATES_EX 16
#define IEEE80211_CCK_RATE_LEN 4
#define IEEE80211_NUM_OFDM_RATESLEN 8

#define _BEACON_IE_OFFSET_ 12
#define _SUPPORTEDRATES_IE_ 1
#define _EXT_SUPPORTEDRATES_IE_ 50

typedef enum _RATE_SECTION {
	CCK = 0,
	OFDM = 1,
} RATE_SECTION;

typedef unsigned char NDIS_802_11_MAC_ADDRESS[6];
typedef long NDIS_802_11_RSSI;
typedef unsigned char NDIS_802_11_RATES_EX[NDIS_802_11_LENGTH_RATES_EX];

typedef struct _NDIS_802_11_SSID {
	u32 SsidLength;
	u8 Ssid[32];
} NDIS_802_11_SSID;

typedef struct _NDIS_802_11_CONFIGURATION {
	u32 Length;
	u32 BeaconPeriod;
	u32 ATIMWindow;
	u32 DSConfig;
} NDIS_802_11_CONFIGURATION;

typedef enum _NDIS_802_11_NETWORK_INFRASTRUCTURE {
	Ndis802_11IBSS,
	Ndis802_11Infrastructure,
} NDIS_802_11_NETWORK_INFRASTRUCTURE;

typedef struct _WLAN_PHY_INFO {
	u8 SignalStrength;
	u8 SignalQuality;
	u8 Optimum_antenna;
} WLAN_PHY_INFO;

#define HOST_IEEE80211_MAX_IE_SZ 256

typedef struct _WLAN_BSSID_EX {
	u32 Length;
	NDIS_802_11_MAC_ADDRESS MacAddress;
	u8 Reserved[2];
	NDIS_802_11_SSID Ssid;
	NDIS_802_11_SSID mesh_id;
	u32 Privacy;
	NDIS_802_11_RSSI Rssi;
	NDIS_802_11_CONFIGURATION Configuration;
	NDIS_802_11_NETWORK_INFRASTRUCTURE InfrastructureMode;
	NDIS_802_11_RATES_EX SupportedRates;
	WLAN_PHY_INFO PhyInfo;
	u32 IELength;
	u8 IEs[HOST_IEEE80211_MAX_IE_SZ];
} WLAN_BSSID_EX;

typedef int sint;

#define WPA_CIPHER_NONE BIT(0)
#define WPA_CIPHER_WEP40 BIT(1)
#define WPA_CIPHER_WEP104 BIT(2)
#define WPA_CIPHER_TKIP BIT(3)
#define WPA_CIPHER_CCMP BIT(4)
#define WPA_CIPHER_GCMP BIT(5)
#define WPA_CIPHER_GCMP_256 BIT(6)
#define WPA_CIPHER_CCMP_256 BIT(7)
#define WPA_CIPHER_BIP_CMAC_128 BIT(8)
#define WPA_CIPHER_BIP_GMAC_128 BIT(9)
#define WPA_CIPHER_BIP_GMAC_256 BIT(10)
#define WPA_CIPHER_BIP_CMAC_256 BIT(11)

#define WPA_SELECTOR_LEN 4
#define RSN_SELECTOR_LEN 4

#define WLAN_AKM_TYPE_8021X BIT(0)
#define WLAN_AKM_TYPE_PSK BIT(1)
#define WLAN_AKM_TYPE_FT_8021X BIT(2)
#define WLAN_AKM_TYPE_FT_PSK BIT(3)
#define WLAN_AKM_TYPE_8021X_SHA256 BIT(4)
#define WLAN_AKM_TYPE_PSK_SHA256 BIT(5)
#define WLAN_AKM_TYPE_TDLS BIT(6)
#define WLAN_AKM_TYPE_SAE BIT(7)
#define WLAN_AKM_TYPE_FT_OVER_SAE BIT(8)
#define WLAN_AKM_TYPE_8021X_SUITE_B BIT(9)
#define WLAN_AKM_TYPE_8021X_SUITE_B_192 BIT(10)
#define WLAN_AKM_TYPE_FILS_SHA256 BIT(11)
#define WLAN_AKM_TYPE_FILS_SHA384 BIT(12)
#define WLAN_AKM_TYPE_FT_FILS_SHA256 BIT(13)
#define WLAN_AKM_TYPE_FT_FILS_SHA384 BIT(14)

#define _WPA_IE_ID_ 0xdd
#define _WPA2_IE_ID_ 0x30
#define _WAPI_IE_ 68
#define _TIMESTAMP_ 8
#define _BEACON_ITERVAL_ 2
#define _CAPABILITY_ 2
#define WLAN_EID_RSN 48

#define LE_BITS_TO_2BYTE(__pStart, __BitOffset, __BitLen) \
	(((((u16)(__pStart)[1] << 8) | (u16)(__pStart)[0]) >> (__BitOffset)) & \
	 ((1U << (__BitLen)) - 1))

#define GET_RSN_CAP_MFP_OPTION(cap) LE_BITS_TO_2BYTE(((u8 *)(cap)), 6, 2)
#define GET_RSN_CAP_SPP_OPT(cap) LE_BITS_TO_2BYTE(((u8 *)(cap)), 10, 2)

#define SET_BITS_TO_LE_2BYTE(__pStart, __BitOffset, __BitLen, __Value) \
	do { \
		u16 __v = (u16)(__pStart)[0] | ((u16)(__pStart)[1] << 8); \
		u16 __mask = ((1U << (__BitLen)) - 1) << (__BitOffset); \
		__v = (__v & ~__mask) | \
		      (((u16)(__Value) & ((1U << (__BitLen)) - 1)) << (__BitOffset)); \
		(__pStart)[0] = (u8)(__v & 0xff); \
		(__pStart)[1] = (u8)(__v >> 8); \
	} while (0)

#define SET_RSN_CAP_SPP(cap, spp) SET_BITS_TO_LE_2BYTE(((u8 *)(cap)), 10, 2, spp)

#define SPP_CAP BIT(0)
#define SPP_REQ BIT(1)

enum rtw_amsdu_mode {
	RTW_AMSDU_MODE_NON_SPP = 0,
	RTW_AMSDU_MODE_SPP = 1,
	RTW_AMSDU_MODE_ALL_DROP = 2,
};

#define MFP_NO 0
#define MFP_INVALID 1
#define MFP_OPTIONAL 2
#define MFP_REQUIRED 3

#define RTW_GET_LE16(a) ((((u16)(a)[1]) << 8) | (u16)(a)[0])
#define RTW_GET_BE16(a) ((((u16)(a)[0]) << 8) | (u16)(a)[1])

#define RTW_PUT_LE16(a, val) \
	do { \
		(a)[0] = (u8)((u16)(val) & 0xff); \
		(a)[1] = (u8)(((u16)(val) >> 8) & 0xff); \
	} while (0)

#define WLAN_EID_VENDOR_SPECIFIC 221
#define _VENDOR_SPECIFIC_IE_ 221
#define MAX_IE_SZ 256

#define P2P_ATTR_STATUS 0x00
#define P2P_ATTR_CAPABILITY 0x02
#define P2P_ATTR_GROUP_INFO 0x0E

#define WFD_ATTR_DEVICE_INFO 0x00
#define MULTI_AP_SUB_ELEM_TYPE 0x06
#define MULTI_AP_BACKHAUL_STA BIT(7)

extern u8 MULTI_AP_OUI[4];

#define BSS_EX_IES(bss_ex) ((bss_ex)->IEs)
#define BSS_EX_IES_LEN(bss_ex) ((bss_ex)->IELength)
#define BSS_EX_FIXED_IE_OFFSET(bss_ex) \
	((bss_ex)->Reserved[0] == 1 ? 0 : 12)
#define BSS_EX_TLV_IES(bss_ex) \
	(BSS_EX_IES((bss_ex)) + BSS_EX_FIXED_IE_OFFSET((bss_ex)))
#define BSS_EX_TLV_IES_LEN(bss_ex) \
	(BSS_EX_IES_LEN((bss_ex)) - BSS_EX_FIXED_IE_OFFSET((bss_ex)))

typedef struct _NDIS_802_11_VARIABLE_IEs {
	u8 ElementID;
	u8 Length;
	u8 data[1];
} NDIS_802_11_VARIABLE_IEs, *PNDIS_802_11_VARIABLE_IEs;

u8 *rtw_get_p2p_ie(const u8 *in_ie, int in_len, u8 *p2p_ie, unsigned int *p2p_ielen);
u8 *rtw_get_p2p_attr(u8 *p2p_ie, unsigned int p2p_ielen, u8 target_attr_id,
		      u8 *buf_attr, u32 *len_attr);

u32 rtw_get_p2p_merged_ies_len(u8 *in_ie, u32 in_len);
int rtw_p2p_merge_ies(u8 *in_ie, u32 in_len, u8 *merge_ie);
u32 rtw_set_p2p_attr_content(u8 *pbuf, u8 attr_id, u16 attr_len, u8 *pdata_attr);
unsigned int rtw_del_p2p_ie(u8 *ies, unsigned int ies_len_ori, const char *msg);
unsigned int rtw_del_p2p_attr(u8 *ie, unsigned int ielen_ori, u8 attr_id);
void rtw_bss_ex_del_p2p_ie(WLAN_BSSID_EX *bss_ex);
void rtw_bss_ex_del_p2p_attr(WLAN_BSSID_EX *bss_ex, u8 attr_id);

u8 *rtw_get_wfd_ie(const u8 *in_ie, int in_len, u8 *wfd_ie, unsigned int *wfd_ielen);
u8 *rtw_get_wfd_attr(u8 *wfd_ie, unsigned int wfd_ielen, u8 target_attr_id,
		      u8 *buf_attr, u32 *len_attr);
unsigned int rtw_del_wfd_ie(u8 *ies, unsigned int ies_len_ori, const char *msg);
unsigned int rtw_del_wfd_attr(u8 *ie, unsigned int ielen_ori, u8 attr_id);
void rtw_bss_ex_del_wfd_ie(WLAN_BSSID_EX *bss_ex);
void rtw_bss_ex_del_wfd_attr(WLAN_BSSID_EX *bss_ex, u8 attr_id);
u8 rtw_get_multi_ap_ie_ext(const u8 *ies, int ies_len);

struct rsne_info {
	u8 *gcs;
	u16 pcs_cnt;
	u8 *pcs_list;
	u16 akm_cnt;
	u8 *akm_list;
	u8 *cap;
	u16 pmkid_cnt;
	u8 *pmkid_list;
	u8 *gmcs;
	u8 err;
};

extern u8 RTW_WPA_OUI_TYPE[];

extern u8 WPA_CIPHER_SUITE_NONE[];
extern u8 WPA_CIPHER_SUITE_WEP40[];
extern u8 WPA_CIPHER_SUITE_TKIP[];
extern u8 WPA_CIPHER_SUITE_CCMP[];
extern u8 WPA_CIPHER_SUITE_WEP104[];
extern u8 RSN_CIPHER_SUITE_NONE[];
extern u8 RSN_CIPHER_SUITE_WEP40[];
extern u8 RSN_CIPHER_SUITE_TKIP[];
extern u8 RSN_CIPHER_SUITE_CCMP[];
extern u8 RSN_CIPHER_SUITE_GCMP[];
extern u8 RSN_CIPHER_SUITE_GCMP_256[];
extern u8 RSN_CIPHER_SUITE_CCMP_256[];
extern u8 RSN_CIPHER_SUITE_WEP104[];
extern u8 RSN_CIPHER_SUITE_AES_128_CMAC[];
extern u8 RSN_CIPHER_SUITE_BIP_GMAC_128[];
extern u8 RSN_CIPHER_SUITE_BIP_GMAC_256[];
extern u8 RSN_CIPHER_SUITE_BIP_CMAC_256[];
extern u8 WLAN_AKM_8021X[];
extern u8 WLAN_AKM_PSK[];
extern u8 WLAN_AKM_FT_8021X[];
extern u8 WLAN_AKM_FT_PSK[];
extern u8 WLAN_AKM_8021X_SHA256[];
extern u8 WLAN_AKM_PSK_SHA256[];
extern u8 WLAN_AKM_TDLS[];
extern u8 WLAN_AKM_SAE[];
extern u8 WLAN_AKM_FT_OVER_SAE[];
extern u8 WLAN_AKM_8021X_SUITE_B[];
extern u8 WLAN_AKM_8021X_SUITE_B_192[];
extern u8 WLAN_AKM_FILS_SHA256[];
extern u8 WLAN_AKM_FILS_SHA384[];
extern u8 WLAN_AKM_FT_FILS_SHA256[];
extern u8 WLAN_AKM_FT_FILS_SHA384[];

int _rtw_memcmp(const void *s1, const void *s2, size_t n);

int rtw_get_wpa_cipher_suite(u8 *s);
int rtw_get_rsn_cipher_suite(u8 *s);
u32 rtw_get_akm_suite_bitmap(u8 *s);
int rtw_parse_wpa_ie(u8 *wpa_ie, int wpa_ie_len, int *group_cipher,
		     int *pairwise_cipher, u32 *akm);
int rtw_rsne_info_parse(const u8 *ie, unsigned int ie_len,
			struct rsne_info *info);
int rtw_parse_wpa2_ie(u8 *rsn_ie, int rsn_ie_len, int *group_cipher,
		      int *pairwise_cipher, int *gmcs, u32 *akm, u8 *mfp_opt,
		      u8 *spp_opt);

int rtw_get_wapi_ie(u8 *in_ie, unsigned int in_len, u8 *wapi_ie, u16 *wapi_len);
int rtw_get_sec_ie(u8 *in_ie, unsigned int in_len, u8 *rsn_ie, u16 *rsn_len,
		   u8 *wpa_ie, u16 *wpa_len);
u8 rtw_is_wps_ie(u8 *ie_ptr, unsigned int *wps_ielen);

u8 *rtw_get_ie(const u8 *pbuf, sint index, sint *len, sint limit);
u8 *rtw_get_ie_ex(const u8 *in_ie, unsigned int in_len, u8 eid, const u8 *oui,
		  u8 oui_len, u8 *ie, unsigned int *ielen);
int rtw_ies_remove_ie(u8 *ies, unsigned int *ies_len, unsigned int offset,
		      u8 eid, u8 *oui, u8 oui_len);
bool rtw_is_cck_rate(u8 rate);
bool rtw_is_ofdm_rate(u8 rate);
bool rtw_is_basic_rate_ofdm(u8 rate);

u8 str_2char2num(u8 hch, u8 lch);
u8 key_2char2num(u8 hch, u8 lch);
void macstr2num(u8 *dst, u8 *src);
u8 convert_ip_addr(u8 hch, u8 mch, u8 lch);
u8 rtw_check_invalid_mac_address(u8 *mac_addr, u8 check_local_bit);
void rtw_macaddr_cfg(u8 *out, const u8 *hw_mac_addr);

u32 rtw_random32(void);
void host_mac_str_test_set_initmac(const char *mac);
void host_mac_str_test_clear_initmac(void);
void host_mac_str_test_set_random32(u32 val);

#define LE_BITS_TO_1BYTE(__pStart, __BitOffset, __BitLen) \
	(((*((u8 *)(__pStart)) >> (__BitOffset)) & ((1U << (__BitLen)) - 1)))

#define CHANNEL_WIDTH_20 0
#define CHANNEL_WIDTH_40 1
#define CHANNEL_WIDTH_80 2
#define HAL_PRIME_CHNL_OFFSET_DONT_CARE 0
#define HAL_PRIME_CHNL_OFFSET_LOWER 1
#define HAL_PRIME_CHNL_OFFSET_UPPER 2

#define _DSSET_IE_ 3
#define EID_HTCapability 45
#define EID_HTInfo 61
#define EID_VHTOperation 192

enum secondary_ch_offset {
	SCN = 0,
	SCA = 1,
	SCB = 3,
};

#define GET_HT_CAP_ELE_CHL_WIDTH(_pEleStart) \
	LE_BITS_TO_1BYTE(((u8 *)(_pEleStart)), 1, 1)
#define GET_HT_OP_ELE_PRI_CHL(_pEleStart) \
	LE_BITS_TO_1BYTE(((u8 *)(_pEleStart)), 0, 8)
#define GET_HT_OP_ELE_2ND_CHL_OFFSET(_pEleStart) \
	LE_BITS_TO_1BYTE(((u8 *)(_pEleStart)) + 1, 0, 2)
#define GET_HT_OP_ELE_STA_CHL_WIDTH(_pEleStart) \
	LE_BITS_TO_1BYTE(((u8 *)(_pEleStart)) + 1, 2, 1)
#define GET_VHT_OPERATION_ELE_CHL_WIDTH(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 0, 8)

typedef struct _NDIS_802_11_FIXED_IEs {
	u8 Timestamp[8];
	u16 BeaconInterval;
	u16 Capabilities;
} NDIS_802_11_FIXED_IEs;

u8 rtw_get_offset_by_chbw(u8 ch, u8 bw, u8 *r_offset);

void rtw_ies_get_chbw(u8 *ies, int ies_len, u8 *ch, u8 *bw, u8 *offset,
		      u8 ht, u8 vht);
void rtw_bss_get_chbw(WLAN_BSSID_EX *bss, u8 *ch, u8 *bw, u8 *offset, u8 ht,
		      u8 vht);
bool rtw_is_chbw_grouped(u8 ch_a, u8 bw_a, u8 offset_a, u8 ch_b, u8 bw_b,
			 u8 offset_b);
void rtw_sync_chbw(u8 *req_ch, u8 *req_bw, u8 *req_offset, u8 *g_ch,
		   u8 *g_bw, u8 *g_offset);

#define le16_to_cpu(x) (x)

#define RTW_IEEE80211_FCTL_FTYPE 0x000c
#define RTW_IEEE80211_FCTL_STYPE 0x00f0
#define RTW_IEEE80211_FCTL_FROMDS 0x0200
#define RTW_IEEE80211_FCTL_TODS 0x0100
#define RTW_IEEE80211_FTYPE_MGMT 0x0000
#define RTW_IEEE80211_FTYPE_DATA 0x0008
#define RTW_IEEE80211_FTYPE_CTL 0x0004
#define RTW_IEEE80211_STYPE_QOS_DATA 0x0080
#define RTW_IEEE80211_STYPE_ACTION 0x00D0
#define RTW_IEEE80211_STYPE_CTS 0x00C0
#define RTW_IEEE80211_STYPE_ACK 0x00D0

#define WLAN_FC_GET_TYPE(fc) ((fc) & RTW_IEEE80211_FCTL_FTYPE)
#define WLAN_FC_GET_STYPE(fc) ((fc) & RTW_IEEE80211_FCTL_STYPE)

enum rtw_wlan_category {
	RTW_WLAN_CATEGORY_PUBLIC = 4,
	RTW_WLAN_CATEGORY_P2P = 0x7f,
};

enum rtw_public_action_field {
	ACT_PUBLIC_BSSCOEXIST = 0,
	ACT_PUBLIC_FTM_REQ = 32,
	ACT_PUBLIC_FTM = 33,
	ACT_PUBLIC_MAX = 34,
};

struct rtw_ieee80211_hdr_3addr {
	u16 frame_ctl;
	u16 duration_id;
	u8 addr1[ETH_ALEN];
	u8 addr2[ETH_ALEN];
	u8 addr3[ETH_ALEN];
	u16 seq_ctl;
} __attribute__((packed));

#define HT_CAP_ELE_SUP_MCS_SET(_pEleStart) (((u8 *)(_pEleStart)) + 3)
#define GET_HT_CAP_ELE_TX_MCS_DEF(_pEleStart) \
	LE_BITS_TO_1BYTE(((u8 *)(_pEleStart)) + 15, 0, 1)
#define GET_HT_CAP_ELE_TRX_MCS_NEQ(_pEleStart) \
	LE_BITS_TO_1BYTE(((u8 *)(_pEleStart)) + 15, 1, 1)
#define GET_HT_CAP_ELE_TX_MAX_SS(_pEleStart) \
	LE_BITS_TO_1BYTE(((u8 *)(_pEleStart)) + 15, 2, 2)

int ieee80211_is_empty_essid(const char *essid, int essid_len);
int ieee80211_get_hdrlen(u16 fc);
u16 rtw_ht_mcs_rate(u8 bw_40MHz, u8 short_GI, unsigned char *MCS_rate);
u8 rtw_ht_cap_get_rx_nss(u8 *ht_cap);
u8 rtw_ht_cap_get_tx_nss(u8 *ht_cap);
int rtw_action_frame_parse(const u8 *frame, u32 frame_len, u8 *category,
			   u8 *action);

#endif /* HOST_IEEE80211_TYPES_H */
