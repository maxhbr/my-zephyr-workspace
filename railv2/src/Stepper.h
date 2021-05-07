#ifndef __STEPPER_H_
#define __STEPPER_H_

#include <device.h>
#include <init.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr.h>
#include <zephyr/types.h>

#include <logging/log.h>

#include "GPIOs.h"

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

#define STEPS_PER_REV 12800
#define DIR_RIGHT 1
#define DIR_LEFT -1

class Stepper {
  struct k_sem *stepper_sem;
  int direction = DIR_RIGHT;
  int stepsize = 1;
  int position = 0;

  LED led0 = LED(LED0_LABEL, LED0_PIN, LED0_FLAGS);
  LED led1 = LED(LED1_LABEL, LED1_PIN, LED1_FLAGS);
  LED led2 = LED(LED2_LABEL, LED2_PIN, LED2_FLAGS);
  PULSE pulse = PULSE(PULSE_LABEL, PULSE_PIN, PULSE_FLAGS);
  GPIO dir = GPIO(DIR_LABEL, DIR_PIN, DIR_FLAGS);

public:
  Stepper();

  void set_direction_towards(int target);
  bool step_towards(int target);

  int get_position();
};

#endif // __STEPPER_H_
