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

/* size of stack area used by each thread */
#define STACKSIZE 8192
/* scheduling priority used by each thread */
#define PRIORITY 7
/* delay between updates (in ms) */
#define SLEEPTIME 500

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

void main(void) {
  Display display = get_display();
  Stepper stepper = get_stepper();

  View view(&display, &stepper);

  while (true) {
    display.update();
    k_sleep(K_MSEC(100));
  }
}
