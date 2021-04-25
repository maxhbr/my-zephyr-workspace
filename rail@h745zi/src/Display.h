#ifndef __DISPLAY_H_
#define __DISPLAY_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr.h>
#include <zephyr/types.h>

#include <device.h>
#include <init.h>
#include <kernel.h>
#include <sys/byteorder.h>
#include <sys/crc.h>
#include <sys/printk.h>

#include <console/console.h>

#include <lvgl.h>

#include <drivers/display.h>

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
