# Copyright (c) 2025 Core Devices LLC
# SPDX-License-Identifier: Apache-2.0

zephyr_library_amend()
zephyr_library_sources_ifdef(CONFIG_PULSE_UART_CONSOLE pulse_uart_console.c)
