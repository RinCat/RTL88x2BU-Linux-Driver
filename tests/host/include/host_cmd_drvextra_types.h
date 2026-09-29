/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_CMD_DRVEXTRA_TYPES_H
#define HOST_CMD_DRVEXTRA_TYPES_H

#include "host_types.h"

#define H2C_SUCCESS 0
#define H2C_PARAMETERS_ERROR 4
#define RTW_ERR(...) do { } while (0)
#define rtw_warn_on(c) ((void)0)

struct drvextra_cmd_parm {
	int ec_id, type, size;
	u8 *pbuf;
};

struct _adapter { u8 pad; };
typedef struct _adapter _adapter;
typedef struct _adapter *PADAPTER;

struct host_drvextra_trace {
	int mfree, dynamic, reset_sec, free_assoc, hiq;
};

void host_drvextra_reset(struct host_drvextra_trace *t);
struct host_drvextra_trace *host_drvextra_get_trace(void);
void *rtw_zmalloc(u32 sz);
void rtw_mfree(u8 *p, u32 sz);
void rtw_dynamic_chk_wk_hdl(PADAPTER a);
void reset_securitypriv_hdl(PADAPTER a);
void free_assoc_resources_hdl(PADAPTER a, u8 type);
void rtw_chk_hi_queue_hdl(PADAPTER a);
u8 rtw_drvextra_cmd_hdl(PADAPTER padapter, unsigned char *pbuf);

#endif /* HOST_CMD_DRVEXTRA_TYPES_H */
