#include "Stepper.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(stepper);

K_SEM_DEFINE(_stepper_sem, 0, 1);
Stepper::Stepper() { stepper_sem = &_stepper_sem; }

void Stepper::log_state() {
  LOG_MODULE_DECLARE(stepper);
  LOG_INF("position: %i", position);
};

void Stepper::start() { k_sem_give(stepper_sem); }

void Stepper::pause() { k_sem_take(stepper_sem, K_FOREVER); }

void Stepper::set_direction_towards(int target) {
  if (target > position) {
    direction = DIR_RIGHT;
    dir.set(true);
    led0.set(true);
    led1.set(false);
  } else {
    direction = DIR_LEFT;
    dir.set(false);
    led0.set(false);
    led1.set(true);
  }
}

bool Stepper::step_towards(int target) {
  if (target == get_position()) {
    return true;
  }
  LOG_MODULE_DECLARE(stepper);

  k_sem_take(stepper_sem, K_FOREVER);

  set_direction_towards(target);
  pulse.pulse();
  position += direction * step_jump;

  k_sem_give(stepper_sem);

  return target == get_position();
}

int Stepper::get_position() { return position; }