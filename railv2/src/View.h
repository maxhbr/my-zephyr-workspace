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
  Model model;
  Controller controller;

  // Devices
  Display *display;

  // labels
  lv_obj_t *cur_pos_label = NULL;
  lv_obj_t *target_pos_label = NULL;

  // fields
  std::map<lv_obj_t *, int> button_to_dist;

  // init functions
  void register_button_to_dist(lv_obj_t *btn, int dist);
  void fill_nav_panel(lv_obj_t *panel);
  void fill_coarse_panel(lv_obj_t *panel);

public:
  View(Model model, Controller controller, Display *_display);
  void event_cb(lv_obj_t *obj, lv_event_t event);

  void update();
};

#endif // __VIEW_H_
