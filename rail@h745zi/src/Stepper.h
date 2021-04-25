#ifndef STEPPER_H
#define STEPPER_H

#include <device.h>
#include <init.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr.h>
#include <zephyr/types.h>

#include <lvgl.h>

#include <drivers/display.h>
#include <drivers/gpio.h>

#include <logging/log.h>

#define LED0_NODE DT_ALIAS(led0)
#define LED0_LABEL DT_GPIO_LABEL(LED0_NODE, gpios)
#define LED0_PIN DT_GPIO_PIN(LED0_NODE, gpios)
#define LED0_FLAGS DT_GPIO_FLAGS(LED0_NODE, gpios)
#define LED1_NODE DT_ALIAS(led1)
#define LED1_LABEL DT_GPIO_LABEL(LED1_NODE, gpios)
#define LED1_PIN DT_GPIO_PIN(LED1_NODE, gpios)
#define LED1_FLAGS DT_GPIO_FLAGS(LED1_NODE, gpios)
#define LED2_NODE DT_ALIAS(led2)
#define LED2_LABEL DT_GPIO_LABEL(LED2_NODE, gpios)
#define LED2_PIN DT_GPIO_PIN(LED2_NODE, gpios)
#define LED2_FLAGS DT_GPIO_FLAGS(LED2_NODE, gpios)

#define PULSE_NODE DT_ALIAS(pulse)
#define PULSE_LABEL DT_GPIO_LABEL(PULSE_NODE, gpios)
#define PULSE_PIN DT_GPIO_PIN(PULSE_NODE, gpios)
#define PULSE_FLAGS DT_GPIO_FLAGS(PULSE_NODE, gpios)
#define DIR_NODE DT_ALIAS(dir)
#define DIR_LABEL DT_GPIO_LABEL(DIR_NODE, gpios)
#define DIR_PIN DT_GPIO_PIN(DIR_NODE, gpios)
#define DIR_FLAGS DT_GPIO_FLAGS(DIR_NODE, gpios)

class GPIO {
  const struct device *dev;
  int pin = -1;
  int ret = -1;
  bool cur_value = false;

public:
  GPIO(const char *label, int _pin, int flags) {
    LOG_MODULE_DECLARE(stepper);
    pin = _pin;
    dev = device_get_binding(label);
    if (dev != NULL) {
      ret = gpio_pin_configure(dev, pin, GPIO_OUTPUT_ACTIVE | flags);
      if (ret != 0) {
        LOG_ERR("Failed to configure %s pin %i", label, pin);
      }
    } else {
      LOG_ERR("Failed to get device %s", label);
    }
  };
  void set(bool value) {
    LOG_MODULE_DECLARE(stepper);
    if (ret == 0) {
      if (value != cur_value) {
        cur_value = value;

        gpio_pin_set(dev, pin, value);
      }
    } else {
      LOG_WRN("Do nothing, since ret=%i", ret);
    }
  };
};

class LED : public GPIO {
public:
  LED(const char *label, int _pin, int flags) : GPIO(label, _pin, flags) {
    set(true);
    k_msleep(100);
    set(false);
  };
};

class PULSE : public GPIO {
public:
  PULSE(const char *label, int _pin, int flags) : GPIO(label, _pin, flags){};
  void pulse() {
    LOG_MODULE_DECLARE(stepper);
    set(true);
    k_usleep(10);
    set(false);
    k_usleep(10);
  };
};

class Stepper {
  struct k_sem *threadStepper_sem;
  struct k_sem *is_moving_sem;
  int pos = 0;
  int target_pos = 0;
  lv_obj_t *label;
  lv_obj_t *target_label;
  int sleep_msec = 1;
  bool current_to_left = true;

  LED led0 = LED(LED0_LABEL, LED0_PIN, LED0_FLAGS);
  LED led1 = LED(LED1_LABEL, LED1_PIN, LED1_FLAGS);
  LED led2 = LED(LED2_LABEL, LED2_PIN, LED2_FLAGS);
  PULSE pulse = PULSE(PULSE_LABEL, PULSE_PIN, PULSE_FLAGS);
  GPIO dir = GPIO(DIR_LABEL, DIR_PIN, DIR_FLAGS);

  void set_dir(bool to_left);
  void step(bool to_left);
  bool is_in_pos();
  void run_to_target();
  void print_to_label();

public:
  Stepper(struct k_sem *_threadStepper_sem, lv_obj_t *_label, lv_obj_t *_target_label);
  void loop();
  int go(int relative);
  void wait();
  int go_and_wait(int relative);
};

#endif /* STEPPER_H */
