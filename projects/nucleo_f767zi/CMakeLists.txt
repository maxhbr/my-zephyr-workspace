# SPDX-License-Identifier: Apache-2.0

# Re-direct the directory where the 'boards' directory is found from
# $ZEPHYR_BASE to this directory.
set(BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR})


set(BOARD my_f767zi)
set(SHIELD adafruit_2_8_tft_touch_v2)

cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(my_f767zi)


FILE(GLOB app_sources src/*.cpp)
target_sources(app PRIVATE ${app_sources})
