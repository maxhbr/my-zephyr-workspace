# SPDX-License-Identifier: Apache-2.0

# Re-direct the directory where the 'boards' directory is found from
# $ZEPHYR_BASE to this directory.
set(BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR})


set(BOARD adafruit_feather_stm32f405)
# set(SHIELD ssd1306_128x64)

cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(feather_stm32f405_lvgl)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})
