/*
 * Copyright (c) 2025 Core Devices LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>

#include <zephyr/arch/cpu.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>

#include <pb/bootbit.h>

#include <register.h>

LOG_MODULE_REGISTER(pb_bootbit, CONFIG_PB_BOOTBIT_LOG_LEVEL);

/* Use RTC backup register 2 for boot bits */
#define RTC_BKP2R offsetof(RTC_TypeDef, BKP2R)

static const char *bootbit_str(enum pb_bootbit bit)
{
	switch (bit) {
	case PB_BOOTBIT_INITIALIZED:
		return "INITIALIZED";
	case PB_BOOTBIT_NEW_FW_AVAILABLE:
		return "NEW_FW_AVAILABLE";
	case PB_BOOTBIT_NEW_FW_UPDATE_IN_PROGRESS:
		return "NEW_FW_UPDATE_IN_PROGRESS";
	case PB_BOOTBIT_FW_START_FAIL_STRIKE_ONE:
		return "FW_START_FAIL_STRIKE_ONE";
	case PB_BOOTBIT_FW_START_FAIL_STRIKE_TWO:
		return "FW_START_FAIL_STRIKE_TWO";
	case PB_BOOTBIT_RECOVERY_LOAD_FAIL_STRIKE_ONE:
		return "RECOVERY_LOAD_FAIL_STRIKE_ONE";
	case PB_BOOTBIT_RECOVERY_LOAD_FAIL_STRIKE_TWO:
		return "RECOVERY_LOAD_FAIL_STRIKE_TWO";
	case PB_BOOTBIT_RECOVERY_START_IN_PROGRESS:
		return "RECOVERY_START_IN_PROGRESS";
	case PB_BOOTBIT_SOFTWARE_FAILURE_OCCURRED:
		return "SOFTWARE_FAILURE_OCCURRED";
	case PB_BOOTBIT_RESET_LOOP_DETECT_ONE:
		return "RESET_LOOP_DETECT_ONE";
	case PB_BOOTBIT_RESET_LOOP_DETECT_TWO:
		return "RESET_LOOP_DETECT_TWO";
	case PB_BOOTBIT_RESET_LOOP_DETECT_THREE:
		return "RESET_LOOP_DETECT_THREE";
	case PB_BOOTBIT_FW_STABLE:
		return "FW_STABLE";
	case PB_BOOTBIT_NEW_FW_INSTALLED:
		return "NEW_FW_INSTALLED";
	case PB_BOOTBIT_FORCE_PRF:
		return "FORCE_PRF";
	case PB_BOOTBIT_NEW_PRF_AVAILABLE:
		return "NEW_PRF_AVAILABLE";
	default:
		return "UNKNOWN";
	}
}

void pb_bootbit_init(void)
{
	if (!pb_bootbit_tst(PB_BOOTBIT_INITIALIZED)) {
		LOG_DBG("Boot bits not initialized, initializing now");
		sys_write32(BIT(PB_BOOTBIT_INITIALIZED), RTC_BASE + RTC_BKP2R);
	}

	LOG_DBG("Boot bits: 0x%08" PRIx32, sys_read32(RTC_BASE + RTC_BKP2R));
}

void pb_bootbit_set(enum pb_bootbit bit)
{
	sys_bitfield_set_bit(RTC_BASE + RTC_BKP2R, bit);
	LOG_DBG("Set boot bit %d (%s)", bit, bootbit_str(bit));
}

void pb_bootbit_clr(enum pb_bootbit bit)
{
	sys_bitfield_clear_bit(RTC_BASE + RTC_BKP2R, bit);
	LOG_DBG("Cleared boot bit %d (%s)", bit, bootbit_str(bit));
}

bool pb_bootbit_tst(enum pb_bootbit bit)
{
	bool ret;

	ret = sys_bitfield_test_bit(RTC_BASE + RTC_BKP2R, bit);
	LOG_DBG("Tested boot bit %d (%s): %u", bit, bootbit_str(bit), ret);

	return ret;
}
