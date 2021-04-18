# SPDX-License-Identifier: Apache-2.0

set(BOARD nrf52840dongle_nrf52840)

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(console)

FILE(GLOB app_sources src/*.cpp)
target_sources(app PRIVATE ${app_sources})
