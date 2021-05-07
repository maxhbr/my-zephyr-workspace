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
LOG_MODULE_REGISTER(myapp);

#include "Display.h"
#include "GyroWaiter.h"
#include "Stepper.h"

/* size of stack area used by each thread */
#define STACKSIZE 8192
/* scheduling priority used by each thread */
#define PRIORITY 7
/* delay between updates (in ms) */
#define SLEEPTIME 500

K_THREAD_STACK_DEFINE(threadStepper_stack_area, STACKSIZE);
static struct k_thread threadStepper_data;
void threadStepper(void *stepperV, void *dummy2, void *dummy3) {
  ARG_UNUSED(dummy2);
  ARG_UNUSED(dummy3);
  Stepper *stepper = static_cast<Stepper *>(stepperV);
  stepper->loop();
}

#define CONSOLE_HELP                                                           \
  "   s -> do a single shot\n"                                                 \
  "   $int -> move relative\n"
K_THREAD_STACK_DEFINE(threadConsole_stack_area, STACKSIZE);
static struct k_thread threadConsole_data;
void threadConsole(void *stepperV, void *waiterV, void *dummy3) {
  ARG_UNUSED(dummy3);
  Stepper *stepper = static_cast<Stepper *>(stepperV);
  GyroWaiter *waiter = static_cast<GyroWaiter *>(waiterV);
  int rc = 0;

  k_sleep(K_MSEC(2000));
  printk(CONSOLE_HELP);
  while (1) {
    printk("$ ");
    char *s = console_getline();
    printk("<-- %s\n", s);

    if (s[0] == 's') {
      rc = waiter->wait();
      if (rc != 0) {
        printk("!!! returned rc=%i", rc);
      }
    } else {
      int x = atoi(s);
      stepper->go(x);
      // stepper->wait();
    }

    lv_task_handler();
    k_sleep(K_MSEC(10));
  }
}

void init_console(Stepper *stepper, GyroWaiter *waiter) {
  console_getline_init();

  k_tid_t my_tid_console = k_thread_create(
      &threadConsole_data, threadConsole_stack_area,
      K_THREAD_STACK_SIZEOF(threadConsole_stack_area), threadConsole, stepper,
      waiter, NULL, PRIORITY, 0, K_NO_WAIT);
  k_thread_name_set(&threadConsole_data, "thread_c");
}

K_SEM_DEFINE(threadStepper_sem, 1, 1);
void main(void) {
  const struct device *display_dev =
      device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);
  if (display_dev == NULL) {
    LOG_ERR("display device not found.");
    return;
  }
  Display display(display_dev);
  Stepper stepper(&threadStepper_sem, &display);

  const char *const mpu_label = DT_LABEL(DT_INST(0, invensense_mpu6050));
  const struct device *mpu6050 = device_get_binding(mpu_label);
  GyroWaiter waiter(mpu6050, &threadStepper_sem, &display);

  init_console(&stepper, &waiter);

  k_tid_t my_tid_stepper = k_thread_create(
      &threadStepper_data, threadStepper_stack_area,
      K_THREAD_STACK_SIZEOF(threadStepper_stack_area), threadStepper, &stepper,
      NULL, NULL, PRIORITY, 0, K_NO_WAIT);
  k_thread_name_set(&threadStepper_data, "thread_r");

  k_thread_start(&threadConsole_data);
  k_thread_start(&threadStepper_data);
}
