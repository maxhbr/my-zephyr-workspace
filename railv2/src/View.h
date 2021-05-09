#ifndef __VIEW_H_
#define __VIEW_H_

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

#include <map>

#include "Controller.h"
#include "Display.h"
#include "Model.h"
#include "Stepper.h"

class View {
private:
  Model *model;
  Controller *controller;

  // Devices
  Display *display;

  // labels
  lv_obj_t *pos_label = NULL;
  lv_obj_t *lower_label = NULL;
  lv_obj_t *upper_label = NULL;

  lv_obj_t *step_size_roller = NULL;
  int read_step_size_roller();
  lv_obj_t *step_number_roller = NULL;
  int read_step_number_roller();

  // init functions
  void fill_move_panel(lv_obj_t *parent);
  void fill_shoot_panel(lv_obj_t *parent);

public:
  View(Model *_model, Controller *_controller, Display *_display);
  void event_cb(char action_type, lv_obj_t *obj, lv_event_t event);

  void update();
};

#endif // __VIEW_H_
