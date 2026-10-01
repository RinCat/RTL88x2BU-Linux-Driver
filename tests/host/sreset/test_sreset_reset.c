// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_sreset_reset_types.h"

static struct hal_data_type g_hal;
static struct dvobj_priv g_dvobj;
static _adapter g_adapter;

static int check_basic_reset(void)
{
	struct host_sreset_reset_trace *tr;

	memset(&g_hal, 0, sizeof(g_hal));
	memset(&g_dvobj, 0, sizeof(g_dvobj));
	memset(&g_adapter, 0, sizeof(g_adapter));
	g_adapter.HalData = &g_hal;
	g_adapter.dvobj = &g_dvobj;
	host_sreset_reset_reset_trace();

	sreset_reset(&g_adapter);

	tr = host_sreset_reset_get_trace();
	if (tr->ps_mode_active != 1 || tr->pwrlock_enter != 1 || tr->pwrlock_exit != 1 ||
	    tr->mi_stop != 1 || tr->mi_start != 1 || tr->ips_enter != 1 ||
	    tr->ips_leave != 1 || tr->ap_restore != 1 || tr->inprogress_after_stop != 1) {
		fprintf(stderr, "FAIL basic_reset trace\n");
		return 1;
	}
	if (g_hal.srestpriv.silent_reset_inprogress || g_hal.srestpriv.Wifi_Error_Status ||
	    g_hal.srestpriv.self_dect_fw || g_hal.srestpriv.rx_cnt ||
	    g_adapter.pwrctl_priv.change_rfpwrstate != rf_off ||
	    g_dvobj.drv_dbg.dbg_sreset_cnt != 1) {
		fprintf(stderr, "FAIL basic_reset state\n");
		return 1;
	}
	printf("PASS basic_reset\n");
	return 0;
}

int main(void)
{
	int bad = check_basic_reset();

	if (!bad)
		printf("PASS 1 vectors (embedded)\n");
	return bad ? 1 : 0;
}
