/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_VHT_BUILD_TYPES_H
#define HOST_VHT_BUILD_TYPES_H

#include "host_types.h"
#include <stdbool.h>

typedef unsigned int uint;
#define BIT(x) (1U << (x))
#define BIT0 BIT(0)
#define BIT1 BIT(1)
#define TEST_FLAG(v, f) (((v) & (f)) != 0)
#define SET_FLAG(v, f) ((v) |= (f))
#define _TRUE 1
#define _FALSE 0
typedef u8 BOOLEAN;
#define GET_VHT_CAPABILITY_ELE_RX_LDPC(p) LE_BITS_TO_1BYTE(p, 4, 1)
#define GET_VHT_CAPABILITY_ELE_SHORT_GI80M(p) LE_BITS_TO_1BYTE(p, 5, 1)
#define GET_VHT_CAPABILITY_ELE_RX_STBC(p) LE_BITS_TO_1BYTE((p) + 1, 0, 3)
#define GET_VHT_CAPABILITY_ELE_MAX_RXAMPDU_FACTOR(p) LE_BITS_TO_2BYTE((p) + 2, 7, 3)
#define GET_HAL_RX_NSS(a) ((a)->host_fixture.rx_nss)
#define RTW_DBG(...) do { } while (0)
#define RTW_INFO(...) do { } while (0)
#define RTW_ERR(...) do { } while (0)
#define MGN_VHT1SS_MCS0 0xA0
#define EID_VHTCapability 191
#define EID_VHTOperation 192
#define VHT_CAP_IE_LEN 12
#define CHANNEL_WIDTH_20 0
#define CHANNEL_WIDTH_80 2
#define CHANNEL_WIDTH_160 3
#define CHANNEL_WIDTH_80_80 4
#define BW_CAP_80M (1U << 4)
#define BW_CAP_160M (1U << 5)
#define BW_CAP_80_80M (1U << 6)
#define HAL_PRIME_CHNL_OFFSET_LOWER 1
#define LDPC_VHT_ENABLE_RX BIT0
#define LDPC_VHT_ENABLE_TX BIT1
#define LDPC_VHT_CAP_TX BIT(3)
#define STBC_VHT_ENABLE_RX BIT0
#define STBC_VHT_ENABLE_TX BIT1
#define STBC_VHT_CAP_TX BIT(3)
#define LE_BITS_TO_1BYTE(p, o, l) (((*((u8 *)(p)) >> (o)) & ((1U << (l)) - 1)))
#define LE_BITS_TO_2BYTE(p, o, l) \
	(((u16)(*((u8 *)(p)) | (*((u8 *)(p) + 1) << 8)) >> (o)) & ((1U << (l)) - 1))
#define SET_BITS_TO_LE_1BYTE(p, o, l, v) do { u8 *__x = (u8 *)(p); u8 __m = (((1U << (l)) - 1) << (o)); \
	*__x = (*__x & ~__m) | (((v) & ((1U << (l)) - 1)) << (o)); } while (0)
#define SET_BITS_TO_LE_2BYTE(p, o, l, v) do { u8 *__x = (u8 *)(p); u16 __c = (u16)(*__x | (__x[1] << 8)); \
	u16 __m = (u16)(((1U << (l)) - 1) << (o)); __c = (__c & ~__m) | (u16)(((v) & ((1U << (l)) - 1)) << (o)); \
	*__x = (u8)(__c & 0xff); *(__x + 1) = (u8)(__c >> 8); } while (0)
#define SET_VHT_CAPABILITY_ELE_MAX_MPDU_LENGTH(p, v) SET_BITS_TO_LE_1BYTE(p, 0, 2, v)
#define SET_VHT_CAPABILITY_ELE_CHL_WIDTH(p, v) SET_BITS_TO_LE_1BYTE(p, 2, 2, v)
#define SET_VHT_CAPABILITY_ELE_RX_LDPC(p, v) SET_BITS_TO_LE_1BYTE(p, 4, 1, v)
#define SET_VHT_CAPABILITY_ELE_SHORT_GI80M(p, v) SET_BITS_TO_LE_1BYTE(p, 5, 1, v)
#define SET_VHT_CAPABILITY_ELE_TX_STBC(p, v) SET_BITS_TO_LE_1BYTE(p, 7, 1, v)
#define SET_VHT_CAPABILITY_ELE_RX_STBC(p, v) SET_BITS_TO_LE_1BYTE((p) + 1, 0, 3, v)
#define SET_VHT_CAPABILITY_ELE_TXOP_PS(p, v) SET_BITS_TO_LE_1BYTE((p) + 2, 5, 1, v)
#define SET_VHT_CAPABILITY_ELE_HTC_VHT(p, v) SET_BITS_TO_LE_1BYTE((p) + 2, 6, 1, v)
#define SET_VHT_CAPABILITY_ELE_MAX_RXAMPDU_FACTOR(p, v) SET_BITS_TO_LE_2BYTE((p) + 2, 7, 3, v)
#define SET_VHT_CAPABILITY_ELE_LINK_ADAPTION(p, v) SET_BITS_TO_LE_1BYTE((p) + 3, 2, 2, v)
#define SET_VHT_CAPABILITY_ELE_MCS_RX_HIGHEST_RATE(p, v) SET_BITS_TO_LE_2BYTE((p) + 6, 0, 13, v)
#define SET_VHT_CAPABILITY_ELE_MCS_TX_HIGHEST_RATE(p, v) SET_BITS_TO_LE_2BYTE((p) + 10, 0, 13, v)
#define GET_VHT_CAPABILITY_ELE_RX_MCS(p) ((p) + 4)
#define GET_VHT_CAPABILITY_ELE_TX_MCS(p) ((p) + 8)
#define GET_VHT_OPERATION_ELE_CHL_WIDTH(p) LE_BITS_TO_1BYTE(p, 0, 8)
#define SET_VHT_OPERATION_ELE_CHL_WIDTH(p, v) SET_BITS_TO_LE_1BYTE(p, 0, 8, v)
#define SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ1(p, v) SET_BITS_TO_LE_1BYTE((p) + 1, 0, 8, v)
#define SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ2(p, v) SET_BITS_TO_LE_1BYTE((p) + 2, 0, 8, v)
#define BW_MODE_5G(bw_mode) ((bw_mode) >> 4)
#define REGSTY_BW_5G(r) BW_MODE_5G((r)->bw_mode)
#define REGSTY_IS_BW_5G_SUPPORT(regsty, bw) (REGSTY_BW_5G((regsty)) >= (bw))

typedef enum { HAL_DEF_MAX_RECVBUF_SZ = 3, HAL_DEF_RX_PACKET_OFFSET = 4, HAL_DEF_RX_STBC = 15 } HAL_DEF_VARIABLE;

struct registry_priv { u8 bw_mode; u8 ampdu_factor; };
struct vht_priv {
	u8 vht_cap[32];
	u8 vht_mcs_map[2];
	u8 ldpc_cap;
	u8 stbc_cap;
	u8 sgi_80m;
	u8 vht_highest_rate;
	u8 ampdu_len;
	u16 beamform_cap;
	BOOLEAN vht_option;
};
struct mlme_ext_info { u8 assoc_AP_vendor; u8 VHT_enable; };
struct mlme_ext_priv { struct mlme_ext_info mlmext_info; };
struct mlme_priv { struct vht_priv vhtpriv; };
struct host_vht_build_fixture {
	u32 rx_packet_offset;
	u32 max_recvbuf_sz;
	u8 rx_stbc_nss;
	u8 rx_nss;
	u8 hal_max_bw;
	u8 hal_bw_support[5];
};

typedef struct {
	u8 ElementID;
	u8 Length;
	u8 data[12];
} NDIS_802_11_VARIABLE_IEs, *PNDIS_802_11_VARIABLE_IEs;
typedef struct { struct registry_priv registrypriv; struct mlme_priv mlmepriv; struct mlme_ext_priv mlmeextpriv;
	struct host_vht_build_fixture host_fixture; } _adapter;

extern const u16 VHT_MCS_DATA_RATE[3][2][40];
extern _adapter host_vht_build_adapter;
extern u8 host_vht_build_hal_bw_cap;
u8 *rtw_set_ie(u8 *pbuf, int index, uint len, const u8 *source, uint *frlen);
bool hal_chk_bw_cap(_adapter *adapter, u8 cap);
u8 hal_largest_bw(_adapter *padapter, u8 bw_cap);
u8 rtw_get_center_ch(u8 ch, u8 bw, u8 offset);
void rtw_hal_get_def_var(_adapter *padapter, HAL_DEF_VARIABLE variable, void *value);
u32 rtw_build_vht_cap_ie(_adapter *padapter, u8 *pbuf);
u32 rtw_build_vht_operation_ie(_adapter *padapter, u8 *pbuf, u8 channel);
void VHT_caps_handler(_adapter *padapter, PNDIS_802_11_VARIABLE_IEs pIE);
void rtw_vht_nss_to_mcsmap(u8 nss, u8 *target_mcs_map, u8 *cur_mcs_map);
u8 rtw_get_vht_highest_rate(u8 *pvht_mcs_map);

#endif /* HOST_VHT_BUILD_TYPES_H */
