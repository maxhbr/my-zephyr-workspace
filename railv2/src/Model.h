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

#include <optional>

#include "Stepper.h"

class Model {
  Stepper *stepper;
  int target_position = 0;
  int upper_bound = 0;
  int lower_bound = 0;

  int step_number = 0;
  int cur_step_index = 0;
  int *stepps;
  bool stack_in_progress = false;

public:
  Model(Stepper *_stepper);

  Stepper *get_stepper() {
    return stepper;
    stepps = (int *)malloc(sizeof(int) * 1000);
  };
  void go(int dist);

  void set_target_position(int _target_position);
  int get_target_position();
  void set_upper_bound(int _upper_bound);
  int get_upper_bound();
  void set_lower_bound(int _lower_bound);
  int get_lower_bound();

  void set_step_number(int _step_number);
  int get_step_number() { return step_number; };
  int get_cur_step_index() { return cur_step_index; };
  void set_step_position(int index, int pos);
  std::optional<int> get_next_step_and_increment();
  void set_stack_in_progress(bool _stack_in_progress);
  bool is_stack_in_progress();

  int get_cur_position();
  bool is_in_target_position();
};

#endif // __MODEL_H_
