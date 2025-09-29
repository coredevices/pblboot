/*
 * Copyright (c) 2025 Core Devices LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PB_BOOTBIT_H
#define PB_BOOTBIT_H

#include <stdbool.h>

#include <zephyr/sys/util.h>

/** Maximum firmware failure count value */
#define PB_BOOTBIT_FW_FAIL_CNT_MAX    3U
/** Maximum PRF failure count value */
#define PB_BOOTBIT_PRF_FAIL_CNT_MAX   3U
/** Maximum reset loop count value */
#define PB_BOOTBIT_RESET_LOOP_CNT_MAX 7U

/**
 * @brief Boot bit flags for bootloader state tracking
 *
 * These flags are used to track various states and conditions
 * during the boot process, firmware updates, and recovery operations.
 */
enum pb_bootbit {
	/** Bootbits have been initialized */
	PB_BOOTBIT_INITIALIZED = BIT(0),
	/** New firmware is available for installation */
	PB_BOOTBIT_NEW_FW_AVAILABLE = BIT(1),
	/** Firmware update is currently in progress */
	PB_BOOTBIT_NEW_FW_UPDATE_IN_PROGRESS = BIT(2),
	/** Firmware start failure counter (bit 0) */
	PB_BOOTBIT_FW_START_FAIL_STRIKE_ONE = BIT(3),
	/** Firmware start failure counter (bit 1) */
	PB_BOOTBIT_FW_START_FAIL_STRIKE_TWO = BIT(4),
	/** PRF firmware start failure counter (bit 0) */
	PB_BOOTBIT_RECOVERY_LOAD_FAIL_STRIKE_ONE = BIT(5),
	/** PRF firmware start failure counter (bit 1) */
	PB_BOOTBIT_RECOVERY_LOAD_FAIL_STRIKE_TWO = BIT(6),
	/** Recovery start is in progress */
	PB_BOOTBIT_RECOVERY_START_IN_PROGRESS = BIT(7),
	/** A software failure has occurred */
	PB_BOOTBIT_SOFTWARE_FAILURE_OCCURRED = BIT(9),
	/** Reset loop detection counter (bit 0) */
	PB_BOOTBIT_RESET_LOOP_DETECT_ONE = BIT(11),
	/** Reset loop detection counter (bit 2) */
	PB_BOOTBIT_RESET_LOOP_DETECT_TWO = BIT(12),
	/** Reset loop detection counter (bit 3) */
	PB_BOOTBIT_RESET_LOOP_DETECT_THREE = BIT(13),
	/** Firmware is stable and running correctly */
	PB_BOOTBIT_FW_STABLE = BIT(14),
	/** New firmware has been successfully installed */
	PB_BOOTBIT_NEW_FW_INSTALLED = BIT(15),
	/** Force entry into PRF */
	PB_BOOTBIT_FORCE_PRF = BIT(17),
	/** New PRF is available for installation */
	PB_BOOTBIT_NEW_PRF_AVAILABLE = BIT(18),
};

/**
 * @brief Initialize the boot bits system.
 */
void pb_bootbit_init(void);

/**
 * @brief Set the specified boot bit.
 *
 * @param bit Boot bit.
 */
void pb_bootbit_set(enum pb_bootbit bit);

/**
 * @brief Clear the specified boot bit.
 *
 * @param bit Boot bit.
 */
void pb_bootbit_clr(enum pb_bootbit bit);

/**
 * @brief Test if the specified boot bit is set.
 *
 * @param bit Boot bit.
 *
 * @retval true if the boot bit is set.
 * @retval false If the boot bit is not set.
 */
bool pb_bootbit_tst(enum pb_bootbit bit);

/**
 * @brief Get the firmware failure counter value
 *
 * @return Firmware failure counter value (0-3)
 */
static inline uint8_t pb_bootbit_fw_fail_cnt_get(void)
{
	uint8_t cnt;

	cnt = (!!pb_bootbit_tst(PB_BOOTBIT_FW_START_FAIL_STRIKE_ONE) << 0U) +
	      (!!pb_bootbit_tst(PB_BOOTBIT_FW_START_FAIL_STRIKE_TWO) << 1U);

	return cnt;
}

/**
 * @brief Set the firmware failure counter value
 *
 * @param cnt Firmware failure counter value (0-3)
 */
static inline void pb_bootbit_fw_fail_cnt_set(uint8_t cnt)
{
	if (cnt & BIT(0U)) {
		pb_bootbit_set(PB_BOOTBIT_FW_START_FAIL_STRIKE_ONE);
	} else {
		pb_bootbit_clr(PB_BOOTBIT_FW_START_FAIL_STRIKE_ONE);
	}

	if (cnt & BIT(1U)) {
		pb_bootbit_set(PB_BOOTBIT_FW_START_FAIL_STRIKE_TWO);
	} else {
		pb_bootbit_clr(PB_BOOTBIT_FW_START_FAIL_STRIKE_TWO);
	}
}

/**
 * @brief Get the PRF failure counter value
 *
 * @return Firmware failure counter value (0-3)
 */
static inline uint8_t pb_bootbit_prf_fail_cnt_get(void)
{
	uint8_t cnt;

	cnt = (!!pb_bootbit_tst(PB_BOOTBIT_RECOVERY_LOAD_FAIL_STRIKE_ONE) << 0U) +
	      (!!pb_bootbit_tst(PB_BOOTBIT_RECOVERY_LOAD_FAIL_STRIKE_TWO) << 1U);

	return cnt;
}

/**
 * @brief Set the PRF failure counter value
 *
 * @param cnt PRF failure counter value (0-3)
 */
static inline void pb_bootbit_prf_fail_cnt_set(uint8_t cnt)
{
	if (cnt & BIT(0U)) {
		pb_bootbit_set(PB_BOOTBIT_RECOVERY_LOAD_FAIL_STRIKE_ONE);
	} else {
		pb_bootbit_clr(PB_BOOTBIT_RECOVERY_LOAD_FAIL_STRIKE_ONE);
	}

	if (cnt & BIT(1U)) {
		pb_bootbit_set(PB_BOOTBIT_RECOVERY_LOAD_FAIL_STRIKE_TWO);
	} else {
		pb_bootbit_clr(PB_BOOTBIT_RECOVERY_LOAD_FAIL_STRIKE_TWO);
	}
}

/**
 * @brief Check if PRF is starting and clear the flag
 *
 * @retval true if PRF was starting
 * @retval false if PRF was not starting
 */
static inline bool pb_bootbit_prf_starting_tst_and_clr(void)
{
	bool ret;

	ret = pb_bootbit_tst(PB_BOOTBIT_RECOVERY_START_IN_PROGRESS);
	if (ret) {
		pb_bootbit_clr(PB_BOOTBIT_RECOVERY_START_IN_PROGRESS);
	}

	return ret;
}

/**
 * @brief Mark that PRF is starting
 */
static inline void pb_bootbit_prf_starting_set(void)
{
	pb_bootbit_set(PB_BOOTBIT_RECOVERY_START_IN_PROGRESS);
}

/**
 * @brief Check for firmware failure and clear the flag
 *
 * @retval true if a firmware failure was detected
 * @retval false if no firmware failure was detected
 */
static inline bool pb_bootbit_fw_fail_tst_and_clr(void)
{
	bool ret;

	ret = pb_bootbit_tst(PB_BOOTBIT_SOFTWARE_FAILURE_OCCURRED);
	if (ret) {
		pb_bootbit_clr(PB_BOOTBIT_SOFTWARE_FAILURE_OCCURRED);
	}

	return ret;
}

/**
 * @brief Get the reset loop counter value
 *
 * @return Reset loop counter value (0-7)
 */
static inline uint8_t pb_bootbit_reset_loop_cnt_get(void)
{
	uint8_t cnt;

	cnt = (!!pb_bootbit_tst(PB_BOOTBIT_RESET_LOOP_DETECT_ONE) << 0U) +
	      (!!pb_bootbit_tst(PB_BOOTBIT_RESET_LOOP_DETECT_TWO) << 1U) +
	      (!!pb_bootbit_tst(PB_BOOTBIT_RESET_LOOP_DETECT_THREE) << 2U);

	return cnt;
}

/**
 * @brief Set the reset loop counter value
 *
 * @param cnt Reset loop counter value (0-7)
 */
static inline void pb_bootbit_reset_loop_cnt_set(uint8_t cnt)
{
	if (cnt & BIT(0U)) {
		pb_bootbit_set(PB_BOOTBIT_RESET_LOOP_DETECT_ONE);
	} else {
		pb_bootbit_clr(PB_BOOTBIT_RESET_LOOP_DETECT_ONE);
	}
	if (cnt & BIT(1U)) {
		pb_bootbit_set(PB_BOOTBIT_RESET_LOOP_DETECT_TWO);
	} else {
		pb_bootbit_clr(PB_BOOTBIT_RESET_LOOP_DETECT_TWO);
	}
	if (cnt & BIT(2U)) {
		pb_bootbit_set(PB_BOOTBIT_RESET_LOOP_DETECT_THREE);
	} else {
		pb_bootbit_clr(PB_BOOTBIT_RESET_LOOP_DETECT_THREE);
	}
}

/**
 * @brief Check if firmware is stable and clear the flag
 *
 * @retval true if firmware was marked as stable
 * @retval false if firmware was not marked as stable
 */
static inline bool pb_bootbit_fw_stable_tst_and_clr(void)
{
	bool ret;

	ret = pb_bootbit_tst(PB_BOOTBIT_FW_STABLE);
	if (ret) {
		pb_bootbit_clr(PB_BOOTBIT_FW_STABLE);
	}

	return ret;
}

/**
 * @brief Check if PRF is forced and clear the flag
 *
 * @retval true if PRF boot was forced
 * @retval false if PRF boot was not forced
 */
static inline bool pb_bootbit_force_prf_tst_and_clr(void)
{
	bool ret;

	ret = pb_bootbit_tst(PB_BOOTBIT_FORCE_PRF);
	if (ret) {
		pb_bootbit_clr(PB_BOOTBIT_FORCE_PRF);
	}

	return ret;
}

#endif /* PB_BOOTBIT_H */
