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

#include "Display.h"
#include "Stepper.h"
#include "View.h"

Display get_display() {
  const struct device *display_dev =
      device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);
  __ASSERT(display_dev != NULL, "display device not found.");
  Display display(display_dev);
  return display;
}

Stepper get_stepper() {
  Stepper stepper;
  return stepper;
}

/* size of stack area used by each thread */
#define STACKSIZE 8192
/* scheduling priority used by each thread */
#define PRIORITY 7

// K_THREAD_STACK_DEFINE(thread_controller_stack_area, STACKSIZE);
// static struct k_thread thread_controller_data;
// void thread_controller(void *_controller, void *dummy2, void *dummy3) {
//   ARG_UNUSED(dummy2);
//   ARG_UNUSED(dummy3);
//   Controller *controller = static_cast<Controller *>(_controller);
//   controller->iterate();
// }

// void start_controller_thread(Controller *controller) {
//   k_tid_t my_tid_controller = k_thread_create(
//       &thread_controller_data, thread_controller_stack_area,
//       K_THREAD_STACK_SIZEOF(thread_controller_stack_area), thread_controller,
//       controller, NULL, NULL, PRIORITY, 0, K_NO_WAIT);
//   k_thread_name_set(&thread_controller_data, "thread_controller");
//   k_thread_start(&thread_controller_data);
// }

void main(void) {
  Display display = get_display();
  Stepper stepper = get_stepper();

  Model model(&stepper);
  Controller controller(model);
  View view(model, controller, &display);
  int i=0;
  while (true) {
    controller.work();
    if ((++i) % 10 == 0) {
      lv_task_handler();
    }
    // k_sleep(K_MSEC(10));
  }
}
