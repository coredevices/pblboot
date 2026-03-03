/*
 * Copyright (c) 2026 Core Devices LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PB_VERSION_H
#define PB_VERSION_H

#include <stdint.h>

#include <zephyr/toolchain.h>

/** Magic value for bootloader version structure */
#define PB_VERSION_MAGIC 0x50425652UL

/**
 * @brief Bootloader version structure
 *
 * This structure is placed at a fixed location in flash so that
 * the firmware can read the bootloader version at runtime.
 */
struct pb_version {
	/** Magic value to identify this structure (0x50425652 = "PBVR") */
	uint32_t magic;
	/** Major version number */
	uint8_t major;
	/** Minor version number */
	uint8_t minor;
	/** Patch level */
	uint8_t patch;
	/** Tweak version */
	uint8_t tweak;
} __packed;

BUILD_ASSERT(sizeof(struct pb_version) == 8, "pb_version struct size must be 8 bytes");

#endif /* PB_VERSION_H */
