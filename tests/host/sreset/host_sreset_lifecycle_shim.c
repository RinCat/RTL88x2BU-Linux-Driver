// SPDX-License-Identifier: GPL-2.0
#include "host_sreset_types.h"

static u32 g_reg_txdma;

void host_sreset_set_reg_read(u32 addr, u32 val)
{
	if (addr == REG_TXDMA_STATUS)
		g_reg_txdma = val;
}

void host_sreset_clear_reg_reads(void)
{
	g_reg_txdma = 0;
}

u32 rtw_read32(PADAPTER padapter, u32 addr)
{
	(void)padapter;
	return addr == REG_TXDMA_STATUS ? g_reg_txdma : 0;
}

void sreset_init_value(PADAPTER padapter)
{
	struct sreset_priv *p = &GET_HAL_DATA(padapter)->srestpriv;

	p->silent_reset_inprogress = _FALSE;
	p->Wifi_Error_Status = WIFI_STATUS_SUCCESS;
	p->last_tx_time = 0;
	p->last_tx_complete_time = 0;
}

void sreset_reset_value(PADAPTER padapter)
{
	struct sreset_priv *p = &GET_HAL_DATA(padapter)->srestpriv;

	p->Wifi_Error_Status = WIFI_STATUS_SUCCESS;
	p->last_tx_time = 0;
	p->last_tx_complete_time = 0;
}

u8 sreset_get_wifi_status(PADAPTER padapter)
{
	struct sreset_priv *p = &GET_HAL_DATA(padapter)->srestpriv;
	u8 status = WIFI_STATUS_SUCCESS;
	u32 val32;

	if (p->silent_reset_inprogress == _TRUE)
		return status;
	val32 = rtw_read32(padapter, REG_TXDMA_STATUS);
	if (val32 == 0xeaeaeaea)
		p->Wifi_Error_Status = WIFI_IF_NOT_EXIST;
	else if (val32)
		p->Wifi_Error_Status = WIFI_MAC_TXDMA_ERROR;
	if (p->Wifi_Error_Status != WIFI_STATUS_SUCCESS)
		status = p->Wifi_Error_Status & (~(USB_READ_PORT_FAIL | USB_WRITE_PORT_FAIL));
	p->Wifi_Error_Status = WIFI_STATUS_SUCCESS;
	return status;
}

void sreset_set_wifi_error_status(PADAPTER padapter, u32 status)
{
	GET_HAL_DATA(padapter)->srestpriv.Wifi_Error_Status = (u8)status;
}

u8 sreset_inprogress(PADAPTER padapter)
{
	return GET_HAL_DATA(padapter)->srestpriv.silent_reset_inprogress;
}
