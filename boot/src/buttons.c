/*
 * Copyright (c) 2025 Core Devices LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#include "buttons.h"

#include <errno.h>

#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(pblboot, CONFIG_PBLBOOT_LOG_LEVEL);

static const struct gpio_dt_spec btn_back = GPIO_DT_SPEC_GET(DT_CHOSEN(pb_btn_back), gpios);
static const struct gpio_dt_spec btn_up = GPIO_DT_SPEC_GET(DT_CHOSEN(pb_btn_up), gpios);
static const struct gpio_dt_spec btn_center = GPIO_DT_SPEC_GET(DT_CHOSEN(pb_btn_center), gpios);
static const struct gpio_dt_spec btn_down = GPIO_DT_SPEC_GET(DT_CHOSEN(pb_btn_down), gpios);

static inline bool buttons_prf_pressed(void)
{
	return (gpio_pin_get_dt(&btn_back) == 1) && (gpio_pin_get_dt(&btn_up) == 1) &&
	       (gpio_pin_get_dt(&btn_center) == 1) && (gpio_pin_get_dt(&btn_down) == 0);
}

int pb_buttons_init(void)
{
	int ret;

	if (!gpio_is_ready_dt(&btn_back) || !gpio_is_ready_dt(&btn_up) ||
	    !gpio_is_ready_dt(&btn_center) || !gpio_is_ready_dt(&btn_down)) {
		LOG_ERR("Button GPIO device/s not ready");
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(&btn_back, GPIO_INPUT);
	if (ret < 0) {
		LOG_ERR("Failed to configure back button (err %d)", ret);
		return ret;
	}

	ret = gpio_pin_configure_dt(&btn_up, GPIO_INPUT);
	if (ret < 0) {
		LOG_ERR("Failed to configure up button (err %d)", ret);
		return ret;
	}

	ret = gpio_pin_configure_dt(&btn_center, GPIO_INPUT);
	if (ret < 0) {
		LOG_ERR("Failed to configure center button (err %d)", ret);
		return ret;
	}

	ret = gpio_pin_configure_dt(&btn_down, GPIO_INPUT);
	if (ret < 0) {
		LOG_ERR("Failed to configure down button (err %d)", ret);
		return ret;
	}

	return 0;
}

bool pb_buttons_prf_requested(void)
{
	if (!buttons_prf_pressed()) {
		return false;
	}

	LOG_INF("PRF button combo detected, waiting %d ms to confirm",
		CONFIG_PB_PRF_BUTTON_COMBO_TIME_MS);

	for (unsigned int i = 0U; i < CONFIG_PB_PRF_BUTTON_COMBO_TIME_MS; i++) {
		k_msleep(1);
		if (!buttons_prf_pressed()) {
			LOG_INF("Button combo released");
			return false;
		}
	}

	return true;
}

bool pb_buttons_any_pressed(void)
{
	return (gpio_pin_get_dt(&btn_back) == 1) || (gpio_pin_get_dt(&btn_up) == 1) ||
	       (gpio_pin_get_dt(&btn_center) == 1) || (gpio_pin_get_dt(&btn_down) == 1);
}
