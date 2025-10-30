/*
 * Copyright (c) 2025 Core Devices LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#include "buttons.h"
#include "panic.h"
#include "watchdog.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/toolchain.h>

LOG_MODULE_DECLARE(pblboot, CONFIG_PBLBOOT_LOG_LEVEL);

static bool initialized = false;

/* override Zephyr's default fatal error handler */
FUNC_NORETURN void arch_system_halt(unsigned int reason);

void k_sys_fatal_error_handler(unsigned int reason, const struct arch_esf *esf)
{
	ARG_UNUSED(esf);

	if (!initialized) {
		LOG_PANIC();
		arch_system_halt(reason);
	} else {
		pb_panic(PB_PANIC_REASON_SYSTEM(reason));
	}
}

void pb_panic_init(void)
{
	initialized = true;
}

void FUNC_NORETURN pb_panic(pb_panic_reason_t reason)
{
	LOG_ERR("System panic (0x%08x), press any button to reset", reason);

	while (1) {
		if (pb_buttons_any_pressed()) {
			LOG_INF("Resetting system due to button press");
			sys_reboot(SYS_REBOOT_COLD);
		}

		(void)pb_watchdog_feed();

		k_msleep(10);
	}
}
