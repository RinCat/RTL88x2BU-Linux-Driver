EXTRA_CFLAGS += -DCONFIG_RTL8822B

ifeq ($(CONFIG_MP_INCLUDED), y)
### 8822B Default Enable VHT MP HW TX MODE ###
#EXTRA_CFLAGS += -DCONFIG_MP_VHT_HW_TX_MODE
#CONFIG_MP_VHT_HW_TX_MODE = y
endif

_HAL_HALMAC_FILES +=	hal/halmac/halmac_api.o

_HAL_HALMAC_FILES +=	hal/halmac/halmac_88xx/halmac_bb_rf_88xx.o \
			hal/halmac/halmac_88xx/halmac_cfg_wmac_88xx.o \
			hal/halmac/halmac_88xx/halmac_common_88xx.o \
			hal/halmac/halmac_88xx/halmac_efuse_88xx.o \
			hal/halmac/halmac_88xx/halmac_flash_88xx.o \
			hal/halmac/halmac_88xx/halmac_fw_88xx.o \
			hal/halmac/halmac_88xx/halmac_gpio_88xx.o \
			hal/halmac/halmac_88xx/halmac_init_88xx.o \
			hal/halmac/halmac_88xx/halmac_mimo_88xx.o \
			hal/halmac/halmac_88xx/halmac_pcie_88xx.o \
			hal/halmac/halmac_88xx/halmac_sdio_88xx.o \
			hal/halmac/halmac_88xx/halmac_usb_88xx.o

_HAL_HALMAC_FILES +=	hal/halmac/halmac_88xx/halmac_8822b/halmac_cfg_wmac_8822b.o \
			hal/halmac/halmac_88xx/halmac_8822b/halmac_common_8822b.o \
			hal/halmac/halmac_88xx/halmac_8822b/halmac_gpio_8822b.o \
			hal/halmac/halmac_88xx/halmac_8822b/halmac_init_8822b.o \
			hal/halmac/halmac_88xx/halmac_8822b/halmac_pcie_8822b.o \
			hal/halmac/halmac_88xx/halmac_8822b/halmac_phy_8822b.o \
			hal/halmac/halmac_88xx/halmac_8822b/halmac_pwr_seq_8822b.o \
			hal/halmac/halmac_88xx/halmac_8822b/halmac_sdio_8822b.o \
			hal/halmac/halmac_88xx/halmac_8822b/halmac_usb_8822b.o

_HAL_INTFS_FILES +=	hal/hal_halmac.o

_HAL_INTFS_FILES +=	hal/rtl8822b/rtl8822b_halinit.o \
			hal/rtl8822b/rtl8822b_mac.o \
			hal/rtl8822b/rtl8822b_cmd.o \
			hal/rtl8822b/rtl8822b_phy.o \
			hal/rtl8822b/rtl8822b_ops.o \
			hal/rtl8822b/hal8822b_fw.o

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=	hal/rtl8822b/$(HCI_NAME)/rtl8822bu_halinit.o \
			hal/rtl8822b/$(HCI_NAME)/rtl8822bu_halmac.o \
			hal/rtl8822b/$(HCI_NAME)/rtl8822bu_io.o \
			hal/rtl8822b/$(HCI_NAME)/rtl8822bu_xmit.o \
			hal/rtl8822b/$(HCI_NAME)/rtl8822bu_recv.o \
			hal/rtl8822b/$(HCI_NAME)/rtl8822bu_led.o \
			hal/rtl8822b/$(HCI_NAME)/rtl8822bu_ops.o

_HAL_INTFS_FILES +=hal/efuse/rtl8822b/HalEfuseMask8822B_USB.o
endif
ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES +=	hal/rtl8822b/$(HCI_NAME)/rtl8822be_halinit.o \
			hal/rtl8822b/$(HCI_NAME)/rtl8822be_halmac.o \
			hal/rtl8822b/$(HCI_NAME)/rtl8822be_io.o \
			hal/rtl8822b/$(HCI_NAME)/rtl8822be_xmit.o \
			hal/rtl8822b/$(HCI_NAME)/rtl8822be_recv.o \
			hal/rtl8822b/$(HCI_NAME)/rtl8822be_led.o \
			hal/rtl8822b/$(HCI_NAME)/rtl8822be_ops.o

_HAL_INTFS_FILES +=hal/efuse/rtl8822b/HalEfuseMask8822B_PCIE.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES +=	hal/rtl8822b/$(HCI_NAME)/rtl8822bs_halinit.o \
			hal/rtl8822b/$(HCI_NAME)/rtl8822bs_halmac.o \
			hal/rtl8822b/$(HCI_NAME)/rtl8822bs_io.o \
			hal/rtl8822b/$(HCI_NAME)/rtl8822bs_xmit.o \
			hal/rtl8822b/$(HCI_NAME)/rtl8822bs_recv.o \
			hal/rtl8822b/$(HCI_NAME)/rtl8822bs_led.o \
			hal/rtl8822b/$(HCI_NAME)/rtl8822bs_ops.o

_HAL_INTFS_FILES +=hal/efuse/rtl8822b/HalEfuseMask8822B_SDIO.o
endif

_HAL_INTFS_FILES += $(_HAL_HALMAC_FILES)

_BTC_FILES += hal/btc/halbtc8822bwifionly.o
ifeq ($(CONFIG_BT_COEXIST), y)
_BTC_FILES += hal/btc/halbtc8822b1ant.o \
				hal/btc/halbtc8822b2ant.o
endif