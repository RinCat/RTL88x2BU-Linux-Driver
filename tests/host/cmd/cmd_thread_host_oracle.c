// SPDX-License-Identifier: GPL-2.0
#include "host_cmd_thread_types.h"

void rtw_cmd_clr_isr(struct cmd_priv *pcmdpriv)
{
	pcmdpriv->cmd_done_cnt++;
}

void rtw_stop_cmd_thread(_adapter *adapter)
{
	if (adapter->cmdThread) {
		_rtw_up_sema(&adapter->cmdpriv.cmd_queue_sema);
		rtw_thread_stop(adapter->cmdThread);
		adapter->cmdThread = NULL;
	}
}
