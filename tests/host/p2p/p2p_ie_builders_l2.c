// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_p2p_ie_build.h"
#include "host_vector_json.h"

typedef struct {
	char name[48];
	char fn[16];
	int role, p2p_state, status, persistent, ui_wps, wps_cm;
	char dev_addr[24];
	char dev_name[40];
	char ssid[32];
	char go_addr[24];
	char expect_hex[512];
	u32 expect_len;
} vector_t;

static int parse_mac(const char *s, u8 *out)
{
	unsigned a, b, c, d, e, f;

	if (!s || !*s)
		return 0;
	if (sscanf(s, "%02x:%02x:%02x:%02x:%02x:%02x", &a, &b, &c, &d, &e, &f) != 6)
		return -1;
	out[0] = (u8)a;
	out[1] = (u8)b;
	out[2] = (u8)c;
	out[3] = (u8)d;
	out[4] = (u8)e;
	out[5] = (u8)f;
	return 0;
}

static int load_wd(vector_t *v, struct wifidirect_info *w)
{
	size_t dn = strnlen(v->dev_name, sizeof(v->dev_name));

	memset(w, 0, sizeof(*w));
	w->role = (u8)v->role;
	w->p2p_state = (u8)v->p2p_state;
	w->persistent_supported = (u8)v->persistent;
	w->ui_got_wps_info = (u8)v->ui_wps;
	w->supported_wps_cm = (u16)v->wps_cm;
	if (dn > WPS_MAX_DEVICE_NAME_LEN)
		dn = WPS_MAX_DEVICE_NAME_LEN;
	memcpy(w->device_name, v->dev_name, dn);
	w->device_name_len = (u16)dn;
	if (parse_mac(v->dev_addr, w->device_addr))
		return -1;
	return 0;
}

static u32 dispatch(vector_t *v, u8 *out)
{
	struct wifidirect_info wd;
	u8 ssid[32], go[ETH_ALEN];
	u8 slen;

	if (load_wd(v, &wd))
		return (u32)-1;
	if (!strcmp(v->fn, "beacon"))
		return build_beacon_p2p_ie(&wd, out);
	if (!strcmp(v->fn, "assoc_resp"))
		return build_assoc_resp_p2p_ie(&wd, out, (u8)v->status);
	if (!strcmp(v->fn, "deauth"))
		return build_deauth_p2p_ie(&wd, out);
	if (!strcmp(v->fn, "probe_resp"))
		return build_probe_resp_p2p_ie(&wd, out);
	if (!strcmp(v->fn, "prov_disc")) {
		slen = (u8)strnlen(v->ssid, sizeof(v->ssid));
		memcpy(ssid, v->ssid, slen);
		if (*v->go_addr && parse_mac(v->go_addr, go))
			return (u32)-1;
		return build_prov_disc_request_p2p_ie(&wd, out, ssid, slen,
						      *v->go_addr ? go : ssid);
	}
	return (u32)-1;
}

static int parse_vec(const char *o, size_t l, void *vv)
{
	vector_t *v = vv;
	int tmp = 0;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(o, l, "name", v->name, sizeof(v->name)) ||
	    host_json_parse_string_in(o, l, "fn", v->fn, sizeof(v->fn)))
		return -1;
#define I(k, f) host_json_parse_int_in(o, l, k, &v->f)
	I("role", role);
	I("p2p_state", p2p_state);
	I("status", status);
	I("persistent", persistent);
	I("ui_wps", ui_wps);
	I("wps_cm", wps_cm);
#undef I
	if (!host_json_parse_int_in(o, l, "expect_len", &tmp))
		v->expect_len = (u32)tmp;
	host_json_parse_string_in(o, l, "dev_addr", v->dev_addr, sizeof(v->dev_addr));
	host_json_parse_string_in(o, l, "dev_name", v->dev_name, sizeof(v->dev_name));
	host_json_parse_string_in(o, l, "ssid", v->ssid, sizeof(v->ssid));
	host_json_parse_string_in(o, l, "go_addr", v->go_addr, sizeof(v->go_addr));
	host_json_parse_string_in(o, l, "expect_ie", v->expect_hex, sizeof(v->expect_hex));
	return 0;
}

int main(int argc, char **argv)
{
	vector_t v[16];
	u8 out[320], exp[320];
	size_t n = 0, elen, fail = 0;

	if (argc != 2 || host_load_vectors(argv[1], v, sizeof(v[0]), 16, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++) {
		u32 len = dispatch(&v[i], out);

		if (len == (u32)-1 || len != v[i].expect_len ||
		    (v[i].expect_len &&
		     (host_hex_decode(v[i].expect_hex, exp, sizeof(exp), &elen) ||
		      elen != v[i].expect_len || memcmp(out, exp, len)))) {
			fprintf(stderr, "FAIL %s\n", v[i].name);
			fail++;
		}
	}
	printf("%zu vectors, %zu failures\n", n, fail);
	return fail ? 1 : 0;
}
