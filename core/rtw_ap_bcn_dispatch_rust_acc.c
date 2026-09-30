// SPDX-License-Identifier: GPL-2.0
/* Kernel accessors for rust/rtw_ap_bcn_dispatch.rs (W3-81 PR5). */
#include <drv_types.h>
#ifdef CONFIG_RTW_MESH
#include "../core/mesh/rtw_mesh.h"
#endif

#if defined(CONFIG_RUST_AP_BCN_DISPATCH) && !defined(HOST_AP_BCN_DISPATCH_TEST)

u8 rtw_rust_bcn_dispatch_bstart_bss(_adapter *adapter)
{
	return adapter->mlmeextpriv.bstart_bss;
}

_lock *rtw_rust_bcn_dispatch_bcn_lock(_adapter *adapter)
{
	return &adapter->mlmepriv.bcn_update_lock;
}

void rtw_rust_bcn_dispatch_set_update_bcn(_adapter *adapter, u8 v)
{
	adapter->mlmepriv.update_bcn = v;
}

void rtw_rust_bcn_dispatch_enter_critical(_lock *lock, _irqL *irqL)
{
	_enter_critical_bh(lock, irqL);
}

void rtw_rust_bcn_dispatch_exit_critical(_lock *lock, _irqL *irqL)
{
	_exit_critical_bh(lock, irqL);
}

#ifdef CONFIG_RTW_MESH
u8 rtw_rust_bcn_dispatch_mesh_config(_adapter *adapter)
{
	struct mlme_ext_priv *pmlmeext = &adapter->mlmeextpriv;
	WLAN_BSSID_EX *bss = &pmlmeext->mlmext_info.network;
	bool updated;

	updated = rtw_mesh_update_bss_peering_status(adapter, bss);
	updated |= rtw_mesh_update_bss_formation_info(adapter, bss);
	updated |= rtw_mesh_update_bss_forwarding_state(adapter, bss);
	return updated ? 1 : 0;
}
#endif

#endif /* CONFIG_RUST_AP_BCN_DISPATCH && !HOST_AP_BCN_DISPATCH_TEST */
