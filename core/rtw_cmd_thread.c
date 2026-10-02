/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#define _RTW_CMD_THREAD_C_

#ifdef HOST_CMD_THREAD_TEST
#include "host_cmd_thread_types.h"
#define RTW_CMDTABLE_SIZE HOST_CMD_WLANCMDS_SIZE
#define HOST_CMD_THREAD_LOOP_BREAK() \
	do { \
		if (!host_cmd_thread_loop_continue()) \
			goto host_cmd_thread_exit; \
	} while (0)
#else
#include <drv_types.h>
#include <hal_data.h>
#endif

#ifndef DBG_CMD_EXECUTE
#define DBG_CMD_EXECUTE 0
#endif

#if !defined(CONFIG_RUST) || defined(HOST_CMD_THREAD_TEST) || !defined(CONFIG_RUST_CMD_THREAD)

void rtw_cmd_clr_isr(struct cmd_priv *pcmdpriv)
{
	pcmdpriv->cmd_done_cnt++;
#ifndef HOST_CMD_THREAD_TEST
	/* _rtw_up_sema(&(pcmdpriv->cmd_done_sema)); */
#endif
}

void rtw_stop_cmd_thread(_adapter *adapter)
{
	if (adapter->cmdThread) {
		_rtw_up_sema(&adapter->cmdpriv.cmd_queue_sema);
		rtw_thread_stop(adapter->cmdThread);
		adapter->cmdThread = NULL;
	}
}

#ifdef HOST_CMD_THREAD_TEST

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

#else /* !HOST_CMD_THREAD_TEST */

extern struct rtw_cmd wlancmds[];
int rtw_cmd_filter(struct cmd_priv *pcmdpriv, struct cmd_obj *cmd_obj);

#define RTW_WLANCMDS_NUM ((unsigned int)CMD_ID_MAX)

thread_return rtw_cmd_thread(thread_context context)
{
	u8 ret;
	struct cmd_obj *pcmd;
	u8 *pcmdbuf, *prspbuf;
	systime cmd_start_time;
	u32 cmd_process_time;
	u8(*cmd_hdl)(_adapter *padapter, u8 *pbuf);
	void (*pcmd_callback)(_adapter *dev, struct cmd_obj *pcmd);
	PADAPTER padapter = (PADAPTER)context;
	struct cmd_priv *pcmdpriv = &(padapter->cmdpriv);
	struct drvextra_cmd_parm *extra_parm = NULL;
	_irqL irqL;

	thread_enter("RTW_CMD_THREAD");

	pcmdbuf = pcmdpriv->cmd_buf;
	prspbuf = pcmdpriv->rsp_buf;
	ATOMIC_SET(&(pcmdpriv->cmdthd_running), _TRUE);
	_rtw_up_sema(&pcmdpriv->start_cmdthread_sema);


	while (1) {
		if (_rtw_down_sema(&pcmdpriv->cmd_queue_sema) == _FAIL) {
			RTW_PRINT(FUNC_ADPT_FMT" _rtw_down_sema(&pcmdpriv->cmd_queue_sema) return _FAIL, break\n", FUNC_ADPT_ARG(padapter));
			break;
		}

		if (RTW_CANNOT_RUN(padapter)) {
			RTW_DBG(FUNC_ADPT_FMT "- bDriverStopped(%s) bSurpriseRemoved(%s)\n",
				FUNC_ADPT_ARG(padapter),
				rtw_is_drv_stopped(padapter) ? "True" : "False",
				rtw_is_surprise_removed(padapter) ? "True" : "False");
			break;
		}

		_enter_critical(&pcmdpriv->cmd_queue.lock, &irqL);
		if (rtw_is_list_empty(&(pcmdpriv->cmd_queue.queue))) {
			/* RTW_INFO("%s: cmd queue is empty!\n", __func__); */
			_exit_critical(&pcmdpriv->cmd_queue.lock, &irqL);
			continue;
		}
		_exit_critical(&pcmdpriv->cmd_queue.lock, &irqL);

_next:
		if (RTW_CANNOT_RUN(padapter)) {
			RTW_PRINT("%s: DriverStopped(%s) SurpriseRemoved(%s) break at line %d\n",
				  __func__
				, rtw_is_drv_stopped(padapter) ? "True" : "False"
				, rtw_is_surprise_removed(padapter) ? "True" : "False"
				  , __LINE__);
			break;
		}

		pcmd = rtw_dequeue_cmd(pcmdpriv);
		if (!pcmd) {
#ifdef CONFIG_LPS_LCLK
			rtw_unregister_cmd_alive(padapter);
#endif
			continue;
		}

		cmd_start_time = rtw_get_current_time();
		pcmdpriv->cmd_issued_cnt++;

		if (pcmd->cmdsz > MAX_CMDSZ) {
			RTW_ERR("%s cmdsz:%d > MAX_CMDSZ:%d\n", __func__, pcmd->cmdsz, MAX_CMDSZ);
			pcmd->res = H2C_PARAMETERS_ERROR;
			goto post_process;
		}

		if (pcmd->cmdcode >= RTW_WLANCMDS_NUM) {
			RTW_ERR("%s undefined cmdcode:%d\n", __func__, pcmd->cmdcode);
			pcmd->res = H2C_PARAMETERS_ERROR;
			goto post_process;
		}

		cmd_hdl = wlancmds[pcmd->cmdcode].cmd_hdl;
		if (!cmd_hdl) {
			RTW_ERR("%s no cmd_hdl for cmdcode:%d\n", __func__, pcmd->cmdcode);
			pcmd->res = H2C_PARAMETERS_ERROR;
			goto post_process;
		}

		if (_FAIL == rtw_cmd_filter(pcmdpriv, pcmd)) {
			pcmd->res = H2C_DROPPED;
			if (pcmd->cmdcode == CMD_SET_DRV_EXTRA) {
				extra_parm = (struct drvextra_cmd_parm *)pcmd->parmbuf;
				if (extra_parm && extra_parm->pbuf && extra_parm->size > 0)
					rtw_mfree(extra_parm->pbuf, extra_parm->size);
			}
			#if CONFIG_DFS
			else if (pcmd->cmdcode == CMD_SET_CHANSWITCH)
				adapter_to_rfctl(padapter)->csa_ch = 0;
			#endif
			goto post_process;
		}

#ifdef CONFIG_LPS_LCLK
		if (pcmd->no_io)
			rtw_unregister_cmd_alive(padapter);
		else {
			if (rtw_register_cmd_alive(padapter) != _SUCCESS) {
				if (DBG_CMD_EXECUTE)
					RTW_PRINT("%s: wait to leave LPS_LCLK\n", __func__);

				pcmd->res = H2C_ENQ_HEAD;
				ret = _rtw_enqueue_cmd(&pcmdpriv->cmd_queue, pcmd, 1);
				if (ret == _SUCCESS) {
					if (DBG_CMD_EXECUTE)
						RTW_INFO(ADPT_FMT" "CMD_FMT" ENQ_HEAD\n", ADPT_ARG(pcmd->padapter), CMD_ARG(pcmd));
					continue;
				}

				RTW_INFO(ADPT_FMT" "CMD_FMT" ENQ_HEAD_FAIL\n", ADPT_ARG(pcmd->padapter), CMD_ARG(pcmd));
				pcmd->res = H2C_ENQ_HEAD_FAIL;
				rtw_warn_on(1);
			}
		}
#endif /* CONFIG_LPS_LCLK */

		if (DBG_CMD_EXECUTE)
			RTW_INFO(ADPT_FMT" "CMD_FMT" %sexecute\n", ADPT_ARG(pcmd->padapter), CMD_ARG(pcmd)
				, pcmd->res == H2C_ENQ_HEAD ? "ENQ_HEAD " : (pcmd->res == H2C_ENQ_HEAD_FAIL ? "ENQ_HEAD_FAIL " : ""));

		_rtw_memcpy(pcmdbuf, pcmd->parmbuf, pcmd->cmdsz);
		ret = cmd_hdl(pcmd->padapter, pcmdbuf);
		pcmd->res = ret;

		pcmdpriv->cmd_seq++;

post_process:

		_enter_critical_mutex(&(pcmd->padapter->cmdpriv.sctx_mutex), NULL);
		if (pcmd->sctx) {
			if (0)
				RTW_PRINT(FUNC_ADPT_FMT" pcmd->sctx\n", FUNC_ADPT_ARG(pcmd->padapter));
			if (pcmd->res == H2C_SUCCESS)
				rtw_sctx_done(&pcmd->sctx);
			else
				rtw_sctx_done_err(&pcmd->sctx, RTW_SCTX_DONE_CMD_ERROR);
		}
		_exit_critical_mutex(&(pcmd->padapter->cmdpriv.sctx_mutex), NULL);

		cmd_process_time = rtw_get_passing_time_ms(cmd_start_time);
		if (cmd_process_time > 1000) {
			RTW_INFO(ADPT_FMT" "CMD_FMT" process_time=%d\n", ADPT_ARG(pcmd->padapter), CMD_ARG(pcmd), cmd_process_time);
			if (0)
				rtw_warn_on(1);
		}

		/* call callback function for post-processed */
		if (pcmd->cmdcode < RTW_WLANCMDS_NUM)
			pcmd_callback = wlancmds[pcmd->cmdcode].callback;
		else
			pcmd_callback = NULL;

		if (pcmd_callback == NULL) {
			rtw_free_cmd_obj(pcmd);
		} else {
			/* todo: !!! fill rsp_buf to pcmd->rsp if (pcmd->rsp!=NULL) */
			pcmd_callback(pcmd->padapter, pcmd);/* need conider that free cmd_obj in rtw_cmd_callback */
		}

		flush_signals_thread();

		goto _next;

	}

#ifdef CONFIG_LPS_LCLK
	rtw_unregister_cmd_alive(padapter);
#endif

	/* to avoid enqueue cmd after free all cmd_obj */
	ATOMIC_SET(&(pcmdpriv->cmdthd_running), _FALSE);

	/* free all cmd_obj resources */
	do {
		pcmd = rtw_dequeue_cmd(pcmdpriv);
		if (pcmd == NULL)
			break;

		if (0)
			RTW_INFO("%s: leaving... drop "CMD_FMT"\n", __func__, CMD_ARG(pcmd));

		if (pcmd->cmdcode == CMD_SET_DRV_EXTRA) {
			extra_parm = (struct drvextra_cmd_parm *)pcmd->parmbuf;
			if (extra_parm->pbuf && extra_parm->size > 0)
				rtw_mfree(extra_parm->pbuf, extra_parm->size);
		}
		#if CONFIG_DFS
		else if (pcmd->cmdcode == CMD_SET_CHANSWITCH)
			adapter_to_rfctl(padapter)->csa_ch = 0;
		#endif

		_enter_critical_mutex(&(pcmd->padapter->cmdpriv.sctx_mutex), NULL);
		if (pcmd->sctx) {
			if (0)
				RTW_PRINT(FUNC_ADPT_FMT" pcmd->sctx\n", FUNC_ADPT_ARG(pcmd->padapter));
			rtw_sctx_done_err(&pcmd->sctx, RTW_SCTX_DONE_CMD_DROP);
		}
		_exit_critical_mutex(&(pcmd->padapter->cmdpriv.sctx_mutex), NULL);

		rtw_free_cmd_obj(pcmd);
	} while (1);

	RTW_INFO(FUNC_ADPT_FMT " Exit\n", FUNC_ADPT_ARG(padapter));

	rtw_thread_wait_stop();

	return 0;
}

#endif /* HOST_CMD_THREAD_TEST */

#endif /* !CONFIG_RUST || HOST_CMD_THREAD_TEST || !CONFIG_RUST_CMD_THREAD */
