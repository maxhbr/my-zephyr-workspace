#include "Controller.h"

void Controller::iterate() {
  model->get_stepper()->step(1);
  k_sleep(K_MSEC(10));
}

void Controller::go(int dist) { model->go(dist); }