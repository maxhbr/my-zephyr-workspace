#include "Stepper.h"
#include <logging/log.h>
LOG_MODULE_REGISTER(stepper);

void Stepper::fill_nav_panel(Display *display, lv_obj_t *panel) {
  lv_obj_t *l_left_btn = display->add_button(panel, "<<<", 60);
  lv_obj_align(l_left_btn, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 2);
  lv_obj_set_event_cb(l_left_btn, [](lv_obj_t *btn, lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
      printf("go(-12800);");
    }
  });
  lv_obj_t *left_btn = display->add_button(panel, "<<", 60);
  lv_obj_align(left_btn, NULL, LV_ALIGN_IN_TOP_LEFT, 75, 2);
  lv_obj_set_event_cb(left_btn, [](lv_obj_t *btn, lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
      printf("go(-1280);");
    }
  });
  lv_obj_t *right_btn = display->add_button(panel, ">>", 60);
  lv_obj_align(right_btn, NULL, LV_ALIGN_IN_TOP_RIGHT, -75, 2);
  lv_obj_set_event_cb(right_btn, [](lv_obj_t *btn, lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
      printf("go(1280);");
    }
  });
  lv_obj_t *r_right_btn = display->add_button(panel, ">>>", 60);
  lv_obj_align(r_right_btn, NULL, LV_ALIGN_IN_TOP_RIGHT, -5, 2);
  lv_obj_set_event_cb(r_right_btn, [](lv_obj_t *btn, lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
      printf("go(12800);");
    }
  });

  // lv_obj_t *slider = lv_slider_create(panel, NULL);
  // lv_obj_align(slider, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -10);
  // lv_slider_set_range(slider, 0, 100);
  // lv_slider_set_value(slider, 50, 0);
  // // lv_obj_set_width(slider, LV_PCT(95))
}

void Stepper::init_coarse_tab(Display *display, lv_obj_t *parent) {
  lv_obj_t *panel_nav = display->add_panel(parent);
  lv_obj_align(panel_nav, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 30);
  lv_obj_set_height(panel_nav, 90);
  lv_obj_set_width(panel_nav, 320);
  fill_nav_panel(display, panel_nav);

  lv_obj_t *header = lv_obj_create(parent, NULL);
  lv_obj_align(header, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
  lv_obj_set_height(header, 20);
  lv_obj_set_width(header, 320);
  //   lv_obj_set_width(header, LV_PCT(100));

  label = display->add_label(header);
  lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 2);
  target_label = display->add_label(header);
  lv_obj_align(target_label, NULL, LV_ALIGN_IN_TOP_MID, 20, 2);
}

K_SEM_DEFINE(_is_moving_sem, 1, 1);
Stepper::Stepper(struct k_sem *_threadStepper_sem, Display *display) {
  threadStepper_sem = _threadStepper_sem;
  is_moving_sem = &_is_moving_sem;

  init_coarse_tab(display, display->make_tab("coarse"));

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
  sprintf(pos_str, "curr: %i", pos);
  lv_label_set_text(label, pos_str);
  sprintf(pos_str, "dest: %i", target_pos);
  lv_label_set_text(target_label, pos_str);
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
