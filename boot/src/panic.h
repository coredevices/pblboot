/*
 * Copyright (c) 2025 Core Devices LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BOOT_SRC_PANIC_H_
#define BOOT_SRC_PANIC_H_

#include <zephyr/toolchain.h>

/**
 * @brief Panics the system
 */
void FUNC_NORETURN pb_panic(void);

#endif /* BOOT_SRC_PANIC_H_ */
