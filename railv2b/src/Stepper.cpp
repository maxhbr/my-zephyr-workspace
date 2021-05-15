#include "Stepper.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(stepper);

K_SEM_DEFINE(_stepper_sem, 1, 1);
Stepper::Stepper() { stepper_sem = &_stepper_sem; }

void Stepper::set_direction_towards(int target) {
  if (target > position) {
    direction = DIR_RIGHT;
    dir.set(true);
  } else {
    direction = DIR_LEFT;
    dir.set(false);
  }
}

bool Stepper::step_towards(int target) {
  if (target == get_position()) {
    return true;
  }
  k_sem_take(stepper_sem, K_FOREVER);
  set_direction_towards(target);
  pulse.pulse();
  position += direction * stepsize;
  k_sem_give(stepper_sem);
  return false;
}

int Stepper::get_position() { return position; }