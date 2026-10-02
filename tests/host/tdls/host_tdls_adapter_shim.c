// SPDX-License-Identifier: GPL-2.0
#include "host_tdls_types.h"

#define RTW_INFO(fmt, ...) ((void)0)

void rtw_mi_update_iface_status(struct mlme_priv *priv, int flags)
{
	(void)priv;
	(void)flags;
}

static void rtw_tdls_set_link_established(PADAPTER adapter, u8 en)
{
	adapter->tdlsinfo.link_established = en;
	rtw_mi_update_iface_status(&adapter->mlmepriv, 0);
}

void rtw_reset_tdls_info(PADAPTER padapter)
{
	struct tdls_info *ptdlsinfo = &padapter->tdlsinfo;

	ptdlsinfo->ap_prohibited = _FALSE;
	ptdlsinfo->ch_switch_prohibited =
		padapter->registrypriv.wifi_spec == 1 ? _FALSE : _TRUE;
	rtw_tdls_set_link_established(padapter, _FALSE);
	ptdlsinfo->sta_cnt = 0;
	ptdlsinfo->sta_maximum = _FALSE;
#ifdef CONFIG_TDLS_CH_SW
	ptdlsinfo->chsw_info.ch_sw_state = TDLS_STATE_NONE;
	ATOMIC_SET(ptdlsinfo->chsw_info.chsw_on, _FALSE);
	ptdlsinfo->chsw_info.off_ch_num = 0;
	ptdlsinfo->chsw_info.ch_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
	ptdlsinfo->chsw_info.cur_time = 0;
	ptdlsinfo->chsw_info.delay_switch_back = _FALSE;
	ptdlsinfo->chsw_info.dump_stack = _FALSE;
#endif
	ptdlsinfo->ch_sensing = 0;
	ptdlsinfo->watchdog_count = 0;
	ptdlsinfo->dev_discovered = _FALSE;
	ptdlsinfo->tdls_sctx = NULL;
}

int rtw_init_tdls_info(PADAPTER padapter)
{
	struct tdls_info *ptdlsinfo = &padapter->tdlsinfo;

	rtw_reset_tdls_info(padapter);
#ifdef CONFIG_TDLS_DRIVER_SETUP
	ptdlsinfo->driver_setup = _TRUE;
#else
	ptdlsinfo->driver_setup = _FALSE;
#endif
	_rtw_spinlock_init(&ptdlsinfo->cmd_lock);
	_rtw_spinlock_init(&ptdlsinfo->hdl_lock);
	return _SUCCESS;
}

void rtw_free_tdls_info(struct tdls_info *ptdlsinfo)
{
	_rtw_spinlock_free(&ptdlsinfo->cmd_lock);
	_rtw_spinlock_free(&ptdlsinfo->hdl_lock);
	_rtw_memset(ptdlsinfo, 0, sizeof(*ptdlsinfo));
}

u8 rtw_is_tdls_enabled(PADAPTER padapter)
{
	return padapter->registrypriv.en_tdls;
}

void rtw_set_tdls_enable(PADAPTER padapter, u8 enable)
{
	padapter->registrypriv.en_tdls = enable;
	RTW_INFO("en_tdls = %d\n", rtw_is_tdls_enabled(padapter));
}

int is_client_associated_to_ap(PADAPTER padapter)
{
	struct mlme_ext_info *pmlmeinfo;

	if (!padapter)
		return _FAIL;
	pmlmeinfo = &padapter->mlmeextpriv.mlmext_info;
	if ((pmlmeinfo->state & WIFI_FW_ASSOC_SUCCESS) &&
	    ((pmlmeinfo->state & 0x03) == WIFI_FW_STATION_STATE))
		return _TRUE;
	return _FAIL;
}

u8 rtw_tdls_is_setup_allowed(PADAPTER padapter)
{
	struct tdls_info *ptdlsinfo = &padapter->tdlsinfo;

	if (is_client_associated_to_ap(padapter) == _FALSE)
		return _FALSE;
	if (ptdlsinfo->ap_prohibited == _TRUE)
		return _FALSE;
	return _TRUE;
}

#ifdef CONFIG_TDLS_CH_SW
u8 rtw_tdls_is_chsw_allowed(PADAPTER padapter)
{
	struct tdls_info *ptdlsinfo = &padapter->tdlsinfo;

	if (ptdlsinfo->ch_switch_prohibited == _TRUE)
		return _FALSE;
	if (padapter->registrypriv.wifi_spec == 0)
		return _FALSE;
	return _TRUE;
}
#endif
