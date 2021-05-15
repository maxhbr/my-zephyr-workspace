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

#include <optional>

#include "Stepper.h"

struct k_timer my_timer;
extern void my_expiry_function(struct k_timer *timer_id);

k_timer_init(&my_timer, my_expiry_function, NULL);

class StepperWithTarget : public Stepper {
    int target_position = 0;;
public:
  StepperWithTarget() : Stepper() {

  };

  void start();

  void go_relative(int dist);
  void set_target_position(int _target_position) {
      target_position = _target_position
  };
  int get_target_position() { return target_position; };

  bool is_in_target_position();
};


#endif // STEPPERWITHTARGET_H_
