#include "Controller.h"
#include <logging/log.h>
LOG_MODULE_REGISTER(controller);

void Controller::work() {
  if (!model->is_in_target_position()) {
    int target_position = model->get_target_position();
    model->get_stepper()->step_towards(target_position);
  }
}

void Controller::go(int dist) { model->go(dist); }
void Controller::go_to(int target_position) {
  model->set_target_position(target_position);
}
void Controller::go_to_upper_bound() {
  model->set_target_position(model->get_upper_bound());
}
void Controller::go_to_lower_bound() {
  model->set_target_position(model->get_lower_bound());
}

void Controller::set_new_upper_bound() {
  model->set_upper_bound(model->get_target_position());
}

void Controller::set_new_lower_bound() {
  model->set_lower_bound(model->get_target_position());
}

void Controller::do_stack(int step_number) {
  if (!model->is_in_target_position()) {
    return;
  }

  int start, stop, distance, step;

  int cur_position = model->get_cur_position();
  int upper_bound = model->get_upper_bound();
  int lower_bound = model->get_lower_bound();

  if (abs(lower_bound - cur_position) < abs(upper_bound - cur_position)) {
    start = lower_bound;
    stop = upper_bound;
  } else {
    start = upper_bound;
    stop = lower_bound;
  }
  LOG_INF("do_stack: %d -> %d\n", start, stop);

  go_to(start);
  k_sleep(K_MSEC(2000));

  distance = stop - start;
  step = distance / step_number;
  int i;
  for (i = 0; i < step_number; i++) {
    LOG_INF("fire @%d\n", model->get_cur_position());
    go_to(start + i * step);
    k_sleep(K_MSEC(2000));
  }
}