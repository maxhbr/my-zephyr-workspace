#include "Controller.h"
#include <logging/log.h>
LOG_MODULE_REGISTER(controller);

void Controller::work() {
  if (!model.is_in_target_position()) {
    int target_position = model.get_target_position();
    model.get_stepper()->step_towards(target_position);
  }
}

void Controller::go(int dist) { model.go(dist); }