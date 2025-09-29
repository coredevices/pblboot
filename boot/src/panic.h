/*
 * Copyright (c) 2025 Core Devices LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BOOT_SRC_PANIC_H_
#define BOOT_SRC_PANIC_H_

#include <stdint.h>

#include <zephyr/toolchain.h>

/**
 * @name PB_PANIC_REASON Panic reasons
 * @{
 */

/** Init failure */
#define PB_PANIC_REASON_INIT_FAIL          0x00000001UL
/** PRF is unstable */
#define PB_PANIC_REASON_PRF_UNSTABLE       0x00000002UL
/** Reset loop detected */
#define PB_PANIC_REASON_RESET_LOOP         0x00000003UL
/**
 * Firmware load failure
 * @param err Error code (negative errno value)
 */
#define PB_PANIC_REASON_FW_LOAD_FAIL(err)  (0x00010000UL | ((uint16_t)(-(err))))
/**
 * PRF load failure
 * @param err Error code (negative errno value)
 */
#define PB_PANIC_REASON_PRF_LOAD_FAIL(err) (0x00020000UL | ((uint16_t)(-(err))))
/**
 * System fatal error.
 *
 * @param code Fatal error reason code.
 */
#define PB_PANIC_REASON_SYSTEM(code)       (0x8000000UL | ((code) & 0x7FFFFFFUL))

/** @} */

/** Panic reason */
typedef uint32_t pb_panic_reason_t;

/**
 * @brief Initialize the panic handling system
 */
void pb_panic_init(void);

/**
 * @brief Panics the system
 */
void FUNC_NORETURN pb_panic(pb_panic_reason_t reason);

#endif /* BOOT_SRC_PANIC_H_ */
