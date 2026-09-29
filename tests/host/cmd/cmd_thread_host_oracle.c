// SPDX-License-Identifier: GPL-2.0
#include "host_cmd_thread_types.h"

#define RTW_CMDTABLE_SIZE HOST_CMD_WLANCMDS_SIZE
#define HOST_CMD_THREAD_LOOP_BREAK() \
	do { \
		if (!host_cmd_thread_loop_continue()) \
			goto host_cmd_thread_exit; \
	} while (0)

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

thread_return rtw_cmd_thread(thread_context context)
{
	u8 ret;
	struct cmd_obj *pcmd;
	u8 *pcmdbuf;
	u8 (*cmd_hdl)(PADAPTER, u8 *);
	PADAPTER padapter = (PADAPTER)context;
	struct cmd_priv *pcmdpriv = &(padapter->cmdpriv);
	_irqL irqL;

	thread_enter("RTW_CMD_THREAD");
	pcmdbuf = pcmdpriv->cmd_buf;
	ATOMIC_SET(&(pcmdpriv->cmdthd_running), _TRUE);
	_rtw_up_sema(&pcmdpriv->start_cmdthread_sema);

	while (1) {
		HOST_CMD_THREAD_LOOP_BREAK();
		if (_rtw_down_sema(&pcmdpriv->cmd_queue_sema) == _FAIL)
			break;
		if (RTW_CANNOT_RUN(padapter))
			break;
		_enter_critical(&pcmdpriv->cmd_queue.lock, &irqL);
		if (rtw_is_list_empty(&(pcmdpriv->cmd_queue.queue))) {
			_exit_critical(&pcmdpriv->cmd_queue.lock, &irqL);
			continue;
		}
		_exit_critical(&pcmdpriv->cmd_queue.lock, &irqL);

_next:
		HOST_CMD_THREAD_LOOP_BREAK();
		if (RTW_CANNOT_RUN(padapter))
			break;

		pcmd = rtw_dequeue_cmd(pcmdpriv);
		if (!pcmd)
			continue;

		pcmdpriv->cmd_issued_cnt++;
		if (pcmd->cmdsz > MAX_CMDSZ) {
			pcmd->res = H2C_PARAMETERS_ERROR;
			goto post_process;
		}
		if (pcmd->cmdcode >= RTW_CMDTABLE_SIZE || !wlancmds[pcmd->cmdcode].cmd_hdl) {
			pcmd->res = H2C_PARAMETERS_ERROR;
			goto post_process;
		}
		if (rtw_cmd_filter(pcmdpriv, pcmd) == _FAIL) {
			pcmd->res = H2C_DROPPED;
			goto post_process;
		}

		cmd_hdl = wlancmds[pcmd->cmdcode].cmd_hdl;
		_rtw_memcpy(pcmdbuf, pcmd->parmbuf, pcmd->cmdsz);
		ret = cmd_hdl(pcmd->padapter, pcmdbuf);
		pcmd->res = ret;
		pcmdpriv->cmd_seq++;

post_process:
		if (pcmd->sctx && pcmd->res != H2C_SUCCESS)
			rtw_sctx_done_err(&pcmd->sctx, RTW_SCTX_DONE_CMD_ERROR);
		else if (pcmd->sctx)
			rtw_sctx_done(&pcmd->sctx);
		rtw_free_cmd_obj(pcmd);
		goto _next;
	}

host_cmd_thread_exit:
	ATOMIC_SET(&(pcmdpriv->cmdthd_running), _FALSE);
	while ((pcmd = rtw_dequeue_cmd(pcmdpriv)) != NULL)
		rtw_free_cmd_obj(pcmd);
	rtw_thread_wait_stop();
	return 0;
}
