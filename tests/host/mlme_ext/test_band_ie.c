// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_mlme_ext_band_ie_types.h"
#include "host_vector_json.h"

static country_ent_t ce;
static _adapter ad;

struct vector {
	char name[64];
	int ch;
	int wireless_mode;
	int ht_option;
	int vht_enable;
	int ori_vht_en;
	int country_en_11ac;
	char ies_in[HOST_VECTOR_MAX_HEX_BUF];
	char expect_ies[HOST_VECTOR_MAX_HEX_BUF];
	char expect_rates[128];
	int expect_length;
};

static int parse_vec(const char *obj, size_t obj_len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
#define I(f, fld) host_json_parse_int_in(obj, obj_len, f, &v->fld)
	I("ch", ch);
	I("wireless_mode", wireless_mode);
	I("ht_option", ht_option);
	I("vht_enable", vht_enable);
	I("ori_vht_en", ori_vht_en);
	I("country_en_11ac", country_en_11ac);
	I("expect_length", expect_length);
#undef I
	if (host_json_parse_string_in(obj, obj_len, "ies_in", v->ies_in,
				      sizeof(v->ies_in)))
		return -1;
	if (host_json_parse_string_in(obj, obj_len, "expect_ies", v->expect_ies,
				      sizeof(v->expect_ies)))
		return -1;
	if (host_json_parse_string_in(obj, obj_len, "expect_rates", v->expect_rates,
				      sizeof(v->expect_rates)))
		return -1;
	if (!v->country_en_11ac)
		v->country_en_11ac = 1;
	return 0;
}

static void load_ies_from_hex(WLAN_BSSID_EX *n, const char *hex)
{
	size_t l = strlen(hex) / 2;
	unsigned v;

	for (size_t i = 0; i < l; i++) {
		sscanf(hex + i * 2, "%2x", &v);
		n->IEs[i] = (u8)v;
	}
	n->IELength = (u32)l;
}

static void ies_to_hex(const WLAN_BSSID_EX *n, char *out, size_t out_cap)
{
	size_t i;

	for (i = 0; i < n->IELength && (i * 2 + 1) < out_cap; i++)
		sprintf(out + i * 2, "%02x", n->IEs[i]);
	out[i * 2] = '\0';
}

static int run_vector(const struct vector *v)
{
	WLAN_BSSID_EX n;
	char got_ies[HOST_VECTOR_MAX_HEX_BUF];
	size_t i;
	unsigned x;

	memset(&n, 0, sizeof(n));
	memset(&ad, 0, sizeof(ad));
	load_ies_from_hex(&n, v->ies_in);
	ad.registrypriv.wireless_mode = (u32)v->wireless_mode;
	ad.registrypriv.vht_enable = (u8)v->vht_enable;
	ad.mlmepriv.htpriv.ht_option = (u8)v->ht_option;
	ad.mlmepriv.ori_vht_en = (u8)v->ori_vht_en;
	ce.en_11ac = (u8)v->country_en_11ac;
	ad.rfctl.country_ent = &ce;

	change_band_update_ie(&ad, &n, (u8)v->ch);
	ies_to_hex(&n, got_ies, sizeof(got_ies));
	if (strcmp(got_ies, v->expect_ies)) {
		fprintf(stderr, "%s ie: got %s want %s\n", v->name, got_ies,
			v->expect_ies);
		return -1;
	}
	for (i = 0; i < strlen(v->expect_rates) / 2; i++) {
		sscanf(v->expect_rates + i * 2, "%2x", &x);
		if (n.SupportedRates[i] != (u8)x) {
			fprintf(stderr, "%s rates mismatch at %zu\n", v->name, i);
			return -1;
		}
	}
	for (; i < NDIS_802_11_LENGTH_RATES_EX; i++) {
		if (n.SupportedRates[i] != 0) {
			fprintf(stderr, "%s rates tail non-zero at %zu\n", v->name,
				i);
			return -1;
		}
	}
	if (n.Length != (u32)v->expect_length) {
		fprintf(stderr, "%s len %u != %d\n", v->name, n.Length,
			v->expect_length);
		return -1;
	}
	printf("PASS: %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = (argc > 1) ? argv[1] : "band_ie_vectors.json";
	struct vector vectors[32];
	size_t count = 0;
	size_t i;

	if (host_load_vectors(path, vectors, sizeof(vectors[0]),
			      sizeof(vectors) / sizeof(vectors[0]), parse_vec,
			      &count)) {
		fprintf(stderr, "load %s failed\n", path);
		return 1;
	}
	for (i = 0; i < count; i++) {
		if (run_vector(&vectors[i]))
			return 1;
	}
	printf("All band_ie vectors passed (%zu).\n", count);
	return 0;
}
