ccflags-y += $(USER_EXTRA_CFLAGS)
ccflags-y += -O2
#ccflags-y += -O3
#ccflags-y += -Wall
#ccflags-y += -Wextra
#ccflags-y += -Werror
#ccflags-y += -pedantic
#ccflags-y += -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes
#ccflags-y += -Wimplicit-fallthrough=1

ccflags-y += -Wno-unused-variable
#ccflags-y += -Wno-unused-value
#ccflags-y += -Wno-unused-label
#ccflags-y += -Wno-unused-parameter
#ccflags-y += -Wno-unused-function
#ccflags-y += -Wno-unused
#ccflags-y += -Wno-uninitialized

# Clang (LLVM=1 / RfL out-of-tree) turns several noisy diagnostics into errors.
# Keep these gated so default GCC / distro-header builds stay unchanged.
ifeq ($(LLVM),1)
ccflags-y += -Wno-missing-prototypes
ccflags-y += -Wno-missing-declarations
ccflags-y += -Wno-implicit-fallthrough
ccflags-y += -Wno-unused-function
ccflags-y += -Wno-unused-label
ccflags-y += -Wno-unused-parameter
ccflags-y += -Wno-uninitialized
ccflags-y += -Wno-address-of-packed-member
ccflags-y += -Wno-format
ccflags-y += -Wno-frame-larger-than=
ccflags-y += -Wno-vla
# Drop platform soft/hard-float flags under Clang; they fight kernel code model.
ccflags-remove-y += -mhard-float
ccflags-remove-y += -mfloat-abi=hard
endif

GCC_VER_49 := $(shell echo `$(CC) -dumpversion | cut -f1-2 -d.` \>= 4.9 | bc )
ifeq ($(GCC_VER_49),1)
ccflags-y += -Wno-date-time	# Fix compile error && warning on gcc 4.9 and later
endif

ccflags-y += -I$(src)/include

ldflags-y += --strip-debug

CONFIG_AUTOCFG_CP = n

########################## WIFI IC ############################
CONFIG_MULTIDRV = n
CONFIG_RTL8188E = n
CONFIG_RTL8812A = n
CONFIG_RTL8821A = n
CONFIG_RTL8192E = n
CONFIG_RTL8723B = n
CONFIG_RTL8814A = n
CONFIG_RTL8723C = n
CONFIG_RTL8188F = n
CONFIG_RTL8188GTV = n
CONFIG_RTL8822B = y
CONFIG_RTL8723D = n
CONFIG_RTL8821C = n
CONFIG_RTL8710B = n
CONFIG_RTL8192F = n
CONFIG_RTL8822C = n
CONFIG_RTL8814B = n
CONFIG_RTL8723F = n
######################### Interface ###########################
CONFIG_USB_HCI = y
CONFIG_PCI_HCI = n
CONFIG_SDIO_HCI = n
CONFIG_GSPI_HCI = n
########################## Features ###########################
CONFIG_AP_MODE = y
CONFIG_P2P = y
CONFIG_MP_INCLUDED = y
CONFIG_POWER_SAVING = y
CONFIG_IPS_MODE = default
CONFIG_LPS_MODE = default
CONFIG_USB_AUTOSUSPEND = n
CONFIG_HW_PWRP_DETECTION = n
CONFIG_BT_COEXIST = y
CONFIG_WAPI_SUPPORT = n
CONFIG_EFUSE_CONFIG_FILE = y
CONFIG_EXT_CLK = n
CONFIG_TRAFFIC_PROTECT = n
CONFIG_LOAD_PHY_PARA_FROM_FILE = y
CONFIG_TXPWR_BY_RATE = y
CONFIG_TXPWR_BY_RATE_EN = y
CONFIG_TXPWR_LIMIT = y
CONFIG_TXPWR_LIMIT_EN = n
CONFIG_RTW_CHPLAN = 0xFF
CONFIG_RTW_ADAPTIVITY_EN = disable
CONFIG_RTW_ADAPTIVITY_MODE = normal
CONFIG_SIGNAL_SCALE_MAPPING = n
CONFIG_80211W = y
CONFIG_REDUCE_TX_CPU_LOADING = n
CONFIG_BR_EXT = y
CONFIG_TDLS = n
CONFIG_WIFI_MONITOR = y
CONFIG_MCC_MODE = n
CONFIG_APPEND_VENDOR_IE_ENABLE = n
CONFIG_RTW_NAPI = y
CONFIG_RTW_GRO = y
CONFIG_RTW_NETIF_SG = y
CONFIG_RTW_IPCAM_APPLICATION = n
CONFIG_RTW_REPEATER_SON = n
CONFIG_ICMP_VOQ = n
CONFIG_IP_R_MONITOR = n #arp VOQ and high rate
# user priority mapping rule : tos, dscp
CONFIG_RTW_UP_MAPPING_RULE = tos
CONFIG_RTW_MBO = n
CONFIG_RTW_80211K = n
CONFIG_RTW_IOCTL_SET_COUNTRY = y
########################## Android ###########################
# CONFIG_RTW_ANDROID - 0: no Android, 4/5/6/7/8/9/10/11 : Android version
CONFIG_RTW_ANDROID = 0

ifeq ($(shell test $(CONFIG_RTW_ANDROID) -gt 0; echo $$?), 0)
ccflags-y += -DCONFIG_RTW_ANDROID=$(CONFIG_RTW_ANDROID)
endif

########################## Debug ###########################
CONFIG_RTW_DEBUG = y
# default log level is _DRV_INFO_ = 4,
# please refer to "How_to_set_driver_debug_log_level.doc" to set the available level.
CONFIG_RTW_LOG_LEVEL = 3

# enable /proc/net/rtlxxxx/ debug interfaces
CONFIG_PROC_DEBUG = y

######################## Wake On Lan ##########################
CONFIG_WOWLAN = n
#bit3: ARP enable, bit2: deauth, bit1: unicast, bit0: magic pkt.
CONFIG_WAKEUP_TYPE = 0xf
CONFIG_WOW_LPS_MODE = default
#bit0: disBBRF off, #bit1: Wireless remote controller (WRC)
CONFIG_SUSPEND_TYPE = 0
CONFIG_WOW_STA_MIX = n
CONFIG_GPIO_WAKEUP = n
# Please contact with RTK support team first. After getting the agreement from RTK support team, 
# you are just able to modify the CONFIG_WAKEUP_GPIO_IDX with customized requirement.
CONFIG_WAKEUP_GPIO_IDX = default
CONFIG_HIGH_ACTIVE_DEV2HST = n
######### only for USB #########
CONFIG_ONE_PIN_GPIO = n
CONFIG_HIGH_ACTIVE_HST2DEV = n
CONFIG_PNO_SUPPORT = n
CONFIG_PNO_SET_DEBUG = n
CONFIG_AP_WOWLAN = n
######### Notify SDIO Host Keep Power During Syspend ##########
CONFIG_RTW_SDIO_PM_KEEP_POWER = y
###################### MP HW TX MODE FOR VHT #######################
CONFIG_MP_VHT_HW_TX_MODE = n
###################### ROAMING #####################################
CONFIG_LAYER2_ROAMING = y
#bit0: ROAM_ON_EXPIRED, #bit1: ROAM_ON_RESUME, #bit2: ROAM_ACTIVE
CONFIG_ROAMING_FLAG = 0x3
###################### Platform Related #######################
CONFIG_PLATFORM_I386_PC = y
CONFIG_PLATFORM_ANDROID_X86 = n
CONFIG_PLATFORM_ANDROID_INTEL_X86 = n
CONFIG_PLATFORM_JB_X86 = n
CONFIG_PLATFORM_ARM_S3C2K4 = n
CONFIG_PLATFORM_ARM_PXA2XX = n
CONFIG_PLATFORM_ARM_S3C6K4 = n
CONFIG_PLATFORM_MIPS_RMI = n
CONFIG_PLATFORM_RTD2880B = n
CONFIG_PLATFORM_MIPS_AR9132 = n
CONFIG_PLATFORM_RTK_DMP = n
CONFIG_PLATFORM_MIPS_PLM = n
CONFIG_PLATFORM_MSTAR389 = n
CONFIG_PLATFORM_MT53XX = n
CONFIG_PLATFORM_ARM_MX51_241H = n
CONFIG_PLATFORM_FS_MX61 = n
CONFIG_PLATFORM_ACTIONS_ATJ227X = n
CONFIG_PLATFORM_TEGRA3_CARDHU = n
CONFIG_PLATFORM_TEGRA4_DALMORE = n
CONFIG_PLATFORM_ARM_TCC8900 = n
CONFIG_PLATFORM_ARM_TCC8920 = n
CONFIG_PLATFORM_ARM_TCC8920_JB42 = n
CONFIG_PLATFORM_ARM_TCC8930_JB42 = n
CONFIG_PLATFORM_ARM_RK2818 = n
CONFIG_PLATFORM_ARM_RK3066 = n
CONFIG_PLATFORM_ARM_RK3188 = n
CONFIG_PLATFORM_ARM_URBETTER = n
CONFIG_PLATFORM_ARM_TI_PANDA = n
CONFIG_PLATFORM_MIPS_JZ4760 = n
CONFIG_PLATFORM_DMP_PHILIPS = n
CONFIG_PLATFORM_MSTAR_TITANIA12 = n
CONFIG_PLATFORM_MSTAR = n
CONFIG_PLATFORM_SZEBOOK = n
CONFIG_PLATFORM_ARM_SUNxI = n
CONFIG_PLATFORM_ARM_SUN6I = n
CONFIG_PLATFORM_ARM_SUN7I = n
CONFIG_PLATFORM_ARM_SUN8I_W3P1 = n
CONFIG_PLATFORM_ARM_SUN8I_W5P1 = n
CONFIG_PLATFORM_ACTIONS_ATM702X = n
CONFIG_PLATFORM_ACTIONS_ATV5201 = n
CONFIG_PLATFORM_ACTIONS_ATM705X = n
CONFIG_PLATFORM_ARM_SUN50IW1P1 = n
CONFIG_PLATFORM_ARM_RTD299X = n
CONFIG_PLATFORM_ARM_LGE = n
CONFIG_PLATFORM_ARM_SPREADTRUM_6820 = n
CONFIG_PLATFORM_ARM_SPREADTRUM_8810 = n
CONFIG_PLATFORM_ARM_WMT = n
CONFIG_PLATFORM_TI_DM365 = n
CONFIG_PLATFORM_MOZART = n
CONFIG_PLATFORM_RTK119X = n
CONFIG_PLATFORM_RTK119X_AM = n
CONFIG_PLATFORM_RTK129X = n
CONFIG_PLATFORM_RTK1319 = n
CONFIG_PLATFORM_RTK390X = n
CONFIG_PLATFORM_NOVATEK_NT72668 = n
CONFIG_PLATFORM_HISILICON = n
CONFIG_PLATFORM_HISILICON_HI3798 = n
CONFIG_PLATFORM_NV_TK1 = n
CONFIG_PLATFORM_NV_TK1_UBUNTU = n
CONFIG_PLATFORM_RTL8197D = n
CONFIG_PLATFORM_AML_S905 = n
CONFIG_PLATFORM_ZTE_ZX296716 = n
CONFIG_PLATFORM_RISCV_LIPI4A = n
########### CUSTOMER ################################
CONFIG_CUSTOMER_HUAWEI_GENERAL = n

CONFIG_DRVEXT_MODULE = n

export TopDIR ?= $(shell pwd)

########### COMMON  #################################
ifeq ($(CONFIG_GSPI_HCI), y)
HCI_NAME = gspi
endif

ifeq ($(CONFIG_SDIO_HCI), y)
HCI_NAME = sdio
endif

ifeq ($(CONFIG_USB_HCI), y)
HCI_NAME = usb
endif

ifeq ($(CONFIG_PCI_HCI), y)
HCI_NAME = pci
endif


_OS_INTFS_FILES :=	os_dep/osdep_service.o \
			os_dep/linux/os_intfs.o \
			os_dep/linux/$(HCI_NAME)_intf.o \
			os_dep/linux/$(HCI_NAME)_ops_linux.o \
			os_dep/linux/ioctl_linux.o \
			os_dep/linux/xmit_linux.o \
			os_dep/linux/mlme_linux.o \
			os_dep/linux/recv_linux.o \
			os_dep/linux/ioctl_cfg80211.o \
			os_dep/linux/rtw_cfgvendor.o \
			os_dep/linux/wifi_regd.o \
			os_dep/linux/rtw_android.o \
			os_dep/linux/rtw_proc.o \
			os_dep/linux/nlrtw.o \
			os_dep/linux/rtw_rhashtable.o

ifeq ($(CONFIG_MP_INCLUDED), y)
_OS_INTFS_FILES += os_dep/linux/ioctl_mp.o
endif

ifeq ($(CONFIG_SDIO_HCI), y)
_OS_INTFS_FILES += os_dep/linux/custom_gpio_linux.o
_OS_INTFS_FILES += os_dep/linux/$(HCI_NAME)_ops_linux.o
endif

ifeq ($(CONFIG_GSPI_HCI), y)
_OS_INTFS_FILES += os_dep/linux/custom_gpio_linux.o
_OS_INTFS_FILES += os_dep/linux/$(HCI_NAME)_ops_linux.o
endif


_HAL_INTFS_FILES :=	hal/hal_intf.o \
			hal/hal_com.o \
			hal/hal_com_phycfg.o \
			hal/hal_phy.o \
			hal/hal_dm.o \
			hal/hal_dm_acs.o \
			hal/hal_btcoex_wifionly.o \
			hal/hal_btcoex.o \
			hal/hal_mp.o \
			hal/hal_mcc.o \
			hal/hal_hci/hal_$(HCI_NAME).o \
			hal/led/hal_led.o \
			hal/led/hal_$(HCI_NAME)_led.o


ccflags-y += -I$(src)/platform
_PLATFORM_FILES := platform/platform_ops.o

ccflags-y += -I$(src)/hal/btc

########### HAL_RTL8188E #################################
ifeq ($(CONFIG_RTL8188E), y)

RTL871X = rtl8188e
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8189es
endif

ifeq ($(CONFIG_GSPI_HCI), y)
MODULE_NAME = 8189es
endif

ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8188eu
endif

ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8188ee
endif
ccflags-y += -DCONFIG_RTL8188E

_HAL_INTFS_FILES +=	hal/HalPwrSeqCmd.o \
					hal/$(RTL871X)/Hal8188EPwrSeq.o\
					hal/$(RTL871X)/$(RTL871X)_xmit.o\
					hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES +=	hal/$(RTL871X)/$(RTL871X)_hal_init.o \
			hal/$(RTL871X)/$(RTL871X)_phycfg.o \
			hal/$(RTL871X)/$(RTL871X)_rf6052.o \
			hal/$(RTL871X)/$(RTL871X)_dm.o \
			hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
			hal/$(RTL871X)/$(RTL871X)_cmd.o \
			hal/$(RTL871X)/hal8188e_s_fw.o \
			hal/$(RTL871X)/hal8188e_t_fw.o \
			hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_led.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_xmit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_recv.o

ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
else
ifeq ($(CONFIG_GSPI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
endif
endif

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8188E_USB.o
endif
ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8188E_PCIE.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8188E_SDIO.o
endif

endif

########### HAL_RTL8192E #################################
ifeq ($(CONFIG_RTL8192E), y)

RTL871X = rtl8192e
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8192es
endif

ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8192eu
endif

ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8192ee
endif
ccflags-y += -DCONFIG_RTL8192E
_HAL_INTFS_FILES += hal/HalPwrSeqCmd.o \
					hal/$(RTL871X)/Hal8192EPwrSeq.o\
					hal/$(RTL871X)/$(RTL871X)_xmit.o\
					hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES +=	hal/$(RTL871X)/$(RTL871X)_hal_init.o \
			hal/$(RTL871X)/$(RTL871X)_phycfg.o \
			hal/$(RTL871X)/$(RTL871X)_rf6052.o \
			hal/$(RTL871X)/$(RTL871X)_dm.o \
			hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
			hal/$(RTL871X)/$(RTL871X)_cmd.o \
			hal/$(RTL871X)/hal8192e_fw.o \
			hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_led.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_xmit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_recv.o

ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
else
ifeq ($(CONFIG_GSPI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
endif
endif

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8192E_USB.o
endif
ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8192E_PCIE.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8192E_SDIO.o
endif

ifeq ($(CONFIG_BT_COEXIST), y)
_BTC_FILES += hal/btc/halbtc8192e1ant.o \
				hal/btc/halbtc8192e2ant.o
endif

endif

########### HAL_RTL8812A_RTL8821A #################################

ifneq ($(CONFIG_RTL8812A)_$(CONFIG_RTL8821A), n_n)

RTL871X = rtl8812a
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8812au
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8812ae
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8812as
endif

_HAL_INTFS_FILES +=  hal/HalPwrSeqCmd.o \
					hal/$(RTL871X)/Hal8812PwrSeq.o \
					hal/$(RTL871X)/Hal8821APwrSeq.o\
					hal/$(RTL871X)/$(RTL871X)_xmit.o\
					hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES +=	hal/$(RTL871X)/$(RTL871X)_hal_init.o \
			hal/$(RTL871X)/$(RTL871X)_phycfg.o \
			hal/$(RTL871X)/$(RTL871X)_rf6052.o \
			hal/$(RTL871X)/$(RTL871X)_dm.o \
			hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
			hal/$(RTL871X)/$(RTL871X)_cmd.o \
			hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_led.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_xmit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_recv.o

ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
else
ifeq ($(CONFIG_GSPI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
endif
endif

ifeq ($(CONFIG_RTL8812A), y)
ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8812A_USB.o
endif
ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8812A_PCIE.o
endif
endif
ifeq ($(CONFIG_RTL8821A), y)
ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8821A_USB.o
endif
ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8821A_PCIE.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8821A_SDIO.o
endif
endif

ifeq ($(CONFIG_RTL8812A), y)
ccflags-y += -DCONFIG_RTL8812A
_HAL_INTFS_FILES +=	hal/rtl8812a/hal8812a_fw.o
endif

ifeq ($(CONFIG_RTL8821A), y)

ifeq ($(CONFIG_RTL8812A), n)

RTL871X = rtl8821a
ifeq ($(CONFIG_USB_HCI), y)
ifeq ($(CONFIG_BT_COEXIST), y)
MODULE_NAME := 8821au
else
MODULE_NAME := 8811au
endif
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME := 8821ae
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME := 8821as
endif

endif

ccflags-y += -DCONFIG_RTL8821A

_HAL_INTFS_FILES +=	hal/rtl8812a/hal8821a_fw.o

endif

ifeq ($(CONFIG_BT_COEXIST), y)
ifeq ($(CONFIG_RTL8812A), y)
_BTC_FILES += hal/btc/halbtc8812a1ant.o \
				hal/btc/halbtc8812a2ant.o
endif
ifeq ($(CONFIG_RTL8821A), y)
_BTC_FILES += hal/btc/halbtc8821a1ant.o \
				hal/btc/halbtc8821a2ant.o
endif
endif

endif

########### HAL_RTL8723B #################################
ifeq ($(CONFIG_RTL8723B), y)

RTL871X = rtl8723b
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8723bu
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8723be
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8723bs
endif

ccflags-y += -DCONFIG_RTL8723B

_HAL_INTFS_FILES += hal/HalPwrSeqCmd.o \
					hal/$(RTL871X)/Hal8723BPwrSeq.o\
					hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES +=	hal/$(RTL871X)/$(RTL871X)_hal_init.o \
			hal/$(RTL871X)/$(RTL871X)_phycfg.o \
			hal/$(RTL871X)/$(RTL871X)_rf6052.o \
			hal/$(RTL871X)/$(RTL871X)_dm.o \
			hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
			hal/$(RTL871X)/$(RTL871X)_cmd.o \
			hal/$(RTL871X)/hal8723b_fw.o

_HAL_INTFS_FILES +=	\
			hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_led.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_xmit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_recv.o

ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
endif

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8723B_USB.o
endif
ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8723B_PCIE.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8723B_SDIO.o
endif

_BTC_FILES += hal/btc/halbtc8723bwifionly.o
ifeq ($(CONFIG_BT_COEXIST), y)
_BTC_FILES += hal/btc/halbtc8723b1ant.o \
				hal/btc/halbtc8723b2ant.o
endif

endif

########### HAL_RTL8814A #################################
ifeq ($(CONFIG_RTL8814A), y)
## ADD NEW VHT MP HW TX MODE ##
#ccflags-y += -DCONFIG_MP_VHT_HW_TX_MODE
#CONFIG_MP_VHT_HW_TX_MODE = y
##########################################
RTL871X = rtl8814a
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8814au
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8814ae
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8814as
endif

ccflags-y += -DCONFIG_RTL8814A

_HAL_INTFS_FILES +=  hal/HalPwrSeqCmd.o \
					hal/$(RTL871X)/Hal8814PwrSeq.o \
					hal/$(RTL871X)/$(RTL871X)_xmit.o\
					hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES +=	hal/$(RTL871X)/$(RTL871X)_hal_init.o \
			hal/$(RTL871X)/$(RTL871X)_phycfg.o \
			hal/$(RTL871X)/$(RTL871X)_rf6052.o \
			hal/$(RTL871X)/$(RTL871X)_dm.o \
			hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
			hal/$(RTL871X)/$(RTL871X)_cmd.o \
			hal/$(RTL871X)/hal8814a_fw.o


_HAL_INTFS_FILES +=	\
			hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_led.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_xmit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_recv.o

ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
else
ifeq ($(CONFIG_GSPI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
endif
endif

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8814A_USB.o
endif
ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8814A_PCIE.o
endif

ifeq ($(CONFIG_BT_COEXIST), y)
_BTC_FILES += hal/btc/halbtc8814a2ant.o
endif
endif

########### HAL_RTL8723C #################################
ifeq ($(CONFIG_RTL8723C), y)

RTL871X = rtl8703b
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8723cu
MODULE_SUB_NAME = 8703bu
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8723ce
MODULE_SUB_NAME = 8703be
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8723cs
MODULE_SUB_NAME = 8703bs
endif

ccflags-y += -DCONFIG_RTL8703B

_HAL_INTFS_FILES += hal/HalPwrSeqCmd.o \
					hal/$(RTL871X)/Hal8703BPwrSeq.o\
					hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES +=	hal/$(RTL871X)/$(RTL871X)_hal_init.o \
			hal/$(RTL871X)/$(RTL871X)_phycfg.o \
			hal/$(RTL871X)/$(RTL871X)_rf6052.o \
			hal/$(RTL871X)/$(RTL871X)_dm.o \
			hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
			hal/$(RTL871X)/$(RTL871X)_cmd.o \
			hal/$(RTL871X)/hal8703b_fw.o

_HAL_INTFS_FILES +=	\
			hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_led.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_xmit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_recv.o

ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
endif

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8703B_USB.o
endif
ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8703B_PCIE.o
endif

ifeq ($(CONFIG_BT_COEXIST), y)
_BTC_FILES += hal/btc/halbtc8703b1ant.o
endif

endif

########### HAL_RTL8723D #################################
ifeq ($(CONFIG_RTL8723D), y)

RTL871X = rtl8723d
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8723du
MODULE_SUB_NAME = 8723du
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8723de
MODULE_SUB_NAME = 8723de
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8723ds
MODULE_SUB_NAME = 8723ds
endif

ccflags-y += -DCONFIG_RTL8723D

_HAL_INTFS_FILES += hal/HalPwrSeqCmd.o \
					hal/$(RTL871X)/Hal8723DPwrSeq.o\
					hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES +=	hal/$(RTL871X)/$(RTL871X)_hal_init.o \
			hal/$(RTL871X)/$(RTL871X)_phycfg.o \
			hal/$(RTL871X)/$(RTL871X)_rf6052.o \
			hal/$(RTL871X)/$(RTL871X)_dm.o \
			hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
			hal/$(RTL871X)/$(RTL871X)_cmd.o \
			hal/$(RTL871X)/hal8723d_fw.o \
			hal/$(RTL871X)/$(RTL871X)_lps_poff.o


_HAL_INTFS_FILES +=	\
			hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_led.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_xmit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_recv.o

ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
endif

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8723D_USB.o
endif
ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8723D_PCIE.o
endif

ifeq ($(CONFIG_BT_COEXIST), y)
_BTC_FILES += hal/btc/halbtc8723d1ant.o \
				hal/btc/halbtc8723d2ant.o
endif

endif

########### HAL_RTL8723F #################################
ifeq ($(CONFIG_RTL8723F), y)
RTL871X := rtl8723f
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8723fu
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8723fs
endif

endif

########### HAL_RTL8188F #################################
ifeq ($(CONFIG_RTL8188F), y)

RTL871X = rtl8188f
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8188fu
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8188fe
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8189fs
endif

ccflags-y += -DCONFIG_RTL8188F

_HAL_INTFS_FILES += hal/HalPwrSeqCmd.o \
					hal/$(RTL871X)/Hal8188FPwrSeq.o\
					hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES +=	hal/$(RTL871X)/$(RTL871X)_hal_init.o \
			hal/$(RTL871X)/$(RTL871X)_phycfg.o \
			hal/$(RTL871X)/$(RTL871X)_rf6052.o \
			hal/$(RTL871X)/$(RTL871X)_dm.o \
			hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
			hal/$(RTL871X)/$(RTL871X)_cmd.o \
			hal/$(RTL871X)/hal8188f_fw.o

_HAL_INTFS_FILES +=	\
			hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_led.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_xmit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_recv.o

ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
endif

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8188F_USB.o
endif

ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8188F_SDIO.o
endif

endif

########### HAL_RTL8188GTV #################################
ifeq ($(CONFIG_RTL8188GTV), y)

RTL871X = rtl8188gtv
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8188gtvu
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8189gtvs
endif

ccflags-y += -DCONFIG_RTL8188GTV

_HAL_INTFS_FILES += hal/HalPwrSeqCmd.o \
					hal/$(RTL871X)/Hal8188GTVPwrSeq.o\
					hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES +=	hal/$(RTL871X)/$(RTL871X)_hal_init.o \
			hal/$(RTL871X)/$(RTL871X)_phycfg.o \
			hal/$(RTL871X)/$(RTL871X)_rf6052.o \
			hal/$(RTL871X)/$(RTL871X)_dm.o \
			hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
			hal/$(RTL871X)/$(RTL871X)_cmd.o \
			hal/$(RTL871X)/hal8188gtv_fw.o

_HAL_INTFS_FILES +=	\
			hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_led.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_xmit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_recv.o

ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
endif

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8188GTV_USB.o
endif

ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8188GTV_SDIO.o
endif

endif

########### HAL_RTL8822B #################################
ifeq ($(CONFIG_RTL8822B), y)
RTL871X := rtl8822b
ifeq ($(CONFIG_USB_HCI), y)
ifeq ($(CONFIG_BT_COEXIST), n)
MODULE_NAME = 8812bu
else
MODULE_NAME = 88x2bu
endif
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 88x2be
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 88x2bs
endif

endif
########### HAL_RTL8821C #################################
ifeq ($(CONFIG_RTL8821C), y)
RTL871X := rtl8821c
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8821cu
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8821ce
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8821cs
endif

endif

########### HAL_RTL8710B #################################
ifeq ($(CONFIG_RTL8710B), y)

RTL871X = rtl8710b
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8710bu
MODULE_SUB_NAME = 8710bu
endif

ccflags-y += -DCONFIG_RTL8710B

_HAL_INTFS_FILES += hal/HalPwrSeqCmd.o \
					hal/$(RTL871X)/Hal8710BPwrSeq.o\
					hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES +=	hal/$(RTL871X)/$(RTL871X)_hal_init.o \
			hal/$(RTL871X)/$(RTL871X)_phycfg.o \
			hal/$(RTL871X)/$(RTL871X)_rf6052.o \
			hal/$(RTL871X)/$(RTL871X)_dm.o \
			hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
			hal/$(RTL871X)/$(RTL871X)_cmd.o \
			hal/$(RTL871X)/hal8710b_fw.o \
			hal/$(RTL871X)/$(RTL871X)_lps_poff.o


_HAL_INTFS_FILES +=	\
			hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_led.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_xmit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_recv.o

_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8710B_USB.o
endif

endif

########### HAL_RTL8192F #################################
ifeq ($(CONFIG_RTL8192F), y)

RTL871X = rtl8192f
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8192fu
MODULE_SUB_NAME = 8192fu
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8192fe
MODULE_SUB_NAME = 8192fe
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8192fs
MODULE_SUB_NAME = 8192fs
endif

ccflags-y += -DCONFIG_RTL8192F

_HAL_INTFS_FILES += hal/HalPwrSeqCmd.o \
					hal/$(RTL871X)/Hal8192FPwrSeq.o\
					hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES +=	hal/$(RTL871X)/$(RTL871X)_hal_init.o \
			hal/$(RTL871X)/$(RTL871X)_phycfg.o \
			hal/$(RTL871X)/$(RTL871X)_rf6052.o \
			hal/$(RTL871X)/$(RTL871X)_dm.o \
			hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
			hal/$(RTL871X)/$(RTL871X)_cmd.o \
			hal/$(RTL871X)/hal8192f_fw.o \
			hal/$(RTL871X)/$(RTL871X)_lps_poff.o


_HAL_INTFS_FILES +=	\
			hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_led.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_xmit.o \
			hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_recv.o

ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
endif

ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES += hal/efuse/$(RTL871X)/HalEfuseMask8192F_SDIO.o
endif

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES += hal/efuse/$(RTL871X)/HalEfuseMask8192F_USB.o
endif

ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES += hal/efuse/$(RTL871X)/HalEfuseMask8192F_PCIE.o
endif

ifeq ($(CONFIG_BT_COEXIST), y)
_BTC_FILES += hal/btc/halbtccommon.o \
				hal/btc/halbtc8192f.o
endif

endif

########### HAL_RTL8822C #################################
ifeq ($(CONFIG_RTL8822C), y)
RTL871X := rtl8822c
ifeq ($(CONFIG_USB_HCI), y)
ifeq ($(CONFIG_BT_COEXIST), n)
MODULE_NAME = 8812cu
else
MODULE_NAME = 88x2cu
endif
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 88x2ce
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 88x2cs
endif

endif

########### HAL_RTL8814B #################################
ifeq ($(CONFIG_RTL8814B), y)
RTL871X := rtl8814b
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8814bu
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8814be
endif

endif

########### AUTO_CFG  #################################

ifeq ($(CONFIG_AUTOCFG_CP), y)

ifeq ($(CONFIG_MULTIDRV), y)
$(shell cp $(TopDIR)/autoconf_multidrv_$(HCI_NAME)_linux.h $(TopDIR)/include/autoconf.h)
else
ifeq ($(CONFIG_RTL8188E)$(CONFIG_SDIO_HCI),yy)
$(shell cp $(TopDIR)/autoconf_rtl8189e_$(HCI_NAME)_linux.h $(TopDIR)/include/autoconf.h)
else ifeq ($(CONFIG_RTL8188F)$(CONFIG_SDIO_HCI),yy)
$(shell cp $(TopDIR)/autoconf_rtl8189f_$(HCI_NAME)_linux.h $(TopDIR)/include/autoconf.h)
else ifeq ($(CONFIG_RTL8723C),y)
$(shell cp $(TopDIR)/autoconf_rtl8723c_$(HCI_NAME)_linux.h $(TopDIR)/include/autoconf.h)
else
$(shell cp $(TopDIR)/autoconf_$(RTL871X)_$(HCI_NAME)_linux.h $(TopDIR)/include/autoconf.h)
endif
endif

endif

########### END OF PATH  #################################

ifeq ($(CONFIG_AP_MODE), y)
ccflags-y += -DCONFIG_AP_MODE
endif

ifeq ($(CONFIG_P2P), y)
ccflags-y += -DCONFIG_P2P
rustflags-y += --cfg config_p2p
ifneq ($(CONFIG_AP_MODE), y)
$(error "CONFIG_AP_MODE is required for CONFIG_P2P")
endif
endif

ifeq ($(CONFIG_USB_HCI), y)
ifeq ($(CONFIG_USB_AUTOSUSPEND), y)
ccflags-y += -DCONFIG_USB_AUTOSUSPEND
endif
endif

ifeq ($(CONFIG_MP_INCLUDED), y)
#MODULE_NAME := $(MODULE_NAME)_mp
ccflags-y += -DCONFIG_MP_INCLUDED
endif

ifeq ($(CONFIG_POWER_SAVING), y)
ifneq ($(CONFIG_IPS_MODE), default)
ccflags-y += -DRTW_IPS_MODE=$(CONFIG_IPS_MODE)
endif
ifneq ($(CONFIG_LPS_MODE), default)
ccflags-y += -DRTW_LPS_MODE=$(CONFIG_LPS_MODE)
endif
ifneq ($(CONFIG_WOW_LPS_MODE), default)
ccflags-y += -DRTW_WOW_LPS_MODE=$(CONFIG_WOW_LPS_MODE)
endif
ccflags-y += -DCONFIG_POWER_SAVING
endif

ifeq ($(CONFIG_HW_PWRP_DETECTION), y)
ccflags-y += -DCONFIG_HW_PWRP_DETECTION
endif

ifeq ($(CONFIG_BT_COEXIST), y)
ccflags-y += -DCONFIG_BT_COEXIST
endif

ifeq ($(CONFIG_WAPI_SUPPORT), y)
ccflags-y += -DCONFIG_WAPI_SUPPORT
endif


ifeq ($(CONFIG_EFUSE_CONFIG_FILE), y)
ccflags-y += -DCONFIG_EFUSE_CONFIG_FILE

#EFUSE_MAP_PATH
USER_EFUSE_MAP_PATH ?=
ifneq ($(USER_EFUSE_MAP_PATH),)
ccflags-y += -DEFUSE_MAP_PATH=\"$(USER_EFUSE_MAP_PATH)\"
else ifeq ($(MODULE_NAME), 8189es)
ccflags-y += -DEFUSE_MAP_PATH=\"/system/etc/wifi/wifi_efuse_8189e.map\"
else ifeq ($(MODULE_NAME), 8723bs)
ccflags-y += -DEFUSE_MAP_PATH=\"/system/etc/wifi/wifi_efuse_8723bs.map\"
else
ccflags-y += -DEFUSE_MAP_PATH=\"/system/etc/wifi/wifi_efuse_$(MODULE_NAME).map\"
endif

#WIFIMAC_PATH
USER_WIFIMAC_PATH ?=
ifneq ($(USER_WIFIMAC_PATH),)
ccflags-y += -DWIFIMAC_PATH=\"$(USER_WIFIMAC_PATH)\"
else
ccflags-y += -DWIFIMAC_PATH=\"/data/wifimac.txt\"
endif

endif

ifeq ($(CONFIG_EXT_CLK), y)
ccflags-y += -DCONFIG_EXT_CLK
endif

ifeq ($(CONFIG_TRAFFIC_PROTECT), y)
ccflags-y += -DCONFIG_TRAFFIC_PROTECT
endif

ifeq ($(CONFIG_LOAD_PHY_PARA_FROM_FILE), y)
ccflags-y += -DCONFIG_LOAD_PHY_PARA_FROM_FILE
#ccflags-y += -DREALTEK_CONFIG_PATH_WITH_IC_NAME_FOLDER
ccflags-y += -DREALTEK_CONFIG_PATH=\"/lib/firmware/\"
endif

ifeq ($(CONFIG_TXPWR_BY_RATE), n)
ccflags-y += -DCONFIG_TXPWR_BY_RATE=0
else ifeq ($(CONFIG_TXPWR_BY_RATE), y)
ccflags-y += -DCONFIG_TXPWR_BY_RATE=1
endif
ifeq ($(CONFIG_TXPWR_BY_RATE_EN), n)
ccflags-y += -DCONFIG_TXPWR_BY_RATE_EN=0
else ifeq ($(CONFIG_TXPWR_BY_RATE_EN), y)
ccflags-y += -DCONFIG_TXPWR_BY_RATE_EN=1
else ifeq ($(CONFIG_TXPWR_BY_RATE_EN), auto)
ccflags-y += -DCONFIG_TXPWR_BY_RATE_EN=2
endif

ifeq ($(CONFIG_TXPWR_LIMIT), n)
ccflags-y += -DCONFIG_TXPWR_LIMIT=0
else ifeq ($(CONFIG_TXPWR_LIMIT), y)
ccflags-y += -DCONFIG_TXPWR_LIMIT=1
endif
ifeq ($(CONFIG_TXPWR_LIMIT_EN), n)
ccflags-y += -DCONFIG_TXPWR_LIMIT_EN=0
else ifeq ($(CONFIG_TXPWR_LIMIT_EN), y)
ccflags-y += -DCONFIG_TXPWR_LIMIT_EN=1
else ifeq ($(CONFIG_TXPWR_LIMIT_EN), auto)
ccflags-y += -DCONFIG_TXPWR_LIMIT_EN=2
endif

ifneq ($(CONFIG_RTW_CHPLAN), 0xFF)
ccflags-y += -DCONFIG_RTW_CHPLAN=$(CONFIG_RTW_CHPLAN)
endif

ifeq ($(CONFIG_CALIBRATE_TX_POWER_BY_REGULATORY), y)
ccflags-y += -DCONFIG_CALIBRATE_TX_POWER_BY_REGULATORY
endif

ifeq ($(CONFIG_CALIBRATE_TX_POWER_TO_MAX), y)
ccflags-y += -DCONFIG_CALIBRATE_TX_POWER_TO_MAX
endif

ifeq ($(CONFIG_RTW_ADAPTIVITY_EN), disable)
ccflags-y += -DCONFIG_RTW_ADAPTIVITY_EN=0
else ifeq ($(CONFIG_RTW_ADAPTIVITY_EN), enable)
ccflags-y += -DCONFIG_RTW_ADAPTIVITY_EN=1
endif

ifeq ($(CONFIG_RTW_ADAPTIVITY_MODE), normal)
ccflags-y += -DCONFIG_RTW_ADAPTIVITY_MODE=0
else ifeq ($(CONFIG_RTW_ADAPTIVITY_MODE), carrier_sense)
ccflags-y += -DCONFIG_RTW_ADAPTIVITY_MODE=1
endif

ifeq ($(CONFIG_SIGNAL_SCALE_MAPPING), y)
ccflags-y += -DCONFIG_SIGNAL_SCALE_MAPPING
endif

ifeq ($(CONFIG_80211W), y)
ccflags-y += -DCONFIG_IEEE80211W
endif

ifeq ($(CONFIG_WOWLAN), y)
ccflags-y += -DCONFIG_WOWLAN -DRTW_WAKEUP_EVENT=$(CONFIG_WAKEUP_TYPE)
ccflags-y += -DRTW_SUSPEND_TYPE=$(CONFIG_SUSPEND_TYPE)
ifeq ($(CONFIG_WOW_STA_MIX), y)
ccflags-y += -DRTW_WOW_STA_MIX
endif
ifeq ($(CONFIG_SDIO_HCI), y)
ccflags-y += -DCONFIG_RTW_SDIO_PM_KEEP_POWER
endif
endif

ifeq ($(CONFIG_AP_WOWLAN), y)
ccflags-y += -DCONFIG_AP_WOWLAN
ifeq ($(CONFIG_AP_MODE), n)
ccflags-y += -DCONFIG_AP_MODE
endif
ifeq ($(CONFIG_SDIO_HCI), y)
ccflags-y += -DCONFIG_RTW_SDIO_PM_KEEP_POWER
endif
endif

ifeq ($(CONFIG_LAYER2_ROAMING), y)
ccflags-y += -DCONFIG_LAYER2_ROAMING -DCONFIG_ROAMING_FLAG=$(CONFIG_ROAMING_FLAG)
endif

ifeq ($(CONFIG_PNO_SUPPORT), y)
ccflags-y += -DCONFIG_PNO_SUPPORT
ifeq ($(CONFIG_PNO_SET_DEBUG), y)
ccflags-y += -DCONFIG_PNO_SET_DEBUG
endif
endif

ifeq ($(CONFIG_GPIO_WAKEUP), y)
ccflags-y += -DCONFIG_GPIO_WAKEUP
ifeq ($(CONFIG_ONE_PIN_GPIO), y)
ccflags-y += -DCONFIG_RTW_ONE_PIN_GPIO
endif
ifeq ($(CONFIG_HIGH_ACTIVE_DEV2HST), y)
ccflags-y += -DHIGH_ACTIVE_DEV2HST=1
else
ccflags-y += -DHIGH_ACTIVE_DEV2HST=0
endif
endif

ifeq ($(CONFIG_HIGH_ACTIVE_HST2DEV), y)
ccflags-y += -DHIGH_ACTIVE_HST2DEV=1
else
ccflags-y += -DHIGH_ACTIVE_HST2DEV=0
endif

ifneq ($(CONFIG_WAKEUP_GPIO_IDX), default)
ccflags-y += -DWAKEUP_GPIO_IDX=$(CONFIG_WAKEUP_GPIO_IDX)
endif

ifeq ($(CONFIG_RTW_SDIO_PM_KEEP_POWER), y)
ifeq ($(CONFIG_SDIO_HCI), y)
ccflags-y += -DCONFIG_RTW_SDIO_PM_KEEP_POWER
endif
endif

ifeq ($(CONFIG_REDUCE_TX_CPU_LOADING), y)
ccflags-y += -DCONFIG_REDUCE_TX_CPU_LOADING
endif

ifeq ($(CONFIG_BR_EXT), y)
BR_NAME = br0
ccflags-y += -DCONFIG_BR_EXT
ccflags-y += '-DCONFIG_BR_EXT_BRNAME="'$(BR_NAME)'"'
endif


ifeq ($(CONFIG_TDLS), y)
ccflags-y += -DCONFIG_TDLS
rustflags-y += --cfg tdls
endif

ifeq ($(CONFIG_WIFI_MONITOR), y)
ccflags-y += -DCONFIG_WIFI_MONITOR
endif

ifeq ($(CONFIG_MCC_MODE), y)
ccflags-y += -DCONFIG_MCC_MODE
endif

ifeq ($(CONFIG_RTW_NAPI), y)
ccflags-y += -DCONFIG_RTW_NAPI
endif

ifeq ($(CONFIG_RTW_GRO), y)
ccflags-y += -DCONFIG_RTW_GRO
endif

ifeq ($(CONFIG_RTW_REPEATER_SON), y)
ccflags-y += -DCONFIG_RTW_REPEATER_SON
endif

ifeq ($(CONFIG_RTW_IPCAM_APPLICATION), y)
ccflags-y += -DCONFIG_RTW_IPCAM_APPLICATION
ifeq ($(CONFIG_WIFI_MONITOR), n)
ccflags-y += -DCONFIG_WIFI_MONITOR
endif
endif

ifeq ($(CONFIG_RTW_NETIF_SG), y)
ccflags-y += -DCONFIG_RTW_NETIF_SG
endif

ifeq ($(CONFIG_ICMP_VOQ), y)
ccflags-y += -DCONFIG_ICMP_VOQ
endif

ifeq ($(CONFIG_IP_R_MONITOR), y)
ccflags-y += -DCONFIG_IP_R_MONITOR
endif

ifeq ($(CONFIG_MP_VHT_HW_TX_MODE), y)
ccflags-y += -DCONFIG_MP_VHT_HW_TX_MODE
ifeq ($(CONFIG_PLATFORM_I386_PC), y)
## For I386 X86 ToolChain use Hardware FLOATING
ccflags-y += -mhard-float
else
## For ARM ToolChain use Hardware FLOATING
ccflags-y += -mfloat-abi=hard
endif
endif

ifeq ($(CONFIG_APPEND_VENDOR_IE_ENABLE), y)
ccflags-y += -DCONFIG_APPEND_VENDOR_IE_ENABLE
endif

ifeq ($(CONFIG_RTW_DEBUG), y)
ccflags-y += -DCONFIG_RTW_DEBUG
ccflags-y += -DRTW_LOG_LEVEL=$(CONFIG_RTW_LOG_LEVEL)
endif

ifeq ($(CONFIG_PROC_DEBUG), y)
ccflags-y += -DCONFIG_PROC_DEBUG
endif

ifeq ($(CONFIG_RTW_UP_MAPPING_RULE), dscp)
ccflags-y += -DCONFIG_RTW_UP_MAPPING_RULE=1
rustflags-y += --cfg rtw_up_mapping_dscp
else
ccflags-y += -DCONFIG_RTW_UP_MAPPING_RULE=0
endif

ccflags-y += -DDM_ODM_SUPPORT_TYPE=0x04

ifeq ($(CONFIG_RTW_MBO), y)
ccflags-y += -DCONFIG_RTW_MBO -DCONFIG_RTW_WNM -DCONFIG_RTW_BTM_ROAM
ccflags-y += -DCONFIG_RTW_80211R
CONFIG_RTW_80211K := y
endif

# Match C #ifdef CONFIG_RTW_80211K when enabled via headers (autoconf.h or drv_conf.h).
_autoconf_has_80211k := $(shell grep -E '^[[:space:]]*#define[[:space:]]+CONFIG_RTW_80211K' $(src)/include/autoconf.h 2>/dev/null)
_autoconf_has_multi_ap := $(shell grep -E '^[[:space:]]*#define[[:space:]]+CONFIG_RTW_MULTI_AP' $(src)/include/autoconf.h 2>/dev/null)
_skip_80211k_cflag :=
ifneq ($(_autoconf_has_80211k),)
CONFIG_RTW_80211K := y
_skip_80211k_cflag := y
endif
ifneq ($(_autoconf_has_multi_ap),)
CONFIG_RTW_80211K := y
_skip_80211k_cflag := y
endif
ifneq ($(filter -DCONFIG_RTW_MULTI_AP,$(USER_EXTRA_CFLAGS)),)
CONFIG_RTW_80211K := y
_skip_80211k_cflag := y
endif

ifneq ($(filter -DCONFIG_RTW_80211K,$(USER_EXTRA_CFLAGS)),)
CONFIG_RTW_80211K := y
endif

ifeq ($(CONFIG_RTW_80211K), y)
ifeq ($(_skip_80211k_cflag),)
ccflags-y += -DCONFIG_RTW_80211K
endif
endif

ifeq ($(CONFIG_RTW_IOCTL_SET_COUNTRY), y)
ccflags-y += -DCONFIG_RTW_IOCTL_SET_COUNTRY
endif

ifeq ($(CONFIG_PLATFORM_I386_PC), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT

SUBARCH := $(shell uname -m | sed -e 's/i.86/i386/;s/armv7l/arm/;s/aarch64/arm64/;s/riscv64/riscv/')
ARCH ?= $(SUBARCH)
CROSS_COMPILE ?=
KVER  := $(shell uname -r)
KSRC := /lib/modules/$(KVER)/build
MODDESTDIR := /lib/modules/$(KVER)/kernel/drivers/net/wireless/
INSTALL_PREFIX :=
STAGINGMODDIR := /lib/modules/$(KVER)/kernel/drivers/staging
endif

ifeq ($(CONFIG_PLATFORM_NV_TK1), y)
ccflags-y += -DCONFIG_PLATFORM_NV_TK1
ccflags-y += -DCONFIG_LITTLE_ENDIAN
# default setting for Android 4.1, 4.2
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_P2P_IPS -DCONFIG_PLATFORM_ANDROID
# Enable this for Android 5.0
ccflags-y += -DCONFIG_RADIO_WORK
ccflags-y += -DRTW_VENDOR_EXT_SUPPORT
ccflags-y += -DRTW_ENABLE_WIFI_CONTROL_FUNC
ARCH ?= arm

CROSS_COMPILE := /mnt/newdisk/android_sdk/nvidia_tk1/android_L/prebuilts/gcc/linux-x86/arm/arm-eabi-4.8/bin/arm-eabi-
KSRC :=/mnt/newdisk/android_sdk/nvidia_tk1/android_L/out/target/product/shieldtablet/obj/KERNEL/
MODULE_NAME = wlan
endif

ifeq ($(CONFIG_PLATFORM_NV_TK1_UBUNTU), y)
ccflags-y += -DCONFIG_PLATFORM_NV_TK1
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT

ARCH ?= arm

CROSS_COMPILE ?=
KVER := $(shell uname -r)
KSRC := /lib/modules/$(KVER)/build
MODDESTDIR := /lib/modules/$(KVER)/kernel/drivers/net/wireless/
INSTALL_PREFIX :=
endif

ifeq ($(CONFIG_PLATFORM_ACTIONS_ATM702X), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_ANDROID -DCONFIG_PLATFORM_ACTIONS_ATM702X
#ARCH := arm
ARCH := $(R_ARCH)
#CROSS_COMPILE := arm-none-linux-gnueabi-
CROSS_COMPILE := $(R_CROSS_COMPILE)
KVER:= 3.4.0
#KSRC := ../../../../build/out/kernel
KSRC := $(KERNEL_BUILD_PATH)
MODULE_NAME :=wlan
endif


ifeq ($(CONFIG_PLATFORM_ACTIONS_ATM705X), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
#ccflags-y += -DRTW_ENABLE_WIFI_CONTROL_FUNC
# default setting for Android 4.1, 4.2, 4.3, 4.4
ccflags-y += -DCONFIG_PLATFORM_ACTIONS_ATM705X
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT

# Enable this for Android 5.0
ccflags-y += -DCONFIG_RADIO_WORK

ifeq ($(CONFIG_SDIO_HCI), y)
ccflags-y += -DCONFIG_PLATFORM_OPS
_PLATFORM_FILES += platform/platform_arm_act_sdio.o
endif

ARCH := arm
CROSS_COMPILE := /opt/arm-2011.09/bin/arm-none-linux-gnueabi-
KSRC := /home/android_sdk/Action-semi/705a_android_L/android/kernel
endif

ifeq ($(CONFIG_PLATFORM_ARM_SUN50IW1P1), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ccflags-y += -DCONFIG_PLATFORM_ARM_SUN50IW1P1
ccflags-y += -DCONFIG_TRAFFIC_PROTECT
# default setting for Android 4.1, 4.2
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
ccflags-y += -DCONFIG_RESUME_IN_WORKQUEUE
ccflags-y += -DCONFIG_PLATFORM_OPS

# Enable this for Android 5.0
ccflags-y += -DCONFIG_RADIO_WORK

ifeq ($(CONFIG_USB_HCI), y)
ccflags-y += -DCONFIG_USE_USB_BUFFER_ALLOC_TX
_PLATFORM_FILES += platform/platform_ARM_SUNxI_usb.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
_PLATFORM_FILES += platform/platform_ARM_SUN50IW1P1_sdio.o
endif

ARCH := arm64
# ===Cross compile setting for Android 5.1(64) SDK ===
CROSS_COMPILE := /home/android_sdk/Allwinner/a64/android-51/lichee/out/sun50iw1p1/android/common/buildroot/external-toolchain/bin/aarch64-linux-gnu-
KSRC :=/home/android_sdk/Allwinner/a64/android-51/lichee/linux-3.10/
endif

ifeq ($(CONFIG_PLATFORM_TI_AM3517), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_ANDROID -DCONFIG_PLATFORM_SHUTTLE
CROSS_COMPILE := arm-eabi-
KSRC := $(shell pwd)/../../../Android/kernel
ARCH := arm
endif

ifeq ($(CONFIG_PLATFORM_MSTAR_TITANIA12), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_MSTAR -DCONFIG_PLATFORM_MSTAR_TITANIA12
ARCH:=mips
CROSS_COMPILE:= /usr/src/Mstar_kernel/mips-4.3/bin/mips-linux-gnu-
KVER:= 2.6.28.9
KSRC:= /usr/src/Mstar_kernel/2.6.28.9/
endif

ifeq ($(CONFIG_PLATFORM_MSTAR), y)
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ccflags-y += -DCONFIG_PLATFORM_MSTAR
ccflags-y += -DCONFIG_PLATFORM_MSTAR_HIGH
ifeq ($(CONFIG_USB_HCI), y)
ccflags-y += -DCONFIG_USE_USB_BUFFER_ALLOC_TX -DCONFIG_FIX_NR_BULKIN_BUFFER
endif
ARCH:=arm
CROSS_COMPILE:= /usr/src/bin/arm-none-linux-gnueabi-
KVER:= 3.1.10
KSRC:= /usr/src/Mstar_kernel/3.1.10/
endif

ifeq ($(CONFIG_PLATFORM_ANDROID_X86), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
SUBARCH := $(shell uname -m | sed -e s/i.86/i386/)
ARCH := $(SUBARCH)
CROSS_COMPILE := /media/DATA-2/android-x86/ics-x86_20120130/prebuilt/linux-x86/toolchain/i686-unknown-linux-gnu-4.2.1/bin/i686-unknown-linux-gnu-
KSRC := /media/DATA-2/android-x86/ics-x86_20120130/out/target/product/generic_x86/obj/kernel
MODULE_NAME :=wlan
endif

ifeq ($(CONFIG_PLATFORM_ANDROID_INTEL_X86), y)
ccflags-y += -DCONFIG_PLATFORM_ANDROID_INTEL_X86
ccflags-y += -DCONFIG_PLATFORM_INTEL_BYT
ccflags-y += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_ANDROID
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
ccflags-y += -DCONFIG_SKIP_SIGNAL_SCALE_MAPPING
ifeq ($(CONFIG_SDIO_HCI), y)
ccflags-y += -DCONFIG_RESUME_IN_WORKQUEUE
endif
endif

ifeq ($(CONFIG_PLATFORM_JB_X86), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
SUBARCH := $(shell uname -m | sed -e s/i.86/i386/)
ARCH := $(SUBARCH)
CROSS_COMPILE := /home/android_sdk/android-x86_JB/prebuilts/gcc/linux-x86/x86/i686-linux-android-4.7/bin/i686-linux-android-
KSRC := /home/android_sdk/android-x86_JB/out/target/product/x86/obj/kernel/
MODULE_NAME :=wlan
endif

ifeq ($(CONFIG_PLATFORM_ARM_PXA2XX), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ARCH := arm
CROSS_COMPILE := arm-none-linux-gnueabi-
KVER  := 2.6.34.1
KSRC ?= /usr/src/linux-2.6.34.1
endif

ifeq ($(CONFIG_PLATFORM_ARM_S3C2K4), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ARCH := arm
CROSS_COMPILE := arm-linux-
KVER  := 2.6.24.7_$(ARCH)
KSRC := /usr/src/kernels/linux-$(KVER)
endif

ifeq ($(CONFIG_PLATFORM_ARM_S3C6K4), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ARCH := arm
CROSS_COMPILE := arm-none-linux-gnueabi-
KVER  := 2.6.34.1
KSRC ?= /usr/src/linux-2.6.34.1
endif

ifeq ($(CONFIG_PLATFORM_RTD2880B), y)
ccflags-y += -DCONFIG_BIG_ENDIAN -DCONFIG_PLATFORM_RTD2880B
ARCH:=
CROSS_COMPILE:=
KVER:=
KSRC:=
endif

ifeq ($(CONFIG_PLATFORM_MIPS_RMI), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ARCH:=mips
CROSS_COMPILE:=mipsisa32r2-uclibc-
KVER:=
KSRC:= /root/work/kernel_realtek
endif

ifeq ($(CONFIG_PLATFORM_MIPS_PLM), y)
ccflags-y += -DCONFIG_BIG_ENDIAN
ARCH:=mips
CROSS_COMPILE:=mipsisa32r2-uclibc-
KVER:=
KSRC:= /root/work/kernel_realtek
endif

ifeq ($(CONFIG_PLATFORM_MSTAR389), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_MSTAR389
ARCH:=mips
CROSS_COMPILE:= mips-linux-gnu-
KVER:= 2.6.28.10
KSRC:= /home/mstar/mstar_linux/2.6.28.9/
endif

ifeq ($(CONFIG_PLATFORM_MIPS_AR9132), y)
ccflags-y += -DCONFIG_BIG_ENDIAN
ARCH := mips
CROSS_COMPILE := mips-openwrt-linux-
KSRC := /home/alex/test_openwrt/tmp/linux-2.6.30.9
endif

ifeq ($(CONFIG_PLATFORM_DMP_PHILIPS), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN -DRTK_DMP_PLATFORM
ARCH := mips
#CROSS_COMPILE:=/usr/local/msdk-4.3.6-mips-EL-2.6.12.6-0.9.30.3/bin/mipsel-linux-
CROSS_COMPILE:=/usr/local/toolchain_mipsel/bin/mipsel-linux-
KSRC ?=/usr/local/Jupiter/linux-2.6.12
endif

ifeq ($(CONFIG_PLATFORM_RTK_DMP), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN -DRTK_DMP_PLATFORM  -DCONFIG_WIRELESS_EXT
ccflags-y += -DCONFIG_PLATFORM_OPS
ifeq ($(CONFIG_USB_HCI), y)
_PLATFORM_FILES += platform/platform_RTK_DMP_usb.o
endif
ARCH:=mips
CROSS_COMPILE:=mipsel-linux-
KVER:=
KSRC ?= /usr/src/DMP_Kernel/jupiter/linux-2.6.12
endif

ifeq ($(CONFIG_PLATFORM_MT53XX), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_MT53XX
ARCH:= arm
CROSS_COMPILE:= arm11_mtk_le-
KVER:= 2.6.27
KSRC?= /proj/mtk00802/BD_Compare/BDP/Dev/BDP_V301/BDP_Linux/linux-2.6.27
endif

ifeq ($(CONFIG_PLATFORM_ARM_MX51_241H), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN -DCONFIG_WISTRON_PLATFORM
ARCH := arm
CROSS_COMPILE := /opt/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/arm-none-linux-gnueabi-
KVER  := 2.6.31
KSRC ?= /lib/modules/2.6.31-770-g0e46b52/source
endif

ifeq ($(CONFIG_PLATFORM_FS_MX61), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ARCH := arm
CROSS_COMPILE := /home/share/CusEnv/FreeScale/arm-eabi-4.4.3/bin/arm-eabi-
KSRC ?= /home/share/CusEnv/FreeScale/FS_kernel_env
endif



ifeq ($(CONFIG_PLATFORM_ACTIONS_ATJ227X), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_ACTIONS_ATJ227X
ARCH := mips
CROSS_COMPILE := /home/cnsd4/project/actions/tools-2.6.27/bin/mipsel-linux-gnu-
KVER  := 2.6.27
KSRC := /home/cnsd4/project/actions/linux-2.6.27.28
endif

ifeq ($(CONFIG_PLATFORM_TI_DM365), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_TI_DM365
ccflags-y += -DCONFIG_USE_USB_BUFFER_ALLOC_RX
ccflags-y += -DCONFIG_SINGLE_XMIT_BUF -DCONFIG_SINGLE_RECV_BUF
ARCH := arm
#CROSS_COMPILE := /home/cnsd4/Appro/mv_pro_5.0/montavista/pro/devkit/arm/v5t_le/bin/arm_v5t_le-
#KSRC := /home/cnsd4/Appro/mv_pro_5.0/montavista/pro/devkit/lsp/ti-davinci/linux-dm365
CROSS_COMPILE := /opt/montavista/pro5.0/devkit/arm/v5t_le/bin/arm-linux-
KSRC:= /home/vivotek/lsp/DM365/kernel_platform/kernel/linux-2.6.18
KERNELOUTPUT := ${PRODUCTDIR}/tmp
KVER  := 2.6.18
endif

ifeq ($(CONFIG_PLATFORM_MOZART), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_MOZART
ARCH := arm
CROSS_COMPILE := /home/vivotek/lsp/mozart3v2/Mozart3e_Toolchain/build_arm_nofpu/usr/bin/arm-linux-
KVER  := $(shell uname -r)
KSRC:= /opt/Vivotek/lsp/mozart3v2/kernel_platform/kernel/mozart_kernel-1.17
KERNELOUTPUT := /home/pink/sample/ODM/IP8136W-VINT/tmp/kernel
endif

ifeq ($(CONFIG_PLATFORM_TEGRA3_CARDHU), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
# default setting for Android 4.1, 4.2
ccflags-y += -DRTW_ENABLE_WIFI_CONTROL_FUNC
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
ARCH := arm
CROSS_COMPILE := /home/android_sdk/nvidia/tegra-16r3-partner-android-4.1_20120723/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-
KSRC := /home/android_sdk/nvidia/tegra-16r3-partner-android-4.1_20120723/out/target/product/cardhu/obj/KERNEL
MODULE_NAME := wlan
endif

ifeq ($(CONFIG_PLATFORM_TEGRA4_DALMORE), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
# default setting for Android 4.1, 4.2
ccflags-y += -DRTW_ENABLE_WIFI_CONTROL_FUNC
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
ARCH := arm
CROSS_COMPILE := /home/android_sdk/nvidia/tegra-17r9-partner-android-4.2-dalmore_20130131/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-
KSRC := /home/android_sdk/nvidia/tegra-17r9-partner-android-4.2-dalmore_20130131/out/target/product/dalmore/obj/KERNEL
MODULE_NAME := wlan
endif

ifeq ($(CONFIG_PLATFORM_ARM_TCC8900), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ARCH := arm
CROSS_COMPILE := /home/android_sdk/Telechips/SDK_2304_20110613/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-
KSRC := /home/android_sdk/Telechips/SDK_2304_20110613/kernel
MODULE_NAME := wlan
endif

ifeq ($(CONFIG_PLATFORM_ARM_TCC8920), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ARCH := arm
CROSS_COMPILE := /home/android_sdk/Telechips/v12.06_r1-tcc-android-4.0.4/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-
KSRC := /home/android_sdk/Telechips/v12.06_r1-tcc-android-4.0.4/kernel
MODULE_NAME := wlan
endif

ifeq ($(CONFIG_PLATFORM_ARM_TCC8920_JB42), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
# default setting for Android 4.1, 4.2
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
ARCH := arm
CROSS_COMPILE := /home/android_sdk/Telechips/v13.03_r1-tcc-android-4.2.2_ds_patched/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-
KSRC := /home/android_sdk/Telechips/v13.03_r1-tcc-android-4.2.2_ds_patched/kernel
MODULE_NAME := wlan
endif

ifeq ($(CONFIG_PLATFORM_ARM_RK2818), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_ANDROID -DCONFIG_PLATFORM_ROCKCHIPS
ARCH := arm
CROSS_COMPILE := /usr/src/release_fae_version/toolchain/arm-eabi-4.4.0/bin/arm-eabi-
KSRC := /usr/src/release_fae_version/kernel25_A7_281x
MODULE_NAME := wlan
endif

ifeq ($(CONFIG_PLATFORM_ARM_RK3188), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_ANDROID -DCONFIG_PLATFORM_ROCKCHIPS
# default setting for Android 4.1, 4.2, 4.3, 4.4
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
ccflags-y += -DCONFIG_CONCURRENT_MODE
# default setting for Power control
ccflags-y += -DRTW_ENABLE_WIFI_CONTROL_FUNC
ifeq ($(CONFIG_SDIO_HCI), y)
ccflags-y += -DRTW_SUPPORT_PLATFORM_SHUTDOWN
endif
# default setting for Special function
ARCH := arm
CROSS_COMPILE := /home/android_sdk/Rockchip/Rk3188/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-
KSRC := /home/android_sdk/Rockchip/Rk3188/kernel
MODULE_NAME := wlan
endif

ifeq ($(CONFIG_PLATFORM_ARM_RK3066), y)
ccflags-y += -DCONFIG_PLATFORM_ARM_RK3066
ccflags-y += -DRTW_ENABLE_WIFI_CONTROL_FUNC
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_IOCTL_CFG80211
ifeq ($(CONFIG_SDIO_HCI), y)
ccflags-y += -DRTW_SUPPORT_PLATFORM_SHUTDOWN
endif
ccflags-y += -fno-pic
ARCH := arm
CROSS_COMPILE := /home/android_sdk/Rockchip/rk3066_20130607/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-
#CROSS_COMPILE := /home/android_sdk/Rockchip/Rk3066sdk/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.6/bin/arm-linux-androideabi-
KSRC := /home/android_sdk/Rockchip/Rk3066sdk/kernel
MODULE_NAME :=wlan
endif

ifeq ($(CONFIG_PLATFORM_ARM_URBETTER), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN #-DCONFIG_MINIMAL_MEMORY_USAGE
ARCH := arm
CROSS_COMPILE := /media/DATA-1/urbetter/arm-2009q3/bin/arm-none-linux-gnueabi-
KSRC := /media/DATA-1/urbetter/ics-urbetter/kernel
MODULE_NAME := wlan
endif

ifeq ($(CONFIG_PLATFORM_ARM_TI_PANDA), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN #-DCONFIG_MINIMAL_MEMORY_USAGE
ARCH := arm
#CROSS_COMPILE := /media/DATA-1/aosp/ics-aosp_20111227/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-
#KSRC := /media/DATA-1/aosp/android-omap-panda-3.0_20120104
CROSS_COMPILE := /media/DATA-1/android-4.0/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-
KSRC := /media/DATA-1/android-4.0/panda_kernel/omap
MODULE_NAME := wlan
endif

ifeq ($(CONFIG_PLATFORM_MIPS_JZ4760), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN -DCONFIG_MINIMAL_MEMORY_USAGE
ARCH ?= mips
CROSS_COMPILE ?= /mnt/sdb5/Ingenic/Umido/mips-4.3/bin/mips-linux-gnu-
KSRC ?= /mnt/sdb5/Ingenic/Umido/kernel
endif

ifeq ($(CONFIG_PLATFORM_SZEBOOK), y)
ccflags-y += -DCONFIG_BIG_ENDIAN
ARCH:=arm
CROSS_COMPILE:=/opt/crosstool2/bin/armeb-unknown-linux-gnueabi-
KVER:= 2.6.31.6
KSRC:= ../code/linux-2.6.31.6-2020/
endif

ifeq ($(CONFIG_PLATFORM_ARM_SUNxI), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ccflags-y += -DCONFIG_PLATFORM_ARM_SUNxI
# default setting for Android 4.1, 4.2
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT

ccflags-y += -DCONFIG_PLATFORM_OPS
ifeq ($(CONFIG_USB_HCI), y)
ccflags-y += -DCONFIG_USE_USB_BUFFER_ALLOC_TX
_PLATFORM_FILES += platform/platform_ARM_SUNxI_usb.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
# default setting for A10-EVB mmc0
#ccflags-y += -DCONFIG_WITS_EVB_V13
_PLATFORM_FILES += platform/platform_ARM_SUNxI_sdio.o
endif

ARCH := arm
#CROSS_COMPILE := arm-none-linux-gnueabi-
CROSS_COMPILE=/home/android_sdk/Allwinner/a10/android-jb42/lichee-jb42/buildroot/output/external-toolchain/bin/arm-none-linux-gnueabi-
KVER  := 3.0.8
#KSRC:= ../lichee/linux-3.0/
KSRC=/home/android_sdk/Allwinner/a10/android-jb42/lichee-jb42/linux-3.0
endif

ifeq ($(CONFIG_PLATFORM_ARM_SUN6I), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ccflags-y += -DCONFIG_PLATFORM_ARM_SUN6I
ccflags-y += -DCONFIG_TRAFFIC_PROTECT
# default setting for Android 4.1, 4.2, 4.3, 4.4
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
ccflags-y +=  -DCONFIG_QOS_OPTIMIZATION

ccflags-y += -DCONFIG_PLATFORM_OPS
ifeq ($(CONFIG_USB_HCI), y)
ccflags-y += -DCONFIG_USE_USB_BUFFER_ALLOC_TX
_PLATFORM_FILES += platform/platform_ARM_SUNxI_usb.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
# default setting for A31-EVB mmc0
ccflags-y += -DCONFIG_A31_EVB
_PLATFORM_FILES += platform/platform_ARM_SUNnI_sdio.o
endif

ARCH := arm
#Android-JB42
#CROSS_COMPILE := /home/android_sdk/Allwinner/a31/android-jb42/lichee/buildroot/output/external-toolchain/bin/arm-linux-gnueabi-
#KSRC :=/home/android_sdk/Allwinner/a31/android-jb42/lichee/linux-3.3
#ifeq ($(CONFIG_USB_HCI), y)
#MODULE_NAME := 8188eu_sw
#endif
# ==== Cross compile setting for kitkat-a3x_v4.5 =====
CROSS_COMPILE := /home/android_sdk/Allwinner/a31/kitkat-a3x_v4.5/lichee/buildroot/output/external-toolchain/bin/arm-linux-gnueabi-
KSRC :=/home/android_sdk/Allwinner/a31/kitkat-a3x_v4.5/lichee/linux-3.3
endif

ifeq ($(CONFIG_PLATFORM_ARM_SUN7I), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ccflags-y += -DCONFIG_PLATFORM_ARM_SUN7I
ccflags-y += -DCONFIG_TRAFFIC_PROTECT
# default setting for Android 4.1, 4.2, 4.3, 4.4
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
ccflags-y +=  -DCONFIG_QOS_OPTIMIZATION

ccflags-y += -DCONFIG_PLATFORM_OPS
ifeq ($(CONFIG_USB_HCI), y)
ccflags-y += -DCONFIG_USE_USB_BUFFER_ALLOC_TX
_PLATFORM_FILES += platform/platform_ARM_SUNxI_usb.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
_PLATFORM_FILES += platform/platform_ARM_SUNnI_sdio.o
endif

ARCH := arm
# ===Cross compile setting for Android 4.2 SDK ===
#CROSS_COMPILE := /home/android_sdk/Allwinner/a20_evb/lichee/out/android/common/buildroot/external-toolchain/bin/arm-linux-gnueabi-
#KSRC := /home/android_sdk/Allwinner/a20_evb/lichee/linux-3.3
# ==== Cross compile setting for Android 4.3 SDK =====
#CROSS_COMPILE := /home/android_sdk/Allwinner/a20/android-jb43/lichee/out/android/common/buildroot/external-toolchain/bin/arm-linux-gnueabi-
#KSRC := /home/android_sdk/Allwinner/a20/android-jb43/lichee/linux-3.4
# ==== Cross compile setting for kitkat-a20_v4.4 =====
CROSS_COMPILE := /home/android_sdk/Allwinner/a20/kitkat-a20_v4.4/lichee/out/android/common/buildroot/external-toolchain/bin/arm-linux-gnueabi-
KSRC := /home/android_sdk/Allwinner/a20/kitkat-a20_v4.4/lichee/linux-3.4
endif

ifeq ($(CONFIG_PLATFORM_ARM_SUN8I_W3P1), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ccflags-y += -DCONFIG_PLATFORM_ARM_SUN8I
ccflags-y += -DCONFIG_PLATFORM_ARM_SUN8I_W3P1
ccflags-y += -DCONFIG_TRAFFIC_PROTECT
# default setting for Android 4.1, 4.2
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT

ccflags-y += -DCONFIG_PLATFORM_OPS
ifeq ($(CONFIG_USB_HCI), y)
ccflags-y += -DCONFIG_USE_USB_BUFFER_ALLOC_TX
_PLATFORM_FILES += platform/platform_ARM_SUNxI_usb.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
_PLATFORM_FILES += platform/platform_ARM_SUNnI_sdio.o
endif

ARCH := arm
# ===Cross compile setting for Android 4.2 SDK ===
#CROSS_COMPILE := /home/android_sdk/Allwinner/a23/android-jb42/lichee/out/android/common/buildroot/external-toolchain/bin/arm-linux-gnueabi-
#KSRC :=/home/android_sdk/Allwinner/a23/android-jb42/lichee/linux-3.4
# ===Cross compile setting for Android 4.4 SDK ===
CROSS_COMPILE := /home/android_sdk/Allwinner/a23/android-kk44/lichee/out/android/common/buildroot/external-toolchain/bin/arm-linux-gnueabi-
KSRC :=/home/android_sdk/Allwinner/a23/android-kk44/lichee/linux-3.4
endif

ifeq ($(CONFIG_PLATFORM_ARM_SUN8I_W5P1), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ccflags-y += -DCONFIG_PLATFORM_ARM_SUN8I
ccflags-y += -DCONFIG_PLATFORM_ARM_SUN8I_W5P1
ccflags-y += -DCONFIG_TRAFFIC_PROTECT
# default setting for Android 4.1, 4.2
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT

# Enable this for Android 5.0
ccflags-y += -DCONFIG_RADIO_WORK

ccflags-y += -DCONFIG_PLATFORM_OPS
ifeq ($(CONFIG_USB_HCI), y)
ccflags-y += -DCONFIG_USE_USB_BUFFER_ALLOC_TX
_PLATFORM_FILES += platform/platform_ARM_SUNxI_usb.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
_PLATFORM_FILES += platform/platform_ARM_SUNnI_sdio.o
endif

ARCH := arm
# ===Cross compile setting for Android L SDK ===
CROSS_COMPILE := /home/android_sdk/Allwinner/a33/android-L/lichee/out/sun8iw5p1/android/common/buildroot/external-toolchain/bin/arm-linux-gnueabi-
KSRC :=/home/android_sdk/Allwinner/a33/android-L/lichee/linux-3.4
endif

ifeq ($(CONFIG_PLATFORM_ACTIONS_ATV5201), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_ACTIONS_ATV5201
ccflags-y += -DCONFIG_SDIO_DISABLE_RXFIFO_POLLING_LOOP
ARCH := mips
CROSS_COMPILE := mipsel-linux-gnu-
KVER  := $(KERNEL_VER)
KSRC:= $(CFGDIR)/../../kernel/linux-$(KERNEL_VER)
endif

ifeq ($(CONFIG_PLATFORM_ARM_RTD299X), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
ifeq ($(CONFIG_ANDROID), y)
# Enable this for Android 5.0
ccflags-y += -DCONFIG_RADIO_WORK
endif
#ARCH, CROSS_COMPILE, KSRC,and  MODDESTDIR are provided by external makefile
INSTALL_PREFIX :=
MODULE_NAME := wlan
endif

ifeq ($(CONFIG_PLATFORM_ARM_RTD299X_LG), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DRTW_P2P_GROUP_INTERFACE=1
ccflags-y += -DCONFIG_IFACE_NUMBER=3
#ccflags-y += -DCONFIG_FIX_HWPORT
ccflags-y += -DLGE_PRIVATE
ccflags-y += -DPURE_SUPPLICANT
ccflags-y += -DCONFIG_CUSTOMIZED_COUNTRY_CHPLAN_MAP -DCONFIG_RTW_IOCTL_SET_COUNTRY
ccflags-y += -DDBG_RX_DFRAME_RAW_DATA
ccflags-y += -DRTW_REDUCE_SCAN_SWITCH_CH_TIME
ARCH ?= arm
KVER ?=

ifneq ($(PLATFORM), WEBOS)
$(info PLATFORM is empty)
CROSS_COMPILE ?= /mnt/newdisk/LGE/arm-lg115x-linux-gnueabi-4.8-2016.03-x86_64/bin/arm-lg115x-linux-gnueabi-
KSRC ?= /mnt/newdisk/LGE/linux-rockhopper_k3lp_drd4tv_423
endif

CROSS_COMPILE ?=
KSRC ?= $(LINUX_SRC)
INSTALL_PREFIX ?=
endif

ifeq ($(CONFIG_PLATFORM_HISILICON), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_HISILICON
ifeq ($(SUPPORT_CONCURRENT),y)
ccflags-y += -DCONFIG_CONCURRENT_MODE
endif
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
ARCH := arm
ifeq ($(CROSS_COMPILE),)
       CROSS_COMPILE = arm-hisiv200-linux-
endif
MODULE_NAME := rtl8192eu
ifeq ($(KSRC),)
       KSRC := ../../../../../../kernel/linux-3.4.y
endif
endif

ifeq ($(CONFIG_PLATFORM_HISILICON_HI3798), y)
ccflags-y += -DCONFIG_PLATFORM_HISILICON
ccflags-y += -DCONFIG_PLATFORM_HISILICON_HI3798
#ccflags-y += -DCONFIG_PLATFORM_HISILICON_HI3798_MV200_HDMI_DONGLE
ccflags-y += -DCONFIG_LITTLE_ENDIAN
# default setting for Android
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_IOCTL_CFG80211
ccflags-y += -DRTW_USE_CFG80211_STA_EVENT
# default setting for Android 5.x and later
#ccflags-y += -DCONFIG_RADIO_WORK

# If system could power on and recognize Wi-Fi SDIO automatically,
# platfrom operations are not necessary.
#ifeq ($(CONFIG_SDIO_HCI), y)
#ccflags-y += -DCONFIG_PLATFORM_OPS
#_PLATFORM_FILES += platform/platform_hisilicon_hi3798_sdio.o
#ccflags-y += -DCONFIG_HISI_SDIO_ID=1
#endif

ARCH ?= arm
CROSS_COMPILE ?= /HiSTBAndroidV600R003C00SPC021_git_0512/device/hisilicon/bigfish/sdk/tools/linux/toolchains/arm-histbv310-linux/bin/arm-histbv310-linux-
ifndef KSRC
KSRC := /HiSTBAndroidV600R003C00SPC021_git_0512/device/hisilicon/bigfish/sdk/source/kernel/linux-3.18.y
KSRC += O=/HiSTBAndroidV600R003C00SPC021_git_0512/out/target/product/Hi3798MV200/obj/KERNEL_OBJ
endif

ifeq ($(CONFIG_RTL8822B), y)
ifeq ($(CONFIG_SDIO_HCI), y)
CONFIG_RTL8822BS ?= m
USER_MODULE_NAME := rtl8822bs
endif
endif

endif

# Platform setting
ifeq ($(CONFIG_PLATFORM_ARM_SPREADTRUM_6820), y)
ifeq ($(CONFIG_ANDROID_2X), y)
ccflags-y += -DANDROID_2X
endif
ccflags-y += -DCONFIG_PLATFORM_SPRD
ccflags-y += -DPLATFORM_SPREADTRUM_6820
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ifeq ($(RTL871X), rtl8188e)
ccflags-y += -DSOFTAP_PS_DURATION=50
endif
ifeq ($(CONFIG_SDIO_HCI), y)
ccflags-y += -DCONFIG_PLATFORM_OPS
_PLATFORM_FILES += platform/platform_sprd_sdio.o
endif
endif

ifeq ($(CONFIG_PLATFORM_ARM_SPREADTRUM_8810), y)
ifeq ($(CONFIG_ANDROID_2X), y)
ccflags-y += -DANDROID_2X
endif
ccflags-y += -DCONFIG_PLATFORM_SPRD
ccflags-y += -DPLATFORM_SPREADTRUM_8810
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ifeq ($(RTL871X), rtl8188e)
ccflags-y += -DSOFTAP_PS_DURATION=50
endif
ifeq ($(CONFIG_SDIO_HCI), y)
ccflags-y += -DCONFIG_PLATFORM_OPS
_PLATFORM_FILES += platform/platform_sprd_sdio.o
endif
endif

ifeq ($(CONFIG_PLATFORM_ARM_WMT), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
ccflags-y += -DCONFIG_PLATFORM_OPS
ifeq ($(CONFIG_SDIO_HCI), y)
_PLATFORM_FILES += platform/platform_ARM_WMT_sdio.o
endif
ARCH := arm
CROSS_COMPILE := /home/android_sdk/WonderMedia/wm8880-android4.4/toolchain/arm_201103_gcc4.5.2/mybin/arm_1103_le-
KSRC := /home/android_sdk/WonderMedia/wm8880-android4.4/kernel4.4/
MODULE_NAME :=8189es_kk
endif

ifeq ($(CONFIG_PLATFORM_RTK119X), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
#ccflags-y += -DCONFIG_PLATFORM_ARM_SUN7I
ccflags-y += -DCONFIG_TRAFFIC_PROTECT
# default setting for Android 4.1, 4.2
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
#ccflags-y +=  -DCONFIG_QOS_OPTIMIZATION
ccflags-y += -DCONFIG_QOS_OPTIMIZATION

#ccflags-y += -DCONFIG_#PLATFORM_OPS
ifeq ($(CONFIG_USB_HCI), y)
ccflags-y += -DCONFIG_USE_USB_BUFFER_ALLOC_TX
#_PLATFORM_FILES += platform/platform_ARM_SUNxI_usb.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
_PLATFORM_FILES += platform/platform_ARM_SUNnI_sdio.o
endif

ARCH := arm

# ==== Cross compile setting for Android 4.4 SDK =====
#CROSS_COMPILE := arm-linux-gnueabihf-
KVER  := 3.10.24
#KSRC :=/home/android_sdk/Allwinner/a20/android-kitkat44/lichee/linux-3.4
CROSS_COMPILE := /home/realtek/software_phoenix/phoenix/toolchain/usr/local/arm-2013.11/bin/arm-linux-gnueabihf-
KSRC := /home/realtek/software_phoenix/linux-kernel
MODULE_NAME := 8192eu

endif

# Actions-Micro use this flag for DHC 1195 and DHC 1395
ifeq ($(CONFIG_PLATFORM_RTK119X_AM), y)
ccflags-y += -DCONFIG_PLATFORM_RTK119X_AM
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_FULL_CH_IN_P2P_HANDSHAKE
ccflags-y += -DCONFIG_SEL_P2P_IFACE=2
ccflags-y += -DCONFIG_IFACE_NUMBER=3
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT

ifeq ($(CONFIG_USB_HCI), y)
ccflags-y += -DCONFIG_USE_USB_BUFFER_ALLOC_TX
endif

ARCH := arm

#CROSS_COMPILE := arm-linux-gnueabihf-
KVER  := 3.10.24
#KSRC :=
CROSS_COMPILE :=
endif

ifeq ($(CONFIG_PLATFORM_RTK129X), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ccflags-y += -DCONFIG_PLATFORM_RTK129X
ccflags-y += -DCONFIG_TRAFFIC_PROTECT
# default setting for Android 4.1, 4.2
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
#ccflags-y += -DCONFIG_P2P_IPS -DCONFIG_QOS_OPTIMIZATION
ccflags-y += -DCONFIG_QOS_OPTIMIZATION
# Enable this for Android 5.0
ccflags-y += -DCONFIG_RADIO_WORK
ifeq ($(CONFIG_RTL8821C)$(CONFIG_SDIO_HCI),yy)
ccflags-y += -DCONFIG_WAKEUP_GPIO_INPUT_MODE
ccflags-y += -DCONFIG_BT_WAKE_HST_OPEN_DRAIN
endif
ccflags-y += -Wno-error=date-time
# default setting for Android 7.0
ifeq ($(RTK_ANDROID_VERSION), nougat)
ccflags-y += -DRTW_P2P_GROUP_INTERFACE=1
endif
#ccflags-y += -DCONFIG_#PLATFORM_OPS
ifeq ($(CONFIG_USB_HCI), y)
ccflags-y += -DCONFIG_USE_USB_BUFFER_ALLOC_TX
endif

ARCH := arm64

# ==== Cross compile setting for Android 4.4 SDK =====
#CROSS_COMPILE := arm-linux-gnueabihf-
#KVER  := 4.1.10
#CROSS_COMPILE := $(CROSS)
#KSRC := $(LINUX_KERNEL_PATH)
CROSS_COMPILE := /home/android_sdk/DHC/trunk-6.0.0_r1-QA160627/phoenix/toolchain/asdk64-4.9.4-a53-EL-3.10-g2.19-a64nt-160307/bin/asdk64-linux-
KSRC := /home/android_sdk/DHC/trunk-6.0.0_r1-QA160627/linux-kernel
endif

ifeq ($(CONFIG_PLATFORM_RTK1319), y)
ccflags-y += -DCONFIG_PLATFORM_RTK1319
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ccflags-y += -DCONFIG_TRAFFIC_PROTECT
# default setting for Android 4.1, 4.2
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
#ccflags-y += -DCONFIG_P2P_IPS -DCONFIG_QOS_OPTIMIZATION
ccflags-y += -DCONFIG_QOS_OPTIMIZATION
# Enable this for Android 5.0
ccflags-y += -DCONFIG_RADIO_WORK
ifeq ($(CONFIG_RTL8821C)$(CONFIG_SDIO_HCI),yy)
ccflags-y += -DCONFIG_WAKEUP_GPIO_INPUT_MODE
ccflags-y += -DCONFIG_BT_WAKE_HST_OPEN_DRAIN
endif
ccflags-y += -Wno-error=date-time
# default setting for Android 7.0
ifeq ($(RTK_ANDROID_VERSION), nougat)
ccflags-y += -DRTW_P2P_GROUP_INTERFACE=1
endif
#ccflags-y += -DCONFIG_#PLATFORM_OPS
ifeq ($(CONFIG_USB_HCI), y)
ccflags-y += -DCONFIG_USE_USB_BUFFER_ALLOC_TX
endif

ARCH := arm64

# ==== Cross compile setting for Android 4.4 SDK =====
#CROSS_COMPILE := arm-linux-gnueabihf-
#KVER  := 4.1.10
#CROSS_COMPILE := $(CROSS)
#KSRC := $(LINUX_KERNEL_PATH)
CROSS_COMPILE := /home/android_sdk/DHC/trunk-6.0.0_r1-QA160627/phoenix/toolchain/asdk64-4.9.4-a53-EL-3.10-g2.19-a64nt-160307/bin/asdk64-linux-
KSRC := /home/android_sdk/DHC/trunk-6.0.0_r1-QA160627/linux-kernel
endif

ifeq ($(CONFIG_PLATFORM_RTK390X), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ccflags-y += -DCONFIG_PLATFORM_RTK390X
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
ccflags-y += -DCONFIG_RTW_NETIF_SG
ifeq ($(CONFIG_USB_HCI), y)
ccflags-y += -DCONFIG_USE_USB_BUFFER_ALLOC_TX
endif

ARCH:=rlx

CROSS_COMPILE:=mips-linux-
KSRC:= /home/realtek/share/Develop/IPCAM_SDK/RealSil/rts3901_sdk_v1.2_vanilla/linux-3.10

endif

ifeq ($(CONFIG_PLATFORM_NOVATEK_NT72668), y)
ccflags-y += -DCONFIG_PLATFORM_NOVATEK_NT72668
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
ccflags-y += -DCONFIG_USE_USB_BUFFER_ALLOC_RX
ccflags-y += -DCONFIG_USE_USB_BUFFER_ALLOC_TX
ARCH ?= arm
CROSS_COMPILE := arm-linux-gnueabihf-
KVER := 3.8.0
KSRC := /Custom/Novatek/TCL/linux-3.8_header
#KSRC := $(KERNELDIR)
endif

ifeq ($(CONFIG_PLATFORM_ARM_TCC8930_JB42), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
# default setting for Android 4.1, 4.2
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
ARCH := arm
CROSS_COMPILE := /home/android_sdk/Telechips/v13.05_r1-tcc-android-4.2.2_tcc893x-evm_build/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-
KSRC := /home/android_sdk/Telechips/v13.05_r1-tcc-android-4.2.2_tcc893x-evm_build/kernel
MODULE_NAME := wlan
endif 

ifeq ($(CONFIG_PLATFORM_RTL8197D), y)
ccflags-y += -DCONFIG_BIG_ENDIAN -DCONFIG_PLATFORM_RTL8197D
export DIR_LINUX=$(shell pwd)/../SDK/rlxlinux-sdk321-v50/linux-2.6.30
ARCH ?= rlx
CROSS_COMPILE:= $(DIR_LINUX)/../toolchain/rsdk-1.5.5-5281-EB-2.6.30-0.9.30.3-110714/bin/rsdk-linux-
KSRC := $(DIR_LINUX)
endif

ifeq ($(CONFIG_PLATFORM_AML_S905), y)
ccflags-y += -DCONFIG_PLATFORM_AML_S905
ccflags-y += -DCONFIG_LITTLE_ENDIAN -fno-pic
# default setting for Android
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_IOCTL_CFG80211
ccflags-y += -DRTW_USE_CFG80211_STA_EVENT
# default setting for Android 5.x and later
ccflags-y += -DCONFIG_RADIO_WORK

ifeq ($(CONFIG_SDIO_HCI), y)
ccflags-y += -DCONFIG_PLATFORM_OPS
_PLATFORM_FILES += platform/platform_aml_s905_sdio.o
endif

ARCH ?= arm64
CROSS_COMPILE ?= /4.4_S905L_8822bs_compile/gcc-linaro-aarch64-linux-gnu-4.9-2014.09_linux/bin/aarch64-linux-gnu-
ifndef KSRC
KSRC := /4.4_S905L_8822bs_compile/common
# To locate output files in a separate directory.
KSRC += O=/4.4_S905L_8822bs_compile/KERNEL_OBJ
endif

ifeq ($(CONFIG_RTL8822B), y)
ifeq ($(CONFIG_SDIO_HCI), y)
CONFIG_RTL8822BS ?= m
USER_MODULE_NAME := 8822bs
endif
endif

endif

ifeq ($(CONFIG_PLATFORM_ZTE_ZX296716), y)
ccflags-y += -Wno-error=date-time
ccflags-y += -DCONFIG_PLATFORM_ZTE_ZX296716
ccflags-y += -DCONFIG_LITTLE_ENDIAN
# default setting for Android
ccflags-y += -DCONFIG_CONCURRENT_MODE
ccflags-y += -DCONFIG_IOCTL_CFG80211
ccflags-y += -DRTW_USE_CFG80211_STA_EVENT
# default setting for Android 5.x and later
#ccflags-y += -DCONFIG_RADIO_WORK

ifeq ($(CONFIG_SDIO_HCI), y)
# mark this temporarily
#ccflags-y += -DCONFIG_PLATFORM_OPS
#_PLATFORM_FILES += platform/platform_zte_zx296716_sdio.o
endif

ARCH ?= arm64
CROSS_COMPILE ?=
KSRC ?=

ifeq ($(CONFIG_RTL8822B), y)
ifeq ($(CONFIG_SDIO_HCI), y)
CONFIG_RTL8822BS ?= m
USER_MODULE_NAME := 8822bs
endif
endif

endif

ifeq ($(CONFIG_PLATFORM_RISCV_LIPI4A), y)
ccflags-y += -DCONFIG_LITTLE_ENDIAN
ccflags-y += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
ccflags-y += -march=rv64ima_zicsr_zifencei
ARCH ?= riscv
CROSS_COMPILE ?=
KVER  := $(shell uname -r)
KSRC := /lib/modules/$(KVER)/build
MODDESTDIR := /lib/modules/$(KVER)/kernel/drivers/net/wireless/
INSTALL_PREFIX :=
STAGINGMODDIR := /lib/modules/$(KVER)/kernel/drivers/staging

endif

########### CUSTOMER ################################
ifeq ($(CONFIG_CUSTOMER_HUAWEI_GENERAL), y)
CONFIG_CUSTOMER_HUAWEI = y
endif

ifeq ($(CONFIG_CUSTOMER_HUAWEI), y)
ccflags-y += -DCONFIG_HUAWEI_PROC
endif

CONFIG_PLATFORM_CMAP_INTFS = n
ifeq ($(CONFIG_PLATFORM_CMAP_INTFS), y)
PLATFORM_CMAP_INTFS_TYPE = 00
ccflags-y += -DCONFIG_PLATFORM_CMAP_INTFS -DCMAP_UNASSOC_METRICS_STA_MAX=32
_OS_INTFS_FILES += os_dep/linux/custom_multiap_intfs/custom_multiap_intfs.o
_PLATFORM_FILES += platform/custom_multiap_intfs_$(PLATFORM_CMAP_INTFS_TYPE).o
endif

ifeq ($(CONFIG_MULTIDRV), y)

ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME := rtw_sdio
endif

ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME := rtw_usb
endif

ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME := rtw_pci
endif


endif

USER_MODULE_NAME ?=
ifneq ($(USER_MODULE_NAME),)
MODULE_NAME := $(USER_MODULE_NAME)
endif

ifneq ($(KERNELRELEASE),)

########### this part for *.mk ############################
include $(src)/hal/phydm/phydm.mk

########### HAL_RTL8822B #################################
ifeq ($(CONFIG_RTL8822B), y)
include $(src)/rtl8822b.mk
endif

########### HAL_RTL8821C #################################
ifeq ($(CONFIG_RTL8821C), y)
include $(src)/rtl8821c.mk
endif

########### HAL_RTL8822C #################################
ifeq ($(CONFIG_RTL8822C), y)
include $(src)/rtl8822c.mk
endif

########### HAL_RTL8814B #################################
ifeq ($(CONFIG_RTL8814B), y)
include $(src)/rtl8814b.mk
endif

########### HAL_RTL8723F #################################
ifeq ($(CONFIG_RTL8723F), y)
include $(src)/rtl8723f.mk
endif

rtk_core :=	core/rtw_cmd.o \
		core/rtw_cmd_queue.o \
		core/rtw_cmd_priv.o \
		core/rtw_debug.o \
		core/rtw_io.o \
		core/rtw_io_rest.o \
		core/rtw_ioctl_query.o \
		core/rtw_ioctl_set.o \
		core/rtw_ieee80211.o \
		core/rtw_ieee80211_rest.o \
		core/rtw_mlme.o \
		core/rtw_mlme_rest.o \
		core/rtw_mlme_ext.o \
		core/rtw_mlme_ext_rest.o \
		core/rtw_mi.o \
		core/rtw_wlan_util.o \
		core/rtw_vht.o \
		core/rtw_vht_build.o \
		core/rtw_vht_rest.o \
		core/rtw_pwrctrl.o \
		core/rtw_rf.o \
		core/rtw_rf_op_class_pref.o \
		core/rtw_rf_op_class_dump.o \
		core/rtw_rf_dump_txpwr_lmt.o \
		core/rtw_rf_kfree_tx_gain.o \
		core/rtw_rf_rest.o \
		core/rtw_chplan_rest.o \
		core/monitor/rtw_radiotap.o \
		core/rtw_recv.o \
		core/rtw_recv_rest.o \
		core/rtw_recv_llc_rest.o \
		core/rtw_recv_pn_rest.o \
		core/rtw_recv_sta_rest.o \
		core/rtw_recv_sta_rust_acc.o \
		core/rtw_sta_mgt.o \
		core/rtw_sta_mgt_rest.o \
		core/rtw_sta_mgt_stctl.o \
		core/rtw_sta_mgt_lookup.o \
		core/rtw_sta_mgt_alloc.o \
		core/rtw_sta_mgt_free.o \
		core/rtw_ap.o \
		core/rtw_ap_rest.o \
		core/rtw_ap_sta_ie.o \
		core/rtw_ap_sta_ie_rates.o \
		core/rtw_ap_sta_ie_wmm_ht.o \
		core/rtw_ap_sta_ie_vht_multiap.o \
		core/rtw_ap_sta_ie_sec.o \
		core/rtw_ap_bcn_ie.o \
		core/rtw_ap_bcn_ie_rust_acc.o \
		core/rtw_ap_bcn_update.o \
		core/rtw_ap_bcn_update_rust_acc.o \
		core/rtw_ap_bcn_dispatch_rust_acc.o \
		core/rtw_ap_bmc_update.o \
		core/rtw_ap_bmc_update_rust_acc.o \
		core/rtw_ap_sta_alive.o \
		core/rtw_ap_sta_alive_rust_acc.o \
		core/rtw_ap_expire_auth.o \
		core/rtw_ap_expire_auth_rust_acc.o \
		core/rtw_ap_expire_preflight.o \
		core/rtw_ap_aka_chk.o \
		core/rtw_ap_aka_chk_rust_acc.o \
		core/rtw_ap_rf18_restore.o \
		core/rtw_ap_rf18_restore_rust_acc.o \
		core/rtw_ap_sta_ra.o \
		core/rtw_ap_sta_ra_rust_acc.o \
		core/rtw_ap_sta_info.o \
		core/rtw_ap_sta_info_rust_acc.o \
		core/rtw_ap_sta_info_apmode.o \
		core/rtw_ap_sta_info_apmode_rust_acc.o \
		core/rtw_ap_expire_asoc.o \
		core/rtw_ap_expire_asoc_rust_acc.o \
		core/rtw_ap_expire_asoc_list.o \
		core/rtw_ap_expire_chk_alive.o \
		core/rtw_ap_expire_timeout.o \
		core/rtw_ap_expire_timeout_rust_acc.o \
		core/rtw_ap_sta_ie_rust_acc.o \
		core/wds/rtw_wds.o \
		core/mesh/rtw_mesh.o \
		core/mesh/rtw_mesh_pathtbl.o \
		core/mesh/rtw_mesh_hwmp.o \
		core/rtw_xmit.o	\
		core/rtw_xmit_rest.o \
		core/rtw_xmit_qos_rest.o \
		core/rtw_xmit_sctx_rest.o \
		core/rtw_xmit_update_attrib_rest.o \
		core/rtw_xmit_update_attrib_sec_rest.o \
		core/rtw_p2p.o \
		core/rtw_rson.o \
		core/rtw_tdls.o \
		core/rtw_br_ext.o \
		core/rtw_iol.o \
		core/rtw_iol_rest.o \
		core/rtw_sreset.o \
		core/rtw_btcoex_wifionly.o \
		core/rtw_btcoex.o \
		core/rtw_beamforming.o \
		core/rtw_odm.o \
		core/rtw_rm.o \
		core/rtw_rm_fsm.o \
		core/rtw_ft.o \
		core/rtw_wnm.o \
		core/rtw_mbo.o \
		core/rtw_rm_util.o \
		core/rtw_rm_util_rest.o \
		core/efuse/rtw_efuse.o \
		core/rtw_roch.o

ifeq ($(CONFIG_SDIO_HCI), y)
rtk_core += core/rtw_sdio.o
endif

ifeq ($(CONFIG_RUST),)
rtk_core += core/rtw_security.o
else
rtk_core += core/rtw_security_rest.o
endif

ccflags-y += -I$(src)/core/crypto
ifeq ($(CONFIG_RUST),)
rtk_core += \
		core/crypto/aes-internal.o \
		core/crypto/aes-internal-enc.o
endif
ifeq ($(CONFIG_RUST),)
rtk_core += \
		core/rtw_swcrypto.o
endif
rtk_core += \
		core/rtw_swcrypto_rest.o

ifeq ($(CONFIG_RUST),)
rtk_core += \
		core/rtw_chplan.o
endif

ifeq ($(CONFIG_RUST),)
rtk_core += core/crypto/ccmp.o
endif

# W2-07/W2-08: full aes-gcm unit in rust/aes_gcm.rs when CONFIG_RUST.
ifeq ($(CONFIG_RUST),)
rtk_core += core/crypto/aes-gcm.o
endif

$(MODULE_NAME)-y += $(rtk_core)

$(MODULE_NAME)-$(CONFIG_WAPI_SUPPORT) += core/rtw_wapi.o	\
					core/rtw_wapi_sms4.o

$(MODULE_NAME)-y += $(_OS_INTFS_FILES)
$(MODULE_NAME)-y += $(_HAL_INTFS_FILES)
$(MODULE_NAME)-y += $(_PHYDM_FILES)
$(MODULE_NAME)-y += $(_BTC_FILES)
$(MODULE_NAME)-y += $(_PLATFORM_FILES)

$(MODULE_NAME)-$(CONFIG_MP_INCLUDED) += core/rtw_mp.o

ifeq ($(CONFIG_RTL8723B), y)
$(MODULE_NAME)-$(CONFIG_MP_INCLUDED)+= core/rtw_bt_mp.o
endif

# Rust-for-Linux: link .rs objects only when the target kernel has CONFIG_RUST=y.
# C-only builds (distro headers without Rust) omit migrated crypto TUs — no
# ifndef CONFIG_RUST fallback; use a Rust-enabled KDIR for migration work.
ifdef CONFIG_RUST
$(MODULE_NAME)-y += rust/kbuild_stub.o
$(MODULE_NAME)-y += rust/scaffold.o
$(MODULE_NAME)-y += rust/ffi.o
$(MODULE_NAME)-y += rust/domain_types.o
$(MODULE_NAME)-y += rust/aes_ctr.o
$(MODULE_NAME)-y += rust/aes_omac1.o
$(MODULE_NAME)-y += rust/gcmp.o
$(MODULE_NAME)-y += rust/aes_siv.o
$(MODULE_NAME)-y += rust/aes_ccm.o
$(MODULE_NAME)-y += rust/aes_gcm.o
$(MODULE_NAME)-y += rust/ccmp.o
$(MODULE_NAME)-y += rust/aes_internal.o
$(MODULE_NAME)-y += rust/aes_internal_enc.o
$(MODULE_NAME)-y += rust/sha256_internal.o
$(MODULE_NAME)-y += rust/sha256.o
$(MODULE_NAME)-y += rust/sha256_prf.o
# rtw_registrypriv_amsdu_mode uses AMSDU_MODE_OFFSET in rust/rtw_crypto_wrap.rs —
# re-run L1 after any include/drv_types.h _adapter layout change.
$(MODULE_NAME)-y += rust/rtw_crypto_wrap.o
# rtw_regsty_is_excl_chs uses EXCL_CHS_OFFSET in rust/rtw_chplan.rs — re-run L1
# after any registry_priv layout change.
# RUSTFLAGS_<stem>.o is not applied to out-of-tree rustc; rustflags-y is.
ifeq ($(CONFIG_TXPWR_LIMIT), y)
rustflags-y += --cfg txpwr_limit
ccflags-y += -DCONFIG_RUST_TXPWR_LMT
rustflags-y += --cfg rust_txpwr_lmt
endif
ifneq ($(_autoconf_has_multi_ap),)
ccflags-y += -DCONFIG_RUST_MLME_UNASSOC
rustflags-y += --cfg rust_mlme_unassoc --cfg config_rtw_multi_ap
endif
ifneq ($(filter -DCONFIG_RTW_MULTI_AP,$(USER_EXTRA_CFLAGS)),)
ccflags-y += -DCONFIG_RUST_MLME_UNASSOC
rustflags-y += --cfg rust_mlme_unassoc --cfg config_rtw_multi_ap
endif
ifeq ($(CONFIG_LAYER2_ROAMING), y)
ccflags-y += -DCONFIG_RUST_MLME_ROAMING
rustflags-y += --cfg rust_mlme_roaming --cfg config_layer2_roaming
endif
ccflags-y += -DCONFIG_RUST_MLME_WMM_RSN
rustflags-y += --cfg rust_mlme_wmm_rsn --cfg config_wmmps_sta
ccflags-y += -DCONFIG_RUST_MLME_EXT_REST
ccflags-y += -DCONFIG_RUST_MLME_EXT_MGNT_ATTRIB
ccflags-y += -DCONFIG_RUST_MLME_EXT_PEER_ALIVE
ccflags-y += -DCONFIG_RUST_MLME_EXT_SCAN
ccflags-y += -DCONFIG_RUST_MLME_EXT_PICK_CH
ccflags-y += -DCONFIG_RUST_MLME_EXT_BAND_IE
ccflags-y += -DCONFIG_RUST_MLME_HT_RESTRUCTURE
ccflags-y += -DCONFIG_80211D
ccflags-y += -DCONFIG_RUST_MLME_80211D
ccflags-y += -DCONFIG_RUST_STA_MGT_STCTL
ccflags-y += -DCONFIG_RUST_STA_MGT_LOOKUP
ccflags-y += -DCONFIG_RUST_STA_MGT_ALLOC
ccflags-y += -DCONFIG_RUST_STA_MGT_FREE
ccflags-y += -DCONFIG_RUST_STA_MGT_FREE_INIT
ccflags-y += -DCONFIG_RUST_STA_MGT_FREE_STAINFO
ccflags-y += -DCONFIG_RUST_STA_MGT_FREE_DEINIT
ccflags-y += -DCONFIG_RUST_STA_MGT_FREE_BCMc
ccflags-y += -DCONFIG_RUST_AP_STA_IE
ccflags-y += -DCONFIG_RUST_AP_STA_IE_SEC
ccflags-y += -DCONFIG_RUST_AP_STA_ALIVE
ccflags-y += -DCONFIG_RUST_AP_STA_RA
ccflags-y += -DCONFIG_RUST_AP_STA_INFO
ccflags-y += -DCONFIG_RUST_AP_STA_INFO_APMODE
ccflags-y += -DCONFIG_RUST_AP_EXPIRE_ASOC
ccflags-y += -DCONFIG_RUST_AP_EXPIRE_AUTH
ccflags-y += -DCONFIG_RUST_AP_AKA_CHK
ccflags-y += -DCONFIG_RUST_AP_RF18_RESTORE
ccflags-y += -DCONFIG_RUST_AP_EXPIRE_TIMEOUT
ccflags-y += -DCONFIG_RUST_AP_REST
ccflags-y += -DCONFIG_RUST_AP_BCN_IE
ccflags-y += -DCONFIG_RUST_AP_BMC_UPDATE
ccflags-y += -DCONFIG_RUST_AP_BCN_UPDATE
ccflags-y += -DCONFIG_RUST_AP_BCN_DISPATCH
ccflags-y += -DCONFIG_RUST_RF_OP_CLASS_PREF
ccflags-y += -DCONFIG_RUST_RF_OP_CLASS_DUMP
ccflags-y += -DCONFIG_RUST_RF_DUMP_TXPWR_LMT
ccflags-y += -DCONFIG_RUST_RF_KFREE_TX_GAIN
ccflags-y += -DCONFIG_RUST_RF_KFREE_TX_GAIN_SET
ccflags-y += -DCONFIG_RUST_CMD_PRIV
ccflags-y += -DCONFIG_RUST_CMD_PRIV_EVT
ccflags-y += -DCONFIG_RUST_CMD_QUEUE
ifneq ($(shell grep -Eq '^\s*#\s*define\s+CONFIG_EVENT_THREAD_MODE' $(src)/include/autoconf.h 2>/dev/null && echo y),)
rustflags-y += --cfg event_thread_mode
endif
ifneq ($(shell grep -Eq '^\s*#\s*define\s+CONFIG_FW_C2H_REG' $(src)/include/autoconf.h 2>/dev/null && echo y),)
rustflags-y += --cfg c2h_wk
endif
rustflags-y += --cfg rust_mlme_ext_rest
rustflags-y += --cfg rust_mlme_ext_mgnt_attrib
rustflags-y += --cfg rust_mlme_ext_peer_alive
rustflags-y += --cfg rust_mlme_ext_scan --cfg config_scan_sparse_miracast
rustflags-y += --cfg rust_mlme_ext_pick_ch
rustflags-y += --cfg rust_mlme_ext_band_ie
ifneq ($(shell grep -Eq '^\s*#\s*define\s+CONFIG_RTW_MESH' $(src)/include/autoconf.h 2>/dev/null && echo y),)
rustflags-y += --cfg config_rtw_mesh
endif
ifneq ($(shell grep -Eq '^\s*#\s*define\s+CONFIG_RTW_TOKEN_BASED_XMIT' $(src)/include/autoconf.h 2>/dev/null && echo y),)
rustflags-y += --cfg config_rtw_token_based_xmit
endif
ifneq ($(shell grep -Eq '^\s*#\s*define\s+CONFIG_RTW_ACS' $(src)/include/autoconf.h 2>/dev/null && echo y),)
rustflags-y += --cfg config_rtw_acs
endif
ifneq ($(shell grep -Eq '^\s*#\s*define\s+CONFIG_RTW_ACS_DBG' $(src)/include/autoconf.h 2>/dev/null && echo y),)
rustflags-y += --cfg config_rtw_acs_dbg
endif
rustflags-y += --cfg config_rtw_mgmt_queue
rustflags-y += --cfg config_p2p_ps_noa_use_macid_sleep
ifneq ($(filter -DCONFIG_CONCURRENT_MODE,$(ccflags-y)),)
rustflags-y += --cfg config_concurrent_mode
endif
rustflags-y += --cfg rust_mlme_ht_restructure
rustflags-y += --cfg rust_mlme_80211d --cfg config_80211d
ifneq ($(shell grep -Eq '^\s*#\s*define\s+CONFIG_BEAMFORMING' $(src)/include/autoconf.h 2>/dev/null && echo y),)
rustflags-y += --cfg config_beamforming
endif
ifneq ($(shell grep -Eq '^\s*#\s*define\s+CONFIG_80211AC_VHT' $(src)/include/autoconf.h 2>/dev/null && echo y),)
rustflags-y += --cfg config_80211ac_vht
endif
rustflags-y += --cfg rust_sta_mgt_stctl
rustflags-y += --cfg rust_sta_mgt_lookup
rustflags-y += --cfg rust_sta_mgt_alloc
rustflags-y += --cfg rust_sta_mgt_free
rustflags-y += --cfg rust_ap_sta_ie
rustflags-y += --cfg rust_ap_sta_ie_sec
rustflags-y += --cfg rust_ap_sta_alive
rustflags-y += --cfg rust_ap_expire_asoc
# Match core/rtw_ap_expire_asoc.c: clear under_exist_checking when
# !CONFIG_ACTIVE_KEEP_ALIVE_CHECK && CONFIG_80211N_HT
ifeq ($(shell grep -Eq '^\s*#\s*define\s+CONFIG_ACTIVE_KEEP_ALIVE_CHECK' $(src)/include/autoconf.h 2>/dev/null && echo y),)
ifneq ($(shell grep -Eq '^\s*#\s*define\s+CONFIG_80211N_HT' $(src)/include/autoconf.h 2>/dev/null && echo y),)
rustflags-y += --cfg expire_asoc_clear_under_exist_checking
endif
endif
rustflags-y += --cfg rust_ap_expire_auth
ifneq ($(shell grep -Eq '^\s*#\s*define\s+CONFIG_ATMEL_RC_PATCH' $(src)/include/autoconf.h 2>/dev/null && echo y),)
rustflags-y += --cfg config_atmel_rc_patch
endif
rustflags-y += --cfg rust_ap_aka_chk
rustflags-y += --cfg rust_ap_rf18_restore
rustflags-y += --cfg rust_ap_expire_timeout
rustflags-y += --cfg rust_ap_rest
rustflags-y += --cfg rust_ap_bcn_ie
rustflags-y += --cfg rust_ap_bmc_update
rustflags-y += --cfg rust_ap_bcn_update
# W3-81 PR4: match C #if CONFIG_INTERRUPT_BASED_TXBCN || CONFIG_PCI_HCI (WPS fwstate).
ifneq ($(filter -DCONFIG_INTERRUPT_BASED_TXBCN,$(ccflags-y) $(USER_EXTRA_CFLAGS) $(EXTRA_CFLAGS)),)
rustflags-y += --cfg config_interrupt_based_txbcn
else ifneq ($(shell grep -Eq '^\s*#\s*define\s+CONFIG_INTERRUPT_BASED_TXBCN(\s|$$|/\*)' $(src)/include/autoconf.h 2>/dev/null && echo y),)
rustflags-y += --cfg config_interrupt_based_txbcn
endif
ifeq ($(CONFIG_PCI_HCI), y)
rustflags-y += --cfg config_pci_hci
endif
ifeq ($(CONFIG_USB_HCI), y)
rustflags-y += --cfg config_usb_hci
endif
ifneq ($(filter -DCONFIG_PCI_HCI,$(ccflags-y) $(USER_EXTRA_CFLAGS) $(EXTRA_CFLAGS)),)
rustflags-y += --cfg config_pci_hci
endif
ifneq ($(shell grep -Eq '^\s*#\s*define\s+CONFIG_PCI_HCI(\s|$$|/\*)' $(src)/include/autoconf.h 2>/dev/null && echo y),)
rustflags-y += --cfg config_pci_hci
endif
ifneq ($(filter -DCONFIG_BMC_TX_LOW_RATE,$(ccflags-y) $(USER_EXTRA_CFLAGS) $(EXTRA_CFLAGS)),)
rustflags-y += --cfg bmc_tx_low_rate
endif
rustflags-y += --cfg rust_rf_op_class_pref
rustflags-y += --cfg rust_rf_op_class_dump
rustflags-y += --cfg rust_rf_dump_txpwr_lmt
rustflags-y += --cfg rust_rf_kfree_tx_gain
rustflags-y += --cfg rust_cmd_priv
rustflags-y += --cfg rust_cmd_queue
rustflags-y += --cfg config_rtw_debug
rustflags-y += --cfg dfs_master
rustflags-y += --cfg ieee80211_band_5ghz
# CONFIG_DFS defaults to 1 in include/drv_conf.h (#define), not a Makefile y var.
rustflags-y += --cfg dfs
ifneq ($(filter -DCONFIG_REGD_SRC_FROM_OS,$(ccflags-y) $(USER_EXTRA_CFLAGS)),)
rustflags-y += --cfg regd_src_from_os
endif
# Pair C -DRTW_CONFIG_RFREG18_WA with Rust expire_timeout orchestrator (W3-82 PR20).
ifneq ($(filter -DRTW_CONFIG_RFREG18_WA,$(ccflags-y) $(USER_EXTRA_CFLAGS)),)
rustflags-y += --cfg rtw_config_rfreg18_wa
endif
ifneq ($(filter -DCONFIG_RF_POWER_TRIM,$(ccflags-y) $(USER_EXTRA_CFLAGS)),)
rustflags-y += --cfg rf_power_trim
endif
ifneq ($(filter -DCONFIG_PLATFORM_INTEL_BYT,$(ccflags-y) $(USER_EXTRA_CFLAGS)),)
rustflags-y += --cfg CONFIG_PLATFORM_INTEL_BYT
endif
ifneq ($(filter -DDBG_IO,$(ccflags-y) $(USER_EXTRA_CFLAGS)),)
rustflags-y += --cfg dbg_io
endif
ifneq ($(filter -DROKU_PRIVATE,$(ccflags-y) $(USER_EXTRA_CFLAGS)),)
rustflags-y += --cfg roku_private
endif
ifeq ($(CONFIG_RTW_80211K), y)
rustflags-y += --cfg rtw_80211k
endif
# Match C #ifdef CONFIG_FW_HANDLE_TXBCN + CONFIG_SUPPORT_MULTI_BCN (drv_conf.h:
# IFACE_NUMBER > 2 under CONFIG_AP_MODE, plus CONFIG_HWMPCAP_GEN2 for 8822B).
_config_iface_gt2 := $(filter -DCONFIG_IFACE_NUMBER=3 -DCONFIG_IFACE_NUMBER=4 -DCONFIG_IFACE_NUMBER=5 -DCONFIG_IFACE_NUMBER=6 -DCONFIG_IFACE_NUMBER=7 -DCONFIG_IFACE_NUMBER=8,$(ccflags-y) $(USER_EXTRA_CFLAGS))
ifneq ($(_config_iface_gt2),)
CONFIG_SUPPORT_MULTI_BCN := y
endif
ifneq ($(filter -DCONFIG_FW_HANDLE_TXBCN,$(ccflags-y) $(USER_EXTRA_CFLAGS)),)
CONFIG_FW_HANDLE_TXBCN := y
endif
ifeq ($(CONFIG_RTL8822B), y)
ifeq ($(CONFIG_SUPPORT_MULTI_BCN), y)
CONFIG_FW_HANDLE_TXBCN := y
endif
endif
ifeq ($(CONFIG_FW_HANDLE_TXBCN), y)
rustflags-y += --cfg fw_handle_txbcn
endif
ifneq ($(filter -DCONFIG_BMC_TX_RATE_SELECT,$(ccflags-y) $(USER_EXTRA_CFLAGS) $(EXTRA_CFLAGS)),)
rustflags-y += --cfg bmc_tx_rate_select
endif
$(MODULE_NAME)-y += rust/rtw_chplan.o
$(MODULE_NAME)-y += rust/rtw_chplan_rest.o
$(MODULE_NAME)-y += rust/rtw_io_rest.o
$(MODULE_NAME)-y += rust/rtw_rf_rest.o
$(MODULE_NAME)-y += rust/rtw_swcrypto.o
$(MODULE_NAME)-y += rust/rtw_ieee80211.o
$(MODULE_NAME)-y += rust/rtw_ieee80211_rest.o
$(MODULE_NAME)-y += rust/rtw_security.o
$(MODULE_NAME)-y += rust/rtw_security_rest.o
$(MODULE_NAME)-y += rust/rtw_wlan_util.o
$(MODULE_NAME)-y += rust/rtw_rm_util.o
$(MODULE_NAME)-y += rust/rtw_vht.o
$(MODULE_NAME)-y += rust/rtw_sta_mgt.o
$(MODULE_NAME)-y += rust/rtw_sta_mgt_aid.o
$(MODULE_NAME)-y += rust/rtw_sta_mgt_stctl.o
$(MODULE_NAME)-y += rust/rtw_sta_mgt_lookup.o
$(MODULE_NAME)-y += rust/rtw_sta_mgt_alloc.o
$(MODULE_NAME)-y += rust/rtw_sta_mgt_free.o
$(MODULE_NAME)-y += rust/rtw_sta_mgt_free_init_kern.o
$(MODULE_NAME)-y += rust/rtw_sta_mgt_free_stainfo_kern.o
$(MODULE_NAME)-y += rust/rtw_sta_mgt_free_deinit_kern.o
$(MODULE_NAME)-y += rust/rtw_sta_mgt_free_bcmc_kern.o
$(MODULE_NAME)-y += rust/rtw_ap_sta_ie.o
$(MODULE_NAME)-y += rust/rtw_ap_sta_ie_sec.o
$(MODULE_NAME)-y += rust/rtw_ap_rest.o
$(MODULE_NAME)-y += rust/rtw_ap_bcn_ie.o
$(MODULE_NAME)-y += rust/rtw_ap_bmc_update_kern.o
$(MODULE_NAME)-y += rust/rtw_ap_bcn_update_kern.o
$(MODULE_NAME)-y += rust/rtw_ap_bcn_dispatch.o
$(MODULE_NAME)-y += rust/rtw_ap_sta_alive.o
$(MODULE_NAME)-y += rust/rtw_ap_sta_ra.o
$(MODULE_NAME)-y += rust/rtw_ap_sta_info.o
$(MODULE_NAME)-y += rust/rtw_ap_sta_info_apmode.o
$(MODULE_NAME)-y += rust/rtw_ap_expire_asoc.o
$(MODULE_NAME)-y += rust/rtw_ap_expire_auth.o
$(MODULE_NAME)-y += rust/rtw_ap_aka_chk.o
$(MODULE_NAME)-y += rust/rtw_ap_rf18_restore.o
$(MODULE_NAME)-y += rust/rtw_ap_expire_timeout.o
$(MODULE_NAME)-y += rust/rtw_rf_op_class_pref.o
$(MODULE_NAME)-y += rust/rtw_rf_op_class_dump.o
$(MODULE_NAME)-y += rust/rtw_rf_dump_txpwr_lmt.o
$(MODULE_NAME)-y += rust/rtw_rf_kfree_tx_gain.o
$(MODULE_NAME)-y += rust/rtw_recv.o
$(MODULE_NAME)-y += rust/rtw_xmit.o
$(MODULE_NAME)-y += rust/rtw_xmit_update_attrib_kern.o
$(MODULE_NAME)-y += rust/rtw_iol_rest.o
$(MODULE_NAME)-y += rust/rtw_sreset.o
$(MODULE_NAME)-y += rust/rtw_pwrctrl.o
$(MODULE_NAME)-y += rust/rtw_mlme_rest.o
$(MODULE_NAME)-y += rust/rtw_mlme_ht_restructure.o
$(MODULE_NAME)-y += rust/rtw_mlme_80211d.o
$(MODULE_NAME)-y += rust/rtw_mlme_ext_rest.o
$(MODULE_NAME)-y += rust/rtw_mlme_ext_mgnt_attrib.o
$(MODULE_NAME)-y += rust/rtw_mlme_ext_peer_alive.o
$(MODULE_NAME)-y += rust/rtw_mlme_ext_scan.o
$(MODULE_NAME)-y += rust/rtw_mlme_ext_pick_ch.o
$(MODULE_NAME)-y += rust/rtw_mlme_ext_band_ie.o
$(MODULE_NAME)-y += rust/rtw_cmd_rest.o
endif

obj-$(CONFIG_RTL8822BU) := $(MODULE_NAME).o

else

export CONFIG_RTL8822BU = m

# RfL out-of-tree contract: KDIR overrides platform KSRC when set
# (make KDIR=/path/to/rust-enabled-kernel LLVM=1).
ifneq ($(KDIR),)
KSRC := $(KDIR)
endif

KBUILD_OPTS := ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)
ifneq ($(LLVM),)
KBUILD_OPTS += LLVM=$(LLVM)
endif

all: modules

migration-progress:
	@./scripts/ci/compute-migration-progress.sh

# L1 ABI gate (T1): compare global symbols in OLD.o vs NEW.o after a C→Rust swap.
# Example (W1-03 aes-ctr pilot):
#   make rust-check-symbols OLD=/tmp/aes-ctr-c.o NEW=rust/aes_ctr.o
RUST_CHECK_NM ?= $(if $(filter 1,$(LLVM)),llvm-nm,nm)

.PHONY: rust-check-symbols rust-check-symbols-selftest rust-check-symbols-aes-internal rust-check-symbols-aes-internal-part1 rust-check-symbols-aes-internal-part2 rust-check-symbols-aes-internal-part3 rust-objects-aes-ctr rust-objects-aes-omac1 rust-objects-gcmp rust-objects-aes-siv rust-objects-aes-ccm rust-objects-aes-gcm rust-objects-aes-gcm-c rust-objects-ccmp rust-objects-ccmp-c rust-objects-aes-internal rust-objects-aes-internal-c rust-objects-aes-internal-enc rust-objects-aes-internal-enc-c rust-objects-sha256-internal rust-objects-sha256 rust-objects-sha256-c rust-objects-sha256-prf rust-objects-rtw-crypto-wrap rust-objects-rtw-crypto-wrap-c rust-objects-rtw-chplan rust-objects-rtw-chplan-c rust-check-symbols-rtw-chplan rust-objects-rtw-swcrypto rust-objects-rtw-swcrypto-c rust-check-symbols-rtw-swcrypto rust-objects-rtw-ieee80211 rust-objects-rtw-ieee80211-c rust-check-symbols-rtw-ieee80211 rust-objects-rtw-security rust-objects-rtw-security-c rust-check-symbols-rtw-security rust-objects-rtw-security-rest rust-objects-rtw-security-rest-misc-c rust-check-symbols-rtw-security-rest-misc rust-objects-rtw-wlan-util rust-objects-rtw-wlan-util-c rust-check-symbols-rtw-wlan-util
rust-check-symbols:
	@test -n "$(OLD)" && test -n "$(NEW)" || { \
		echo "Usage: make rust-check-symbols OLD=path/to/old.o NEW=path/to/new.o [ALLOWLIST=path.allow] [ALLOW_VACUOUS=1]"; \
		exit 1; }
	NM=$(RUST_CHECK_NM) ./docs/rust-migration/scripts/check-symbols.sh "$(OLD)" "$(NEW)" \
		$(if $(ALLOWLIST),--allowlist "$(ALLOWLIST)",) \
		$(if $(ALLOW_VACUOUS),--allow-vacuous,)

rust-objects-aes-ctr:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-aes-ctr"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/aes_ctr.o

rust-objects-aes-omac1:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-aes-omac1"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/aes_omac1.o

rust-objects-gcmp:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-gcmp"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/gcmp.o

rust-objects-aes-siv:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-aes-siv"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/aes_siv.o

rust-objects-aes-ccm:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-aes-ccm"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/aes_ccm.o

rust-objects-aes-gcm:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-aes-gcm"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/aes_gcm.o

# L1 OLD reference for aes_gcm_ae swap (C TU still in-tree for W2-07 oracle until W2-08).
rust-objects-aes-gcm-c:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-aes-gcm-c"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) core/crypto/aes-gcm.o

rust-objects-ccmp:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-ccmp"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/ccmp.o

# L1 OLD reference for full ccmp swap (C TU still in-tree for host oracle).
rust-objects-ccmp-c:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-ccmp-c"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) core/crypto/ccmp.o

rust-objects-aes-internal:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-aes-internal"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/aes_internal.o

# L1 OLD reference for full aes-internal swap (C TU still in-tree for host oracle).
rust-objects-aes-internal-c:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-aes-internal-c"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) core/crypto/aes-internal.o

# W2-14 L1: full aes-internal.c swap (no allowlist).
rust-check-symbols-aes-internal: rust-objects-aes-internal-c rust-objects-aes-internal
	$(MAKE) rust-check-symbols OLD=core/crypto/aes-internal.o NEW=rust/aes_internal.o

# W2-11 part 1 L1 (historical): Te0 moved to rust/aes_internal.o.
rust-check-symbols-aes-internal-part1: rust-objects-aes-internal-c rust-objects-aes-internal
	$(MAKE) rust-check-symbols OLD=core/crypto/aes-internal.o NEW=rust/aes_internal.o \
		ALLOWLIST=docs/rust-migration/scripts/aes_internal_part1.allow

# W2-12 part 2 L1 (historical): Te0+Td0 in rust/aes_internal.o.
rust-check-symbols-aes-internal-part2: rust-objects-aes-internal-c rust-objects-aes-internal
	$(MAKE) rust-check-symbols OLD=core/crypto/aes-internal.o NEW=rust/aes_internal.o \
		ALLOWLIST=docs/rust-migration/scripts/aes_internal_part2.allow

# W2-13 part 3 L1 (historical): Te0+Td0+Td4s+rcons in rust/aes_internal.o.
rust-check-symbols-aes-internal-part3: rust-objects-aes-internal-c rust-objects-aes-internal
	$(MAKE) rust-check-symbols OLD=core/crypto/aes-internal.o NEW=rust/aes_internal.o \
		ALLOWLIST=docs/rust-migration/scripts/aes_internal_part3.allow

rust-objects-aes-internal-enc:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-aes-internal-enc"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/aes_internal_enc.o

rust-objects-aes-internal-enc-c:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-aes-internal-enc-c"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) core/crypto/aes-internal-enc.o

rust-objects-sha256-internal:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-sha256-internal"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/sha256_internal.o

rust-objects-sha256:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-sha256"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/sha256.o

# L1 OLD reference for sha256.c swap (C TU no longer in rtk_core).
rust-objects-sha256-c:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-sha256-c"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) core/crypto/sha256.o

rust-objects-sha256-prf:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-sha256-prf"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/sha256_prf.o

rust-objects-rtw-crypto-wrap:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-rtw-crypto-wrap"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/rtw_crypto_wrap.o

# L1 OLD reference for rtw_crypto_wrap swap (C TU no longer in rtk_core).
rust-objects-rtw-crypto-wrap-c:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-rtw-crypto-wrap-c"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) core/crypto/rtw_crypto_wrap.o

# L1 helpers for rtw_chplan lookup swap (W2-17b).
rust-objects-rtw-chplan:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-rtw-chplan"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/rtw_chplan.o

rust-objects-rtw-chplan-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_CHPLAN_TEST -o core/rtw_chplan_c_ref.o core/rtw_chplan.c

rust-check-symbols-rtw-chplan: rust-objects-rtw-chplan-c rust-objects-rtw-chplan
	@$(RUST_CHECK_NM) rust/rtw_chplan.o | grep -q ' U rtw_chdef_5g_len' || { \
		echo "rust/rtw_chplan.o missing rtw_chdef_5g_len — ieee80211_band_5ghz cfg not applied?"; \
		exit 1; }
	$(MAKE) rust-check-symbols OLD=core/rtw_chplan_c_ref.o NEW=rust/rtw_chplan.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_chplan_lookup.allow
	$(MAKE) rust-check-symbols OLD=core/rtw_chplan_c_ref.o NEW=rust/rtw_chplan.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_chplan_init.allow

# W3-17: compare pre-port core/rtw_chplan_rest.o against rust/rtw_chplan_rest.o.
rust-objects-rtw-chplan-rest:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-rtw-chplan-rest"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/rtw_chplan_rest.o

rust-objects-rtw-chplan-rest-c:
	@set -e; \
	backup=$$(mktemp); \
	trap 'mv "$$backup" core/rtw_chplan_rest.c; rm -f core/rtw_chplan_rest.o' EXIT; \
	cp core/rtw_chplan_rest.c "$$backup"; \
	git -c safe.directory=* show origin/master:core/rtw_chplan_rest.c > core/rtw_chplan_rest.c; \
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) core/rtw_chplan_rest.o; \
	cp core/rtw_chplan_rest.o tests/host/chplan/chplan_rest_c_ref.o; \
	mv "$$backup" core/rtw_chplan_rest.c; \
	rm -f core/rtw_chplan_rest.o; \
	trap - EXIT

rust-check-symbols-rtw-chplan-rest: rust-objects-rtw-chplan-rest-c rust-objects-rtw-chplan-rest
	$(MAKE) rust-check-symbols OLD=tests/host/chplan/chplan_rest_c_ref.o NEW=rust/rtw_chplan_rest.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_chplan_rest.allow ALLOW_VACUOUS=1

# W3-18: compare pre-port core/rtw_io_rest.o against rust/rtw_io_rest.o.
rust-objects-rtw-io-rest:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-rtw-io-rest"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/rtw_io_rest.o

rust-objects-rtw-io-rest-c:
	@set -e; \
	backup=$$(mktemp); \
	trap 'mv "$$backup" core/rtw_io_rest.c; rm -f core/rtw_io_rest.o' EXIT; \
	cp core/rtw_io_rest.c "$$backup"; \
	git -c safe.directory=* show origin/master:core/rtw_io_rest.c > core/rtw_io_rest.c; \
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) core/rtw_io_rest.o; \
	cp core/rtw_io_rest.o tests/host/io/io_rest_c_ref.o; \
	mv "$$backup" core/rtw_io_rest.c; \
	rm -f core/rtw_io_rest.o; \
	trap - EXIT

rust-check-symbols-rtw-io-rest: rust-objects-rtw-io-rest-c rust-objects-rtw-io-rest
	$(MAKE) rust-check-symbols OLD=tests/host/io/io_rest_c_ref.o NEW=rust/rtw_io_rest.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_io_rest.allow ALLOW_VACUOUS=1

# W3-19: compare pre-port core/rtw_rf_rest.o against rust/rtw_rf_rest.o.
rust-objects-rtw-rf-rest:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-rtw-rf-rest"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/rtw_rf_rest.o

rust-objects-rtw-rf-rest-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -Wno-format-truncation -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_RF_TEST -o tests/host/rf/rf_rest_c_ref.o core/rtw_rf_rest.c

rust-check-symbols-rtw-rf-rest: rust-objects-rtw-rf-rest-c rust-objects-rtw-rf-rest
	$(MAKE) rust-check-symbols OLD=tests/host/rf/rf_rest_c_ref.o NEW=rust/rtw_rf_rest.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_rf_rest.allow

rust-objects-rtw-swcrypto:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-rtw-swcrypto"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/rtw_swcrypto.o

rust-objects-rtw-swcrypto-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core/crypto \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_CRYPTO_TEST -DHOST_SWCRYPTO_TEST -DHOST_SWCRYPTO_WRAPPER_ONLY \
		-o core/rtw_swcrypto_c_ref.o core/rtw_swcrypto.c

rust-check-symbols-rtw-swcrypto: rust-objects-rtw-swcrypto-c rust-objects-rtw-swcrypto
	$(MAKE) rust-check-symbols OLD=core/rtw_swcrypto_c_ref.o NEW=rust/rtw_swcrypto.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_swcrypto_wrappers.allow

rust-objects-rtw-ieee80211:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-rtw-ieee80211"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/rtw_ieee80211.o

rust-objects-rtw-ieee80211-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
		-I$(shell pwd)/tests/host/include \
		-o tests/host/ie/ie_c_ref.o tests/host/ie/ie_c_oracle.c

rust-check-symbols-rtw-ieee80211: rust-objects-rtw-ieee80211-c rust-objects-rtw-ieee80211
	$(MAKE) rust-check-symbols OLD=tests/host/ie/ie_c_ref.o NEW=rust/rtw_ieee80211.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_ieee80211.allow

# W3-26: compare pre-port core/rtw_ieee80211_rest.o against rust/rtw_ieee80211_rest.o.
rust-objects-rtw-ieee80211-rest:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-rtw-ieee80211-rest"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/rtw_ieee80211_rest.o

rust-objects-rtw-ieee80211-rest-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/include \
		-I$(shell pwd)/tests/host/wlan_util \
		-DHOST_IEEE80211_REST_TEST -DHOST_IEEE80211_REST_RATE_SECTION_TEST \
		-DHOST_IEEE80211_REST_HT_MCS_AMSDU_TEST \
		-DHOST_IEEE80211_REST_P2P_IE_TEST \
		-DHOST_IEEE80211_REST_WFD_MULTIAP_TEST -DCONFIG_WFD -DCONFIG_RTW_MULTI_AP \
		-o tests/host/ie/ie_rest_c_ref.o core/rtw_ieee80211_rest.c

rust-check-symbols-rtw-ieee80211-rest: rust-objects-rtw-ieee80211-rest-c rust-objects-rtw-ieee80211-rest
	$(MAKE) rust-check-symbols OLD=tests/host/ie/ie_rest_c_ref.o NEW=rust/rtw_ieee80211_rest.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_ieee80211_rest.allow

rust-objects-rtw-security:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-rtw-security"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/rtw_security.o

rust-objects-rtw-security-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
		-I$(shell pwd)/tests/host/include \
		-o tests/host/security/security_type_c_ref.o tests/host/security/security_type_c_oracle.c

rust-check-symbols-rtw-security: rust-objects-rtw-security-c rust-objects-rtw-security
	$(MAKE) rust-check-symbols OLD=tests/host/security/security_type_c_ref.o NEW=rust/rtw_security.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_security_type_str.allow

# W3-15 misc slice: compare pre-port core/rtw_security_rest.o (frozen at adf5beb)
# against rust/rtw_security_rest.o.
SECURITY_REST_MISC_C_REF_COMMIT ?= adf5beb

rust-objects-rtw-security-rest:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-rtw-security-rest"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/rtw_security_rest.o

rust-objects-rtw-security-rest-misc-c:
	@set -e; \
	ref_commit="$(SECURITY_REST_MISC_C_REF_COMMIT)"; \
	backup=$$(mktemp); \
	trap 'mv "$$backup" core/rtw_security_rest.c; rm -f core/rtw_security_rest.o' EXIT; \
	cp core/rtw_security_rest.c "$$backup"; \
	git -c safe.directory=* show "$$ref_commit:core/rtw_security_rest.c" > core/rtw_security_rest.c; \
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) core/rtw_security_rest.o; \
	cp core/rtw_security_rest.o tests/host/security/security_rest_misc_c_ref.o; \
	mv "$$backup" core/rtw_security_rest.c; \
	rm -f core/rtw_security_rest.o; \
	trap - EXIT

rust-check-symbols-rtw-security-rest-misc: rust-objects-rtw-security-rest-misc-c rust-objects-rtw-security-rest
	$(MAKE) rust-check-symbols OLD=tests/host/security/security_rest_misc_c_ref.o NEW=rust/rtw_security_rest.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_security_rest_misc.allow

rust-objects-rtw-wlan-util:
	@test -n "$(KDIR)" || { \
		echo "Usage: make KDIR=/path/to/rust-enabled-kernel LLVM=1 rust-objects-rtw-wlan-util"; \
		exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/rtw_wlan_util.o

rust-objects-rtw-wlan-util-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
		-I$(shell pwd)/tests/host/include \
		-I$(shell pwd)/tests/host/wlan_util \
		-o tests/host/wlan_util/wlan_util_rate_c_ref.o tests/host/wlan_util/wlan_util_c_oracle.c

rust-check-symbols-rtw-wlan-util: rust-objects-rtw-wlan-util-c rust-objects-rtw-wlan-util
	$(MAKE) rust-check-symbols OLD=tests/host/wlan_util/wlan_util_rate_c_ref.o NEW=rust/rtw_wlan_util.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_wlan_util_rate.allow

# W3-33/W3-34: compare host C oracle (rtw_rm_util_rest.c) against host Rust oracle.
rust-objects-rtw-rm-util-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_RM_TEST -DCONFIG_RTW_80211K \
		-o tests/host/rm/rm_rest_c_ref.o core/rtw_rm_util_rest.c

rust-objects-rtw-rm-util-rust-ref:
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_rm_test \
		--emit=obj=tests/host/rm/rm_rest_rust_ref.o \
		--crate-type lib rust/rtw_rm_util.rs

rust-check-symbols-rtw-rm-util: rust-objects-rtw-rm-util-c rust-objects-rtw-rm-util-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/rm/rm_rest_c_ref.o NEW=tests/host/rm/rm_rest_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_rm_util.allow

# W3-35: compare host C oracle (rtw_vht_rest.c) against host Rust oracle.
rust-objects-rtw-vht-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_VHT_TEST -DROKU_PRIVATE \
		-o tests/host/vht/vht_rest_c_ref.o core/rtw_vht_rest.c

rust-objects-rtw-vht-rust-ref:
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_vht_test \
		--emit=obj=tests/host/vht/vht_rest_rust_ref.o \
		--crate-type lib rust/rtw_vht.rs

rust-check-symbols-rtw-vht: rust-objects-rtw-vht-c rust-objects-rtw-vht-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/vht/vht_rest_c_ref.o NEW=tests/host/vht/vht_rest_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_vht.allow

# W3-36 PR4: compare host C oracle for rtw_restructure_vht_ie against Rust kernel export.
rust-objects-rtw-vht-restructure-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_VHT_RESTRUCTURE_TEST \
		-o tests/host/vht/vht_restructure_c_ref.o core/rtw_vht_rest.c

rust-objects-rtw-vht-restructure-rust-ref:
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_vht_restructure_test \
		--emit=obj=tests/host/vht/vht_restructure_rust_ref.o \
		--crate-type lib rust/rtw_vht.rs

rust-check-symbols-rtw-vht-restructure: rust-objects-rtw-vht-restructure-c rust-objects-rtw-vht-restructure-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/vht/vht_restructure_c_ref.o NEW=tests/host/vht/vht_restructure_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_vht_restructure.allow

# W3-45: compare host C oracle for VHT MCS/rate helpers against Rust export.
rust-objects-rtw-vht-mcs-rate-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_VHT_MCS_RATE_TEST \
		-o tests/host/vht/vht_mcs_rate_c_ref.o tests/host/vht/vht_mcs_rate_c_oracle.c

rust-objects-rtw-vht-mcs-rate-rust-ref:
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_vht_mcs_rate_test \
		--emit=obj=tests/host/vht/vht_mcs_rate_rust_ref.o \
		--crate-type lib rust/rtw_vht.rs

rust-check-symbols-rtw-vht-mcs-rate: rust-objects-rtw-vht-mcs-rate-c rust-objects-rtw-vht-mcs-rate-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/vht/vht_mcs_rate_c_ref.o NEW=tests/host/vht/vht_mcs_rate_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_vht_mcs_rate.allow

# W3-37: compare host C oracle (rtw_sta_mgt_rest.c) against host Rust oracle.
rust-objects-rtw-sta-mgt-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_STA_MGT_TEST -DCONFIG_RTW_MACADDR_ACL -DCONFIG_RTW_PRE_LINK_STA \
		-o tests/host/sta_mgt/sta_mgt_rest_c_ref.o core/rtw_sta_mgt_rest.c

rust-objects-rtw-sta-mgt-rust-ref:
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_sta_mgt_test \
		--emit=obj=tests/host/sta_mgt/sta_mgt_rest_rust_ref.o \
		--crate-type lib rust/rtw_sta_mgt.rs

rust-objects-rtw-sta-mgt-aid-rust-ref:
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_sta_mgt_test \
		--emit=obj=tests/host/sta_mgt/sta_mgt_aid_rust_ref.o \
		--crate-type lib rust/rtw_sta_mgt_aid.rs

rust-check-symbols-rtw-sta-mgt: rust-objects-rtw-sta-mgt-c rust-objects-rtw-sta-mgt-rust-ref rust-objects-rtw-sta-mgt-aid-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/sta_mgt/sta_mgt_rest_c_ref.o NEW=tests/host/sta_mgt/sta_mgt_rest_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_sta_mgt.allow
	$(MAKE) rust-check-symbols OLD=tests/host/sta_mgt/sta_mgt_rest_c_ref.o NEW=tests/host/sta_mgt/sta_mgt_aid_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_sta_mgt_aid.allow

# W3-38 PR5/PR6: aid-only L1 (CI l1-targets invokes this separately from ACL check).
rust-check-symbols-rtw-sta-mgt-aid: rust-objects-rtw-sta-mgt-c rust-objects-rtw-sta-mgt-aid-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/sta_mgt/sta_mgt_rest_c_ref.o NEW=tests/host/sta_mgt/sta_mgt_aid_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_sta_mgt_aid.allow

# W3-55 PR3: stctl-only L1 (host C oracle vs host Rust oracle).
rust-objects-rtw-sta-mgt-stctl-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_STA_MGT_TEST -o tests/host/sta_mgt/sta_mgt_stctl_c_ref.o core/rtw_sta_mgt_stctl.c

rust-objects-rtw-sta-mgt-stctl-rust-ref:
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_sta_mgt_test \
		--emit=obj=tests/host/sta_mgt/sta_mgt_stctl_rust_ref.o \
		--crate-type lib rust/rtw_sta_mgt_stctl.rs

rust-check-symbols-rtw-sta-mgt-stctl: rust-objects-rtw-sta-mgt-stctl-c rust-objects-rtw-sta-mgt-stctl-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/sta_mgt/sta_mgt_stctl_c_ref.o NEW=tests/host/sta_mgt/sta_mgt_stctl_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_sta_mgt_stctl.allow

# W3-77 PR6: lookup-only L1 (host C oracle vs host Rust oracle).
rust-objects-rtw-sta-mgt-lookup-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_STA_MGT_TEST -o tests/host/sta_mgt/sta_mgt_lookup_c_ref.o core/rtw_sta_mgt_lookup.c

rust-objects-rtw-sta-mgt-lookup-rust-ref:
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_sta_mgt_test \
		--emit=obj=tests/host/sta_mgt/sta_mgt_lookup_rust_ref.o \
		--crate-type lib rust/rtw_sta_mgt_lookup.rs

rust-check-symbols-rtw-sta-mgt-lookup: rust-objects-rtw-sta-mgt-lookup-c rust-objects-rtw-sta-mgt-lookup-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/sta_mgt/sta_mgt_lookup_c_ref.o NEW=tests/host/sta_mgt/sta_mgt_lookup_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_sta_mgt_lookup.allow

# W3-55 PR3: ap_rest-only L1 (host C oracle vs host Rust oracle).
rust-objects-rtw-ap-rest-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_AP_REST_TEST -DCONFIG_FW_HANDLE_TXBCN -DCONFIG_LIMITED_AP_NUM=4 \
		-o tests/host/ap/ap_rest_c_ref.o core/rtw_ap_rest.c

rust-objects-rtw-ap-rest-rust-ref:
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_ap_rest_test --cfg fw_handle_txbcn \
		--emit=obj=tests/host/ap/ap_rest_rust_ref.o \
		--crate-type lib rust/rtw_ap_rest.rs

rust-check-symbols-rtw-ap-rest: rust-objects-rtw-ap-rest-c rust-objects-rtw-ap-rest-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/ap/ap_rest_c_ref.o NEW=tests/host/ap/ap_rest_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_ap_rest.allow

# W3-82 PR3: chk_sta_is_alive L1 (host C vs host Rust oracle).
rust-objects-rtw-ap-sta-alive-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_AP_STA_ALIVE_TEST \
		-o tests/host/ap/ap_sta_alive_c_ref.o core/rtw_ap_sta_alive.c

rust-objects-rtw-ap-sta-alive-rust-ref:
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_ap_sta_alive_test \
		--emit=obj=tests/host/ap/ap_sta_alive_rust_ref.o \
		--crate-type lib rust/rtw_ap_sta_alive.rs

rust-check-symbols-rtw-ap-sta-alive: rust-objects-rtw-ap-sta-alive-c rust-objects-rtw-ap-sta-alive-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/ap/ap_sta_alive_c_ref.o NEW=tests/host/ap/ap_sta_alive_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_ap_sta_alive.allow

# W3-83 PR3: rtw_ap_update_sta_ra_info L1 (host C vs host Rust oracle).
rust-objects-rtw-ap-sta-ra-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_AP_STA_RA_TEST \
		-o tests/host/ap/ap_sta_ra_c_ref.o core/rtw_ap_sta_ra.c

rust-objects-rtw-ap-sta-ra-rust-ref:
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_ap_sta_ra_test \
		--emit=obj=tests/host/ap/ap_sta_ra_rust_ref.o \
		--crate-type lib rust/rtw_ap_sta_ra.rs

rust-check-symbols-rtw-ap-sta-ra: rust-objects-rtw-ap-sta-ra-c rust-objects-rtw-ap-sta-ra-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/ap/ap_sta_ra_c_ref.o NEW=tests/host/ap/ap_sta_ra_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_ap_sta_ra.allow

# W3-81 PR3: beacon HT/WPS/ERP kernel object L1 (host C ref vs kbuild Rust object).
rust-objects-rtw-ap-bcn-update-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_AP_BCN_UPDATE_TEST \
		-o tests/host/ap/ap_bcn_update_c_ref.o core/rtw_ap_bcn_update.c

rust-objects-rtw-ap-bcn-update-kern:
	@test -n "$(KDIR)" || { echo "Usage: make KDIR=… LLVM=1 rust-objects-rtw-ap-bcn-update-kern"; exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/rtw_ap_bcn_update_kern.o

rust-check-symbols-rtw-ap-bcn-update: rust-objects-rtw-ap-bcn-update-c rust-objects-rtw-ap-bcn-update-kern
	$(MAKE) rust-check-symbols OLD=tests/host/ap/ap_bcn_update_c_ref.o NEW=rust/rtw_ap_bcn_update_kern.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_ap_bcn_update.allow

# W3-75 follow-up (#830): beacon IE add/remove L1 (host C ref vs kbuild Rust object).
rust-objects-rtw-ap-bcn-ie-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-sign-compare -Wno-pointer-sign -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_AP_BCN_IE_TEST \
		-o tests/host/ap/ap_bcn_ie_c_ref.o core/rtw_ap_bcn_ie.c

rust-objects-rtw-ap-bcn-ie:
	@test -n "$(KDIR)" || { echo "Usage: make KDIR=… LLVM=1 rust-objects-rtw-ap-bcn-ie"; exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/rtw_ap_bcn_ie.o

rust-check-symbols-rtw-ap-bcn-ie: rust-objects-rtw-ap-bcn-ie-c rust-objects-rtw-ap-bcn-ie
	$(MAKE) rust-check-symbols OLD=tests/host/ap/ap_bcn_ie_c_ref.o NEW=rust/rtw_ap_bcn_ie.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_ap_bcn_ie.allow

rust-objects-rtw-ap-sta-info-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_AP_STA_INFO_TEST -DCONFIG_BEAMFORMING \
		-o tests/host/ap/ap_sta_info_c_ref.o core/rtw_ap_sta_info.c

rust-objects-rtw-ap-sta-info-rust-ref:
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_ap_sta_info_test \
		--emit=obj=tests/host/ap/ap_sta_info_rust_ref.o \
		--crate-type lib rust/rtw_ap_sta_info.rs

rust-check-symbols-rtw-ap-sta-info: rust-objects-rtw-ap-sta-info-c rust-objects-rtw-ap-sta-info-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/ap/ap_sta_info_c_ref.o NEW=tests/host/ap/ap_sta_info_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_ap_sta_info.allow

# W3-83 PR15: update_sta_info_apmode L1 (host C vs host Rust oracle).
rust-objects-rtw-ap-sta-info-apmode-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_AP_STA_INFO_APMODE_TEST \
		-o tests/host/ap/ap_sta_info_apmode_c_ref.o core/rtw_ap_sta_info_apmode.c

rust-objects-rtw-ap-sta-info-apmode-rust-ref:
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_ap_sta_info_apmode_test \
		--emit=obj=tests/host/ap/ap_sta_info_apmode_rust_ref.o \
		--crate-type lib rust/rtw_ap_sta_info_apmode.rs

rust-check-symbols-rtw-ap-sta-info-apmode: rust-objects-rtw-ap-sta-info-apmode-c rust-objects-rtw-ap-sta-info-apmode-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/ap/ap_sta_info_apmode_c_ref.o NEW=tests/host/ap/ap_sta_info_apmode_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_ap_sta_info_apmode.allow

# W3-82 PR8: expire asoc tick L1 (host C vs host Rust oracle).
rust-objects-rtw-ap-expire-asoc-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_AP_EXPIRE_ASOC_TEST \
		-o tests/host/ap/ap_expire_asoc_c_ref.o core/rtw_ap_expire_asoc.c

rust-objects-rtw-ap-expire-asoc-rust-ref:
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_ap_expire_asoc_test \
		--emit=obj=tests/host/ap/ap_expire_asoc_rust_ref.o \
		--crate-type lib rust/rtw_ap_expire_asoc.rs

rust-check-symbols-rtw-ap-expire-asoc: rust-objects-rtw-ap-expire-asoc-c rust-objects-rtw-ap-expire-asoc-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/ap/ap_expire_asoc_c_ref.o NEW=tests/host/ap/ap_expire_asoc_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_ap_expire_asoc.allow

# W3-82 PR9: expire auth list L1 (host C vs host Rust oracle).
rust-objects-rtw-ap-expire-auth-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_AP_EXPIRE_AUTH_TEST \
		-o tests/host/ap/ap_expire_auth_c_ref.o core/rtw_ap_expire_auth.c

rust-objects-rtw-ap-expire-auth-rust-ref:
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_ap_expire_auth_test \
		--emit=obj=tests/host/ap/ap_expire_auth_rust_ref.o \
		--crate-type lib rust/rtw_ap_expire_auth.rs

rust-check-symbols-rtw-ap-expire-auth: rust-objects-rtw-ap-expire-auth-c rust-objects-rtw-ap-expire-auth-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/ap/ap_expire_auth_c_ref.o NEW=tests/host/ap/ap_expire_auth_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_ap_expire_auth.allow

# W3-82 PR12: issue_aka_chk_frame L1 (host C vs host Rust oracle).
rust-objects-rtw-ap-aka-chk-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DCONFIG_ACTIVE_KEEP_ALIVE_CHECK -DHOST_AP_AKA_CHK_TEST \
		-o tests/host/ap/ap_aka_chk_c_ref.o core/rtw_ap_aka_chk.c

rust-objects-rtw-ap-aka-chk-rust-ref:
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_ap_aka_chk_test \
		--emit=obj=tests/host/ap/ap_aka_chk_rust_ref.o \
		--crate-type lib rust/rtw_ap_aka_chk.rs

rust-check-symbols-rtw-ap-aka-chk: rust-objects-rtw-ap-aka-chk-c rust-objects-rtw-ap-aka-chk-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/ap/ap_aka_chk_c_ref.o NEW=tests/host/ap/ap_aka_chk_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_ap_aka_chk.allow

# W3-82 PR15: rtw_check_restore_rf18 L1 (host C vs host Rust oracle).
rust-objects-rtw-ap-rf18-restore-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DRTW_CONFIG_RFREG18_WA -DHOST_AP_RF18_RESTORE_TEST \
		-o tests/host/ap/ap_rf18_restore_c_ref.o core/rtw_ap_rf18_restore.c

rust-objects-rtw-ap-rf18-restore-rust-ref:
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_ap_rf18_restore_test \
		--emit=obj=tests/host/ap/ap_rf18_restore_rust_ref.o \
		--crate-type lib rust/rtw_ap_rf18_restore.rs

rust-check-symbols-rtw-ap-rf18-restore: rust-objects-rtw-ap-rf18-restore-c rust-objects-rtw-ap-rf18-restore-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/ap/ap_rf18_restore_c_ref.o NEW=tests/host/ap/ap_rf18_restore_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_ap_rf18_restore.allow

# W3-82 PR20: expire_timeout_chk orchestrator L1 (host C vs host Rust ref).
rust-objects-rtw-ap-expire-timeout-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_AP_EXPIRE_TIMEOUT_TEST \
		-o tests/host/ap/ap_expire_timeout_c_ref.o core/rtw_ap_expire_timeout.c

rust-objects-rtw-ap-expire-timeout-rust-ref:
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_ap_expire_timeout_test \
		--emit=obj=tests/host/ap/ap_expire_timeout_rust_ref.o \
		--crate-type lib rust/rtw_ap_expire_timeout.rs

rust-check-symbols-rtw-ap-expire-timeout: rust-objects-rtw-ap-expire-timeout-c rust-objects-rtw-ap-expire-timeout-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/ap/ap_expire_timeout_c_ref.o NEW=tests/host/ap/ap_expire_timeout_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_ap_expire_timeout.allow

# W3-73 PR8: AP STA IE parse L1 (merged C oracle vs kbuild Rust object).
rust-objects-rtw-ap-sta-ie-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_AP_STA_IE_TEST \
		-o tests/host/ap/ap_sta_ie_cap_c_ref.o core/rtw_ap_sta_ie.c
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_AP_STA_IE_TEST \
		-o tests/host/ap/ap_sta_ie_rates_c_ref.o core/rtw_ap_sta_ie_rates.c
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_AP_STA_IE_TEST \
		-o tests/host/ap/ap_sta_ie_wmm_ht_c_ref.o core/rtw_ap_sta_ie_wmm_ht.c
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_AP_STA_IE_TEST -DCONFIG_RTW_MULTI_AP \
		-o tests/host/ap/ap_sta_ie_vht_multiap_c_ref.o core/rtw_ap_sta_ie_vht_multiap.c
	ld -r -o tests/host/ap/ap_sta_ie_c_ref.o tests/host/ap/ap_sta_ie_cap_c_ref.o \
		tests/host/ap/ap_sta_ie_rates_c_ref.o tests/host/ap/ap_sta_ie_wmm_ht_c_ref.o \
		tests/host/ap/ap_sta_ie_vht_multiap_c_ref.o

rust-objects-rtw-ap-sta-ie:
	@test -n "$(KDIR)" || { echo "Usage: make KDIR=… LLVM=1 rust-objects-rtw-ap-sta-ie"; exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/rtw_ap_sta_ie.o

rust-check-symbols-rtw-ap-sta-ie: rust-objects-rtw-ap-sta-ie-c rust-objects-rtw-ap-sta-ie
	$(MAKE) rust-check-symbols OLD=tests/host/ap/ap_sta_ie_c_ref.o NEW=rust/rtw_ap_sta_ie.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_ap_sta_ie.allow

# W3-74 PR6: STA security IE parse main fn L1 (C ref vs kbuild Rust object).
rust-objects-rtw-ap-sta-ie-sec-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-sign-compare -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_AP_STA_IE_SEC_TEST \
		-o tests/host/ap/ap_sta_ie_sec_c_ref.o core/rtw_ap_sta_ie_sec.c

rust-objects-rtw-ap-sta-ie-sec:
	@test -n "$(KDIR)" || { echo "Usage: make KDIR=… LLVM=1 rust-objects-rtw-ap-sta-ie-sec"; exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/rtw_ap_sta_ie_sec.o

rust-check-symbols-rtw-ap-sta-ie-sec: rust-objects-rtw-ap-sta-ie-sec-c rust-objects-rtw-ap-sta-ie-sec
	$(MAKE) rust-check-symbols OLD=tests/host/ap/ap_sta_ie_sec_c_ref.o NEW=rust/rtw_ap_sta_ie_sec.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_ap_sta_ie_sec.allow

# W3-69 PR4: peer-alive-only L1 (host C oracle vs kbuild Rust object).
rust-objects-rtw-mlme-ext-peer-alive:
	@test -n "$(KDIR)" || { echo "Usage: make KDIR=… LLVM=1 rust-objects-rtw-mlme-ext-peer-alive"; exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/rtw_mlme_ext_peer_alive.o
rust-objects-rtw-mlme-ext-peer-alive-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_MLME_EXT_PEER_ALIVE_TEST \
		-o tests/host/mlme_ext/peer_alive_c_ref.o core/rtw_mlme_ext_rest.c

rust-check-symbols-rtw-mlme-ext-peer-alive: rust-objects-rtw-mlme-ext-peer-alive-c rust-objects-rtw-mlme-ext-peer-alive
	$(MAKE) rust-check-symbols OLD=tests/host/mlme_ext/peer_alive_c_ref.o NEW=rust/rtw_mlme_ext_peer_alive.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_mlme_ext_peer_alive.allow

# W3-70 PR4: scan sparse/backop/timeout L1 (host C oracle vs kbuild Rust object).
rust-objects-rtw-mlme-ext-scan:
	@test -n "$(KDIR)" || { echo "Usage: make KDIR=… LLVM=1 rust-objects-rtw-mlme-ext-scan"; exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/rtw_mlme_ext_scan.o
rust-objects-rtw-mlme-ext-scan-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_MLME_EXT_SCAN_TEST -DCONFIG_SCAN_BACKOP -DCONFIG_RUST -DCONFIG_RUST_MLME_EXT_PICK_CH \
		-o tests/host/mlme_ext/scan_c_ref.o core/rtw_mlme_ext_rest.c

rust-check-symbols-rtw-mlme-ext-scan: rust-objects-rtw-mlme-ext-scan-c rust-objects-rtw-mlme-ext-scan
	$(MAKE) rust-check-symbols OLD=tests/host/mlme_ext/scan_c_ref.o NEW=rust/rtw_mlme_ext_scan.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_mlme_ext_scan.allow

# W3-71 PR3: pick_ch L1 (host C oracle vs kbuild Rust object).
rust-objects-rtw-mlme-ext-pick-ch:
	@test -n "$(KDIR)" || { echo "Usage: make KDIR=… LLVM=1 rust-objects-rtw-mlme-ext-pick-ch"; exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/rtw_mlme_ext_pick_ch.o
rust-objects-rtw-mlme-ext-pick-ch-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_MLME_EXT_SCAN_TEST -DCONFIG_SCAN_BACKOP -DCONFIG_P2P \
		-o tests/host/mlme_ext/pick_ch_c_ref.o core/rtw_mlme_ext_rest.c

rust-check-symbols-rtw-mlme-ext-pick-ch: rust-objects-rtw-mlme-ext-pick-ch-c rust-objects-rtw-mlme-ext-pick-ch
	$(MAKE) rust-check-symbols OLD=tests/host/mlme_ext/pick_ch_c_ref.o NEW=rust/rtw_mlme_ext_pick_ch.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_mlme_ext_pick_ch.allow

# W3-72 PR5: band_ie L1 (host C oracle vs kbuild Rust object).
rust-objects-rtw-mlme-ext-band-ie:
	@test -n "$(KDIR)" || { echo "Usage: make KDIR=… LLVM=1 rust-objects-rtw-mlme-ext-band-ie"; exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/rtw_mlme_ext_band_ie.o
rust-objects-rtw-mlme-ext-band-ie-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/tests/host/wlan_util \
		-I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_MLME_EXT_BAND_IE_TEST \
		-o tests/host/mlme_ext/band_ie_c_ref.o core/rtw_mlme_ext_rest.c

rust-check-symbols-rtw-mlme-ext-band-ie: rust-objects-rtw-mlme-ext-band-ie-c rust-objects-rtw-mlme-ext-band-ie
	$(MAKE) rust-check-symbols OLD=tests/host/mlme_ext/band_ie_c_ref.o NEW=rust/rtw_mlme_ext_band_ie.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_mlme_ext_band_ie.allow

# W3-56 PR3: op_class_pref-only L1 (host C oracle vs host Rust oracle).
rust-objects-rtw-rf-op-class-pref-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_RF_OP_CLASS_PREF_TEST -o tests/host/rf/op_class_pref_c_ref.o core/rtw_rf_op_class_pref.c

rust-objects-rtw-rf-op-class-pref-rust-ref:
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_rf_op_class_pref_test \
		--emit=obj=tests/host/rf/op_class_pref_rust_ref.o \
		--crate-type lib rust/rtw_rf_op_class_pref.rs

rust-check-symbols-rtw-rf-op-class-pref: rust-objects-rtw-rf-op-class-pref-c rust-objects-rtw-rf-op-class-pref-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/rf/op_class_pref_c_ref.o NEW=tests/host/rf/op_class_pref_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_rf_op_class_pref.allow

# W3-57 PR3: op_class_dump-only L1 (host C oracle vs host Rust oracle).
rust-objects-rtw-rf-op-class-dump-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_RF_OP_CLASS_DUMP_TEST \
		-o tests/host/rf/op_class_dump_c_ref.o core/rtw_rf_op_class_dump.c

rust-objects-rtw-rf-op-class-dump-rust-ref:
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_rf_op_class_dump_test --cfg config_rtw_debug \
		--emit=obj=tests/host/rf/op_class_dump_rust_ref.o \
		--crate-type lib rust/rtw_rf_op_class_dump.rs

rust-check-symbols-rtw-rf-op-class-dump: rust-objects-rtw-rf-op-class-dump-c rust-objects-rtw-rf-op-class-dump-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/rf/op_class_dump_c_ref.o NEW=tests/host/rf/op_class_dump_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_rf_op_class_dump.allow

# W3-58 PR3: dump_txpwr_lmt-only L1 (host C oracle vs host Rust oracle).
rust-objects-rtw-rf-dump-txpwr-lmt-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DCONFIG_TXPWR_LIMIT -DHOST_RF_DUMP_TXPWR_LMT_TEST \
		-Wno-format-truncation -Wno-unused-but-set-variable \
		-o tests/host/rf/dump_txpwr_lmt_c_ref.o core/rtw_rf_dump_txpwr_lmt.c

rust-objects-rtw-rf-dump-txpwr-lmt-rust-ref:
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_rf_dump_txpwr_lmt_test \
		--cfg txpwr_limit --cfg ieee80211_band_5ghz \
		--emit=obj=tests/host/rf/dump_txpwr_lmt_rust_ref.o \
		--crate-type lib rust/rtw_rf_dump_txpwr_lmt.rs

rust-check-symbols-rtw-rf-dump-txpwr-lmt: rust-objects-rtw-rf-dump-txpwr-lmt-c rust-objects-rtw-rf-dump-txpwr-lmt-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/rf/dump_txpwr_lmt_c_ref.o NEW=tests/host/rf/dump_txpwr_lmt_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_rf_dump_txpwr_lmt.allow

# W3-59 PR3: kfree TX gain L1 (host C oracle vs host Rust oracle for get helper).
rust-objects-rtw-rf-kfree-tx-gain-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_RF_KFREE_TX_GAIN_TEST -Wno-unused-but-set-variable \
		-o tests/host/rf/kfree_tx_gain_c_ref.o core/rtw_rf_kfree_tx_gain.c

rust-objects-rtw-rf-kfree-tx-gain-rust-ref:
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_rf_kfree_tx_gain_test --cfg rf_power_trim \
		--emit=obj=tests/host/rf/kfree_tx_gain_rust_ref.o \
		--crate-type lib rust/rtw_rf_kfree_tx_gain.rs

rust-check-symbols-rtw-rf-kfree-tx-gain: rust-objects-rtw-rf-kfree-tx-gain-c rust-objects-rtw-rf-kfree-tx-gain-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/rf/kfree_tx_gain_c_ref.o NEW=tests/host/rf/kfree_tx_gain_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_rf_kfree_tx_gain.allow

# W3-60 PR4: cmd/evt priv init/teardown L1 (host C oracle vs host Rust oracle).
rust-objects-rtw-cmd-rest-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_CMD_PRIV_TEST -o tests/host/cmd/cmd_priv_c_ref.o core/rtw_cmd_priv.c

rust-objects-rtw-cmd-rest-rust-ref:
	rustc -C opt-level=2 -C overflow-checks=on \
		--cfg host_cmd_priv_test --cfg event_thread_mode --cfg c2h_wk \
		--emit=obj=tests/host/cmd/cmd_priv_rust_ref.o \
		--crate-type lib rust/rtw_cmd_rest.rs

rust-check-symbols-rtw-cmd-rest: rust-objects-rtw-cmd-rest-c rust-objects-rtw-cmd-rest-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/cmd/cmd_priv_c_ref.o NEW=tests/host/cmd/cmd_priv_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_cmd_rest.allow ALLOW_VACUOUS=1

rust-objects-rtw-cmd-queue-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_CMD_QUEUE_TEST -DCONFIG_EVENT_THREAD_MODE \
		-o tests/host/cmd/cmd_queue_c_ref.o core/rtw_cmd_queue.c

rust-objects-rtw-cmd-queue-rust-ref:
	rustc -C opt-level=2 -C overflow-checks=on \
		--cfg host_cmd_queue_test --cfg event_thread_mode --cfg rust_cmd_queue \
		--emit=obj=tests/host/cmd/cmd_queue_rust_ref.o \
		--crate-type lib rust/rtw_cmd_rest.rs

rust-check-symbols-rtw-cmd-queue: rust-objects-rtw-cmd-queue-c rust-objects-rtw-cmd-queue-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/cmd/cmd_queue_c_ref.o NEW=tests/host/cmd/cmd_queue_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_cmd_queue.allow ALLOW_VACUOUS=1

# W3-39: host C oracle recv_rest vs rust/rtw_recv.o.
rust-objects-rtw-recv:
	@test -n "$(KDIR)" || { echo "Usage: make KDIR=… LLVM=1 rust-objects-rtw-recv"; exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/rtw_recv.o
rust-objects-rtw-recv-rest-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_RECV_TEST -DHOST_RECV_WFD_TEST -o tests/host/recv/recv_rest_c_ref.o core/rtw_recv_rest.c
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_RECV_TEST -DHOST_RECV_LLC_TEST -o tests/host/recv/recv_llc_c_ref.o core/rtw_recv_llc_rest.c
rust-check-symbols-rtw-recv: rust-objects-rtw-recv-rest-c rust-objects-rtw-recv-pn-rest-c rust-objects-rtw-recv
	$(MAKE) rust-check-symbols OLD=tests/host/recv/recv_rest_c_ref.o NEW=rust/rtw_recv.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_recv.allow ALLOW_VACUOUS=1
	$(MAKE) rust-check-symbols OLD=tests/host/recv/recv_llc_c_ref.o NEW=rust/rtw_recv.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_recv_llc.allow ALLOW_VACUOUS=1
	$(MAKE) rust-check-symbols OLD=tests/host/recv/recv_pn_c_ref.o NEW=rust/rtw_recv.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_recv_pn.allow ALLOW_VACUOUS=1
rust-objects-rtw-recv-pn-rest-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_RECV_TEST -DHOST_RECV_PN_TEST -o tests/host/recv/recv_pn_c_ref.o core/rtw_recv_pn_rest.c
rust-check-symbols-rtw-recv-pn: rust-objects-rtw-recv-pn-rest-c rust-objects-rtw-recv
	$(MAKE) rust-check-symbols OLD=tests/host/recv/recv_pn_c_ref.o NEW=rust/rtw_recv.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_recv_pn.allow ALLOW_VACUOUS=1

# W3-85 PR1: count_rx_stats host L1 (C vs Rust oracle).
rust-objects-rtw-recv-sta-count-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_RECV_STA_TEST -DHOST_RECV_STA_RUST_VALIDATE \
		-o tests/host/recv/recv_sta_count_c_ref.o core/rtw_recv_sta_rest.c

rust-objects-rtw-recv-sta-count-rust-ref:
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--emit=obj=tests/host/recv/recv_sta_count_rust_ref.o \
		--crate-type lib rust/rtw_recv_sta_count.rs

rust-check-symbols-rtw-recv-sta-count: rust-objects-rtw-recv-sta-count-c rust-objects-rtw-recv-sta-count-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/recv/recv_sta_count_c_ref.o NEW=tests/host/recv/recv_sta_count_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_recv_sta_count.allow ALLOW_VACUOUS=1

rust-objects-rtw-recv-sta-validate-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/core \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_RECV_STA_TEST -DHOST_RECV_STA_RUST_COUNT \
		-o tests/host/recv/recv_sta_validate_c_ref.o core/rtw_recv_sta_rest.c
rust-objects-rtw-recv-sta-validate-rust-ref:
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--emit=obj=tests/host/recv/recv_sta_validate_rust_ref.o \
		--crate-type lib rust/rtw_recv_sta_validate.rs
rust-check-symbols-rtw-recv-sta-validate: rust-objects-rtw-recv-sta-validate-c rust-objects-rtw-recv-sta-validate-rust-ref
	$(MAKE) rust-check-symbols OLD=tests/host/recv/recv_sta_validate_c_ref.o NEW=tests/host/recv/recv_sta_validate_rust_ref.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_recv_sta_count.allow ALLOW_VACUOUS=1

# W3-40: host C oracle xmit_rest vs rust/rtw_xmit.o.
rust-objects-rtw-xmit:
	@test -n "$(KDIR)" || { echo "Usage: make KDIR=… LLVM=1 rust-objects-rtw-xmit"; exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/rtw_xmit.o
rust-objects-rtw-xmit-rest-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_XMIT_TEST -o tests/host/xmit/xmit_rest_c_ref.o core/rtw_xmit_rest.c
rust-check-symbols-rtw-xmit: rust-objects-rtw-xmit-rest-c rust-objects-rtw-xmit
	$(MAKE) rust-check-symbols OLD=tests/host/xmit/xmit_rest_c_ref.o NEW=rust/rtw_xmit.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_xmit.allow ALLOW_VACUOUS=1

rust-objects-rtw-xmit-qos-rest-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_XMIT_QOS_TEST -o tests/host/xmit/xmit_qos_c_ref.o core/rtw_xmit_qos_rest.c
rust-check-symbols-rtw-xmit-qos: rust-objects-rtw-xmit-qos-rest-c rust-objects-rtw-xmit
	$(MAKE) rust-check-symbols OLD=tests/host/xmit/xmit_qos_c_ref.o NEW=rust/rtw_xmit.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_xmit_qos.allow ALLOW_VACUOUS=1

rust-objects-rtw-xmit-sctx-rest-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_XMIT_SCTX_TEST -o tests/host/xmit/xmit_sctx_c_ref.o core/rtw_xmit_sctx_rest.c
rust-check-symbols-rtw-xmit-sctx: rust-objects-rtw-xmit-sctx-rest-c rust-objects-rtw-xmit
	$(MAKE) rust-check-symbols OLD=tests/host/xmit/xmit_sctx_c_ref.o NEW=rust/rtw_xmit.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_xmit_sctx.allow ALLOW_VACUOUS=1

rust-objects-rtw-xmit-update-attrib-rest-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_XMIT_UPDATE_ATTRIB_TEST -o tests/host/xmit/xmit_update_attrib_c_ref.o \
		core/rtw_xmit_update_attrib_rest.c
rust-objects-rtw-xmit-update-attrib:
	@test -n "$(KDIR)" || { echo "Usage: make KDIR=… LLVM=1 rust-objects-rtw-xmit-update-attrib"; exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/rtw_xmit_update_attrib_kern.o
rust-check-symbols-rtw-xmit-update-attrib: rust-objects-rtw-xmit-update-attrib-rest-c rust-objects-rtw-xmit-update-attrib
	$(MAKE) rust-check-symbols OLD=tests/host/xmit/xmit_update_attrib_c_ref.o NEW=rust/rtw_xmit_update_attrib_kern.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_xmit_update_attrib.allow ALLOW_VACUOUS=1
# W3-50: host C oracle iol_rest vs rust/rtw_iol_rest.o.
rust-objects-rtw-iol-rest:
	@test -n "$(KDIR)" || { echo "Usage: make KDIR=… LLVM=1 rust-objects-rtw-iol-rest"; exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/rtw_iol_rest.o
rust-objects-rtw-iol-rest-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I$(shell pwd)/tests/host/include -I$(shell pwd)/include \
		-include $(shell pwd)/tests/host/include/host_autoconf.h \
		-DHOST_IOL_TEST -o tests/host/iol/iol_rest_c_ref.o core/rtw_iol_rest.c
rust-check-symbols-rtw-iol-rest: rust-objects-rtw-iol-rest-c rust-objects-rtw-iol-rest
	$(MAKE) rust-check-symbols OLD=tests/host/iol/iol_rest_c_ref.o NEW=rust/rtw_iol_rest.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_iol_rest.allow ALLOW_VACUOUS=1

# W3-95 PR3: host C oracle (sreset lifecycle shim) vs rust/rtw_sreset.o.
rust-objects-rtw-sreset:
	@test -n "$(KDIR)" || { echo "Usage: make KDIR=… LLVM=1 rust-objects-rtw-sreset"; exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/rtw_sreset.o
rust-objects-rtw-sreset-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
		-I tests/host/include -I include \
		-include tests/host/include/host_autoconf.h \
		-DHOST_SRESET_TEST -o tests/host/sreset/sreset_lifecycle_c_ref.o \
		tests/host/sreset/host_sreset_lifecycle_shim.c
rust-check-symbols-rtw-sreset: rust-objects-rtw-sreset-c rust-objects-rtw-sreset
	$(MAKE) rust-check-symbols OLD=tests/host/sreset/sreset_lifecycle_c_ref.o NEW=rust/rtw_sreset.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_sreset.allow ALLOW_VACUOUS=1

# W3-94 follow-up PR4: host C oracle (ps deny shim) vs rust/rtw_pwrctrl.o.
rust-objects-rtw-pwrctrl:
	@test -n "$(KDIR)" || { echo "Usage: make KDIR=… LLVM=1 rust-objects-rtw-pwrctrl"; exit 1; }
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) rust/rtw_pwrctrl.o
rust-objects-rtw-pwrctrl-c:
	gcc -c -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
		-I tests/host/include -I include \
		-include tests/host/include/host_autoconf.h \
		-o tests/host/pwrctrl/ps_deny_c_ref.o \
		tests/host/pwrctrl/host_pwrctrl_ps_deny_shim.c
rust-check-symbols-rtw-pwrctrl: rust-objects-rtw-pwrctrl-c rust-objects-rtw-pwrctrl
	$(MAKE) rust-check-symbols OLD=tests/host/pwrctrl/ps_deny_c_ref.o NEW=rust/rtw_pwrctrl.o \
		ALLOWLIST=docs/rust-migration/scripts/rtw_pwrctrl.allow ALLOW_VACUOUS=1

# Smoke test for check-symbols.sh (T1). Builds only rust/aes_ctr.o via kbuild, not the
# full module. The C reference uses host gcc + HOST_CRYPTO_TEST for speed; production
# L1 on a swap should compare against a kbuild-produced OLD.o from master.
rust-check-symbols-selftest: rust-objects-aes-ctr
	@set -e; \
	drop_fixture=docs/rust-migration/scripts/fixtures/omac1_vs_aes_ctr.allow; \
	rename_fixture=docs/rust-migration/scripts/fixtures/aes_ctr_rename_smoke.allow; \
	tmp_ctr=$$(mktemp /tmp/aes-ctr-ref.XXXXXX.o); \
	tmp_omac=$$(mktemp /tmp/aes-omac1-ref.XXXXXX.o); \
	gcc -c -Wall -I$(shell pwd)/tests/host/include -I$(shell pwd)/core/crypto \
		-DHOST_CRYPTO_TEST -o "$$tmp_ctr" core/crypto/aes-ctr.c; \
	gcc -c -Wall -I$(shell pwd)/tests/host/include -I$(shell pwd)/core/crypto \
		-DHOST_CRYPTO_TEST -o "$$tmp_omac" core/crypto/aes-omac1.c; \
	NM=$(RUST_CHECK_NM) ./docs/rust-migration/scripts/check-symbols.sh "$$tmp_ctr" rust/aes_ctr.o; \
	NM=$(RUST_CHECK_NM) ./docs/rust-migration/scripts/check-symbols.sh "$$tmp_ctr" rust/aes_ctr.o \
		--allowlist "$$rename_fixture"; \
	if NM=$(RUST_CHECK_NM) ./docs/rust-migration/scripts/check-symbols.sh "$$tmp_omac" rust/aes_ctr.o 2>/dev/null; then \
		echo "rust-check-symbols-selftest: expected mismatch without allowlist" >&2; \
		exit 1; \
	fi; \
	NM=$(RUST_CHECK_NM) ./docs/rust-migration/scripts/check-symbols.sh "$$tmp_omac" rust/aes_ctr.o \
		--allowlist "$$drop_fixture" --allow-vacuous; \
	rm -f "$$tmp_ctr" "$$tmp_omac"; \
	echo "rust-check-symbols-selftest: OK"

modules:
	$(MAKE) $(KBUILD_OPTS) -C $(KSRC) M=$(shell pwd) modules

strip:
	$(CROSS_COMPILE)strip $(MODULE_NAME).ko --strip-unneeded

install:
	install -p -m 644 $(MODULE_NAME).ko  $(MODDESTDIR)
	/sbin/depmod -a ${KVER}

uninstall:
	rm -f $(MODDESTDIR)/$(MODULE_NAME).ko
	/sbin/depmod -a ${KVER}

backup_rtlwifi:
	@echo "Making backup rtlwifi drivers"
ifneq (,$(wildcard $(STAGINGMODDIR)/rtl*))
	@tar cPf $(wildcard $(STAGINGMODDIR))/backup_rtlwifi_driver.tar $(wildcard $(STAGINGMODDIR)/rtl*)
	@rm -rf $(wildcard $(STAGINGMODDIR)/rtl*)
endif
ifneq (,$(wildcard $(MODDESTDIR)realtek))
	@tar cPf $(MODDESTDIR)backup_rtlwifi_driver.tar $(MODDESTDIR)realtek
	@rm -fr $(MODDESTDIR)realtek
endif
ifneq (,$(wildcard $(MODDESTDIR)rtl*))
	@tar cPf $(MODDESTDIR)../backup_rtlwifi_driver.tar $(wildcard $(MODDESTDIR)rtl*)
	@rm -fr $(wildcard $(MODDESTDIR)rtl*)
endif
	@/sbin/depmod -a ${KVER}
	@echo "Please reboot your system"

restore_rtlwifi:
	@echo "Restoring backups"
ifneq (,$(wildcard $(STAGINGMODDIR)/backup_rtlwifi_driver.tar))
	@tar xPf $(STAGINGMODDIR)/backup_rtlwifi_driver.tar
	@rm $(STAGINGMODDIR)/backup_rtlwifi_driver.tar
endif
ifneq (,$(wildcard $(MODDESTDIR)backup_rtlwifi_driver.tar))
	@tar xPf $(MODDESTDIR)backup_rtlwifi_driver.tar
	@rm $(MODDESTDIR)backup_rtlwifi_driver.tar
endif
ifneq (,$(wildcard $(MODDESTDIR)../backup_rtlwifi_driver.tar))
	@tar xPf $(MODDESTDIR)../backup_rtlwifi_driver.tar
	@rm $(MODDESTDIR)../backup_rtlwifi_driver.tar
endif
	@/sbin/depmod -a ${KVER}
	@echo "Please reboot your system"

config_r:
	@echo "make config"
	/bin/bash script/Configure script/config.in


.PHONY: modules clean

clean:
	#$(MAKE) -C $(KSRC) M=$(shell pwd) clean
	cd hal ; rm -fr */*/*/*.mod.c */*/*/*.mod */*/*/*.o */*/*/.*.cmd */*/*/*.ko
	cd hal ; rm -fr */*/*.mod.c */*/*.mod */*/*.o */*/.*.cmd */*/*.ko
	cd hal ; rm -fr */*.mod.c */*.mod */*.o */.*.cmd */*.ko
	cd hal ; rm -fr *.mod.c *.mod *.o .*.cmd *.ko
	cd core ; rm -fr */*.mod.c */*.mod */*.o */.*.cmd */*.ko
	cd core ; rm -fr *.mod.c *.mod *.o .*.cmd *.ko
	cd os_dep/linux ; rm -fr *.mod.c *.mod *.o .*.cmd *.ko
	cd os_dep ; rm -fr *.mod.c *.mod *.o .*.cmd *.ko
	cd platform ; rm -fr *.mod.c *.mod *.o .*.cmd *.ko
	cd rust ; rm -fr *.mod.c *.mod *.o .*.cmd *.ko *.rmeta 2>/dev/null || true
	rm -fr Module.symvers ; rm -fr Module.markers ; rm -fr modules.order
	rm -fr *.mod.c *.mod *.o .*.cmd *.ko *~
	rm -fr .tmp_versions
endif
