#include "View.h"
#include <logging/log.h>
LOG_MODULE_REGISTER(view);

#define ACTION_GO_LOWER '<'
#define ACTION_GO_UPPER '>'
#define ACTION_SET_LOWER 'l'
#define ACTION_GO_TO_LOWER 'L'
#define ACTION_SET_UPPER 'u'
#define ACTION_GO_TO_UPPER 'U'
#define ACTION_PREPARE_STACK 's'

static View *static_view_pointer = NULL;

View::View(Model *_model, Controller *_controller, Display *_display)
    : model{_model}, controller(_controller), display{_display} {
  LOG_MODULE_DECLARE(view);
  static_view_pointer = this;

  lv_obj_t *move_tab = display->make_tab("move");
  fill_move_panel(move_tab);
  lv_obj_t *shoot_tab = display->make_tab("shoot");
  fill_shoot_panel(shoot_tab);

  lv_task_t *task = lv_task_create(
      [](lv_task_t *task) {
        View *view = static_cast<View *>(task->user_data);
        view->update();
      },
      100, LV_TASK_PRIO_MID, this);
};

int View::read_step_size_roller() {
  char str[7] = {0};
  lv_roller_get_selected_str(step_size_roller, str, sizeof(str));
  return atoi(str);
}

int View::read_step_number_roller() {
  char str[12] = {0};
  lv_roller_get_selected_str(step_number_roller, str, sizeof(str));
  int step_number = atoi(str);
  if (step_number < 10) {
    return 10;
  }
  return step_number;
}

void View::event_cb(char action_type, lv_obj_t *obj, lv_event_t event) {
  LOG_MODULE_DECLARE(view);
  char str[12] = {0};

  if (!(event == LV_EVENT_PRESSED || event == LV_EVENT_LONG_PRESSED ||
        event == LV_EVENT_LONG_PRESSED_REPEAT)) {
    return;
  }

  switch (action_type) {
  case ACTION_PREPARE_STACK: {
    if (!(event == LV_EVENT_PRESSED)) {
      return;
    }
    int step_number = read_step_number_roller();
    LOG_INF("prepare_stack: %d\n", step_number);
    controller->prepare_stack(step_number);
    break;
  }
  case ACTION_GO_LOWER: {
    int dist = -read_step_size_roller();
    LOG_INF("go: %d\n", dist);
    controller->go(dist);
    break;
  }
  case ACTION_GO_UPPER: {
    int dist = read_step_size_roller();
    LOG_INF("go: %d\n", dist);
    controller->go(dist);
    break;
  }
  case ACTION_SET_LOWER: {
    if (!(event == LV_EVENT_LONG_PRESSED)) {
      return;
    }
    controller->set_new_lower_bound();
    sprintf(str, "%d <-", model->get_lower_bound());
    lv_label_set_text(lower_label, str);
    break;
  }
  case ACTION_GO_TO_LOWER: {
    if (!(event == LV_EVENT_LONG_PRESSED)) {
      return;
    }
    controller->go_to_lower_bound();
    break;
  }
  case ACTION_SET_UPPER: {
    if (!(event == LV_EVENT_LONG_PRESSED)) {
      return;
    }
    controller->set_new_upper_bound();
    sprintf(str, "-> %d", model->get_upper_bound());
    lv_label_set_text(upper_label, str);
    break;
  }
  case ACTION_GO_TO_UPPER: {
    if (!(event == LV_EVENT_LONG_PRESSED)) {
      return;
    }
    controller->go_to_upper_bound();
    break;
  }
  default: {
    LOG_ERR("does not understand action=%c", action_type);
    break;
  }
  }
}

void View::fill_move_panel(lv_obj_t *parent) {
  step_size_roller = display->add_roller(parent, "12800\n"
                                                 "6400\n"
                                                 "1280\n"
                                                 "640\n"
                                                 "128\n"
                                                 "64\n"
                                                 "12\n"
                                                 "6\n"
                                                 "1");
  lv_obj_align(step_size_roller, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

  lv_obj_t *left_btn = display->add_button(parent, "<<", 100, 60);
  lv_obj_align(left_btn, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 10);
  lv_obj_set_event_cb(left_btn, [](lv_obj_t *btn, lv_event_t event) {
    static_view_pointer->event_cb(ACTION_GO_LOWER, btn, event);
  });
  lv_obj_t *right_btn = display->add_button(parent, ">>", 100, 60);
  lv_obj_align(right_btn, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 10);
  lv_obj_set_event_cb(right_btn, [](lv_obj_t *btn, lv_event_t event) {
    static_view_pointer->event_cb(ACTION_GO_UPPER, btn, event);
  });

  pos_label = display->add_label(parent);
  lv_obj_align(pos_label, NULL, LV_ALIGN_IN_BOTTOM_MID, -100, -10);
  lv_obj_set_width(pos_label, 200);
  lv_obj_set_height(pos_label, 20);
  lv_label_set_align(pos_label, LV_LABEL_ALIGN_CENTER);

  lv_obj_t *set_lower = display->add_button(parent, "Set lower", 100, 30);
  lv_obj_align(set_lower, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 75);
  lv_obj_set_event_cb(set_lower, [](lv_obj_t *btn, lv_event_t event) {
    static_view_pointer->event_cb(ACTION_SET_LOWER, btn, event);
  });
  lv_obj_t *set_upper = display->add_button(parent, "Set upper", 100, 30);
  lv_obj_align(set_upper, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 75);
  lv_obj_set_event_cb(set_upper, [](lv_obj_t *btn, lv_event_t event) {
    static_view_pointer->event_cb(ACTION_SET_UPPER, btn, event);
  });

  lv_obj_t *go_to_lower = display->add_button(parent, NULL, 100, 30);
  lower_label = display->add_label(go_to_lower);
  lv_label_set_text(lower_label, "Go to lower");
  lv_obj_align(go_to_lower, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 110);
  lv_obj_set_event_cb(go_to_lower, [](lv_obj_t *btn, lv_event_t event) {
    static_view_pointer->event_cb(ACTION_GO_TO_LOWER, btn, event);
  });

  lv_obj_t *go_to_upper = display->add_button(parent, NULL, 100, 30);
  upper_label = display->add_label(go_to_upper);
  lv_label_set_text(upper_label, "Go to upper");
  lv_obj_align(go_to_upper, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 110);
  lv_obj_set_event_cb(go_to_upper, [](lv_obj_t *btn, lv_event_t event) {
    static_view_pointer->event_cb(ACTION_GO_TO_UPPER, btn, event);
  });

  // lv_obj_t *slider = lv_slider_create(parent, NULL);
  // lv_obj_align(slider, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -10);
  // lv_slider_set_range(slider, 0, 100);
  // lv_slider_set_value(slider, 50, 0);
  // // lv_obj_set_width(slider, LV_PCT(95))
}

void View::fill_shoot_panel(lv_obj_t *parent) {
  step_number_roller = display->add_roller(parent, "10\n"
                                                   "20\n"
                                                   "30\n"
                                                   "50\n"
                                                   "100\n"
                                                   "200\n"
                                                   "300\n"
                                                   "500\n"
                                                   "1000");
  lv_obj_align(step_number_roller, NULL, LV_ALIGN_IN_LEFT_MID, 20, 0);

  lv_obj_t *fire_button = display->add_button(parent, "fire", 100, 100);
  lv_obj_align(fire_button, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_event_cb(fire_button, [](lv_obj_t *btn, lv_event_t event) {
    static_view_pointer->event_cb(ACTION_PREPARE_STACK, btn, event);
  });
}

void View::update() {
  LOG_MODULE_DECLARE(view);

  if (pos_label == NULL) {
    LOG_WRN("cur_label not yet initialized");
    return;
  }

  char str[25] = {0};
  int cur_position = model->get_cur_position();
  int target_position = model->get_target_position();
  if (cur_position == target_position) {
    sprintf(str, "@%d", cur_position);
  } else {
    if (cur_position < target_position) {
      sprintf(str, "%d > %d", cur_position, target_position);
    } else {
      sprintf(str, "%d < %d", target_position, cur_position);
    }
  }
  lv_label_set_text(pos_label, str);
}