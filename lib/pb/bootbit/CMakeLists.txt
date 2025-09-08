# Copyright (c) 2025 Core Devices LLC
# SPDX-License-Identifier: Apache-2.0

zephyr_library()
zephyr_library_sources_ifdef(CONFIG_PB_BOOTBIT_SF32LB bootbit_sf32lb.c)
