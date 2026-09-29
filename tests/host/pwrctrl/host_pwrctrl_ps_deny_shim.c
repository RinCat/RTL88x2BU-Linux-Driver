// SPDX-License-Identifier: GPL-2.0
/* Host fixtures + C oracle for W3-94 ps deny gate helpers. */

#include <stdio.h>
#include "host_pwrctrl_types.h"

#define RTW_INFO(fmt, ...) ((void)0)

void _enter_pwrlock(_pwrlock *plock)
{
	(void)plock;
}

void _exit_pwrlock(_pwrlock *plock)
{
	(void)plock;
}

void rtw_ps_deny(PADAPTER padapter, PS_DENY_REASON reason)
{
	struct pwrctrl_priv *pwrpriv;

	pwrpriv = adapter_to_pwrctl(padapter);

	_enter_pwrlock(&pwrpriv->lock);
	if (pwrpriv->ps_deny & (1U << reason)) {
		RTW_INFO("duplicate ps_deny reason %d\n", reason);
	}
	pwrpriv->ps_deny |= (1U << reason);
	_exit_pwrlock(&pwrpriv->lock);
}

void rtw_ps_deny_cancel(PADAPTER padapter, PS_DENY_REASON reason)
{
	struct pwrctrl_priv *pwrpriv;

	pwrpriv = adapter_to_pwrctl(padapter);

	_enter_pwrlock(&pwrpriv->lock);
	if ((pwrpriv->ps_deny & (1U << reason)) == 0) {
		RTW_INFO("cancel ps_deny reason %d not set\n", reason);
	}
	pwrpriv->ps_deny &= ~(1U << reason);
	_exit_pwrlock(&pwrpriv->lock);
}

u32 rtw_ps_deny_get(PADAPTER padapter)
{
	return adapter_to_pwrctl(padapter)->ps_deny;
}
