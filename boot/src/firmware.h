/*
 * Copyright (c) 2025 Core Devices LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BOOT_SRC_FIRMWARE_H_
#define BOOT_SRC_FIRMWARE_H_

/**
 * @brief Initialize the firmware module
 *
 * @retval 0 on success
 * @retval -errno negative error code on failure
 */
int pb_firmware_init(void);

/**
 * @brief Load the PRF firmware
 *
 * @retval 0 on success
 * @retval -errno negative error code on failure
 */
int pb_firmware_load_prf(void);

/**
 * @brief Load the normal firmware
 *
 * This function will load the most recent valid firmware from either slot0
 * or slot1. If neither slot contains a valid firmware image, the PRF will be
 * loaded instead.
 *
 * @retval 0 on success
 * @retval -errno negative error code on failure
 */
int pb_firmware_load(void);

#endif /* BOOT_SRC_FIRMWARE_H_ */
