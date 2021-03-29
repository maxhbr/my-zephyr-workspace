# SPDX-License-Identifier: Apache-2.0

set(BOARD nucleo_l432kc)

cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(lvgl)


FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})
