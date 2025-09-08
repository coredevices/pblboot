/*
 * Copyright (c) 2025 Core Devices LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PB_FWJUMP_H
#define PB_FWJUMP_H

#include <stdint.h>

#include <zephyr/toolchain.h>

/**
 * @brief Jump to the specified address
 *
 * @param addr Address to jump to.
 */
void FUNC_NORETURN pb_fwjump(uintptr_t addr);

#endif /* PB_FWJUMP_H */
