# REALTEK RTL88x2B USB Linux Driver
**Current Driver Version**: 5.13.1-30
**Support Kernel**: 2.6.24 ~ 6.15 (with unofficial patches)

Linux in-tree rtw8822bu driver is a work in progress. Check [this](https://lore.kernel.org/lkml/20220518082318.3898514-1-s.hauer@pengutronix.de/) patchset.

For official release notes please check ReleaseNotes.pdf.

**Note:** if you believe your device is **RTL8812BU** or **RTL8822BU** but after loading the module no NIC shows up, the device ID maybe not be in the driver whitelist. In this case, please submit a new issue with `lsusb` result, and your device name, brand, website, etc.

This driver does *NOT* support newer Realtek 802.11ax (Wi-Fi 6) chipsets such as RTL8852BU.

## Linux 5.18+ and RTW88 Driver
Starting from Linux 5.18, some distributions have added experimental RTW88 USB support (include RTW88x2BU support).
It is not yet stable but if it works well on your system, then you no longer need this driver.
If it doesn't, then you need to manually blacklist it because it has a higher loading priority than this external drivers.

Check the currently loaded modules using `lsmod`. If you see `rtw88_core`, `rtw88_usb`, or any name beginning with `rtw88_` then you are using the RTW88 driver.
If you see `88x2bu` then you are using this RTW88x2BU driver.

To blacklist RTW88 8822bu USB driver run:

```
echo "blacklist rtw88_8822bu" > /etc/modprobe.d/rtw8822bu.conf
```

...and reboot your system.

## Supported Devices
<details>
  <summary>
    ASUS
  </summary>

* ASUS AC1300 USB-AC55 B1
* ASUS U2
* ASUS USB-AC53 Nano
* ASUS USB-AC58
</details>

<details>
  <summary>
    Dlink
  </summary>

* Dlink - DWA-181
* Dlink - DWA-182
* Dlink - DWA-183 D Version
* Dlink - DWA-185
* Dlink - DWA-T185
</details>

<details>
  <summary>
    Edimax
  </summary>

* Edimax EW-7822ULC
* Edimax EW-7822UTC
* Edimax EW-7822UAD
</details>

<details>
  <summary>
    Mercusys
  </summary>

* Mercusys MA30N
* Mercusys MA30H V2
</details>

<details>
  <summary>
    NetGear
  </summary>

* NetGear A6150
</details>

<details>
  <summary>
    TP-Link
  </summary>

* TP-Link Archer T3U
* TP-Link Archer T3U Plus
* TP-Link Archer T3U Nano
* TP-Link Archer T4U V3
* TP-Link Archer T4U Plus
</details>

<details>
  <summary>
    TRENDnet
  </summary>

* TRENDnet TEW-808UBM
</details>

<details>
  <summary>
    ZYXEL
  </summary>

* ZYXEL NWD6602
</details>


And more.

# How to use this kernel module
* Ensure you have C compiler & toolchains, e.g. `build-essential` for Debian/Ubuntu, `base-devel` for Arch, etc.
* Make sure you have installed the corresponding kernel headers
* All commands need to be run in the driver directory
* You need rebuild the kernel module everytime you update/change the kernel if you are not using DKMS


## Manual installation
### Clean
* Make sure you clean old build files before building new ones
```
make clean
```

### Building module for current running kernel
```
make
```

### Building module for other kernels
```
make KSRC=/lib/modules/YOUR_KERNEL_VERSION/build
```

### Installing
```
sudo make install
```

### Uninstalling
```
sudo make uninstall
```

## Using DKMS (Dynamic Kernel Module Support)

Allows smooth integration with kernel updates.

### Initial DKMS installation
```
git clone "https://github.com/RinCat/RTL88x2BU-Linux-Driver.git" /usr/src/rtl88x2bu-git
sed -i 's/PACKAGE_VERSION="@PKGVER@"/PACKAGE_VERSION="git"/g' /usr/src/rtl88x2bu-git/dkms.conf
dkms add -m rtl88x2bu -v git
dkms autoinstall
```
### Upgrading the driver, when already under DKMS
```
cd  /usr/src/rtl88x2bu-git
git fetch
git rebase origin/master --autostash
dkms build rtl88x2bu/git --force
dkms install rtl88x2bu/git --force
```

# USB 3.0 Support
You can try using `modprobe 88x2bu rtw_switch_usb_mode=1` to force the adapter to run under USB 3.0. But if your adapter/port/motherboard does not support it, the driver will be stuck in a restart loop. Remove the parameter and reload the driver to restore. Alternatively, `modprobe 88x2bu rtw_switch_usb_mode=2` runs it as a USB 2 device.

Notice: If you had already loaded the module, use `modprobe -r 88x2bu` to unload it first.

If you want to force a given mode permanently (even when switching the adapter across devices), create the file `/etc/modprobe.d/99-RTL88x2BU.conf` with the following content:
`options 88x2bu rtw_switch_usb_mode=1`


# Debug
To set debug log use `echo 5 > /proc/net/rtl88x2bu/log_level` or `modprobe 88x2bu rtw_drv_log_level=5`

# Distribution
* Archlinux AUR https://aur.archlinux.org/packages/rtl88x2bu-dkms-git/
