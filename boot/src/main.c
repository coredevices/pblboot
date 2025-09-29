/*
 * Copyright (c) 2025 Core Devices LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#include "buttons.h"
#include "firmware.h"
#include "panic.h"

#include <inttypes.h>

#include <zephyr/logging/log.h>

#include <pb/bootbit.h>

#include <app_version.h>

LOG_MODULE_REGISTER(pblboot, CONFIG_PBLBOOT_LOG_LEVEL);

int main(void)
{
	uint8_t rst_loop_cnt;
	bool prf_requested = false;
	int ret;

	LOG_INF("PebbleOS bootloader %s", APP_VERSION_STRING);

	ret = pb_buttons_init();
	if (ret < 0) {
		LOG_ERR("Failed to initialize buttons module (err %d)", ret);
		return 0;
	}

	ret = pb_firmware_init();
	if (ret < 0) {
		LOG_ERR("Failed to initialize firmware module (err %d)", ret);
		pb_panic(PB_PANIC_REASON_INIT_FAIL);
	}

	/* firmware/PRF start failures */
	if (pb_bootbit_fw_stable_tst_and_clr()) {
		LOG_INF("Last firmware or PRF boot was stable; clear strikes");

		pb_bootbit_fw_fail_cnt_set(0U);
		pb_bootbit_prf_fail_cnt_set(0U);
	} else if (pb_bootbit_fw_fail_tst_and_clr()) {
		uint8_t cnt;

		if (pb_bootbit_prf_starting_tst_and_clr()) {
			cnt = pb_bootbit_prf_fail_cnt_get();
			LOG_ERR("PRF failure caused a reset (strikes: %" PRIu8 "/%" PRIu8 ")", cnt,
				PB_BOOTBIT_PRF_FAIL_CNT_MAX);

			if (cnt == PB_BOOTBIT_PRF_FAIL_CNT_MAX) {
				pb_bootbit_prf_fail_cnt_set(0U);
				pb_panic(PB_PANIC_REASON_PRF_UNSTABLE);
			} else {
				cnt++;
				pb_bootbit_prf_fail_cnt_set(cnt);
				prf_requested = true;
			}
		} else {
			cnt = pb_bootbit_fw_fail_cnt_get();
			LOG_ERR("Firmware failure caused a reset (strikes: %" PRIu8 "/%" PRIu8 ")",
				cnt, PB_BOOTBIT_FW_FAIL_CNT_MAX);

			if (cnt == PB_BOOTBIT_FW_FAIL_CNT_MAX) {
				pb_bootbit_fw_fail_cnt_set(0U);
				prf_requested = true;
			} else {
				cnt++;
				pb_bootbit_fw_fail_cnt_set(cnt);
			}
		}
	}

	/* prf manual requests */
	if (!prf_requested) {
		if (pb_bootbit_force_prf_tst_and_clr()) {
			LOG_INF("Forced PRF load requested");
			prf_requested = true;
		} else if (pb_buttons_prf_requested()) {
			prf_requested = true;
		}
	}

	if (prf_requested) {
		ret = pb_firmware_load_prf();
		if (ret < 0) {
			LOG_ERR("Failed to load PRF (err %d)", ret);
			pb_panic(PB_PANIC_REASON_PRF_LOAD_FAIL(ret));
		}
	}

	/* reset loop counter handling */
	rst_loop_cnt = pb_bootbit_reset_loop_cnt_get();
	if (rst_loop_cnt == PB_BOOTBIT_RESET_LOOP_CNT_MAX) {
		LOG_ERR("Reset loop detected");
		pb_bootbit_reset_loop_cnt_set(0U);
		pb_panic(PB_PANIC_REASON_RESET_LOOP);
	} else {
		rst_loop_cnt++;
		pb_bootbit_reset_loop_cnt_set(rst_loop_cnt);
	}

	/* load firmware */
	ret = pb_firmware_load();
	if (ret < 0) {
		LOG_ERR("Failed to load firmware (err %d)", ret);
		pb_panic(PB_PANIC_REASON_FW_LOAD_FAIL(ret));
	}

	return 0;
}
