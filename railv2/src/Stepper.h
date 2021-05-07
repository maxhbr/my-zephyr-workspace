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

class Stepper {
  struct k_sem *stepper_sem;
  int stepsize = 1;
  int position = 0;

  void set_direction(bool to_right);

public:
  Stepper();
  int get_position();
  void step(bool to_right);
};

#endif // __STEPPER_H_
