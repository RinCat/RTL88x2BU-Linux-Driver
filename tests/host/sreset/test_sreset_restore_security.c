// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_sreset_security_types.h"

struct expect {
	const char *name;
	u32 auth_algo;
	u32 privacy;
	u8 grp_keyid;
	u8 sta_present;
	u8 expect_sec_cfg;
	unsigned expect_stakey;
	unsigned expect_set_key;
};

static _adapter g_adapter;

static const struct expect cases[] = {
	{ "open_no_keys", 0, 0, 0, 0, 0xcf, 0, 0 },
	{ "8021x_no_keys", dot11AuthAlgrthm_8021X, 0, 0, 0, 0xcc, 0, 0 },
	{ "aes_with_sta", 0, _AES_, 1, 1, 0xcf, 1, 1 },
	{ "tkip_no_sta", 0, _TKIP_, 2, 0, 0xcf, 0, 0 },
};

static int run_expect(const struct expect *e)
{
	struct host_sreset_security_trace *tr;

	memset(&g_adapter, 0, sizeof(g_adapter));
	host_sreset_security_reset_trace();
	host_sreset_security_set_sta_present(e->sta_present);
	g_adapter.mlmeextpriv.mlmext_info.auth_algo = e->auth_algo;
	g_adapter.securitypriv.dot11PrivacyAlgrthm = e->privacy;
	g_adapter.securitypriv.dot118021XGrpKeyid = e->grp_keyid;
	sreset_restore_security_station(&g_adapter);
	tr = host_sreset_security_get_trace();
	if (tr->last_sec_cfg != e->expect_sec_cfg ||
	    tr->setstakey_calls != e->expect_stakey ||
	    tr->set_key_calls != e->expect_set_key) {
		fprintf(stderr, "FAIL %s\n", e->name);
		return 1;
	}
	printf("PASS %s\n", e->name);
	return 0;
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
