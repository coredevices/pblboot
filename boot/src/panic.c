/*
 * Copyright (c) 2025 Core Devices LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#include "buttons.h"
#include "panic.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/reboot.h>

LOG_MODULE_DECLARE(pblboot, CONFIG_PBLBOOT_LOG_LEVEL);

/* override Zephyr's default fatal error handler */
void k_sys_fatal_error_handler(unsigned int reason, const struct arch_esf *esf)
{
	ARG_UNUSED(reason);
	ARG_UNUSED(esf);

	pb_panic();
}

void FUNC_NORETURN pb_panic(void)
{
	LOG_ERR("System panic, press any button to reset");

	while (1) {
		if (pb_buttons_any_pressed()) {
			LOG_INF("Resetting system due to button press");
			sys_reboot(SYS_REBOOT_COLD);
		}

		k_msleep(10);
	}
}
