// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_sreset_adapter_types.h"

struct expect {
	const char *name;
	const char *op;
	u32 fw_state;
	u8 adapter_type;
	unsigned netif_stop, netif_wake, cancel_timers, tasklet_kill;
	unsigned tasklet_schedule, scan_abort, set_to_roam, join_timeout;
	unsigned restore_network;
	int timer_ms;
};

static struct net_device g_netdev;
static _adapter g_adapter;

static const struct expect cases[] = {
	{ "stop_idle", "stop", 0, PRIMARY_ADAPTER, 1, 0, 1, 1, 0, 0, 0, 0, 0, -1 },
	{ "stop_survey", "stop", WIFI_UNDER_SURVEY, PRIMARY_ADAPTER, 1, 0, 1, 1, 0, 1, 0, 0, 0, -1 },
	{ "stop_linking", "stop", WIFI_UNDER_LINKING, PRIMARY_ADAPTER, 1, 0, 1, 1, 0, 0, 1, 1, 0, -1 },
	{ "stop_survey_linking", "stop", WIFI_UNDER_SURVEY | WIFI_UNDER_LINKING,
	  PRIMARY_ADAPTER, 1, 0, 1, 1, 0, 1, 1, 1, 0, -1 },
	{ "start_not_asoc_primary", "start", 0, PRIMARY_ADAPTER, 0, 1, 0, 0, 1, 0, 0, 0, 0, 2000 },
	{ "start_asoc_primary", "start", WIFI_ASOC_STATE, PRIMARY_ADAPTER, 0, 1, 0, 0, 1, 0, 0, 0, 1, 2000 },
	{ "start_not_asoc_virtual", "start", 0, VIRTUAL_ADAPTER, 0, 1, 0, 0, 1, 0, 0, 0, 0, -1 },
	{ "start_asoc_virtual", "start", WIFI_ASOC_STATE, VIRTUAL_ADAPTER, 0, 1, 0, 0, 1, 0, 0, 0, 1, -1 },
};

static int check_trace(const struct expect *e)
{
	struct host_sreset_adapter_trace *tr = host_sreset_adapter_get_trace();
#define CHK(f) \
	do { \
		if (tr->f != e->f) { \
			fprintf(stderr, "FAIL %s " #f " got=%u expect=%u\n", e->name, tr->f, e->f); \
			return 1; \
		} \
	} while (0)
	CHK(netif_stop);
	CHK(netif_wake);
	CHK(cancel_timers);
	CHK(tasklet_kill);
	CHK(tasklet_schedule);
	CHK(scan_abort);
	CHK(set_to_roam);
	CHK(join_timeout);
	CHK(restore_network);
	if (e->timer_ms >= 0 && tr->dynamic_chk_timer_ms != (unsigned)e->timer_ms) {
		fprintf(stderr, "FAIL %s timer got=%u expect=%d\n", e->name,
			tr->dynamic_chk_timer_ms, e->timer_ms);
		return 1;
	}
	printf("PASS %s\n", e->name);
	return 0;
#undef CHK
}

static int run_expect(const struct expect *e)
{
	memset(&g_adapter, 0, sizeof(g_adapter));
	g_adapter.pnetdev = &g_netdev;
	g_adapter.mlmepriv.fw_state = e->fw_state;
	g_adapter.adapter_type = e->adapter_type;
	host_sreset_adapter_reset_trace();
	if (!strcmp(e->op, "stop"))
		sreset_stop_adapter(&g_adapter);
	else
		sreset_start_adapter(&g_adapter);
	return check_trace(e);
}

int main(void)
{
	int bad = 0;
	for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++)
		bad += run_expect(&cases[i]);
	if (!bad)
		printf("PASS %zu vectors (embedded)\n", sizeof(cases) / sizeof(cases[0]));
	return bad ? 1 : 0;
}
