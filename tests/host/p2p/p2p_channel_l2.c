// SPDX-License-Identifier: GPL-2.0
/* W3-98 L2 C oracle: P2P channel/negotiation leaf helpers. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_types.h"
#include "host_vector_json.h"

#define _TRUE 1
#define _FALSE 0
#define _BE 12
#define P2P_ATTR_MANAGEABILITY 0x0a
#define LE16(x) ((u16)(((x)[1] << 8) | (x)[0]))

typedef unsigned int uint;

struct wifidirect_info {
	u8 _pad;
};

struct host_rf_chan {
	u8 ChannelNum;
};

struct rf_ctl_t {
	u8 max_chan_nums;
	struct host_rf_chan channel_set[16];
};

struct _adapter {
	struct wifidirect_info wdinfo;
	struct rf_ctl_t rfctl;
};

typedef struct _adapter *PADAPTER;

#ifndef HOST_P2P_RUST
int rtw_p2p_is_channel_list_ok(u8 desired_ch, u8 *ch_list, u8 ch_cnt)
{
	u8 i;

	for (i = 0; i < ch_cnt; i++)
		if (ch_list[i] == desired_ch)
			return 1;
	return 0;
}

u8 rtw_p2p_get_peer_ch_list(struct wifidirect_info *pwdinfo, u8 *ch_content, u8 ch_cnt,
			    u8 *peer_ch_list)
{
	u8 i, j = 0, temp, ch_no = 0;

	(void)pwdinfo;
	ch_content += 3;
	ch_cnt -= 3;
	while (ch_cnt > 0) {
		ch_content += 1;
		ch_cnt -= 1;
		temp = *ch_content;
		for (i = 0; i < temp; i++, j++)
			peer_ch_list[j] = *(ch_content + 1 + i);
		ch_content += (temp + 1);
		ch_cnt -= (temp + 1);
		ch_no += temp;
	}
	return ch_no;
}

u8 rtw_p2p_ch_inclusion(PADAPTER adapter, u8 *peer_ch_list, u8 peer_ch_num,
			u8 *ch_list_inclusioned)
{
	struct rf_ctl_t *rfctl = &adapter->rfctl;
	int i, j, temp = 0;
	u8 ch_no = 0;

	for (i = 0; i < peer_ch_num; i++) {
		for (j = temp; j < rfctl->max_chan_nums; j++) {
			if (*(peer_ch_list + i) == rfctl->channel_set[j].ChannelNum) {
				ch_list_inclusioned[ch_no++] = *(peer_ch_list + i);
				temp = j;
				break;
			}
		}
	}
	return ch_no;
}

u8 rtw_p2p_nego_intent_compare(u8 req, u8 resp)
{
	if ((req >> 1) == (resp >> 1))
		return req & 0x01 ? _TRUE : _FALSE;
	return (req >> 1) > (resp >> 1) ? _TRUE : _FALSE;
}

static u8 *p2p_ie(const u8 *in, int len, uint *ielen)
{
	u8 oui[4] = {0x50, 0x6F, 0x9A, 0x09};
	uint c = 0;

	if (ielen)
		*ielen = 0;
	if (!in || len <= 0)
		return NULL;
	while (c + 5 < (uint)len) {
		if (in[c] == 221 && !memcmp(&in[c + 2], oui, 4)) {
			if (ielen)
				*ielen = in[c + 1] + 2;
			return (u8 *)(in + c);
		}
		c += in[c + 1] + 2;
	}
	return NULL;
}

static u8 *p2p_attr_content(u8 *ie, uint ilen, u8 id, u8 *buf, uint *len)
{
	u8 oui[4] = {0x50, 0x6F, 0x9A, 0x09}, *ap;

	if (!ie || ilen <= 6 || ie[0] != 221 || memcmp(ie + 2, oui, 4))
		return NULL;
	for (ap = ie + 6; (ap - ie + 3) <= ilen;) {
		u16 alen = LE16(ap + 1) + 3;
		uint content_len = alen - 3;

		if ((ap - ie + alen) > ilen)
			break;
		if (*ap == id) {
			if (len) {
				if (!buf || *len > content_len)
					*len = content_len;
			}
			if (buf && len)
				memcpy(buf, ap + 3, *len);
			else if (buf)
				memcpy(buf, ap + 3, 1);
			return ap + 3;
		}
		ap += alen;
	}
	return NULL;
}

int process_p2p_cross_connect_ie(PADAPTER a, u8 *IEs, u32 len)
{
	u8 *ies, *pie, attr[32];
	u32 il, pl = 0, al;
	int ret = _TRUE;

	(void)a;
	if (len <= _BE)
		return ret;
	ies = IEs + _BE;
	il = len - _BE;
	pie = p2p_ie(ies, il, &pl);
	while (pie) {
		al = sizeof(attr);
		memset(attr, 0, sizeof(attr));
		if (p2p_attr_content(pie, pl, P2P_ATTR_MANAGEABILITY, attr, &al)) {
			if ((attr[0] & 0x03) == 0x01)
				ret = _FALSE;
			break;
		}
		pie = p2p_ie(pie + pl, il - (pie - ies + pl), &pl);
	}
	return ret;
}
#else
int process_p2p_cross_connect_ie(PADAPTER a, u8 *IEs, u32 len);
int rtw_p2p_is_channel_list_ok(u8 desired_ch, u8 *ch_list, u8 ch_cnt);
u8 rtw_p2p_get_peer_ch_list(struct wifidirect_info *pwdinfo, u8 *ch_content, u8 ch_cnt,
			    u8 *peer_ch_list);
u8 rtw_p2p_ch_inclusion(PADAPTER adapter, u8 *peer_ch_list, u8 peer_ch_num,
			u8 *ch_list_inclusioned);
u8 rtw_p2p_nego_intent_compare(u8 req, u8 resp);
#endif

typedef struct {
	char name[48], op[12], ch_list[64], ch_content[128], peer[32], rf[64], exp_list[32], ie[256];
	int desired, exp, exp_cnt, req, resp;
} vector_t;

static int parse_u8_list(const char *s, u8 *out, int cap)
{
	int n = 0;
	char buf[128];

	if (!s || !*s)
		return 0;
	strncpy(buf, s, sizeof(buf) - 1);
	buf[sizeof(buf) - 1] = 0;
	for (char *tok = strtok(buf, " "); tok && n < cap; tok = strtok(NULL, " "))
		out[n++] = (u8)atoi(tok);
	return n;
}

static int parse_vec(const char *o, size_t l, void *vv)
{
	vector_t *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(o, l, "name", v->name, sizeof(v->name)))
		return -1;
#define S(k, f) host_json_parse_string_in(o, l, k, v->f, sizeof(v->f))
#define I(k, f) host_json_parse_int_in(o, l, k, &v->f)
	S("op", op);
	S("ch_list", ch_list);
	S("ch_content", ch_content);
	S("peer", peer);
	S("rf", rf);
	S("exp_list", exp_list);
	S("ie", ie);
	I("desired", desired);
	I("exp", exp);
	I("exp_cnt", exp_cnt);
	I("req", req);
	I("resp", resp);
#undef S
#undef I
	return 0;
}

static int lists_eq(const u8 *a, int na, const char *exp)
{
	u8 b[32];
	int nb = parse_u8_list(exp, b, 32);
	int i;

	if (na != nb)
		return 0;
	for (i = 0; i < na; i++)
		if (a[i] != b[i])
			return 0;
	return 1;
}

static int run_vec(vector_t *v)
{
	struct _adapter a;
	u8 ch_list[32], peer[32], out[32], ch_content[64], frame[320];
	size_t clen, ie_len = 0;

	memset(&a, 0, sizeof(a));
	if (!strcmp(v->op, "ch_ok")) {
		int n = parse_u8_list(v->ch_list, ch_list, 32);
		if (rtw_p2p_is_channel_list_ok((u8)v->desired, ch_list, (u8)n) != v->exp)
			goto fail;
	} else if (!strcmp(v->op, "peer_ch")) {
		if (host_hex_decode(v->ch_content, ch_content, sizeof(ch_content), &clen))
			goto fail;
		if (rtw_p2p_get_peer_ch_list(&a.wdinfo, ch_content, (u8)clen, out) != (u8)v->exp_cnt ||
		    !lists_eq(out, v->exp_cnt, v->exp_list))
			goto fail;
	} else if (!strcmp(v->op, "inclusion")) {
		int pn = parse_u8_list(v->peer, peer, 32);
		int rn = parse_u8_list(v->rf, ch_list, 32);
		int i;

		a.rfctl.max_chan_nums = (u8)rn;
		for (i = 0; i < rn; i++)
			a.rfctl.channel_set[i].ChannelNum = ch_list[i];
		if (rtw_p2p_ch_inclusion(&a, peer, (u8)pn, out) != (u8)v->exp_cnt ||
		    !lists_eq(out, v->exp_cnt, v->exp_list))
			goto fail;
	} else if (!strcmp(v->op, "nego")) {
		if (rtw_p2p_nego_intent_compare((u8)v->req, (u8)v->resp) != (u8)v->exp)
			goto fail;
	} else if (!strcmp(v->op, "cross")) {
		memset(frame, 0, sizeof(frame));
		if (*v->ie && host_hex_decode(v->ie, frame + _BE, sizeof(frame) - _BE, &ie_len))
			goto fail;
		if (process_p2p_cross_connect_ie(&a, frame, (u32)(_BE + ie_len)) != v->exp)
			goto fail;
	} else
		goto fail;
	printf("PASS %s\n", v->name);
	return 0;
fail:
	fprintf(stderr, "FAIL %s\n", v->name);
	return 1;
}

int main(int argc, char **argv)
{
	vector_t v[16];
	size_t n = 0;
	int bad = 0;
	const char *p = argc > 1 ? argv[1] : "p2p_channel_pure_vectors.json";

	if (host_load_vectors(p, v, sizeof(v[0]), 16, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&v[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, p);
	return bad ? 1 : 0;
}
