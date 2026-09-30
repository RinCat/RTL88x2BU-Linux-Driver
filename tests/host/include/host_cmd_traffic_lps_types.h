/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_CMD_TRAFFIC_LPS_TYPES_H
#define HOST_CMD_TRAFFIC_LPS_TYPES_H
#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define WIFI_ASOC_STATE 0x01
#define WIFI_STATION_STATE 0x08
#define WIFI_ADHOC_STATE 0x20
#define WIFI_ADHOC_MASTER_STATE 0x40
#define LPS_CTRL_LEAVE 5
#define LPS_CTRL_ENTER 9
#define LPS_CTRL_CONNECT 2
#define LPS_CTRL_SPECIAL_PACKET 4
#define LPS_CTRL_TRAFFIC_BUSY 6
#define HW_PORT0 0
#define HW_VAR_H2C_FW_JOINBSSRPT 0
#define LPS_DELAY_MS 1000
#define ETH_ALEN 6
typedef int sint;

struct RT_LINK_DETECT_T {
	u32 NumRxOkInPeriod, NumTxOkInPeriod, NumRxUnicastOkInPeriod;
	u8 bBusyTraffic, bTxBusyTraffic, bRxBusyTraffic;
	u8 bHigherBusyTraffic, bHigherBusyRxTraffic, bHigherBusyTxTraffic;
};

struct mlme_priv {
	u32 fw_state;
	struct RT_LINK_DETECT_T LinkDetectInfo;
	u8 assoc_bssid[ETH_ALEN];
};

struct pwrctrl_priv {
	s8 lps_level;
	u8 LpsIdleCount;
	u8 bLeisurePs;
	u8 lps_chk_by_tp;
	int lps_bi_tp_th;
	int lps_tx_tp_th;
	int lps_rx_tp_th;
	int lps_chk_cnt;
	int lps_chk_cnt_th;
};

struct sta_stats {
	u32 tx_bytes, rx_bytes, acc_tx_bytes, acc_rx_bytes, tx_tp_kbits, rx_tp_kbits;
};

struct _adapter;
struct sta_info {
	struct _adapter *padapter;
	struct sta_stats sta_stats;
};

struct sta_priv {
	struct sta_info *sta;
};

struct _adapter {
	struct mlme_priv mlmepriv;
	struct pwrctrl_priv pwrctrlpriv;
	struct sta_priv stapriv;
	u8 hw_port;
};

typedef struct _adapter _adapter;

struct lps_ctrl_wk_parm { s8 lps_level; };

struct host_traffic_lps_trace {
	int lps_enter;
	int lps_leave;
	int hw_joinbss_rpt;
	int set_lps_deny;
	int lps_ctrl_wk_cmd;
	u8 hw_joinbss_val;
	u8 last_lps_ctrl_type;
};

#define adapter_to_pwrctl(a) (&(a)->pwrctrlpriv)
#define MLME_IS_STA(a) (((a)->mlmepriv.fw_state & WIFI_STATION_STATE) != 0)

sint check_fwstate(struct mlme_priv *m, sint s);
void LPS_Enter(_adapter *a, const char *reason);
void LPS_Leave(_adapter *a, const char *reason);
void rtw_hal_set_hwreg(_adapter *a, int id, u8 *val);
void rtw_set_lps_deny(_adapter *a, int ms);
void host_traffic_lps_reset(void);
struct host_traffic_lps_trace *host_traffic_lps_get_trace(void);
void host_traffic_lps_set_sta(_adapter *a);
void host_traffic_lps_set_bcn_cnt(u8 c);

u8 *get_bssid(struct mlme_priv *pmlmepriv);
struct sta_info *rtw_get_stainfo(struct sta_priv *pstapriv, u8 *bssid);
u8 rtw_get_bcn_cnt(_adapter *adapter);
void rtw_lps_ctrl_wk_cmd(_adapter *adapter, u8 lps_ctrl_type, u8 flags);
sint rtw_mi_get_assoc_if_num(_adapter *padapter);
void session_tracker_chk_cmd(_adapter *padapter, void *parm);

u8 _lps_chk_by_tp(_adapter *adapter, u8 from_timer);
u8 _lps_chk_by_pkt_cnts(_adapter *padapter, u8 from_timer, u8 bBusyTraffic);
u8 traffic_status_watchdog(_adapter *padapter, u8 from_timer);
void lps_ctrl_wk_hdl(_adapter *padapter, u8 lps_ctrl_type, u8 *buf);

#endif
