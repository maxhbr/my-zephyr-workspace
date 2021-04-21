#include "Rail.h"
#include <logging/log.h>
LOG_MODULE_REGISTER(rail);

void Rail::set_dir(bool to_left) {
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
void Rail::step(bool to_left) {
  k_sem_take(threadRail_sem, K_FOREVER);
  set_dir(to_left);
  pulse.pulse();
  if (to_left) {
    ++pos;
  } else {
    --pos;
  }
  k_sem_give(threadRail_sem);
  if (pos % 100 == 0) {
    print_to_label();
  }
};
bool Rail::is_in_pos() { return pos == target_pos; }
void Rail::run_to_target() {
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
}
void Rail::print_to_label() {
  char pos_str[21] = {0};
  sprintf(pos_str, "%i > %i", target_pos, pos);
  lv_label_set_text(label, pos_str);
  lv_task_handler();
}
Rail::Rail(struct k_sem *_threadRail_sem) {
  threadRail_sem = _threadRail_sem;

  label = lv_label_create(lv_scr_act(), NULL);
  lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

  go(1);
  go(-1);
};
void Rail::loop() {
  int counter = 0;
  while (1) {
    if (!is_in_pos()) {
      run_to_target();
      print_to_label();
    }
    if((++counter)%100==0) {
      print_to_label();
    }
    k_sleep(K_MSEC(1));
  }
}
int Rail::go(int relative) {
  target_pos += relative;
  print_to_label();
  return pos;
};
