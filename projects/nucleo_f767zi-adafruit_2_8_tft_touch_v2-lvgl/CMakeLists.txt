# SPDX-License-Identifier: Apache-2.0

set(BOARD nucleo_f767zi)
set(SHIELD adafruit_2_8_tft_touch_v2)

cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(nucleo_f767zi-adafruit_2_8_tft_touch_v2-lvgl)


FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})
