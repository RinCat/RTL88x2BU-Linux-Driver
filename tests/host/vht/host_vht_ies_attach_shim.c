// SPDX-License-Identifier: GPL-2.0
/* Host shims for rtw_vht_ies_attach L2 (W3-84 PR7). */

#include "host_vht_ies_attach_types.h"

u8 *rtw_get_ie(const u8 *pbuf, sint index, sint *len, sint limit)
{
	sint tmp, i;
	const u8 *p;

	if (limit < 1)
		return NULL;

	p = pbuf;
	i = 0;
	*len = 0;
	while (1) {
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

void rtw_check_for_vht20(_adapter *padapter, u8 *ies, int ies_len)
{
	(void)padapter;
	(void)ies;
	(void)ies_len;
}

void rtw_vht_use_default_setting(_adapter *padapter)
{
	struct vht_priv *pvhtpriv = &padapter->mlmepriv.vhtpriv;

	pvhtpriv->vht_option = _FALSE;
	pvhtpriv->sgi_80m = _TRUE;
	pvhtpriv->ampdu_len = padapter->registrypriv.ampdu_factor;
	pvhtpriv->vht_mcs_map[0] = 0xff;
	pvhtpriv->vht_mcs_map[1] = 0xff;
}
