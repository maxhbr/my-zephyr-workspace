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

#include "Controller.h"
#include "Display.h"
#include "Model.h"
#include "Stepper.h"

class CurrentPositionView {
private:
  lv_obj_t *cur_pos_label = NULL;
  lv_obj_t *target_pos_label = NULL;

public:
  CurrentPositionView() = default;

  void set_cur_pos_label(lv_obj_t *_cur_pos_label) {
    cur_pos_label = _cur_pos_label;
  }
  void set_target_pos_label(lv_obj_t *_target_pos_label) {
    target_pos_label = _target_pos_label;
  }
};

class View {
private:
  Model *model;
  Controller *controller;

  // Devices
  Display *display;

  // sub views
  CurrentPositionView current_position_view;

  // init functions
  void fill_nav_panel(lv_obj_t *panel);
  void fill_coarse_panel(lv_obj_t *panel);

public:
  View(Display *_display, Stepper *_stepper) {
    display = _display;

    Model _model;
    model = &_model;
    Controller _controller(model);
    controller = &_controller;

    lv_obj_t *coarse_tab = display->make_tab("coarse");
    fill_coarse_panel(coarse_tab);
    display->make_tab("fine");
    display->make_tab("go");
  };
};

#endif // __VIEW_H_
