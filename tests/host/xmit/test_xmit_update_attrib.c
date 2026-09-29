// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>

#include "host_xmit_update_attrib_types.h"
#include "host_xmit_update_attrib_sec_types.h"

static void setup_adapter_vcs(_adapter *a, u8 cur_wm, u16 rts_thresh, u32 frag_len)
{
	memset(a, 0, sizeof(*a));
	a->mlmeextpriv.cur_wireless_mode = cur_wm;
	a->registrypriv.rts_thresh = rts_thresh;
	a->registrypriv.vrtl_carrier_sense = AUTO_VCS;
	a->xmitpriv.frag_len = frag_len;
}

static int run_vcs_case_ex(const char *name, u32 sz, u8 cur_wm, u8 ampdu, u8 rtsen,
			   u8 vcs_sense, u8 expect)
{
	_adapter adapter;
	struct xmit_frame frame;

	setup_adapter_vcs(&adapter, cur_wm, 2347, 500);
	adapter.registrypriv.vrtl_carrier_sense = vcs_sense;
	memset(&frame, 0, sizeof(frame));
	frame.attrib.nr_frags = 1;
	frame.attrib.last_txcmdsz = sz;
	frame.attrib.ampdu_en = ampdu;
	frame.attrib.rtsen = rtsen;

	update_attrib_vcs_info(&adapter, &frame);
	if (frame.attrib.vcs_mode != expect) {
		fprintf(stderr, "%s: got %u expect %u\n", name,
			frame.attrib.vcs_mode, expect);
		return -1;
	}
	printf("PASS %s\n", name);
	return 0;
}

static int run_vcs_case(const char *name, u32 sz, u8 cur_wm, u8 ampdu, u8 expect)
{
	return run_vcs_case_ex(name, sz, cur_wm, ampdu, 0, AUTO_VCS, expect);
}

static void setup_ht_phy(_adapter *a, struct sta_info *sta, u8 cur_bw, u8 sta_bw)
{
	memset(a, 0, sizeof(*a));
	memset(sta, 0, sizeof(*sta));
	a->registrypriv.ht_enable = 1;
	a->registrypriv.wireless_mode = WIRELESS_11_24N;
	a->mlmeextpriv.cur_bwmode = cur_bw;
	sta->cmn.bw_mode = sta_bw;
}

static int run_phy_case(const char *name, _adapter *adapter, struct pkt_attrib *attrib,
			struct sta_info *sta, u8 expect_bw, u8 expect_ampdu)
{
	update_attrib_phy_info(adapter, attrib, sta);
	if (attrib->bwmode != expect_bw || attrib->ampdu_en != expect_ampdu ||
	    attrib->rtsen != sta->rtsen || attrib->retry_ctrl != _FALSE) {
		fprintf(stderr, "%s: bw=%u ampdu=%u\n", name, attrib->bwmode,
			attrib->ampdu_en);
		return -1;
	}
	printf("PASS %s\n", name);
	return 0;
}

int main(void)
{
	int fail = 0;

	fail |= run_vcs_case("vcs_legacy_rts_thresh", 3000, 3, 0, RTS_CTS);
	fail |= run_vcs_case("vcs_legacy_none", 100, 3, 0, NONE_VCS);
	fail |= run_vcs_case("vcs_ht_ampdu_rts", 100, WIRELESS_11_24N, 1, RTS_CTS);
	fail |= run_vcs_case_ex("vcs_legacy_rtsen", 100, 3, 0, 1, AUTO_VCS, RTS_CTS);
	fail |= run_vcs_case_ex("vcs_validate_disable", 3000, 3, 0, 0, DISABLE_VCS,
				NONE_VCS);

	{
		_adapter adapter;
		struct pkt_attrib attrib;
		struct sta_info sta;

		memset(&attrib, 0, sizeof(attrib));
		setup_ht_phy(&adapter, &sta, CHANNEL_WIDTH_20, CHANNEL_WIDTH_40);
		sta.rtsen = 1;
		fail |= run_phy_case("phy_bw_min", &adapter, &attrib, &sta,
				     CHANNEL_WIDTH_20, _FALSE);
	}

	{
		_adapter adapter;
		struct pkt_attrib attrib;
		struct sta_info sta;

		memset(&attrib, 0, sizeof(attrib));
		setup_ht_phy(&adapter, &sta, CHANNEL_WIDTH_40, CHANNEL_WIDTH_40);
		sta.htpriv.ht_option = 1;
		sta.htpriv.ampdu_enable = 1;
		sta.htpriv.agg_enable_bitmap = BIT(2);
		attrib.priority = 2;
		fail |= run_phy_case("phy_ht_ampdu_tid", &adapter, &attrib, &sta,
				     CHANNEL_WIDTH_40, _TRUE);
	}

	{
		_adapter adapter;
		struct pkt_attrib_sec_ext attrib;
		struct sta_info_sec_ext sta;

		memset(&adapter, 0, sizeof(adapter));
		memset(&attrib, 0, sizeof(attrib));
		memset(&sta, 0, sizeof(sta));
		host_xmit_sec_cfg.dot11AuthAlgrthm = 0;
		adapter.securitypriv.dot11PrivacyAlgrthm = 0x04;
		sta.mac_id = 3;
		sta.dot11txpn.val = 0xffff;
		attrib.ether_type = 0x0800;
		attrib.ra[0] = 0x02;
		if (update_attrib_sec_info_l2(&adapter, &attrib, &sta, 0) != 0 ||
		    attrib.encrypt != 0x04 || attrib.iv_len != 8 ||
		    attrib.mac_id != 3 || attrib.iv[0] != 0 || attrib.iv[1] != 0 ||
		    attrib.iv[3] != (0x20 | 0) || attrib.iv[4] != 1 ||
		    attrib.iv[5] != 0) {
			fprintf(stderr, "sec_open_aes failed\n");
			fail = 1;
		} else {
			printf("PASS sec_open_aes\n");
		}
	}

	{
		_adapter adapter;
		struct pkt_attrib_sec_ext attrib;
		struct sta_info_sec_ext sta;

		memset(&adapter, 0, sizeof(adapter));
		memset(&attrib, 0, sizeof(attrib));
		memset(&sta, 0, sizeof(sta));
		host_xmit_sec_cfg.dot11AuthAlgrthm = 0;
		adapter.securitypriv.dot11PrivacyAlgrthm = 0x04;
		sta.dot11txpn.val = 0x00000000ffff0000ULL;
		attrib.ether_type = 0x0800;
		attrib.ra[0] = 0x01;
		if (update_attrib_sec_info_l2(&adapter, &attrib, &sta, 0) != 0 ||
		    attrib.encrypt != 0x04 || attrib.iv_len != 8 ||
		    attrib.iv[0] != 1 || attrib.iv[4] != 0xff || attrib.iv[5] != 0xff ||
		    attrib.iv[6] != 0 || attrib.iv[7] != 0) {
			fprintf(stderr, "sec_open_aes_mc_high_pn failed\n");
			fail = 1;
		} else {
			printf("PASS sec_open_aes_mc_high_pn\n");
		}
	}

	{
		_adapter adapter;
		struct pkt_attrib_sec_ext attrib;
		struct sta_info_sec_ext sta;

		memset(&adapter, 0, sizeof(adapter));
		memset(&attrib, 0, sizeof(attrib));
		memset(&sta, 0, sizeof(sta));
		host_xmit_sec_passing_ms = 50;
		sta.resp_nonenc_eapol_key_starttime = 1;
		attrib.ether_type = 0x888e;
		if (update_attrib_sec_info_l2(&adapter, &attrib, &sta, 12) != 0 ||
		    attrib.encrypt != 0) {
			fprintf(stderr, "sec_eapol_4_4_clear failed\n");
			fail = 1;
		} else {
			printf("PASS sec_eapol_4_4_clear\n");
		}
	}

	{
		_adapter adapter;
		struct pkt_attrib_sec_ext attrib;
		struct sta_info_sec_ext sta;

		memset(&adapter, 0, sizeof(adapter));
		memset(&attrib, 0, sizeof(attrib));
		memset(&sta, 0, sizeof(sta));
		sta.ieee8021x_blocked = _TRUE;
		attrib.ether_type = 0x0800;
		if (update_attrib_sec_info_l2(&adapter, &attrib, &sta, 0) != -1) {
			fprintf(stderr, "sec_blocked_data expect fail\n");
			fail = 1;
		} else {
			printf("PASS sec_blocked_data\n");
		}
	}

	{
		_adapter adapter;
		struct pkt_attrib_sec_ext attrib;
		struct sta_info_sec_ext sta;

		memset(&adapter, 0, sizeof(adapter));
		memset(&attrib, 0, sizeof(attrib));
		memset(&sta, 0, sizeof(sta));
		host_xmit_sec_passing_ms = 50;
		sta.resp_nonenc_eapol_key_starttime = 1;
		attrib.ether_type = 0x888e;
		if (update_attrib_sec_info_l2(&adapter, &attrib, &sta, 10) != 0 ||
		    attrib.encrypt != 0) {
			fprintf(stderr, "sec_eapol_2_4_clear failed\n");
			fail = 1;
		} else {
			printf("PASS sec_eapol_2_4_clear\n");
		}
	}

	return fail ? 1 : 0;
}
