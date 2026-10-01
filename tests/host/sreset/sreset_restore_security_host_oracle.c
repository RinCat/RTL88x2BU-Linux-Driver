// SPDX-License-Identifier: GPL-2.0
/* C oracle for sreset_restore_security_station (W3-95 PR7). */
#include "host_sreset_security_types.h"

void sreset_restore_security_station(PADAPTER padapter)
{
	struct mlme_priv *mlmepriv = &padapter->mlmepriv;
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct sta_info *psta;
	struct mlme_ext_info *pmlmeinfo = &padapter->mlmeextpriv.mlmext_info;
	u8 val8;

	if (pmlmeinfo->auth_algo == dot11AuthAlgrthm_8021X)
		val8 = 0xcc;
	else
		val8 = 0xcf;
	rtw_hal_set_hwreg(padapter, HW_VAR_SEC_CFG, &val8);

	if ((padapter->securitypriv.dot11PrivacyAlgrthm == _TKIP_) ||
	    (padapter->securitypriv.dot11PrivacyAlgrthm == _AES_)) {
		psta = rtw_get_stainfo(pstapriv, get_bssid(mlmepriv));
		if (psta != NULL) {
			rtw_setstakey_cmd(padapter, psta, UNICAST_KEY, 0);
			rtw_set_key(padapter, &padapter->securitypriv,
				    padapter->securitypriv.dot118021XGrpKeyid,
				    0, 0);
		}
	}
}
