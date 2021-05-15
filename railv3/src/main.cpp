/*
 * Copyright (c) 2018 Google LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr.h>
#include <zephyr/types.h>

#include <device.h>
#include <init.h>
#include <kernel.h>
#include <sys/byteorder.h>
#include <sys/crc.h>
#include <sys/printk.h>

#include <console/console.h>

#include <lvgl.h>

#include <drivers/display.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(rail);

#include "StepperWithTarget.h"

// ############################################################################
// initialize Stepper

StepperWithTarget stepper;

void stepper_work_handler(struct k_work *work) { stepper.step_towards_target(); }
K_WORK_DEFINE(stepper_work, stepper_work_handler);
void stepper_expiry_function(struct k_timer *timer_id) { k_work_submit(&stepper_work); }
K_TIMER_DEFINE(stepper_timer, stepper_expiry_function, NULL);
void start_stepper() {
  stepper.start();
  k_timer_start(&stepper_timer, K_USEC(20), K_USEC(20));
}

// ############################################################################
// Main

void main(void) {
  start_stepper();

  LOG_INF("stepper = %p", &stepper);
  int target = stepper.go_relative(12800);
  LOG_INF("new_target = %i", target);

  while (true) {
    lv_task_handler();
    k_sleep(K_MSEC(100));
  }
}
