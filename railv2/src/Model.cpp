#include "Model.h"

int Model::get_cur_position() { return stepper->get_position(); }

int Model::get_target_position() { return target_position; }

bool Model::is_in_target_position() {
  return get_cur_position() == get_target_position();
}

void Model::go(int dist) { target_position += dist; }