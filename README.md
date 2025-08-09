# REALTEK RTL88x2B USB Linux Driver
**Current Driver Version**: 5.13.1
**Support Kernel**: 2.6.24 ~ 6.8

Official release note please check ReleaseNotes.pdf

**Note:** if you believe your device is **RTL8812BU** or **RTL8822BU** but after loaded the module no NIC shows up, the device ID maybe not in the driver whitelist. In this case please submit a new issue with `lsusb` result, and your device name, brand, website, etc.

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
* Make sure you cleaned old build files before builds new one
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

## Manual DKMS installation
```
git clone "https://github.com/RinCat/RTL88x2BU-Linux-Driver.git" /usr/src/rtl88x2bu-git
sed -i 's/PACKAGE_VERSION="@PKGVER@"/PACKAGE_VERSION="git"/g' /usr/src/rtl88x2bu-git/dkms.conf
dkms add -m rtl88x2bu -v git
dkms autoinstall
```

# USB 3.0 Support
You can try use `modprobe 88x2bu rtw_switch_usb_mode=1` to force the adapter run under USB 3.0. But if your adapter/port/motherboard not support it, the driver will be in restart loop. Remove the parameter and reload the driver to restore. Alternatively, `modprobe 88x2bu rtw_switch_usb_mode=2` let\'s it run as USB 2 device.

Notice: If you had already loaded the moduel, use `modprobe -r 88x2bu` to unload it first.

If you want to force a given mode permanently (even when switching the adapter across devices), create the file `/etc/modprobe.d/99-RTL88x2BU.conf` with the following content:
`options 88x2bu rtw_switch_usb_mode=1`


# Debug
Set debug log use `echo 5 > /proc/net/rtl88x2bu/log_level` or `modprobe 88x2bu rtw_drv_log_level=5`

# Distribution
* Archlinux AUR https://aur.archlinux.org/packages/rtl88x2bu-dkms-git/
