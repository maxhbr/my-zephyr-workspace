#include "Stepper.h"
#include <logging/log.h>
LOG_MODULE_REGISTER(stepper);

K_SEM_DEFINE(_is_moving_sem, 1, 1);
Stepper::Stepper(struct k_sem *_threadStepper_sem, lv_obj_t *_label) {
  threadStepper_sem = _threadStepper_sem;
  is_moving_sem = &_is_moving_sem;
  label = _label;

  // TEST...
  dir.set(true);
  k_sleep(K_MSEC(10));
  dir.set(false);
  k_sleep(K_MSEC(10));
  go(1);
  k_sleep(K_MSEC(10));
  go(-1);
  k_sleep(K_MSEC(10));
  go(1);
  k_sleep(K_MSEC(10));
  go(-1);
  k_sleep(K_MSEC(10));
}

void Stepper::set_dir(bool to_left) {
  if (to_left) {
    led0.set(true);
    led1.set(false);
  } else {
    led0.set(false);
    led1.set(true);
  }
  if (to_left != current_to_left) {
    dir.set(to_left);
    current_to_left = to_left;
  }
}
void Stepper::step(bool to_left) {
  k_sem_take(threadStepper_sem, K_FOREVER);
  set_dir(to_left);
  pulse.pulse();
  if (to_left) {
    ++pos;
  } else {
    --pos;
  }
  k_sem_give(threadStepper_sem);
  if (pos % 100 == 0) {
    print_to_label();
  }
}
bool Stepper::is_in_pos() { return pos == target_pos; }
void Stepper::run_to_target() {
  if (k_sem_take(is_moving_sem, K_MSEC(50)) != 0) {
    LOG_INF("run_to_target: did not get Semaphore");
  } else {
    while (1) {
      if (pos < target_pos) {
        step(true);
      } else if (pos > target_pos) {
        step(false);
      } else {
        LOG_INF("...reached");
        led0.set(false);
        led1.set(false);
        break;
      }
    }
    k_sem_give(is_moving_sem);
  }
}
void Stepper::print_to_label() {
  char pos_str[21] = {0};
  sprintf(pos_str, "%i > %i", target_pos, pos);
  lv_label_set_text(label, pos_str);
  lv_task_handler();
}
void Stepper::loop() {
  int counter = 0;
  while (1) {
    if (!is_in_pos()) {
      run_to_target();
      print_to_label();
    }
    if ((++counter) % 100 == 0) {
      print_to_label();
    }
    k_sleep(K_MSEC(1));
  }
}
int Stepper::go(int relative) {
  if (relative != 0) {
    print_to_label();
    target_pos += relative;
  }
  return pos;
};

void Stepper::wait() {
  k_sem_take(is_moving_sem, K_FOREVER);
  k_sem_give(is_moving_sem);
}

int Stepper::go_and_wait(int relative) {
  int res = go(relative);
  while (!is_in_pos()) {
    k_sleep(K_MSEC(1));
  }
  return res;
}
