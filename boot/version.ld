/*
 * Copyright (c) 2026 Core Devices LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/devicetree.h>

#define BOOT DT_CHOSEN(zephyr_code_partition)

/* Place bootloader version at the end of the boot partition */
.pbversion (CONFIG_FLASH_BASE_ADDRESS + DT_REG_ADDR(BOOT) + DT_REG_SIZE(BOOT) - 8) :
{
	KEEP(*(.pbversion))
}

ASSERT(__rom_region_end <= ADDR(.pbversion), "ROM content overlaps version region")
