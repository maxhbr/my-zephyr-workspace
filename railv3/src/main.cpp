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
  irsony.shoot();
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
// ...

/* size of stack area used by each thread */
#define STACKSIZE 8192
/* scheduling priority used by each thread */
#define PRIORITY 7

K_THREAD_STACK_DEFINE(thread_controller_stack_area, STACKSIZE);
static struct k_thread thread_controller_data;
void thread_controller(void *_controller, void *dummy2, void *dummy3) {
  ARG_UNUSED(dummy2);
  ARG_UNUSED(dummy3);
  Controller *controller = static_cast<Controller *>(_controller);
  while (true) {
    controller->work();
    k_sleep(K_MSEC(0));
  }
}

void start_controller_thread(Controller *controller) {
  k_tid_t my_tid_controller = k_thread_create(
      &thread_controller_data, thread_controller_stack_area,
      K_THREAD_STACK_SIZEOF(thread_controller_stack_area), thread_controller,
      controller, NULL, NULL, PRIORITY, 0, K_NO_WAIT);
  k_thread_name_set(&thread_controller_data, "thread_controller");
  k_thread_start(&thread_controller_data);
}

// ############################################################################
// Main

void main(void) {
  start_stepper();
  LOG_INF("stepper = %p", &stepper);

  init_button();

  Display display = get_display();

  Model model(&stepper);
  Controller controller(&model, &irsony);
  View view(&model, &controller, &display);
  LOG_INF("model = %p", &model);
  LOG_INF("controller = %p", &controller);
  LOG_INF("view = %p", &view);

  // start_controller_thread(&controller);
  while (true) {
    lv_task_handler();
    k_sleep(K_MSEC(100));
  }
}
