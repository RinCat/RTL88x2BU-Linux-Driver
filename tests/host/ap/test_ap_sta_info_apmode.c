// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_ap_sta_info_apmode_types.h"
#include "host_vector_json.h"

struct vector {
	char name[64];
	u32 dot11_auth, expect_state;
	u8 ht_option, sta_cap, ap_cap, ap_ampdu_en, sta_ampdu_para;
	u8 cur_bwmode, cur_ch_offset, ht_40_intol, op_present, ht_op_sta_width;
	u8 expect_8021x_blocked, expect_bw, expect_ampdu_en, expect_min_spacing;
	u8 expect_sgi_20, expect_sgi_40, expect_qos, expect_delba;
	u8 preset_ldpc_cap, preset_stbc_cap, expect_ldpc_cap, expect_stbc_cap;
};

static int parse_int(const char *obj, size_t len, const char *key, int *out)
{
	return host_json_parse_int_in(obj, len, key, out);
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int t;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
#define P8(f) if (parse_int(obj, len, #f, &t)) return -1; v->f = (u8)t
#define P16(f) if (parse_int(obj, len, #f, &t)) return -1; v->f = (u16)t
#define P32(f) if (parse_int(obj, len, #f, &t)) return -1; v->f = (u32)t
	P32(dot11_auth);
	P8(ht_option);
	P16(sta_cap);
	P16(ap_cap);
	P8(ap_ampdu_en);
	P8(sta_ampdu_para);
	P8(cur_bwmode);
	P8(cur_ch_offset);
	P8(ht_40_intol);
	P8(op_present);
	P8(ht_op_sta_width);
	P8(expect_8021x_blocked);
	P8(expect_bw);
	P8(expect_ampdu_en);
	P8(expect_min_spacing);
	P8(expect_sgi_20);
	P8(expect_sgi_40);
	P8(expect_qos);
	P32(expect_state);
	P8(expect_delba);
	P8(preset_ldpc_cap);
	P8(preset_stbc_cap);
	P8(expect_ldpc_cap);
	P8(expect_stbc_cap);
	return 0;
}

static int run_vector(const struct vector *v)
{
	_adapter adapter;
	struct sta_info sta;

	host_apmode_reset();
	host_apmode_set_ra_sgi(1);
	memset(&adapter, 0, sizeof(adapter));
	memset(&sta, 0, sizeof(sta));
	adapter.securitypriv.dot11AuthAlgrthm = v->dot11_auth;
	adapter.mlmepriv.htpriv.ampdu_enable = v->ap_ampdu_en;
	adapter.mlmepriv.htpriv.ht_cap.cap_info = v->ap_cap;
	adapter.mlmeextpriv.cur_bwmode = v->cur_bwmode;
	adapter.mlmeextpriv.cur_ch_offset = v->cur_ch_offset;
	sta.htpriv.ht_option = v->ht_option;
	sta.htpriv.ht_cap.cap_info = v->sta_cap;
	sta.htpriv.ht_cap.ampdu_params_info = v->sta_ampdu_para;
	sta.htpriv.op_present = v->op_present;
	sta.htpriv.ht_op[1] = v->ht_op_sta_width ? (1U << 2) : 0;
	sta.ht_40mhz_intolerant = v->ht_40_intol;
	sta.htpriv.ldpc_cap = v->preset_ldpc_cap;
	sta.htpriv.stbc_cap = v->preset_stbc_cap;
	sta.sta_stats.rx_data_pkts = 99;
	update_sta_info_apmode(&adapter, &sta);
	if (sta.ieee8021x_blocked != v->expect_8021x_blocked ||
	    sta.cmn.bw_mode != v->expect_bw ||
	    sta.htpriv.ampdu_enable != v->expect_ampdu_en ||
	    sta.htpriv.rx_ampdu_min_spacing != v->expect_min_spacing ||
	    sta.htpriv.sgi_20m != v->expect_sgi_20 ||
	    sta.htpriv.sgi_40m != v->expect_sgi_40 ||
	    sta.qos_option != v->expect_qos ||
	    sta.state != v->expect_state ||
	    host_apmode_delba_calls() != v->expect_delba ||
	    sta.htpriv.ldpc_cap != v->expect_ldpc_cap ||
	    sta.htpriv.stbc_cap != v->expect_stbc_cap ||
	    sta.sta_stats.rx_data_pkts != 0 ||
	    sta.cmn.ra_info.is_support_sgi != 1) {
		fprintf(stderr, "FAIL %s\n", v->name);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector v[16];
	size_t n = 0, i, fail = 0;

	if (argc != 2 ||
	    host_load_vectors(argv[1], v, sizeof(v[0]), 16, parse_vector_object, &n))
		return 2;
	for (i = 0; i < n; i++)
		if (run_vector(&v[i]))
			fail++;
	printf("%zu vectors, %zu failures\n", n, fail);
	return fail ? 1 : 0;
}
