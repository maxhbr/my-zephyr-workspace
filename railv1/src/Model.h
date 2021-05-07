#ifndef __MODEL_H_
#define __MODEL_H_

#include "Stepper.h"

class Model {
  Stepper *stepper;
public:
  Model(Stepper *_stepper);
}

#endif // __MODEL_H_
