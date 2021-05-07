#ifndef __MODEL_H_
#define __MODEL_H_

#include <device.h>
#include <init.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr.h>
#include <zephyr/types.h>

#include "Stepper.h"

class Model {
  Stepper *stepper;
  int target_position = 0;

public:
  Model(Stepper *_stepper) { stepper = _stepper; };

  Stepper *get_stepper() { return stepper; };
  int get_cur_position();
  int get_target_position();
  bool is_in_target_position();

  void go(int dist);
};

#endif // __MODEL_H_
