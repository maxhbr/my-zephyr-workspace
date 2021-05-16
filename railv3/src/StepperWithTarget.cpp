#include "StepperWithTarget.h"

void StepperWithTarget::start() { Stepper::start(); }
void StepperWithTarget::pause() { Stepper::pause(); }
void StepperWithTarget::wait_and_pause() {
  LOG_MODULE_DECLARE(stepper);
  LOG_INF("wait...");
  while (! is_in_target_position()) {
    k_sleep(K_MSEC(100));
  }
  Stepper::pause();
  LOG_INF("...pause");
}

int StepperWithTarget::get_position() { return Stepper::get_position(); }

int StepperWithTarget::go_relative(int dist) {
  target_position += dist;
  return target_position;
}

void StepperWithTarget::set_target_position(int _target_position) {
  target_position = _target_position;
}
int StepperWithTarget::get_target_position() { return target_position; }

bool StepperWithTarget::step_towards_target() {
  LOG_MODULE_DECLARE(stepper);
  LOG_DBG("step_towards_target");
  is_moving = !step_towards(get_target_position());
  return !is_moving;
}

bool StepperWithTarget::is_in_target_position() {
  return get_position() == get_target_position();
}