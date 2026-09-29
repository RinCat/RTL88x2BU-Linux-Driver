// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle for VHT_caps_handler (W3-84 PR9). */

#include <stdio.h>
#include <string.h>

#include "host_vht_build_types.h"
#include "host_vector_json.h"

struct vector {
	char name[64];
	u8 vht_option, ldpc_cap, stbc_cap, sgi_80m, rx_nss;
	u8 cap_hex[VHT_CAP_IE_LEN];
	u8 expect_vht_enable, expect_ldpc, expect_stbc, expect_sgi, expect_ampdu;
	u8 expect_mcs_map[2], expect_highest_rate;
};

static int parse_vector_object(const char *obj, size_t len, void *vvoid)
{
	struct vector *v = vvoid;
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t d;
	int t, *pi = &t;

	memset(v, 0, sizeof(*v));
#define P8(k, f) (host_json_parse_int_in(obj, len, k, pi) ? -1 : ((v->f = (u8)t), 0))
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)) ||
	    P8("vht_option", vht_option) || P8("ldpc_cap", ldpc_cap) || P8("stbc_cap", stbc_cap) ||
	    P8("sgi_80m", sgi_80m) || P8("rx_nss", rx_nss) ||
	    P8("expect_vht_enable", expect_vht_enable) || P8("expect_ldpc", expect_ldpc) ||
	    P8("expect_stbc", expect_stbc) || P8("expect_sgi", expect_sgi) ||
	    P8("expect_ampdu", expect_ampdu) || P8("expect_highest_rate", expect_highest_rate))
		return -1;
#undef P8
	if (host_json_parse_string_in(obj, len, "cap_hex", hex, sizeof(hex)) ||
	    host_hex_decode(hex, v->cap_hex, VHT_CAP_IE_LEN, &d) || d != VHT_CAP_IE_LEN ||
	    host_json_parse_string_in(obj, len, "expect_mcs_map", hex, sizeof(hex)) ||
	    host_hex_decode(hex, v->expect_mcs_map, 2, &d) || d != 2)
		return -1;
	return 0;
}

static void load_vec(struct vector *v, NDIS_802_11_VARIABLE_IEs *ie)
{
	memset(&host_vht_build_adapter, 0, sizeof(host_vht_build_adapter));
	host_vht_build_adapter.mlmepriv.vhtpriv.vht_option = v->vht_option;
	host_vht_build_adapter.mlmepriv.vhtpriv.ldpc_cap = v->ldpc_cap;
	host_vht_build_adapter.mlmepriv.vhtpriv.stbc_cap = v->stbc_cap;
	host_vht_build_adapter.mlmepriv.vhtpriv.sgi_80m = v->sgi_80m;
	host_vht_build_adapter.host_fixture.rx_nss = v->rx_nss;
	memset(ie, 0, sizeof(*ie));
	ie->ElementID = EID_VHTCapability;
	ie->Length = VHT_CAP_IE_LEN;
	memcpy(ie->data, v->cap_hex, VHT_CAP_IE_LEN);
}

int main(int argc, char **argv)
{
	struct vector vecs[8];
	NDIS_802_11_VARIABLE_IEs ie;
	size_t n = 0, i, fail = 0;
	struct vht_priv *vp;

	if (argc != 2 || host_load_vectors(argv[1], vecs, sizeof(*vecs), 8, parse_vector_object, &n))
		return 2;
	for (i = 0; i < n; i++) {
		load_vec(&vecs[i], &ie);
		vp = &host_vht_build_adapter.mlmepriv.vhtpriv;
		VHT_caps_handler(&host_vht_build_adapter, &ie);
		if (host_vht_build_adapter.mlmeextpriv.mlmext_info.VHT_enable != vecs[i].expect_vht_enable ||
		    vp->ldpc_cap != vecs[i].expect_ldpc || vp->stbc_cap != vecs[i].expect_stbc ||
		    vp->sgi_80m != vecs[i].expect_sgi || vp->ampdu_len != vecs[i].expect_ampdu ||
		    vp->vht_highest_rate != vecs[i].expect_highest_rate ||
		    memcmp(vp->vht_mcs_map, vecs[i].expect_mcs_map, 2))
			fail++, fprintf(stderr, "FAIL %s\n", vecs[i].name);
	}
	printf("%zu vectors, %zu failures\n", n, fail);
	return fail ? 1 : 0;
}
