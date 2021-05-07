#ifndef __STEPPER_H_
#define __STEPPER_H_

#include <device.h>
#include <init.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr.h>
#include <zephyr/types.h>

class Stepper {
  int pos = 0;

public:
  Stepper(){

  };
};

#endif // __STEPPER_H_
