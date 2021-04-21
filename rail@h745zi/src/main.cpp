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
// #include <net/buf.h>
#include <sys/byteorder.h>
#include <sys/crc.h>
#include <sys/printk.h>

#include <console/console.h>

#include <lvgl.h>

#include <drivers/display.h>
#include <drivers/gpio.h>
#include <drivers/sensor.h>
// #include <drivers/led_strip.h>
// #include <drivers/spi.h>
// #include <drivers/uart.h>
// #include <usb/usb_device.h>
// #include <usb/class/usb_hid.h>
// #include <drivers/watchdog.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(myapp);

#include "GyroWaiter.h"
#include "Rail.h"

/* size of stack area used by each thread */
#define STACKSIZE 8192
/* scheduling priority used by each thread */
#define PRIORITY 7
/* delay between updates (in ms) */
#define SLEEPTIME 500

K_THREAD_STACK_DEFINE(threadRail_stack_area, STACKSIZE);
static struct k_thread threadRail_data;
void threadRail(void *railV, void *dummy2, void *dummy3) {
  ARG_UNUSED(dummy2);
  ARG_UNUSED(dummy3);
  Rail *rail = static_cast<Rail *>(railV);
  rail->loop();
}

#define CONSOLE_HELP                                                           \
  "help\n"                                                                     \
  "----\n"                                                                     \
  "   s -> do a single shot\n"                                                 \
  "   $int -> move relative\n"
K_THREAD_STACK_DEFINE(threadConsole_stack_area, STACKSIZE);
static struct k_thread threadConsole_data;
void threadConsole(void *railV, void *waiterV, void *dummy3) {
  ARG_UNUSED(dummy3);
  Rail *rail = static_cast<Rail *>(railV);
  GyroWaiter *waiter = static_cast<GyroWaiter *>(waiterV);
  int rc = 0;

  printk(CONSOLE_HELP);
  while (1) {
    char *s = console_getline();
    printk("<-- %s\n", s);

    if (s[0] == 's') {
      rc = waiter->wait();
      if (rc != 0) {
        printk("!!! returned rc=%i", rc);
      }
    } else {
      int x = atoi(s);
      rail->go(x);
    }

    lv_task_handler();
    k_sleep(K_MSEC(10));
  }
}

void display_init() {
  const struct device *display_dev;
  display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);

  if (display_dev == NULL) {
    LOG_ERR("device not found.  Aborting test.");
    return;
  }
  display_blanking_off(display_dev);
}

K_SEM_DEFINE(threadRail_sem, 1, 1);
void main(void) {

  display_init();
  console_getline_init();
  GyroWaiter waiter(&threadRail_sem);
  Rail rail(&threadRail_sem);

  k_tid_t my_tid_console = k_thread_create(
      &threadConsole_data, threadConsole_stack_area,
      K_THREAD_STACK_SIZEOF(threadConsole_stack_area), threadConsole, &rail,
      &waiter, NULL, PRIORITY, 0, K_NO_WAIT);
  k_thread_name_set(&threadConsole_data, "thread_r");

  k_tid_t my_tid_rail =
      k_thread_create(&threadRail_data, threadRail_stack_area,
                      K_THREAD_STACK_SIZEOF(threadRail_stack_area), threadRail,
                      &rail, NULL, NULL, PRIORITY, 0, K_NO_WAIT);
  k_thread_name_set(&threadRail_data, "thread_r");

  k_thread_start(&threadConsole_data);
  k_thread_start(&threadRail_data);
}
