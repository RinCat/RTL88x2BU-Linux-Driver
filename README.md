# REALTEK RTL88x2B USB Linux Driver
**Current Driver Version**: 5.13.1-30
**Support Kernel**: 2.6.24 ~ 7.2 (with unofficial patches)

**Most users should use the in-kernel `rtw88_8822bu` driver on Linux 6.12 or newer instead of this driver.** Support for RTL8812BU and RTL8822BU adapters was introduced in Linux 6.2, but later kernels include substantial improvements. Use an up-to-date distribution kernel and firmware.

For official release notes please check ReleaseNotes.pdf.

**Note:** if you believe your device is **RTL8812BU** or **RTL8822BU** but after loading the module no NIC shows up, the device ID maybe not be in the driver whitelist. In this case, please submit a new issue with `lsusb` result, and your device name, brand, website, etc.

This driver does *NOT* support newer Realtek 802.11ax (Wi-Fi 6) chipsets such as RTL8852BU.

## Linux 6.12+ and the in-kernel RTW88 driver
The in-kernel driver is recommended for normal use. This external driver is a fallback for older kernels, specific compatibility problems, or specialized features and vendor-specific tools.

Check the currently loaded modules using `lsmod`. `rtw88_8822bu` is the in-kernel module for these adapters; `88x2bu` is the external module from this repository. Other `rtw88_` modules may belong to another wireless device.

### Using this external driver instead

Only blacklist the in-kernel driver if you need to use this external driver. To prevent `rtw88_8822bu` from loading automatically, run:

```sh
echo "blacklist rtw88_8822bu" | sudo tee /etc/modprobe.d/rtw8822bu.conf
```

Install this external driver using the instructions below, then reboot your system.

To return to the in-kernel driver, uninstall this external driver using the method matching your installation (manual or DKMS), remove the blacklist entry created above, and reboot. Also remove any other blacklist entries you previously added for `rtw88_8822bu`.

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
