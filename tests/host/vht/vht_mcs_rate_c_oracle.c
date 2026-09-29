// SPDX-License-Identifier: GPL-2.0
/* C oracle slice: VHT MCS/rate pure helpers from core/rtw_vht.c (W3-45). */

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint64_t u64;

#define MGN_VHT1SS_MCS0 0xA0
#define MGN_VHT1SS_MCS7 0xA7
#define MGN_VHT4SS_MCS9 0xC7

const u16 VHT_MCS_DATA_RATE[3][2][40] = {
	{{
		13, 26, 39, 52, 78, 104, 117, 130, 156, 156,
		26, 52, 78, 104, 156, 208, 234, 260, 312, 312,
		39, 78, 117, 156, 234, 312, 351, 390, 468, 520,
		52, 104, 156, 208, 312, 416, 468, 520, 624, 624,
	}, {
		14, 29, 43, 58, 87, 116, 130, 144, 173, 173,
		29, 58, 87, 116, 173, 231, 260, 289, 347, 347,
		43, 87, 130, 173, 260, 347, 390, 433, 520, 578,
		58, 116, 173, 231, 347, 462, 520, 578, 693, 693,
	}},
	{{
		27, 54, 81, 108, 162, 216, 243, 270, 324, 360,
		54, 108, 162, 216, 324, 432, 486, 540, 648, 720,
		81, 162, 243, 324, 486, 648, 729, 810, 972, 1080,
		108, 216, 324, 432, 648, 864, 972, 1080, 1296, 1440,
	}, {
		30, 60, 90, 120, 180, 240, 270, 300, 360, 400,
		60, 120, 180, 240, 360, 480, 540, 600, 720, 800,
		90, 180, 270, 360, 540, 720, 810, 900, 1080, 1200,
		120, 240, 360, 480, 720, 960, 1080, 1200, 1440, 1600,
	}},
	{{
		59, 117, 176, 234, 351, 468, 527, 585, 702, 780,
		117, 234, 351, 468, 702, 936, 1053, 1170, 1404, 1560,
		176, 351, 527, 702, 1053, 1404, 1580, 1755, 2106, 2340,
		234, 468, 702, 936, 1404, 1872, 2106, 2340, 2808, 3120,
	}, {
		65, 130, 195, 260, 390, 520, 585, 650, 780, 867,
		130, 260, 390, 520, 780, 1040, 1170, 1300, 1560, 1734,
		195, 390, 585, 780, 1170, 1560, 1755, 1950, 2340, 2600,
		260, 520, 780, 1040, 1560, 2080, 2340, 2600, 3120, 3467,
	}}
};

void rtw_vht_nss_to_mcsmap(u8 nss, u8 *target_mcs_map, u8 *cur_mcs_map)
{
	u8 i, j;
	u8 cur_rate, target_rate;

	for (i = 0; i < 2; i++) {
		target_mcs_map[i] = 0;
		for (j = 0; j < 8; j += 2) {
			cur_rate = (cur_mcs_map[i] >> j) & 3;
			if (cur_rate == 3)
				target_rate = 3;
			else if (nss <= ((j / 2) + i * 4))
				target_rate = 3;
			else
				target_rate = cur_rate;

			target_mcs_map[i] |= (target_rate << j);
		}
	}
}

u8 rtw_get_vht_highest_rate(u8 *pvht_mcs_map)
{
	u8 i, j;
	u8 bit_map;
	u8 vht_mcs_rate = 0;

	for (i = 0; i < 2; i++) {
		if (pvht_mcs_map[i] != 0xff) {
			for (j = 0; j < 8; j += 2) {
				bit_map = (pvht_mcs_map[i] >> j) & 3;

				if (bit_map != 3)
					vht_mcs_rate = MGN_VHT1SS_MCS7 + 10 * j / 2 +
						       i * 40 + bit_map;
			}
		}
	}

	return vht_mcs_rate;
}

u8 rtw_vht_mcsmap_to_nss(u8 *pvht_mcs_map)
{
	u8 i, j;
	u8 bit_map;
	u8 nss = 0;

	for (i = 0; i < 2; i++) {
		if (pvht_mcs_map[i] != 0xff) {
			for (j = 0; j < 8; j += 2) {
				bit_map = (pvht_mcs_map[i] >> j) & 3;

				if (bit_map != 3)
					nss++;
			}
		}
	}

	return nss;
}

u16 rtw_vht_mcs_to_data_rate(u8 bw, u8 short_GI, u8 vht_mcs_rate)
{
	if (vht_mcs_rate > MGN_VHT4SS_MCS9)
		vht_mcs_rate = MGN_VHT4SS_MCS9;
	return VHT_MCS_DATA_RATE[bw][short_GI]
			      [((vht_mcs_rate - MGN_VHT1SS_MCS0) & 0x3f)];
}

u64 rtw_vht_mcs_map_to_bitmap(u8 *mcs_map, u8 nss)
{
	u8 i, j, tmp;
	u64 bitmap = 0;
	u8 bits_nss = nss * 2;

	for (i = j = 0; i < bits_nss; i += 2, j += 10) {
		tmp = (mcs_map[i / 8] >> i) & 3;

		switch (tmp) {
		case 2:
			bitmap = bitmap | ((u64)0x03ff << j);
			break;
		case 1:
			bitmap = bitmap | ((u64)0x01ff << j);
			break;
		case 0:
			bitmap = bitmap | ((u64)0x00ff << j);
			break;
		default:
			break;
		}
	}

	return bitmap;
}
