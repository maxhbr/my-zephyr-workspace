#include "Model.h"
#include <logging/log.h>
LOG_MODULE_REGISTER(model);

int Model::get_cur_position() { return stepper->get_position(); }

int Model::get_target_position() { return target_position; }

bool Model::is_in_target_position() {
  return get_cur_position() == get_target_position();
}

void Model::go(int dist) {
  LOG_MODULE_DECLARE(model);
  target_position += dist;
  LOG_INF("target: %i\n", target_position);
}