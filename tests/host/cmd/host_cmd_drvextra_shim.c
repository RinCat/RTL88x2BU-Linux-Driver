// SPDX-License-Identifier: GPL-2.0
#include <stdlib.h>
#include "host_cmd_drvextra_types.h"

static struct host_drvextra_trace g_trace;

void host_drvextra_reset(struct host_drvextra_trace *t)
{
	if (t)
		*t = g_trace = (struct host_drvextra_trace){0};
	else
		g_trace = (struct host_drvextra_trace){0};
}

void *rtw_zmalloc(u32 sz) { return calloc(1, sz); }
void rtw_mfree(u8 *p, u32 sz) { (void)sz; g_trace.mfree++; free(p); }

void rtw_dynamic_chk_wk_hdl(PADAPTER a) { (void)a; g_trace.dynamic++; }
void reset_securitypriv_hdl(PADAPTER a) { (void)a; g_trace.reset_sec++; }
void free_assoc_resources_hdl(PADAPTER a, u8 type)
{
	(void)a;
	(void)type;
	g_trace.free_assoc++;
}

void rtw_chk_hi_queue_hdl(PADAPTER a) { (void)a; g_trace.hiq++; }

struct host_drvextra_trace *host_drvextra_get_trace(void) { return &g_trace; }
