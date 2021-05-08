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
  int upper_bound = 0;
  int lower_bound = 0;

public:
  Model(Stepper *_stepper) { stepper = _stepper; };

  Stepper *get_stepper() { return stepper; };
  void go(int dist);

  void set_target_position(int _target_position);
  int get_target_position();
  void set_upper_bound(int _upper_bound);
  int get_upper_bound();
  void set_lower_bound(int _lower_bound);
  int get_lower_bound();

  int get_cur_position();
  bool is_in_target_position();
};

#endif // __MODEL_H_
