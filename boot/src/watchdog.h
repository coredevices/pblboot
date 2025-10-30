/*
 * Copyright (c) 2025 Core Devices LLC
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file watchdog.h
 * @brief Watchdog handling interface for pblboot.
 */

#ifndef BOOT_SRC_WATCHDOG_H_
#define BOOT_SRC_WATCHDOG_H_

/**
 * @brief Initialize the watchdog module
 *
 * This function initializes the watchdog, so periodic feeding is required
 * to prevent system reset after calling this function.
 *
 * @retval 0 on success
 * @retval -errno negative error code on failure
 */
int pb_watchdog_init(void);

/**
 * @brief Feed the watchdog.
 *
 * @retval 0 on success
 * @retval -errno negative error code on failure
 */
int pb_watchdog_feed(void);

#endif /* BOOT_SRC_WATCHDOG_H_ */
