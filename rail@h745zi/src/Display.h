#ifndef __DISPLAY_H_
#define __DISPLAY_H_

#include <drivers/display.h>
#include <lvgl.h>

class Display {
  const struct device *display_dev;
  lv_obj_t *positionLabel;
  lv_obj_t *secondaryLabel;

public:
  Display();
  lv_obj_t *getPositionLabel() { return positionLabel; };
  lv_obj_t *getSecondaryLabel() { return secondaryLabel; };
};

#endif // __DISPLAY_H_
