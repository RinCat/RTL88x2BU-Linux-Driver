// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle for rtw_vht_ies_attach (W3-84 PR7). */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vht_ies_attach_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 8
#define MAX_NAME 128

struct vector {
	char name[MAX_NAME];
	u8 reg_bw_mode;
	u8 hal_bw_cap;
	u8 channel;
	u8 initial_ies[HOST_VHT_IES_ATTACH_MAX_IE_SZ];
	u32 initial_ie_len;
	u32 expect_ie_len;
	u8 expect_vht_option;
};

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t decoded = 0;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, obj_len, "reg_bw_mode", &tmp))
		return -1;
	v->reg_bw_mode = (u8)tmp;
	if (host_json_parse_int_in(obj, obj_len, "hal_bw_cap", &tmp))
		return -1;
	v->hal_bw_cap = (u8)tmp;
	if (host_json_parse_int_in(obj, obj_len, "channel", &tmp))
		return -1;
	v->channel = (u8)tmp;
	if (host_json_parse_string_in(obj, obj_len, "initial_ies_hex", hex, sizeof(hex)))
		return -1;
	if (host_hex_decode(hex, v->initial_ies, sizeof(v->initial_ies), &decoded))
		return -1;
	if (host_json_parse_int_in(obj, obj_len, "initial_ie_len", &tmp))
		return -1;
	v->initial_ie_len = (u32)tmp;
	if (host_json_parse_int_in(obj, obj_len, "expect_ie_len", &tmp))
		return -1;
	v->expect_ie_len = (u32)tmp;
	if (host_json_parse_int_in(obj, obj_len, "expect_vht_option", &tmp))
		return -1;
	v->expect_vht_option = (u8)tmp;
	return 0;
}

static int run_vector(struct vector *v)
{
	WLAN_BSSID_EX net;
	sint vht_cap_len = 0;
	const u8 *vht_cap;

	memset(&host_vht_build_adapter, 0, sizeof(host_vht_build_adapter));
	memset(&net, 0, sizeof(net));
	host_vht_build_adapter.registrypriv.bw_mode = v->reg_bw_mode;
	host_vht_build_adapter.registrypriv.ampdu_factor = 7;
	host_vht_build_hal_bw_cap = v->hal_bw_cap;
	host_vht_build_adapter.host_fixture.rx_packet_offset = 128;
	host_vht_build_adapter.host_fixture.max_recvbuf_sz = 8192;
	host_vht_build_adapter.host_fixture.rx_stbc_nss = 1;
	host_vht_build_adapter.host_fixture.rx_nss = 1;
	host_vht_build_adapter.host_fixture.hal_bw_support[CHANNEL_WIDTH_80] = 1;

	net.IELength = v->initial_ie_len;
	memcpy(net.IEs, v->initial_ies, v->initial_ie_len);
	net.Configuration.DSConfig = v->channel;

	rtw_vht_ies_attach(&host_vht_build_adapter, &net);

	if (net.IELength != v->expect_ie_len ||
	    host_vht_build_adapter.mlmepriv.vhtpriv.vht_option != v->expect_vht_option) {
		fprintf(stderr, "FAIL %s len/option\n", v->name);
		return -1;
	}

	vht_cap = rtw_get_ie(net.IEs + _BEACON_IE_OFFSET_, EID_VHTCapability, &vht_cap_len,
			     (sint)(net.IELength - _BEACON_IE_OFFSET_));
	if (v->expect_vht_option && (!vht_cap || vht_cap_len != 12)) {
		fprintf(stderr, "FAIL %s missing vht cap\n", v->name);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t n = 0;
	size_t i;
	size_t fail = 0;

	if (argc != 2 ||
	    host_load_vectors(argv[1], vectors, sizeof(*vectors), MAX_VECTORS,
			      parse_vector_object, &n))
		return 2;
	for (i = 0; i < n; i++)
		if (run_vector(&vectors[i]))
			fail++;
	printf("%zu vectors, %zu failures\n", n, fail);
	return fail ? 1 : 0;
}
