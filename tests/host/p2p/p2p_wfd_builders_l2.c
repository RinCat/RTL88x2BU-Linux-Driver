// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_p2p_wfd_build.h"
#include "host_vector_json.h"

typedef struct {
	char name[48], fn[16], assoc_bssid[24], expect_hex[512];
	int miracast, role, wfd_tdls, asoc, clients, wfd_type, rtsp_port;
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

static int init_adapter(vector_t *v, struct _adapter *a)
{
	memset(a, 0, sizeof(*a));
	a->miracast_enabled = (u8)v->miracast;
	a->stapriv.asoc_list_cnt = v->clients;
	a->wfd_info.wfd_device_type = (u8)v->wfd_type;
	a->wfd_info.rtsp_ctrlport = (u16)v->rtsp_port;
	a->wdinfo.padapter = a;
	a->wdinfo.wfd_info = &a->wfd_info;
	a->wdinfo.role = (u8)v->role;
	a->wdinfo.wfd_tdls_enable = (u8)v->wfd_tdls;
	if (v->asoc) {
		a->mlmepriv.fwstate = WIFI_ASOC_STATE;
		if (*v->assoc_bssid && parse_mac(v->assoc_bssid, a->mlmepriv.assoc_bssid))
			return -1;
	}
	return 0;
}

static u32 run_vec(vector_t *v, u8 *out)
{
	struct _adapter a;

	if (init_adapter(v, &a))
		return (u32)-1;
	if (!strcmp(v->fn, "beacon"))
		return build_beacon_wfd_ie(&a.wdinfo, out);
#ifdef HOST_P2P_WFD_PROBE
	if (!strcmp(v->fn, "probe_req"))
		return build_probe_req_wfd_ie(&a.wdinfo, out);
#endif
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
	I("miracast", miracast);
	I("role", role);
	I("wfd_tdls", wfd_tdls);
	I("asoc", asoc);
	I("clients", clients);
	I("wfd_type", wfd_type);
	I("rtsp_port", rtsp_port);
#undef I
	if (!host_json_parse_int_in(o, l, "expect_len", &tmp))
		v->expect_len = (u32)tmp;
	host_json_parse_string_in(o, l, "assoc_bssid", v->assoc_bssid, sizeof(v->assoc_bssid));
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
		u32 len = run_vec(&v[i], out);

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
