# SPDX-License-Identifier: Apache-2.0

# Re-direct the directory where the 'boards' directory is found from
# $ZEPHYR_BASE to this directory.
# set(BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR})


set(BOARD nucleo_f767zi) # nucleo_h745zi_q_m7)
set(SHIELD adafruit_2_8_tft_touch_v2)
# set(SHIELD "adafruit_2_8_tft_touch_v2 esp_8266_arduino")

set(CMAKE_C_FLAGS -lstdc++)
set(CMAKE_CXX_FLAGS -lstdc++)

cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(rail@h745zi)

zephyr_library_compile_options(-lstdc++ -std=c++17 -Wall -Wextra -Wno-unused-variable -Wno-comment)


FILE(GLOB app_sources src/*.cpp)
target_sources(app PRIVATE ${app_sources})
