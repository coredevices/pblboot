/*
 * Copyright (c) 2025 Core Devices LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#include "charger.h"

#include <inttypes.h>
#include <errno.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/sensor/npm13xx_charger.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(pblboot, CONFIG_PBLBOOT_LOG_LEVEL);

static const struct device *charger = DEVICE_DT_GET(DT_CHOSEN(pb_charger));

int pb_charger_init(void)
{
	if (!device_is_ready(charger)) {
		LOG_ERR("Charger device not ready");
		return -ENODEV;
	}

	return 0;
}

bool pb_charger_allow_boot(void)
{
	struct sensor_value val;
	int ret;

	ret = sensor_sample_fetch(charger);
	if (ret < 0) {
		LOG_ERR("Failed to fetch charger sensor data (%d)", ret);
		/* fail safe: allow boot */
		return true;
	}

	ret = sensor_channel_get(charger, SENSOR_CHAN_GAUGE_VOLTAGE, &val);
	if (ret < 0) {
		LOG_ERR("Failed to get battery voltage (%d)", ret);
		/* fail safe: allow boot */
		return true;
	}

	int32_t vbat_mv = val.val1 * 1000 + val.val2 / 1000;
	if (vbat_mv >= CONFIG_PB_VBAT_MIN_BOOT_MV) {
		return true;
	} else {
		LOG_WRN("Battery voltage low: %" PRId32 " mV", vbat_mv);

#ifdef CONFIG_NPM13XX_CHARGER
		/* check if VBUS is connected, allow boot if so */
		ret = sensor_attr_get(
			charger, (enum sensor_channel)SENSOR_CHAN_NPM13XX_CHARGER_VBUS_STATUS,
			(enum sensor_attribute)SENSOR_ATTR_NPM13XX_CHARGER_VBUS_PRESENT, &val);
		if (ret < 0) {
			LOG_ERR("Failed to get VBUS status (%d)", ret);
			/* fail safe: allow boot */
			return true;
		}

		if (val.val1 != 0) {
			return true;
		}

		return false;
#else
		LOG_WRN("VBUS/charge status unavailable");
		return true;
#endif
	}
}
