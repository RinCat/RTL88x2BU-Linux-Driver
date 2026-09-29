// SPDX-License-Identifier: GPL-2.0
#include "host_cmd_drvextra_types.h"

u8 rtw_drvextra_cmd_hdl(PADAPTER padapter, unsigned char *pbuf)
{
	struct drvextra_cmd_parm *pdrvextra_cmd;

	(void)padapter;
	if (!pbuf)
		return H2C_PARAMETERS_ERROR;

	pdrvextra_cmd = (struct drvextra_cmd_parm *)pbuf;

	switch (pdrvextra_cmd->ec_id) {
	case 2: /* DYNAMIC_CHK_WK_CID */
		rtw_dynamic_chk_wk_hdl(padapter);
		break;
	case 10: /* CHECK_HIQ_WK_CID */
#ifdef CONFIG_AP_MODE
		rtw_chk_hi_queue_hdl(padapter);
#endif
		break;
	case 13: /* RESET_SECURITYPRIV */
		reset_securitypriv_hdl(padapter);
		break;
	case 14: /* FREE_ASSOC_RESOURCES */
		free_assoc_resources_hdl(padapter, (u8)pdrvextra_cmd->type);
		break;
	default:
		break;
	}

	if (pdrvextra_cmd->pbuf && pdrvextra_cmd->size > 0)
		rtw_mfree(pdrvextra_cmd->pbuf, pdrvextra_cmd->size);

	return H2C_SUCCESS;
}
