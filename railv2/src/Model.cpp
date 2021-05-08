#include "Model.h"
#include <logging/log.h>
LOG_MODULE_REGISTER(model);

void Model::go(int dist) {
  LOG_MODULE_DECLARE(model);
  target_position += dist;
  LOG_INF("target: %i\n", target_position);
}

void Model::set_target_position(int _target_position) {
  target_position = _target_position;
}
int Model::get_target_position() { return target_position; }
void Model::set_upper_bound(int _upper_bound) { upper_bound = _upper_bound; }
int Model::get_upper_bound() { return upper_bound; }
void Model::set_lower_bound(int _lower_bound) { lower_bound = _lower_bound; }
int Model::get_lower_bound() { return lower_bound; }

int Model::get_cur_position() { return stepper->get_position(); }

bool Model::is_in_target_position() {
  return get_cur_position() == get_target_position();
}