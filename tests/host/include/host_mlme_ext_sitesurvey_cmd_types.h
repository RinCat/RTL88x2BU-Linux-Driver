/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_MLME_EXT_SITESURVEY_CMD_TYPES_H
#define HOST_MLME_EXT_SITESURVEY_CMD_TYPES_H

#include "host_types.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#define _TRUE 1
#define _FALSE 0
#define RTW_INFO(...) do { } while (0)
#define RTW_ERR(...) do { } while (0)
#define RTW_WARN(...) do { } while (0)
#define rtw_warn_on(c) ((void)(c))

#define SCAN_DISABLE 0
#define SCAN_START 1
#define SCAN_PS_ANNC_WAIT 2
#define SCAN_ENTER 3
#define SCAN_PROCESS 4
#define SCAN_BACKING_OP 5
#define SCAN_BACK_OP 6
#define SCAN_LEAVING_OP 7
#define SCAN_LEAVE_OP 8
#define SCAN_COMPLETE 12

#define SS_BACKOP_PS_ANNC (1 << 4)
#define SS_BACKOP_TX_RESUME (1 << 5)

#define mlmeext_chk_scan_backop_flags(e, f) \
	(((e)->sitesurvey_res.backop_flags & (f)) == (f))

#define SCAN_PASSIVE 0
#define SCAN_ACTIVE 1
#define RTW_SSID_SCAN_AMOUNT 4
#define RTW_CHANNEL_SCAN_AMOUNT 8
#define MAX_CHANNEL_NUM 59
#define RX_AMPDU_ACCEPT_INVALID 0xff
#define RX_AMPDU_SIZE_INVALID 0xff

#define HW_VAR_CHECK_TXBUF 0
#define HW_VAR_MLME_SITESURVEY 1

#define mlmeext_scan_state(e) ((e)->sitesurvey_res.state)
#define mlmeext_scan_next_state(e) ((e)->sitesurvey_res.next_state)
#define mlmeext_chk_scan_state(e, s) ((e)->sitesurvey_res.state == (s))
#define mlmeext_set_scan_state(e, s) \
	do { \
		(e)->sitesurvey_res.state = (s); \
		(e)->sitesurvey_res.next_state = (s); \
	} while (0)
#define mlmeext_set_scan_next_state(e, s) \
	do { (e)->sitesurvey_res.next_state = (s); } while (0)

typedef int RT_SCAN_TYPE;
typedef int sint;

struct rtw_ieee80211_channel { u16 hw_value; u32 flags; };
struct RT_CHANNEL_INFO {
	u8 ChannelNum;
	u8 flags;
	u8 hidden_bss_cnt;
};
typedef struct RT_CHANNEL_INFO RT_CHANNEL_INFO;
struct rf_ctl_t { struct RT_CHANNEL_INFO channel_set[MAX_CHANNEL_NUM]; };

typedef struct _NDIS_802_11_SSID {
	u32 SsidLength;
	u8 Ssid[32];
} NDIS_802_11_SSID;

struct ss_res {
	u8 state;
	u8 next_state;
	u16 bss_cnt;
	u16 activate_ch_cnt;
	u16 scan_ch_ms;
	u8 scan_mode;
	u8 force_ssid_scan;
	u8 ssid_num;
	u8 ch_num;
	u8 channel_idx;
	u8 scan_cnt;
	u8 scan_cnt_max;
	u8 igi;
	u8 igi_scan;
	u8 igi_before_scan;
	u32 token;
	u16 duration;
	u8 bw;
	u8 acs;
	u8 rx_ampdu_accept;
	u8 rx_ampdu_size;
	u16 backop_ms;
	u32 backop_time;
	u8 backop_flags;
	NDIS_802_11_SSID ssid[RTW_SSID_SCAN_AMOUNT];
	struct rtw_ieee80211_channel ch[RTW_CHANNEL_SCAN_AMOUNT];
};

struct mlme_ext_info { u8 state; u8 hw_media_state; };
struct mlme_ext_priv {
	struct mlme_ext_info mlmext_info;
	struct ss_res sitesurvey_res;
	u8 cur_channel;
	u8 cur_bwmode;
	u8 cur_ch_offset;
	u8 scan_abort;
};

struct mlme_priv { u32 scan_start_time; };
struct registry_priv { u8 wifi_spec; };

struct sitesurvey_parm {
	sint scan_mode;
	u8 ssid_num;
	u8 ch_num;
	NDIS_802_11_SSID ssid[RTW_SSID_SCAN_AMOUNT];
	struct rtw_ieee80211_channel ch[RTW_CHANNEL_SCAN_AMOUNT];
	u32 token;
	u16 duration;
	u8 igi;
	u8 bw;
	u8 acs;
	u8 reason;
};

struct _adapter {
	struct registry_priv registrypriv;
	struct mlme_priv mlmepriv;
	struct mlme_ext_priv mlmeextpriv;
	struct rf_ctl_t rfctl;
};
typedef struct _adapter _adapter;

struct host_sitesurvey_cmd_trace {
	u8 site_survey_ch;
	RT_SCAN_TYPE site_survey_type;
	u8 survey_timer_ms;
	u8 ps_annc_calls;
	u8 set_igi_enter;
	u8 set_msr_enter;
	u8 hw_survey_on;
	u8 hw_survey_off;
	u8 pick_ch_calls;
	u8 set_channel_ch;
	u8 survey_done;
	u8 phydm_restore;
	u8 macid_wakeup;
	u8 backop_xmit;
};

extern struct host_sitesurvey_cmd_trace host_sitesurvey_cmd_trace;

void host_sitesurvey_cmd_reset_trace(void);
extern int host_ps_annc_result;
extern u32 host_sitesurvey_time_ms;
RT_CHANNEL_INFO *adapter_to_chset(_adapter *a);
int rtw_scan_ch_decision(_adapter *a, struct rtw_ieee80211_channel *out,
			 u8 out_max, struct rtw_ieee80211_channel *in, u8 in_num,
			 u8 acs, int reason);
u8 rtw_ps_annc(_adapter *a, bool ps);
void rtw_phydm_ability_backup(_adapter *a);
void rtw_phydm_func_for_offchannel(_adapter *a);
void sitesurvey_set_igi(_adapter *a);
void sitesurvey_set_msr(_adapter *a, bool enter);
void site_survey(_adapter *a, u8 ch, RT_SCAN_TYPE type);
void set_survey_timer(struct mlme_ext_priv *e, u32 ms);
void rtw_hal_set_hwreg(_adapter *a, int id, u8 *val);
void rtw_hal_macid_sleep_all_used(_adapter *a);
void rtw_rx_ampdu_apply(_adapter *a);
u8 sitesurvey_pick_ch_behavior(_adapter *a, u8 *ch, RT_SCAN_TYPE *type);
void set_channel_bwmode(_adapter *a, u8 ch, u8 offset, u8 bw);
int rtw_mi_get_ch_setting_union(_adapter *a, u8 *ch, u8 *bw, u8 *offset);
void survey_done_set_ch_bw(_adapter *a);
void rtw_phydm_ability_restore(_adapter *a);
void rtw_hal_macid_wakeup_all_used(_adapter *a);
void rtw_mi_os_xmit_schedule(_adapter *a);
u32 rtw_get_current_time(void);
u32 rtw_get_passing_time_ms(u32 start);
u8 sitesurvey_cmd_hdl(_adapter *a, u8 *pbuf);

#endif
