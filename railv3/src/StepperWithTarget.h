#ifndef STEPPERWITHTARGET_H_
#define STEPPERWITHTARGET_H_

#include <device.h>
#include <init.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr.h>
#include <zephyr/types.h>

#include <arch/cpu.h>
#include <drivers/gpio.h>
#include <soc.h>
#include <sys/printk.h>

#include <optional>

#include "Stepper.h"

class StepperWithTarget : private Stepper {
  bool is_moving = false;
  int target_position = 0;

public:
  StepperWithTarget() : Stepper(){};

  void log_state();

  void start();
  void pause();
  void wait_and_pause();

  int get_position();

  int go_relative(int dist);
  void set_target_position(int _target_position);
  int get_target_position();

  bool step_towards_target();

  bool is_in_target_position();
};

#endif // STEPPERWITHTARGET_H_
