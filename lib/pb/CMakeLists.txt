# Copyright (c) 2025 Core Devices LLC
# SPDX-License-Identifier: Apache-2.0

add_subdirectory_ifdef(CONFIG_PB_BOOTBIT bootbit)
add_subdirectory_ifdef(CONFIG_PB_FWJUMP fwjump)
