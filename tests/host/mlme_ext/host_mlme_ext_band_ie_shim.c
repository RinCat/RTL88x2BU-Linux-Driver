// SPDX-License-Identifier: GPL-2.0
#include "host_mlme_ext_band_ie_types.h"
#include <stdlib.h>
#include <string.h>

void *rtw_malloc(size_t sz) { return malloc(sz); }
void rtw_mfree(void *p, size_t sz) { (void)sz; free(p); }

u8 *rtw_get_ie(const u8 *pbuf, sint index, sint *len, sint limit)
{
	sint tmp, i;
	const u8 *p;

	if (limit < 1)
		return NULL;
	p = pbuf;
	for (i = 0, *len = 0;;) {
		if (*p == index) {
			*len = *(p + 1);
			return (u8 *)p;
		}
		tmp = *(p + 1);
		p += (tmp + 2);
		i += (tmp + 2);
		if (i >= limit)
			break;
	}
	return NULL;
}

void rtw_add_bcn_ie(_adapter *padapter, WLAN_BSSID_EX *pnetwork, u8 index,
		    u8 *data, u8 len)
{
	PNDIS_802_11_VARIABLE_IEs pIE;
	u8 *pie = pnetwork->IEs, *p = NULL, *dst = NULL, *bak = NULL, *tail;
	u32 i, ielen = 0, tail_len = 0, match = _FALSE;

	(void)padapter;
	for (i = _BEACON_IE_OFFSET_; i < pnetwork->IELength;) {
		pIE = (PNDIS_802_11_VARIABLE_IEs)(pnetwork->IEs + i);
		if (pIE->ElementID > index)
			break;
		if (pIE->ElementID == index) {
			p = (u8 *)pIE;
			ielen = pIE->Length;
			match = _TRUE;
			break;
		}
		p = (u8 *)pIE;
		ielen = pIE->Length;
		i += (pIE->Length + 2);
	}
	if (!p || !ielen)
		return;
	ielen += 2;
	tail = p + ielen;
	tail_len = pnetwork->IELength - (uint)(p - pie) - ielen;
	dst = match ? p : tail;
	if (tail_len) {
		bak = rtw_malloc(tail_len);
		if (bak)
			memcpy(bak, tail, tail_len);
	}
	*dst++ = index;
	*dst++ = len;
	memcpy(dst, data, len);
	dst += len;
	if (bak) {
		memcpy(dst, bak, tail_len);
		rtw_mfree(bak, tail_len);
	}
	pnetwork->IELength = (uint)(dst - pie) + tail_len;
}

void rtw_remove_bcn_ie(_adapter *padapter, WLAN_BSSID_EX *pnetwork, u8 index)
{
	u8 *p, *pie = pnetwork->IEs, *bak = NULL;
	sint ielen = 0;
	uint tail_len;

	(void)padapter;
	p = rtw_get_ie(pie + _FIXED_IE_LENGTH_, index, &ielen,
		       pnetwork->IELength - _FIXED_IE_LENGTH_);
	if (!p || ielen <= 0)
		return;
	ielen += 2;
	tail_len = pnetwork->IELength - (uint)(p - pie) - ielen;
	if (tail_len) {
		bak = rtw_malloc(tail_len);
		if (bak)
			memcpy(bak, p + ielen, tail_len);
	}
	if (bak) {
		memcpy(p, bak, tail_len);
		rtw_mfree(bak, tail_len);
	}
	pnetwork->IELength = (uint)(p - pie) + tail_len;
}

/* Host stub for kernel rtw_vht_ies_attach() (core/rtw_vht.c); deterministic IE bytes. */
void rtw_vht_ies_attach(_adapter *a, WLAN_BSSID_EX *n)
{
	static const u8 ext[8] = {1}, cap[4] = {0xaa, 0xbb, 0xcc, 0xdd}, op[3] = {1, 2, 3};

	rtw_add_bcn_ie(a, n, EID_EXTCapability, (u8 *)ext, 8);
	rtw_add_bcn_ie(a, n, EID_VHTCapability, (u8 *)cap, 4);
	rtw_add_bcn_ie(a, n, EID_VHTOperation, (u8 *)op, 3);
	a->mlmepriv.vhtpriv.vht_option = _TRUE;
}

void rtw_vht_ies_detach(_adapter *a, WLAN_BSSID_EX *n)
{
	rtw_remove_bcn_ie(a, n, EID_EXTCapability);
	rtw_remove_bcn_ie(a, n, EID_VHTCapability);
	rtw_remove_bcn_ie(a, n, EID_VHTOperation);
	a->mlmepriv.vhtpriv.vht_option = _FALSE;
}

u8 rtw_rust_band_ie_ht_option(_adapter *a)
{
	return a->mlmepriv.htpriv.ht_option;
}

u32 rtw_rust_band_ie_wireless_mode(_adapter *a)
{
	return a->registrypriv.wireless_mode;
}

u8 rtw_rust_band_ie_vht_enable(_adapter *a)
{
	return a->registrypriv.vht_enable;
}

u8 rtw_rust_band_ie_ori_vht_en(_adapter *a)
{
	return a->mlmepriv.ori_vht_en;
}

u8 rtw_rust_band_ie_country_en_11ac(_adapter *a)
{
	country_ent_t *ent = a->rfctl.country_ent;

	return (!ent || ent->en_11ac) ? 1 : 0;
}

u8 *rtw_rust_band_ie_supported_rates(WLAN_BSSID_EX *n)
{
	return n->SupportedRates;
}

void rtw_rust_band_ie_set_length(WLAN_BSSID_EX *n)
{
	n->Length = get_WLAN_BSSID_EX_sz(n);
}
