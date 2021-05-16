#include "Model.h"
#include <logging/log.h>
LOG_MODULE_REGISTER(model);

Model::Model(StepperWithTarget *_stepper) {
  stepper = _stepper;
  stepps = (int *)malloc(sizeof(int) * 1000);
}

void Model::go(int dist) {
  LOG_MODULE_DECLARE(model);
  int target_position = stepper->go_relative(dist);
  LOG_INF("target: %i\n", target_position);
}

void Model::set_target_position(int _target_position) {
  stepper->set_target_position(_target_position);
}
int Model::get_target_position() { return stepper->get_target_position(); }

void Model::set_upper_bound(int _upper_bound) { upper_bound = _upper_bound; }
int Model::get_upper_bound() { return upper_bound; }
void Model::set_lower_bound(int _lower_bound) { lower_bound = _lower_bound; }
int Model::get_lower_bound() { return lower_bound; }

void Model::set_step_number(int _step_number) {
  step_number = _step_number;
  cur_step_index = 0;
}
void Model::set_step_position(int index, int pos) { stepps[index] = pos; }
std::optional<int> Model::get_next_step_and_increment() {
  if (cur_step_index >= step_number) {
    stack_in_progress = false;
    return {};
  }
  return stepps[cur_step_index++];
}
void Model::set_stack_in_progress(bool _stack_in_progress) {
  stack_in_progress = _stack_in_progress;
}
bool Model::is_stack_in_progress() { return stack_in_progress; }

int Model::get_cur_position() { return stepper->get_position(); }

bool Model::is_in_target_position() {
  return get_cur_position() == get_target_position();
}