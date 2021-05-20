#include "Controller.h"
#include <logging/log.h>
LOG_MODULE_REGISTER(controller);

K_SEM_DEFINE(_work_in_progress_sem, 1, 1);
Controller::Controller(Model *_model, IrSony *_irsony, GyroWaiter *_gyro_waiter)
    : model{_model}, irsony{_irsony}, gyro_waiter{_gyro_waiter} {
  work_in_progress_sem = &_work_in_progress_sem;
}

void Controller::work() {
  if (k_sem_take(work_in_progress_sem, K_MSEC(50)) != 0) {
    LOG_WRN("work, but semaphore is not availablel");
    return;
  }
  if (model->is_stack_in_progress() && model->is_in_target_position()) {
    do_next_stack_step();
  }
  k_sem_give(work_in_progress_sem);
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
  prepare_stack();
}

void Controller::set_new_lower_bound() {
  model->set_lower_bound(model->get_target_position());
  prepare_stack();
}

void Controller::set_step_number(int step_number) {
  model->set_step_number(step_number);
  prepare_stack();
}

void Controller::synchronize_and_sleep(k_timeout_t timeout) {
  k_sleep(K_MSEC(10));

  k_sleep(timeout);
}

void Controller::prepare_stack() {
  if (model->is_stack_in_progress()) {
    LOG_WRN("tried to prepare while stack was in progress");
    return;
  }

  int cur_position = model->get_cur_position();
  int upper_bound = model->get_upper_bound();
  int lower_bound = model->get_lower_bound();

  int start, stop;
  if (abs(lower_bound - cur_position) < abs(upper_bound - cur_position)) {
    start = lower_bound;
    stop = upper_bound;
  } else {
    start = upper_bound;
    stop = lower_bound;
  }

  int step_number = model->get_step_number();
  LOG_INF("prepare_stack: %d -> %d, #=%d", start, stop, step_number);

  int distance = stop - start;
  int stack_step_jump = distance / step_number;
  if (stack_step_jump == 0) {
    LOG_WRN("step_jump of 0");
    return;
  }
  int i;
  for (i = 0; i < (step_number - 1); i++) {
    model->set_step_position(i, start + i * stack_step_jump);
  }
  model->set_step_position(step_number - 1, stop);
}

void Controller::start_stack() {
  if (model->is_stack_in_progress()) {
    LOG_WRN("start_stack, stack already running");
  } else {
    LOG_INF("start_stack");
    std::optional<int> start_position = model->get_next_step_and_increment();
    if (!start_position.has_value()) {
      LOG_ERR("stacking start failed");
      return;
    }
    go_to(start_position.value());
    model->set_stack_in_progress(true);
  }
}

void Controller::stop_stack() {
  LOG_INF("stop_stack");
  model->set_stack_in_progress(false);
}

void Controller::do_next_stack_step() {
  if (!model->is_stack_in_progress()) {
    LOG_ERR("stacking, but not in progress");
    return;
  }

  if (!model->is_in_target_position()) {
    LOG_WRN("stacking, but not yet in target position");
    return;
  }

  LOG_INF("stacking, waiting");
  k_sleep(K_MSEC(2000));
  LOG_INF("stacking, gyro_waiter, waiting");
  gyro_waiter->wait();
  LOG_INF("stacking, take pickture");
  irsony->shoot();
  k_sleep(K_MSEC(500));

  std::optional<int> next_position = model->get_next_step_and_increment();
  if (!next_position.has_value()) {
    LOG_INF("stacking, finished");
    return;
  }
  LOG_INF("stacking, next is %d", next_position.value());

  go_to(next_position.value());
}