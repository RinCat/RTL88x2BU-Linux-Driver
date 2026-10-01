// SPDX-License-Identifier: GPL-2.0
/* Host C oracle for sreset_reset orchestration (W3-95 PR9). */
#include "host_sreset_reset_types.h"

void sreset_reset(PADAPTER padapter)
{
	struct hal_data_type *pHalData;
	struct sreset_priv *psrtpriv;
	struct pwrctrl_priv *pwrpriv;
	struct debug_priv *pdbgpriv;

	if (!padapter || !padapter->HalData || !padapter->dvobj)
		return;

	pHalData = GET_HAL_DATA(padapter);
	psrtpriv = &pHalData->srestpriv;
	pwrpriv = adapter_to_pwrctl(padapter);
	pdbgpriv = &adapter_to_dvobj(padapter)->drv_dbg;

	psrtpriv->Wifi_Error_Status = WIFI_STATUS_SUCCESS;

	host_sreset_reset_set_ps_mode(padapter);
	host_sreset_reset_enter_pwrlock(padapter);

	psrtpriv->silent_reset_inprogress = 1;
	pwrpriv->change_rfpwrstate = rf_off;

	host_sreset_reset_mi_adapter_hdl(padapter, 0);
	host_sreset_reset_ips_enter(padapter);
	host_sreset_reset_ips_leave(padapter);
	host_sreset_reset_ap_info_restore(padapter);
	host_sreset_reset_mi_adapter_hdl(padapter, 1);

	psrtpriv->silent_reset_inprogress = 0;

	host_sreset_reset_exit_pwrlock(padapter);

	pdbgpriv->dbg_sreset_cnt++;

	psrtpriv->self_dect_fw = 0;
	psrtpriv->rx_cnt = 0;
}
