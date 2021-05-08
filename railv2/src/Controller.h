#ifndef __CONTROLLER_H_
#define __CONTROLLER_H_

#include <device.h>
#include <init.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr.h>
#include <zephyr/types.h>

#include <drivers/display.h>
#include <lvgl.h>

#include <logging/log.h>

#include "Model.h"

class Controller {
  Model *model;

public:
  Controller(Model *_model) : model{_model} {};
  void work();

  void go(int dist);
  void go_to(int target_position);
  void go_to_upper_bound();
  void go_to_lower_bound();
  void set_new_upper_bound();
  void set_new_lower_bound();

  void do_stack(int step_number);
};

#endif // __CONTROLLER_H_
