/*
 * Copyright (c) 2025 Core Devices LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#include "watchdog.h"

#include <errno.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/watchdog.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(pblboot, CONFIG_PBLBOOT_LOG_LEVEL);

static const struct device *wdt = DEVICE_DT_GET(DT_CHOSEN(pb_wdt));

int pb_watchdog_init(void)
{
	int ret;
	struct wdt_timeout_cfg wdt_cfg = {
		.window.min = 0,
		.window.max = CONFIG_PB_WATCHDOG_TIMEOUT_MS,
		.callback = NULL,
		.flags = WDT_FLAG_RESET_SOC,
	};

	if (!device_is_ready(wdt)) {
		LOG_ERR("Watchdog device not ready");
		return -ENODEV;
	}

	ret = wdt_disable(wdt);
	if (ret < 0) {
		LOG_ERR("Failed to disable WDT (%d)", ret);
		return ret;
	}

	ret = wdt_install_timeout(wdt, &wdt_cfg);
	if (ret < 0) {
		LOG_ERR("Failed to install WDT timeout (%d)", ret);
		return ret;
	}

	ret = wdt_setup(wdt, 0U);
	if (ret < 0) {
		LOG_ERR("Failed to setup WDT (%d)", ret);
		return ret;
	}

	return 0;
}

int pb_watchdog_feed(void)
{
	int ret;

	ret = wdt_feed(wdt, 0U);
	if (ret < 0) {
		LOG_ERR("Failed to feed WDT (%d)", ret);
		return ret;
	}

	return 0;
}
