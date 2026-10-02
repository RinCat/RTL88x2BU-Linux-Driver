// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle for rtw_build_vht_cap_ie (W3-84 PR4). */

#include <stdio.h>
#include <string.h>

#include "host_vht_build_types.h"
#include "host_vector_json.h"

struct vector {
	char name[64];
	u8 reg_bw_mode, hal_bw_cap, hal_max_bw, ldpc_cap, stbc_cap, sgi_80m, rx_stbc_nss;
	u8 mcs_map[2], vht_highest_rate, ampdu_factor;
	u16 beamform_cap;
	u8 beamformer_rf_num, beamformee_rf_num;
	u32 rx_packet_offset, max_recvbuf_sz;
	u8 expect_cap[VHT_CAP_IE_LEN];
};

static int parse_vector_object(const char *obj, size_t len, void *vvoid)
{
	struct vector *v = vvoid;
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t d;
	int t, *pi = &t;

	memset(v, 0, sizeof(*v));
#define P8(k, f) (host_json_parse_int_in(obj, len, k, pi) ? -1 : ((v->f = (u8)t), 0))
#define PU32(k, f) (host_json_parse_int_in(obj, len, k, pi) ? -1 : ((v->f = (u32)t), 0))
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)) ||
	    P8("reg_bw_mode", reg_bw_mode) || P8("hal_bw_cap", hal_bw_cap) || P8("hal_max_bw", hal_max_bw) ||
	    PU32("rx_packet_offset", rx_packet_offset) || PU32("max_recvbuf_sz", max_recvbuf_sz) ||
	    P8("ldpc_cap", ldpc_cap) || P8("stbc_cap", stbc_cap) || P8("sgi_80m", sgi_80m) ||
	    P8("rx_stbc_nss", rx_stbc_nss) || 	    P8("vht_highest_rate", vht_highest_rate) ||
	    P8("ampdu_factor", ampdu_factor))
		return -1;
	if (host_json_parse_int_in(obj, len, "beamform_cap", pi) == 0)
		v->beamform_cap = (u16)t;
	if (host_json_parse_int_in(obj, len, "beamformer_rf_num", pi) == 0)
		v->beamformer_rf_num = (u8)t;
	if (host_json_parse_int_in(obj, len, "beamformee_rf_num", pi) == 0)
		v->beamformee_rf_num = (u8)t;
#undef P8
#undef PU32
	if (host_json_parse_string_in(obj, len, "mcs_map", hex, sizeof(hex)) ||
	    host_hex_decode(hex, v->mcs_map, 2, &d) || d != 2 ||
	    host_json_parse_string_in(obj, len, "expect_cap_hex", hex, sizeof(hex)) ||
	    host_hex_decode(hex, v->expect_cap, VHT_CAP_IE_LEN, &d) || d != VHT_CAP_IE_LEN)
		return -1;
	return 0;
}

static void load_vec(struct vector *v)
{
	size_t i;

	memset(&host_vht_build_adapter, 0, sizeof(host_vht_build_adapter));
	host_vht_build_adapter.registrypriv.bw_mode = v->reg_bw_mode;
	host_vht_build_adapter.registrypriv.ampdu_factor = v->ampdu_factor;
	host_vht_build_hal_bw_cap = v->hal_bw_cap;
	host_vht_build_adapter.host_fixture.rx_packet_offset = v->rx_packet_offset;
	host_vht_build_adapter.host_fixture.max_recvbuf_sz = v->max_recvbuf_sz;
	host_vht_build_adapter.host_fixture.rx_stbc_nss = v->rx_stbc_nss;
	for (i = 0; i <= v->hal_max_bw && i < 5; i++)
		host_vht_build_adapter.host_fixture.hal_bw_support[i] = 1;
	memcpy(host_vht_build_adapter.mlmepriv.vhtpriv.vht_mcs_map, v->mcs_map, 2);
	host_vht_build_adapter.mlmepriv.vhtpriv.ldpc_cap = v->ldpc_cap;
	host_vht_build_adapter.mlmepriv.vhtpriv.stbc_cap = v->stbc_cap;
	host_vht_build_adapter.mlmepriv.vhtpriv.sgi_80m = v->sgi_80m;
	host_vht_build_adapter.mlmepriv.vhtpriv.vht_highest_rate = v->vht_highest_rate;
	host_vht_build_adapter.mlmepriv.vhtpriv.beamform_cap = v->beamform_cap;
	host_vht_build_adapter.host_fixture.beamformer_rf_num = v->beamformer_rf_num;
	host_vht_build_adapter.host_fixture.beamformee_rf_num = v->beamformee_rf_num;
}

int main(int argc, char **argv)
{
	struct vector vecs[8];
	u8 out[32];
	size_t n = 0, i, fail = 0;
	u32 len;

	if (argc != 2 || host_load_vectors(argv[1], vecs, sizeof(*vecs), 8, parse_vector_object, &n))
		return 2;
	for (i = 0; i < n; i++) {
		load_vec(&vecs[i]);
		len = rtw_build_vht_cap_ie(&host_vht_build_adapter, out);
		if (len != VHT_CAP_IE_LEN + 2 || out[0] != EID_VHTCapability || out[1] != VHT_CAP_IE_LEN ||
		    memcmp(out + 2, vecs[i].expect_cap, VHT_CAP_IE_LEN))
			fail++, fprintf(stderr, "FAIL %s\n", vecs[i].name);
	}
	printf("%zu vectors, %zu failures\n", n, fail);
	return fail ? 1 : 0;
}
