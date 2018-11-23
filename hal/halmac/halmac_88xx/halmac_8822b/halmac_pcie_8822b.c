/******************************************************************************
 *
 * Copyright(c) 2016 - 2018 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/

#include "halmac_pcie_8822b.h"
#include "halmac_pwr_seq_8822b.h"
#include "../halmac_init_88xx.h"
#include "../halmac_common_88xx.h"
#include "../halmac_pcie_88xx.h"
#include "../halmac_88xx_cfg.h"

#if HALMAC_8822B_SUPPORT

/**
 * mac_pwr_switch_pcie_8822b() - switch mac power
 * @adapter : the adapter of halmac
 * @pwr : power state
 * Author : KaiYuan Chang
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
mac_pwr_switch_pcie_8822b(struct halmac_adapter *adapter,
			  enum halmac_mac_power pwr)
{
	u8 value8;
	u8 rpwm;
	struct halmac_api *api = (struct halmac_api *)adapter->halmac_api;
	enum halmac_ret_status status;

	PLTFM_MSG_TRACE("[TRACE]%s ===>\n", __func__);
	PLTFM_MSG_TRACE("[TRACE]pwr = %x\n", pwr);
	PLTFM_MSG_TRACE("[TRACE]8822B pwr seq ver = %s\n",
			HALMAC_8822B_PWR_SEQ_VER);

	adapter->rpwm = HALMAC_REG_R8(REG_PCIE_HRPWM1_V1);

	/* Check FW still exist or not */
	if (HALMAC_REG_R16(REG_MCUFW_CTRL) == 0xC078) {
		/* Leave 32K */
		rpwm = (u8)((adapter->rpwm ^ BIT(7)) & 0x80);
		HALMAC_REG_W8(REG_PCIE_HRPWM1_V1, rpwm);
	}

	value8 = HALMAC_REG_R8(REG_CR);
	if (value8 == 0xEA)
		adapter->halmac_state.mac_pwr = HALMAC_MAC_POWER_OFF;
	else
		adapter->halmac_state.mac_pwr = HALMAC_MAC_POWER_ON;

	/* Check if power switch is needed */
	if (pwr == HALMAC_MAC_POWER_ON &&
	    adapter->halmac_state.mac_pwr == HALMAC_MAC_POWER_ON) {
		PLTFM_MSG_WARN("[WARN]power state unchange!!\n");
		return HALMAC_RET_PWR_UNCHANGE;
	}

	if (pwr == HALMAC_MAC_POWER_OFF) {
		status = trxdma_check_idle_88xx(adapter);
		if (status != HALMAC_RET_SUCCESS)
			return status;
		if (pwr_seq_parser_88xx(adapter, card_dis_flow_8822b) !=
		    HALMAC_RET_SUCCESS) {
			PLTFM_MSG_ERR("[ERR]Handle power off cmd error\n");
			return HALMAC_RET_POWER_OFF_FAIL;
		}

		adapter->halmac_state.mac_pwr = HALMAC_MAC_POWER_OFF;
		adapter->halmac_state.dlfw_state = HALMAC_DLFW_NONE;
		init_adapter_dynamic_param_88xx(adapter);
	} else {
		if (pwr_seq_parser_88xx(adapter, card_en_flow_8822b) !=
		    HALMAC_RET_SUCCESS) {
			PLTFM_MSG_ERR("[ERR]Handle power on cmd error\n");
			return HALMAC_RET_POWER_ON_FAIL;
		}

		adapter->halmac_state.mac_pwr = HALMAC_MAC_POWER_ON;
	}

	PLTFM_MSG_TRACE("[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * halmac_pcie_switch_8822b() - pcie gen1/gen2 switch
 * @adapter : the adapter of halmac
 * @cfg : gen1/gen2 selection
 * Author : KaiYuan Chang
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
pcie_switch_8822b(struct halmac_adapter *adapter, enum halmac_pcie_cfg cfg)
{
	u8 value8;
	u32 value32;
	u8 speed = 0;
	u32 cnt = 0;

	PLTFM_MSG_TRACE("[TRACE]%s ===>\n", __func__);

	if (cfg == HALMAC_PCIE_GEN1) {
		value8 = dbi_r8_88xx(adapter, LINK_CTRL2_REG_OFFSET) & 0xF0;
		dbi_w8_88xx(adapter, LINK_CTRL2_REG_OFFSET, value8 | BIT(0));

		value32 = dbi_r32_88xx(adapter, GEN2_CTRL_OFFSET);
		dbi_w32_88xx(adapter, GEN2_CTRL_OFFSET, value32 | BIT(17));

		speed = dbi_r8_88xx(adapter, LINK_STATUS_REG_OFFSET) & 0x0F;
		cnt = 2000;

		while ((speed != PCIE_GEN1_SPEED) && (cnt != 0)) {
			PLTFM_DELAY_US(50);
			speed = dbi_r8_88xx(adapter, LINK_STATUS_REG_OFFSET);
			speed &= 0x0F;
			cnt--;
		}

		if (speed != PCIE_GEN1_SPEED) {
			PLTFM_MSG_ERR("[ERR]Speed change to GEN1 fail !\n");
			return HALMAC_RET_FAIL;
		}

	} else if (cfg == HALMAC_PCIE_GEN2) {
		value8 = dbi_r8_88xx(adapter, LINK_CTRL2_REG_OFFSET) & 0xF0;
		dbi_w8_88xx(adapter, LINK_CTRL2_REG_OFFSET, value8 | BIT(1));

		value32 = dbi_r32_88xx(adapter, GEN2_CTRL_OFFSET);
		dbi_w32_88xx(adapter, GEN2_CTRL_OFFSET, value32 | BIT(17));

		speed = dbi_r8_88xx(adapter, LINK_STATUS_REG_OFFSET) & 0x0F;
		cnt = 2000;

		while ((speed != PCIE_GEN2_SPEED) && (cnt != 0)) {
			PLTFM_DELAY_US(50);
			speed = dbi_r8_88xx(adapter, LINK_STATUS_REG_OFFSET);
			speed &= 0x0F;
			cnt--;
		}

		if (speed != PCIE_GEN2_SPEED) {
			PLTFM_MSG_ERR("[ERR]Speed change to GEN1 fail !\n");
			return HALMAC_RET_FAIL;
		}

	} else {
		PLTFM_MSG_ERR("[ERR]Error Speed !\n");
		return HALMAC_RET_FAIL;
	}

	PLTFM_MSG_TRACE("[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * phy_cfg_pcie_8822b() - phy config
 * @adapter : the adapter of halmac
 * Author : KaiYuan Chang
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
phy_cfg_pcie_8822b(struct halmac_adapter *adapter,
		   enum halmac_intf_phy_platform pltfm)
{
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	PLTFM_MSG_TRACE("[TRACE]%s ===>\n", __func__);

	status = parse_intf_phy_88xx(adapter, pcie_gen1_phy_param_8822b, pltfm,
				     HAL_INTF_PHY_PCIE_GEN1);

	if (status != HALMAC_RET_SUCCESS)
		return status;

	status = parse_intf_phy_88xx(adapter, pcie_gen2_phy_param_8822b, pltfm,
				     HAL_INTF_PHY_PCIE_GEN2);

	if (status != HALMAC_RET_SUCCESS)
		return status;

	PLTFM_MSG_TRACE("[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * intf_tun_pcie_8822b() - pcie interface fine tuning
 * @adapter : the adapter of halmac
 * Author : Rick Liu
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
intf_tun_pcie_8822b(struct halmac_adapter *adapter)
{
	return HALMAC_RET_SUCCESS;
}

#endif /* HALMAC_8822B_SUPPORT*/
