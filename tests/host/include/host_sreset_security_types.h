/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_SRESET_SECURITY_TYPES_H
#define HOST_SRESET_SECURITY_TYPES_H

#include <stdbool.h>
#include "host_types.h"

typedef int sint;

#define dot11AuthAlgrthm_8021X 2u
#define _TKIP_ 0x02u
#define _AES_ 0x04u
#define HW_VAR_SEC_CFG 0x100u
#define UNICAST_KEY 1

struct host_sreset_security_trace {
	u8 last_sec_cfg;
	unsigned setstakey_calls, set_key_calls;
};

struct wlan_network { u8 MacAddress[6]; };
struct cur_network { struct wlan_network network; };
struct mlme_priv { struct cur_network cur_network; };
struct mlme_ext_info { u32 auth_algo; };
struct mlme_ext_priv { struct mlme_ext_info mlmext_info; };
struct security_priv { u32 dot11PrivacyAlgrthm; u8 dot118021XGrpKeyid; };
struct sta_info { u8 dummy; };
struct sta_priv { struct sta_info stub_sta; };

struct _adapter {
	struct mlme_priv mlmepriv;
	struct mlme_ext_priv mlmeextpriv;
	struct security_priv securitypriv;
	struct sta_priv stapriv;
};
typedef struct _adapter _adapter;
typedef _adapter *PADAPTER;

struct host_sreset_security_trace *host_sreset_security_get_trace(void);
void host_sreset_security_reset_trace(void);
void host_sreset_security_set_sta_present(u8 present);
u8 *get_bssid(struct mlme_priv *pmlmepriv);
struct sta_info *rtw_get_stainfo(struct sta_priv *pstapriv, u8 *hwaddr);
void rtw_hal_set_hwreg(PADAPTER padapter, u32 variable, u8 *val);
void rtw_setstakey_cmd(PADAPTER padapter, struct sta_info *psta, int keytype, u8 enqueue);
sint rtw_set_key(PADAPTER padapter, struct security_priv *psecuritypriv, sint keyid,
		 u8 set_tx, bool enqueue);
void sreset_restore_security_station(PADAPTER padapter);

#endif
