/*
 * Copyright (c) 2025 Core Devices LLC
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file charger.h
 * @brief Charger handling interface for pblboot.
 */

#ifndef BOOT_SRC_CHARGER_H_
#define BOOT_SRC_CHARGER_H_

#include <stdbool.h>

/**
 * @brief Initialize the charger module.
 *
 * @retval 0 on success
 * @retval -errno negative error code on failure
 */
int pb_charger_init(void);

/**
 * @brief Check if boot is allowed based on battery/plugged in status.
 *
 * @see CONFIG_PB_VBAT_MIN_BOOT_MV
 *
 * @retval true if boot is allowed
 * @retval false if boot is not allowed
 */
bool pb_charger_allow_boot(void);

#endif /* BOOT_SRC_CHARGER_H_ */
