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

#include "Model.h"

class Controller {
  Model *model;

public:
  Controller(Model *_model) { model = _model; };
};

#endif // __CONTROLLER_H_
