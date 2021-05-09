#include "Controller.h"
#include <logging/log.h>
LOG_MODULE_REGISTER(controller);

K_SEM_DEFINE(_stacking_in_progress_sem, 1, 1);
Controller::Controller(Model *_model) : model{_model} {
  stacking_in_progress_sem = &_stacking_in_progress_sem;
}

void Controller::work() {
  if (!model->is_stack_in_progress()) {
    do_next_stack_step();
  }
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

void Controller::synchronize_and_sleep(k_timeout_t timeout) {
  k_sleep(K_MSEC(10));

  k_sleep(timeout);
}

void Controller::prepare_stack(int step_number) {
  int start, stop, distance, stack_step_jump;

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

  LOG_INF("prepare_stack: %d -> %d, #=%d\n", start, stop, step_number);

  distance = stop - start;
  stack_step_jump = distance / step_number;
  int i;
  for (i = 0; i < (step_number - 1); i++) {
    model->set_step_position(i, start + i * stack_step_jump);
  }
  model->set_step_position(step_number - 1, stop);

  model->set_step_number(step_number);
}

void Controller::do_next_stack_step() {
  if (!model->is_stack_in_progress()) {
    return;
  }

  if (!model->is_in_target_position()) {
    return;
  }

  std::optional<int> nex_position = model->get_next_step_and_increment();
  if (!nex_position.has_value()) {
    return;
  }

  go_to(nex_position.value());
  synchronize_and_sleep(K_MSEC(2000));
}