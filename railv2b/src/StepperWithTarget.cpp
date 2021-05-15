#include "StepperWithTarget.h"

void my_expiry_function(struct k_timer *timer_id) {

}

void StepperWithTarget::start() {
    k_timer_start(&my_timer, K_MSEC(10), K_MSEC(10));
}

void StepperWithTarget::go_relative(int dist) {
  target_position += dist;
}

bool StepperWithTarget::is_in_target_position() {
  return get_position() == get_target_position();
}