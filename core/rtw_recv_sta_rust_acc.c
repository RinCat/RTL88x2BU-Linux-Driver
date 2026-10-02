// SPDX-License-Identifier: GPL-2.0
/* Kernel accessors for rust/rtw_recv_sta_{count,validate}.rs (W3-85 PR3). */
#include <drv_types.h>

#if defined(CONFIG_RUST_RECV_STA) && !defined(HOST_RECV_STA_TEST)

enum rtw_recv_sta_eth_field {
	RTW_RECV_STA_ETH_RA = 0,
	RTW_RECV_STA_ETH_TA,
	RTW_RECV_STA_ETH_DST,
	RTW_RECV_STA_ETH_SRC,
	RTW_RECV_STA_ETH_BSSID,
};

static struct rx_pkt_attrib *rtw_recv_sta_attrib(union recv_frame *rframe)
{
	if (!rframe)
		return NULL;
	return &rframe->u.hdr.attrib;
}

sint rtw_rust_recv_sta_frame_len(union recv_frame *rframe)
{
	return get_recvframe_len(rframe);
}

u8 *rtw_rust_recv_sta_frame_rx_data(union recv_frame *rframe)
{
	if (!rframe)
		return NULL;
	return rframe->u.hdr.rx_data;
}

struct sta_info *rtw_rust_recv_sta_frame_psta(union recv_frame *rframe)
{
	if (!rframe)
		return NULL;
	return rframe->u.hdr.psta;
}

u64 *rtw_rust_recv_sta_recvpriv_rx_bytes(_adapter *adapter)
{
	return &adapter->recvpriv.rx_bytes;
}

u32 *rtw_rust_recv_sta_link_rx_ok(_adapter *adapter)
{
	return &adapter->mlmepriv.LinkDetectInfo.NumRxOkInPeriod;
}

u32 *rtw_rust_recv_sta_link_rx_unicast_ok(_adapter *adapter)
{
	return &adapter->mlmepriv.LinkDetectInfo.NumRxUnicastOkInPeriod;
}

const u8 *rtw_rust_recv_sta_attrib_dst(union recv_frame *rframe)
{
	struct rx_pkt_attrib *att = rtw_recv_sta_attrib(rframe);

	return att ? att->dst : NULL;
}

const u8 *rtw_rust_recv_sta_attrib_ra(union recv_frame *rframe)
{
	struct rx_pkt_attrib *att = rtw_recv_sta_attrib(rframe);

	return att ? att->ra : NULL;
}

u8 rtw_rust_recv_sta_attrib_priority(union recv_frame *rframe)
{
	struct rx_pkt_attrib *att = rtw_recv_sta_attrib(rframe);

	return att ? att->priority : 0;
}

u8 rtw_rust_recv_sta_attrib_data_rate(union recv_frame *rframe)
{
	struct rx_pkt_attrib *att = rtw_recv_sta_attrib(rframe);

	return att ? att->data_rate : 0;
}

systime *rtw_rust_recv_sta_stat_last_rx_time(struct sta_info *psta)
{
	return &psta->sta_stats.last_rx_time;
}

u64 *rtw_rust_recv_sta_stat_rx_data_pkts(struct sta_info *psta)
{
	return &psta->sta_stats.rx_data_pkts;
}

u64 *rtw_rust_recv_sta_stat_rx_bytes(struct sta_info *psta)
{
	return &psta->sta_stats.rx_bytes;
}

u64 *rtw_rust_recv_sta_stat_rx_data_bc_pkts(struct sta_info *psta)
{
	return &psta->sta_stats.rx_data_bc_pkts;
}

u64 *rtw_rust_recv_sta_stat_rx_bc_bytes(struct sta_info *psta)
{
	return &psta->sta_stats.rx_bc_bytes;
}

u64 *rtw_rust_recv_sta_stat_rx_data_mc_pkts(struct sta_info *psta)
{
	return &psta->sta_stats.rx_data_mc_pkts;
}

u64 *rtw_rust_recv_sta_stat_rx_mc_bytes(struct sta_info *psta)
{
	return &psta->sta_stats.rx_mc_bytes;
}

u32 *rtw_rust_recv_sta_stat_rxratecnt(struct sta_info *psta, u8 rate)
{
	if (rate >= sizeof(psta->sta_stats.rxratecnt) / sizeof(psta->sta_stats.rxratecnt[0]))
		return NULL;
	return &psta->sta_stats.rxratecnt[rate];
}

u64 *rtw_rust_recv_sta_stat_rx_data_qos_pkts(struct sta_info *psta, u8 tid)
{
	if (tid >= TID_NUM)
		return NULL;
	return &psta->sta_stats.rx_data_qos_pkts[tid];
}

u8 rtw_rust_recv_sta_attrib_to_fr_ds(union recv_frame *rframe)
{
	struct rx_pkt_attrib *att = rtw_recv_sta_attrib(rframe);

	return att ? att->to_fr_ds : 0;
}

u8 rtw_rust_recv_sta_attrib_amsdu(union recv_frame *rframe)
{
	struct rx_pkt_attrib *att = rtw_recv_sta_attrib(rframe);

	return att ? att->amsdu : 0;
}

void rtw_rust_recv_sta_attrib_set_eth(union recv_frame *rframe, int field, const u8 *mac)
{
	struct rx_pkt_attrib *att = rtw_recv_sta_attrib(rframe);
	u8 *dst;

	if (!att || !mac)
		return;
	switch (field) {
	case RTW_RECV_STA_ETH_RA:
		dst = att->ra;
		break;
	case RTW_RECV_STA_ETH_TA:
		dst = att->ta;
		break;
	case RTW_RECV_STA_ETH_DST:
		dst = att->dst;
		break;
	case RTW_RECV_STA_ETH_SRC:
		dst = att->src;
		break;
	case RTW_RECV_STA_ETH_BSSID:
		dst = att->bssid;
		break;
	default:
		return;
	}
	_rtw_memcpy(dst, mac, ETH_ALEN);
}

int rtw_rust_recv_sta_attrib_eth_eq(union recv_frame *rframe, int field_a, int field_b)
{
	struct rx_pkt_attrib *att = rtw_recv_sta_attrib(rframe);
	const u8 *a;
	const u8 *b;

	if (!att)
		return 0;
	switch (field_a) {
	case RTW_RECV_STA_ETH_RA:
		a = att->ra;
		break;
	case RTW_RECV_STA_ETH_TA:
		a = att->ta;
		break;
	case RTW_RECV_STA_ETH_DST:
		a = att->dst;
		break;
	case RTW_RECV_STA_ETH_SRC:
		a = att->src;
		break;
	case RTW_RECV_STA_ETH_BSSID:
		a = att->bssid;
		break;
	default:
		return 0;
	}
	switch (field_b) {
	case RTW_RECV_STA_ETH_RA:
		b = att->ra;
		break;
	case RTW_RECV_STA_ETH_TA:
		b = att->ta;
		break;
	case RTW_RECV_STA_ETH_DST:
		b = att->dst;
		break;
	case RTW_RECV_STA_ETH_SRC:
		b = att->src;
		break;
	case RTW_RECV_STA_ETH_BSSID:
		b = att->bssid;
		break;
	default:
		return 0;
	}
	return _rtw_memcmp(a, b, ETH_ALEN) ? 1 : 0;
}

const u8 *rtw_rust_recv_sta_attrib_get_eth(union recv_frame *rframe, int field)
{
	struct rx_pkt_attrib *att = rtw_recv_sta_attrib(rframe);

	if (!att)
		return NULL;
	switch (field) {
	case RTW_RECV_STA_ETH_RA:
		return att->ra;
	case RTW_RECV_STA_ETH_TA:
		return att->ta;
	case RTW_RECV_STA_ETH_DST:
		return att->dst;
	case RTW_RECV_STA_ETH_SRC:
		return att->src;
	case RTW_RECV_STA_ETH_BSSID:
		return att->bssid;
	default:
		return NULL;
	}
}

u32 rtw_rust_recv_sta_mlme_fw_state(_adapter *adapter)
{
	return MLME_STATE(adapter);
}

const u8 *rtw_rust_recv_sta_adapter_mac(_adapter *adapter)
{
	return adapter_mac_addr(adapter);
}

struct sta_priv *rtw_rust_recv_sta_stapriv(_adapter *adapter)
{
	return &adapter->stapriv;
}

u8 rtw_rust_recv_sta_radar_detected(_adapter *adapter)
{
	return IS_RADAR_DETECTED(adapter_to_rfctl(adapter)) ? 1 : 0;
}

void rtw_rust_recv_sta_out_sta(struct sta_info **out, struct sta_info *psta)
{
	if (out)
		*out = psta;
}

systime rtw_rust_recv_sta_get_current_time(void)
{
	return rtw_get_current_time();
}

s32 rtw_rust_recv_sta_get_passing_time_ms(systime start)
{
	return rtw_get_passing_time_ms(start);
}

#endif /* CONFIG_RUST_RECV_STA && !HOST_RECV_STA_TEST */
