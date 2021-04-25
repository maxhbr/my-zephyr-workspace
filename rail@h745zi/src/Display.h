#ifndef __DISPLAY_H_
#define __DISPLAY_H_

#include <drivers/display.h>
#include <lvgl.h>

class Display {
  const struct device *display_dev;
  lv_obj_t *position_label;
  lv_obj_t *target_position_label;
  lv_obj_t *secondary_label;

public:
  Display();
  lv_obj_t *getPositionLabel() { return position_label; };
  lv_obj_t *getTargetPositionLabel() { return target_position_label; };
  lv_obj_t *getSecondaryLabel() { return secondary_label; };
};

#endif // __DISPLAY_H_
