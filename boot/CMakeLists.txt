#-------------------------------------------------------------------------------
# PebbleOS bootloader
#
# Copyright (c) 2025 Core Devices LLC
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(app LANGUAGES C)

target_sources(
  app
  PRIVATE
    src/buttons.c
    src/firmware.c
    src/main.c
    src/panic.c
    src/version.c
    src/watchdog.c
)

if(CONFIG_PB_CHARGER)
  target_sources(app PRIVATE src/charger.c)
endif()

zephyr_linker_sources(SECTIONS version.ld)
