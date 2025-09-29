/*
 * Copyright (c) 2025 Core Devices LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/arch/cpu.h>

#include <pb/bootbit.h>

#include <register.h>

/* Use RTC backup register 2 for boot bits */
#define RTC_BKP2R offsetof(RTC_TypeDef, BKP2R)

void pb_bootbit_init(void)
{
	if (!pb_bootbit_tst(PB_BOOTBIT_INITIALIZED)) {
		sys_write32(RTC_BASE + RTC_BKP2R, PB_BOOTBIT_INITIALIZED);
	}
}

void pb_bootbit_set(enum pb_bootbit bit)
{
	sys_bitfield_set_bit(RTC_BASE + RTC_BKP2R, bit);
}

void pb_bootbit_clr(enum pb_bootbit bit)
{
	sys_bitfield_clear_bit(RTC_BASE + RTC_BKP2R, bit);
}

bool pb_bootbit_tst(enum pb_bootbit bit)
{
	return sys_bitfield_test_bit(RTC_BASE + RTC_BKP2R, bit);
}
