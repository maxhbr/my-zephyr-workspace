#include "Stepper.h"

K_SEM_DEFINE(_stepper_sem, 1, 1);
Stepper::Stepper() { stepper_sem = &_stepper_sem; }

void Stepper::set_direction(bool to_right) {
  bool is_to_right = stepsize > 0;
  if (is_to_right != to_right) {
    stepsize *= -1;
  }
}

int Stepper::get_position() { return position; }

void Stepper::step(bool to_right) {
  k_sem_take(stepper_sem, K_FOREVER);
  set_direction(to_right);
  position += stepsize;
  k_sem_give(stepper_sem);
}