// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_ap_bcn_update_types.h"
#include "host_vector_json.h"

extern u8 host_bcn_update_last_erp_byte;
extern u16 host_bcn_update_last_ht_op_mode;
extern u8 host_bcn_update_last_ht_info_byte;
extern u32 host_bcn_update_last_ielen;

struct vector {
	char name[64];
	char fn[48];
	u8 erp_enable;
	int num_sta_non_erp, num_sta_no_short_preamble;
	u8 ht_option, HT_info_enable, ht_20mhz_width_req, ht_intolerant_ch_reported;
	u8 sw_to_20mhz, cur_channel, cur_bwmode, cur_ch_offset;
	int num_sta_40mhz_intolerant, olbc;
	int ht_op_mode;
	int expect_erp_byte;
	int expect_ht_op_mode;
	int expect_ht_info_byte;
	int has_expect_ht;
	int has_expect_ht_info;
	int expect_ielen;
	int has_expect_ielen;
	int expect_vendor_kind;
	int has_expect_vendor_kind;
	u8 oui[4];
	int has_oui;
	u8 wps_beacon_ie[64];
	size_t wps_beacon_ie_len;
	u8 ies[MAX_IE_SZ];
	size_t ies_len;
};

static int parse_hex(const char *hex, u8 *out, size_t cap, size_t *len)
{
	size_t n = strlen(hex), i;

	if (n % 2 || (*len = n / 2) > cap)
		return -1;
	for (i = 0; i < *len; i++) {
		unsigned v;

		if (sscanf(hex + i * 2, "%2x", &v) != 1)
			return -1;
		out[i] = (u8)v;
	}
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	char wps_hex[HOST_VECTOR_MAX_HEX_BUF];
	char oui_hex[16];
	int tmp;

	memset(v, 0, sizeof(*v));
	v->expect_ht_op_mode = -1;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)) ||
	    host_json_parse_string_in(obj, len, "fn", v->fn, sizeof(v->fn)) ||
	    host_json_parse_string_in(obj, len, "ies_hex", hex, sizeof(hex)))
		return -1;
	if (!host_json_parse_int_in(obj, len, "erp_enable", &tmp))
		v->erp_enable = (u8)tmp;
	host_json_parse_int_in(obj, len, "num_sta_non_erp", &v->num_sta_non_erp);
	host_json_parse_int_in(obj, len, "num_sta_no_short_preamble",
			      &v->num_sta_no_short_preamble);
	if (!host_json_parse_int_in(obj, len, "ht_option", &tmp))
		v->ht_option = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "HT_info_enable", &tmp))
		v->HT_info_enable = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "ht_20mhz_width_req", &tmp))
		v->ht_20mhz_width_req = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "ht_intolerant_ch_reported", &tmp))
		v->ht_intolerant_ch_reported = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "sw_to_20mhz", &tmp))
		v->sw_to_20mhz = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "cur_channel", &tmp))
		v->cur_channel = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "cur_bwmode", &tmp))
		v->cur_bwmode = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "cur_ch_offset", &tmp))
		v->cur_ch_offset = (u8)tmp;
	host_json_parse_int_in(obj, len, "num_sta_40mhz_intolerant",
			      &v->num_sta_40mhz_intolerant);
	host_json_parse_int_in(obj, len, "olbc", &v->olbc);
	host_json_parse_int_in(obj, len, "ht_op_mode", &v->ht_op_mode);
	if (!strcmp(v->fn, "update_bcn_erpinfo_ie") &&
	    host_json_parse_int_in(obj, len, "expect_erp_byte", &v->expect_erp_byte))
		return -1;
	if (!strcmp(v->fn, "update_bcn_htinfo_ie"))
		host_json_parse_int_in(obj, len, "expect_erp_byte", &v->expect_erp_byte);
	else if (strcmp(v->fn, "update_bcn_erpinfo_ie"))
		host_json_parse_int_in(obj, len, "expect_erp_byte", &v->expect_erp_byte);
	if (!host_json_parse_int_in(obj, len, "expect_ht_op_mode", &tmp)) {
		v->expect_ht_op_mode = tmp;
		v->has_expect_ht = 1;
	}
	if (!host_json_parse_int_in(obj, len, "expect_ht_info_byte", &tmp)) {
		v->expect_ht_info_byte = tmp;
		v->has_expect_ht_info = 1;
	}
	if (!host_json_parse_int_in(obj, len, "expect_ielen", &tmp)) {
		v->expect_ielen = tmp;
		v->has_expect_ielen = 1;
	}
	if (!host_json_parse_int_in(obj, len, "expect_vendor_kind", &tmp)) {
		v->expect_vendor_kind = tmp;
		v->has_expect_vendor_kind = 1;
	}
	if (!host_json_parse_string_in(obj, len, "oui_hex", oui_hex,
				       sizeof(oui_hex))) {
		size_t oui_len = 0;

		if (parse_hex(oui_hex, v->oui, sizeof(v->oui), &oui_len) ||
		    oui_len != 4)
			return -1;
		v->has_oui = 1;
	}
	if (!host_json_parse_string_in(obj, len, "wps_beacon_ie_hex", wps_hex,
				       sizeof(wps_hex)) &&
	    parse_hex(wps_hex, v->wps_beacon_ie, sizeof(v->wps_beacon_ie),
		      &v->wps_beacon_ie_len))
		return -1;
	return parse_hex(hex, v->ies, sizeof(v->ies), &v->ies_len);
}

static void setup_adapter(struct vector *v, _adapter *ad)
{
	WLAN_BSSID_EX *net = &ad->mlmeextpriv.mlmext_info.network;

	memset(ad, 0, sizeof(*ad));
	ad->mlmeextpriv.mlmext_info.ERP_enable = v->erp_enable;
	ad->mlmeextpriv.mlmext_info.HT_info_enable = v->HT_info_enable;
	ad->mlmepriv.num_sta_non_erp = v->num_sta_non_erp;
	ad->mlmepriv.num_sta_no_short_preamble = v->num_sta_no_short_preamble;
	ad->mlmepriv.htpriv.ht_option = v->ht_option ? _TRUE : _FALSE;
	ad->mlmepriv.ht_op_mode = (u16)v->ht_op_mode;
	ad->mlmepriv.num_sta_40mhz_intolerant = v->num_sta_40mhz_intolerant;
	ad->mlmepriv.ht_20mhz_width_req = v->ht_20mhz_width_req ? _TRUE : _FALSE;
	ad->mlmepriv.ht_intolerant_ch_reported =
		v->ht_intolerant_ch_reported ? _TRUE : _FALSE;
	ad->mlmepriv.olbc = v->olbc;
	ad->mlmepriv.sw_to_20mhz = v->sw_to_20mhz;
	ad->mlmeextpriv.cur_channel = v->cur_channel ? v->cur_channel : 6;
	ad->mlmeextpriv.cur_bwmode = v->cur_bwmode;
	ad->mlmeextpriv.cur_ch_offset = v->cur_ch_offset;
	ad->mlmepriv.wps_beacon_ie =
		v->wps_beacon_ie_len ? v->wps_beacon_ie : NULL;
	memcpy(net->IEs, v->ies, v->ies_len);
	net->IELength = (u32)v->ies_len;
}

static int run_vector(struct vector *v)
{
	_adapter ad;

	setup_adapter(v, &ad);
	if (!strcmp(v->fn, "update_bcn_erpinfo_ie")) {
		host_bcn_update_last_erp_byte = 0;
		update_bcn_erpinfo_ie(&ad);
		if ((int)host_bcn_update_last_erp_byte != v->expect_erp_byte) {
			fprintf(stderr, "FAIL %s erp\n", v->name);
			return -1;
		}
		return 0;
	}
	if (!strcmp(v->fn, "update_bcn_htinfo_ie")) {
		host_bcn_update_last_ht_op_mode = 0xffff;
		update_bcn_htinfo_ie(&ad);
		if (v->has_expect_ht &&
		    (int)host_bcn_update_last_ht_op_mode != v->expect_ht_op_mode) {
			fprintf(stderr, "FAIL %s ht got 0x%x want 0x%x\n", v->name,
				host_bcn_update_last_ht_op_mode, v->expect_ht_op_mode);
			return -1;
		}
		if (v->has_expect_ht_info &&
		    (int)host_bcn_update_last_ht_info_byte != v->expect_ht_info_byte) {
			fprintf(stderr, "FAIL %s ht_info got 0x%x want 0x%x\n", v->name,
				host_bcn_update_last_ht_info_byte, v->expect_ht_info_byte);
			return -1;
		}
		return 0;
	}
	if (!strcmp(v->fn, "update_bcn_wps_ie")) {
		host_bcn_update_last_ielen = 0;
		update_bcn_wps_ie(&ad);
		if (v->has_expect_ielen &&
		    (int)host_bcn_update_last_ielen != v->expect_ielen) {
			fprintf(stderr, "FAIL %s wps ielen got %u want %d\n",
				v->name, host_bcn_update_last_ielen,
				v->expect_ielen);
			return -1;
		}
		return 0;
	}
	if (!strcmp(v->fn, "update_bcn_vendor_spec_ie")) {
		if (!v->has_oui) {
			fprintf(stderr, "FAIL %s missing oui_hex\n", v->name);
			return -1;
		}
		host_bcn_update_last_vendor_kind = 0;
		host_bcn_update_last_ielen = 0;
		update_bcn_vendor_spec_ie(&ad, v->oui);
		if (v->has_expect_vendor_kind &&
		    (int)host_bcn_update_last_vendor_kind != v->expect_vendor_kind) {
			fprintf(stderr, "FAIL %s vendor kind got %u want %d\n",
				v->name, host_bcn_update_last_vendor_kind,
				v->expect_vendor_kind);
			return -1;
		}
		if (v->has_expect_ielen &&
		    (int)host_bcn_update_last_ielen != v->expect_ielen) {
			fprintf(stderr, "FAIL %s vendor wps ielen got %u want %d\n",
				v->name, host_bcn_update_last_ielen,
				v->expect_ielen);
			return -1;
		}
		return 0;
	}
	fprintf(stderr, "FAIL %s unknown fn %s\n", v->name, v->fn);
	return -1;
}

int main(int argc, char **argv)
{
	struct vector v[32];
	size_t n = 0, i, fail = 0;

	if (argc != 2 ||
	    host_load_vectors(argv[1], v, sizeof(v[0]), 32, parse_vector_object, &n))
		return 2;
	for (i = 0; i < n; i++)
		if (run_vector(&v[i]))
			fail++;
	printf("%zu vectors, %zu failures\n", n, fail);
	return fail ? 1 : 0;
}
