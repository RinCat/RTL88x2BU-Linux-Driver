// SPDX-License-Identifier: GPL-2.0
/*
 * W3-86 PR7: host L2 oracle for update_attrib_sec_info (isolated types).
 *
 * Subset of core/rtw_xmit.c: open/802.1X AES paths and blocked/EAPOL clear
 * window only (no MP bypass, TKIP/WEP/GCMP/WAPI, key copy, bswenc, TDLS/WPS).
 * Sec vectors compile only without RUST_XMIT_UPDATE_ATTRIB_ORACLE; Rust
 * differential tests cover vcs/phy/sec via Rust oracle (PR8+).
 */
#include <string.h>
#include "host_xmit_update_attrib_sec_types.h"

#define dot11AuthAlgrthm_Open 0
#define dot11AuthAlgrthm_8021X 2
#define _NO_PRIVACY_ 0
#define _AES_ 0x04
/* Match enum eap_type in include/ieee80211.h (EAPOL_PACKET=6, EAPOL_2_4=10). */
#define EAPOL_2_4 10
#define EAPOL_4_4 12
#define _SUCCESS 0
#define _FAIL (-1)

static int IS_MCAST(const u8 *da)
{
	return da[0] & 0x01;
}

static u8 host_get_encry_algo(struct security_priv *base,
			      struct security_priv_sec_test *sec,
			      struct sta_info_sec_ext *psta, int bmcast)
{
	switch (sec->dot11AuthAlgrthm) {
	case dot11AuthAlgrthm_Open:
		return (u8)base->dot11PrivacyAlgrthm;
	case dot11AuthAlgrthm_8021X:
		if (bmcast)
			return (u8)sec->dot118021XGrpPrivacy;
		return (u8)psta->dot118021XPrivacy;
	default:
		return _NO_PRIVACY_;
	}
}

/* Same layout as AES_IV in include/rtw_xmit.h (pn48 / little-endian TSC bytes). */
#define AES_IV(pattrib_iv, dot11txpn, keyidx)                                  \
	do {                                                                   \
		(dot11txpn).val = (dot11txpn).val == 0xffffffffffffULL        \
					  ? 0                                \
					  : ((dot11txpn).val + 1);           \
		(pattrib_iv)[0] = (u8)((dot11txpn).val);                       \
		(pattrib_iv)[1] = (u8)((dot11txpn).val >> 8);                  \
		(pattrib_iv)[2] = 0;                                           \
		(pattrib_iv)[3] = (u8)(BIT(5) | (((keyidx)&0x3) << 6));        \
		(pattrib_iv)[4] = (u8)((dot11txpn).val >> 16);                 \
		(pattrib_iv)[5] = (u8)((dot11txpn).val >> 24);                 \
		(pattrib_iv)[6] = (u8)((dot11txpn).val >> 32);                 \
		(pattrib_iv)[7] = (u8)((dot11txpn).val >> 40);                 \
	} while (0)

static u32 rtw_get_passing_time_ms(u64 start)
{
	(void)start;
	return host_xmit_sec_passing_ms;
}

int update_attrib_sec_info_l2(_adapter *padapter, struct pkt_attrib_sec_ext *pattrib,
			      struct sta_info_sec_ext *psta, int eapol_type)
{
	int res = _SUCCESS;
	struct security_priv_sec_test *psec = &host_xmit_sec_cfg;
	int bmcast = IS_MCAST(pattrib->ra);

	memset(pattrib->dot118021x_UncstKey.skey, 0, 16);
	memset(pattrib->dot11tkiptxmickey.skey, 0, 16);
	pattrib->mac_id = psta->mac_id;

	if (psta->ieee8021x_blocked == _TRUE ||
	    ((eapol_type == EAPOL_2_4 || eapol_type == EAPOL_4_4) &&
	     rtw_get_passing_time_ms(psta->resp_nonenc_eapol_key_starttime) <=
		     100)) {
		pattrib->encrypt = 0;
		if (pattrib->ether_type != 0x888e) {
			res = _FAIL;
			goto exit;
		}
	} else {
		pattrib->encrypt = host_get_encry_algo(&padapter->securitypriv, psec,
						       psta, bmcast);
		switch (psec->dot11AuthAlgrthm) {
		case dot11AuthAlgrthm_Open:
			pattrib->key_idx = (u8)psec->dot11PrivacyKeyIndex;
			break;
		case dot11AuthAlgrthm_8021X:
			pattrib->key_idx = bmcast ? (u8)psec->dot118021XGrpKeyid : 0;
			break;
		default:
			pattrib->key_idx = 0;
			break;
		}
	}

	switch (pattrib->encrypt) {
	case _AES_:
		pattrib->iv_len = 8;
		pattrib->icv_len = 8;
		if (bmcast)
			AES_IV(pattrib->iv, psta->dot11txpn, pattrib->key_idx);
		else
			AES_IV(pattrib->iv, psta->dot11txpn, 0);
		break;
	default:
		pattrib->iv_len = 0;
		pattrib->icv_len = 0;
		break;
	}

exit:
	return res;
}
