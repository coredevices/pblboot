/*
 * Copyright (c) 2026 Core Devices LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/app_version.h>

#include <pb/version.h>

__attribute__((section(".pbversion"), used))
const struct pb_version pbversion = {
	.magic = PB_VERSION_MAGIC,
	.major = APP_VERSION_MAJOR,
	.minor = APP_VERSION_MINOR,
	.patch = APP_PATCHLEVEL,
	.tweak = APP_TWEAK,
};
