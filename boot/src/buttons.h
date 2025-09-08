/*
 * Copyright (c) 2025 Core Devices LLC
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file buttons.h
 * @brief Button handling interface for PB bootloader
 *
 * This module provides functions for initializing and reading button states
 * in the PB bootloader, including detection of special button combinations
 * for entering recovery mode.
 */

#ifndef BOOT_SRC_BUTTONS_H_
#define BOOT_SRC_BUTTONS_H_

#include <stdbool.h>

/**
 * @brief Initialize the buttons module
 *
 * @retval 0 on success
 * @retval -errno negative error code on failure
 */
int pb_buttons_init(void);

/**
 * @brief Check if PRF  was requested
 *
 * @retval true if PRF mode was requested
 * @retval false if PRF mode was not requested
 */
bool pb_buttons_prf_requested(void);

/**
 * @brief Check if any button is currently pressed
 *
 * @retval true if any button is pressed
 * @retval false if no buttons are pressed
 */
bool pb_buttons_any_pressed(void);

#endif /* BOOT_SRC_BUTTONS_H_ */
