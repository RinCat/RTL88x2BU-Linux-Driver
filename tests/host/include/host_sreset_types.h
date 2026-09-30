/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_SRESET_TYPES_H
#define HOST_SRESET_TYPES_H

#include "host_types.h"
#define _TRUE 1
#define _FALSE 0
#define REG_TXDMA_STATUS 0x0210u
#define WIFI_STATUS_SUCCESS 0u
#define USB_READ_PORT_FAIL 2u
#define USB_WRITE_PORT_FAIL 4u
#define WIFI_MAC_TXDMA_ERROR 8u
#define WIFI_IF_NOT_EXIST 64u

typedef u32 systime;

struct sreset_priv {
	u8 silent_reset_inprogress;
	u8 Wifi_Error_Status;
	systime last_tx_time;
	systime last_tx_complete_time;
};

struct hal_data {
	struct sreset_priv srestpriv;
};

struct _adapter {
	struct hal_data HalData;
};

typedef struct _adapter _adapter;
typedef _adapter *PADAPTER;

#define GET_HAL_DATA(a) (&(a)->HalData)

void host_sreset_set_reg_read(u32 addr, u32 val);
void host_sreset_clear_reg_reads(void);
u32 rtw_read32(PADAPTER padapter, u32 addr);
void sreset_init_value(PADAPTER padapter);
void sreset_reset_value(PADAPTER padapter);
u8 sreset_get_wifi_status(PADAPTER padapter);
void sreset_set_wifi_error_status(PADAPTER padapter, u32 status);
u8 sreset_inprogress(PADAPTER padapter);

#endif
