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
#include "GyroWaiter.h"
#include "IrSony.h"
#include "StepperWithTarget.h"
#include "View.h"

#define SW0_NODE DT_ALIAS(sw0)

// ############################################################################
// initialize Stepper

StepperWithTarget stepper;

void stepper_work_handler(struct k_work *work) {
  ARG_UNUSED(work);
  stepper.step_towards_target();
}
K_WORK_DEFINE(stepper_work, stepper_work_handler);
void stepper_expiry_function(struct k_timer *timer_id) {
  ARG_UNUSED(timer_id);
  k_work_submit(&stepper_work);
}
K_TIMER_DEFINE(stepper_timer, stepper_expiry_function, NULL);
void start_stepper() {
  stepper.start();
  k_timer_start(&stepper_timer, K_USEC(20), K_USEC(20));
}

// ############################################################################
// initialize IrSony
IrSony irsony;

// ############################################################################
// initialize GyroWaiter
GyroWaiter gyro_waiter;

// ############################################################################
// initialize Button
#define SW0_GPIO_LABEL DT_GPIO_LABEL(SW0_NODE, gpios)
#define SW0_GPIO_PIN DT_GPIO_PIN(SW0_NODE, gpios)
#define SW0_GPIO_FLAGS (GPIO_INPUT | DT_GPIO_FLAGS(SW0_NODE, gpios))
static struct gpio_callback button_cb_data;
void button_pressed(const struct device *dev, struct gpio_callback *cb,
                    uint32_t pins) {
  ARG_UNUSED(dev);
  ARG_UNUSED(cb);
  ARG_UNUSED(pins);

  printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
}
void init_button() {
  const struct device *button;
  button = device_get_binding(SW0_GPIO_LABEL);
  gpio_pin_configure(button, SW0_GPIO_PIN, SW0_GPIO_FLAGS);
  gpio_pin_interrupt_configure(button, SW0_GPIO_PIN, GPIO_INT_EDGE_TO_ACTIVE);
  gpio_init_callback(&button_cb_data, button_pressed, BIT(SW0_GPIO_PIN));
  gpio_add_callback(button, &button_cb_data);
}

// ############################################################################
// ...

Display get_display() {
  const struct device *display_dev =
      device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);
  __ASSERT(display_dev != NULL, "display device not found.");
  Display display(display_dev);
  return display;
}

// ############################################################################
// Main

void main(void) {
  start_stepper();
  LOG_INF("stepper = %p", &stepper);

  init_button();

  Display display = get_display();

  Model model(&stepper);
  model.set_upper_bound(12800);
  model.set_step_number(300);

  Controller controller(&model, &irsony, &gyro_waiter);
  View view(&model, &controller, &display);
  LOG_INF("model = %p", &model);
  model.log_state();
  LOG_INF("controller = %p", &controller);
  LOG_INF("view = %p", &view);

  while (true) {
    for (int i = 0; i < 10; i++) {
      view.update();
      k_sleep(K_MSEC(50));
      lv_task_handler();
      k_sleep(K_MSEC(50));
      controller.work();
      k_sleep(K_MSEC(50));
    }
    model.log_state();
  }
}
