#include "View.h"
#include <logging/log.h>
LOG_MODULE_REGISTER(view);

#define ACTION_GO_DIST 'g'
#define ACTION_SET_LOWER 'l'
#define ACTION_GO_TO_LOWER 'L'
#define ACTION_SET_UPPER 'u'
#define ACTION_GO_TO_UPPER 'U'
#define ACTION_FIRE 'f'

static View *static_view_pointer = NULL;

View::View(Model *_model, Controller *_controller, Display *_display)
    : model{_model}, controller(_controller), display{_display} {
  LOG_MODULE_DECLARE(view);
  static_view_pointer = this;

  lv_obj_t *move_tile = display->get_tile(0);
  fill_move_panel(move_tile);
  lv_obj_t *shoot_tile = display->get_tile(1);
  fill_shoot_panel(shoot_tile);

  lv_task_t *task = lv_task_create(
      [](lv_task_t *task) {
        View *view = static_cast<View *>(task->user_data);
        view->update();
      },
      100, LV_TASK_PRIO_MID, this);
};

void View::event_cb(char action_type, lv_obj_t *obj, lv_event_t event) {
  LOG_MODULE_DECLARE(view);
  char str[12] = {0};

  switch (action_type) {
  case ACTION_FIRE: {
    if (!(event == LV_EVENT_PRESSED)) {
      return;
    }
    lv_roller_get_selected_str(step_number_roller, str, sizeof(str));
    int step_number = atoi(str);
    LOG_INF("fire: %d\n", step_number);
    controller->do_stack(atoi(str));
    break;
  }
  case ACTION_GO_DIST: {
    if (!(event == LV_EVENT_PRESSED || event == LV_EVENT_LONG_PRESSED ||
          event == LV_EVENT_LONG_PRESSED_REPEAT)) {
      return;
    }
    int dist = button_to_dist.at(obj);
    LOG_INF("go: %i\n", dist);
    controller->go(dist);
    LOG_INF("target: %i\n", model->get_target_position());
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

void View::register_button_to_dist(lv_obj_t *btn, int dist) {
  button_to_dist.insert_or_assign(btn, dist);
  lv_obj_set_event_cb(btn, [](lv_obj_t *btn, lv_event_t event) {
    static_view_pointer->event_cb(ACTION_GO_DIST, btn, event);
  });
};

void View::fill_nav_row(lv_obj_t *panel, int base) {
  lv_obj_t *l_left_btn = display->add_button(panel, "<<<", 60, 40);
  lv_obj_align(l_left_btn, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 10);
  register_button_to_dist(l_left_btn, -10 * base);
  lv_obj_t *left_btn = display->add_button(panel, "<<", 60, 40);
  lv_obj_align(left_btn, NULL, LV_ALIGN_IN_TOP_LEFT, 75, 10);
  register_button_to_dist(left_btn, -1 * base);
  lv_obj_t *right_btn = display->add_button(panel, ">>", 60, 40);
  lv_obj_align(right_btn, NULL, LV_ALIGN_IN_TOP_RIGHT, -75, 10);
  register_button_to_dist(right_btn, base);
  lv_obj_t *r_right_btn = display->add_button(panel, ">>>", 60, 40);
  lv_obj_align(r_right_btn, NULL, LV_ALIGN_IN_TOP_RIGHT, -5, 10);
  register_button_to_dist(r_right_btn, 10 * base);
}

void View::fill_move_panel(lv_obj_t *parent) {
  lv_obj_t *row_coarse_nav = display->add_panel(parent);
  lv_obj_align(row_coarse_nav, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 5);
  lv_obj_set_height(row_coarse_nav, 60);
  lv_obj_set_width(row_coarse_nav, 300);
  fill_nav_row(row_coarse_nav, 1280);
  lv_obj_t *row_fine_nav = display->add_panel(parent);
  lv_obj_align(row_fine_nav, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 75);
  lv_obj_set_height(row_fine_nav, 60);
  lv_obj_set_width(row_fine_nav, 300);
  fill_nav_row(row_fine_nav, 10);

  pos_label = display->add_label(parent);
  lv_obj_align(pos_label, NULL, LV_ALIGN_IN_TOP_MID, -100, 137);
  lv_obj_set_width(pos_label, 200);
  lv_obj_set_height(pos_label, 20);
  lv_label_set_align(pos_label, LV_LABEL_ALIGN_CENTER);

  lv_obj_t *set_lower = display->add_button(parent, "Set lower", 130, 30);
  lv_obj_align(set_lower, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 5, -50);
  lv_obj_set_event_cb(set_lower, [](lv_obj_t *btn, lv_event_t event) {
    static_view_pointer->event_cb(ACTION_SET_LOWER, btn, event);
  });
  lv_obj_t *set_upper = display->add_button(parent, "Set upper", 130, 30);
  lv_obj_align(set_upper, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -5, -50);
  lv_obj_set_event_cb(set_upper, [](lv_obj_t *btn, lv_event_t event) {
    static_view_pointer->event_cb(ACTION_SET_UPPER, btn, event);
  });

  lv_obj_t *go_to_lower = display->add_button(parent, NULL, 130, 30);
  lower_label = display->add_label(go_to_lower);
  lv_label_set_text(lower_label, "Go to lower");
  lv_obj_align(go_to_lower, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 5, -15);
  lv_obj_set_event_cb(go_to_lower, [](lv_obj_t *btn, lv_event_t event) {
    static_view_pointer->event_cb(ACTION_GO_TO_LOWER, btn, event);
  });
  lv_obj_t *go_to_upper = display->add_button(parent, NULL, 130, 30);
  upper_label = display->add_label(go_to_upper);
  lv_label_set_text(upper_label, "Go to upper");
  lv_obj_align(go_to_upper, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -5, -15);
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
  step_number_roller = lv_roller_create(parent, NULL);
  lv_roller_set_options(step_number_roller,
                        "10\n"
                        "20\n"
                        "30\n"
                        "50\n"
                        "100\n"
                        "200\n"
                        "300\n"
                        "500",
                        LV_ROLLER_MODE_NORMAL);

  lv_roller_set_visible_row_count(step_number_roller, 4);
  lv_obj_align(step_number_roller, NULL, LV_ALIGN_IN_LEFT_MID, 20, 0);

  lv_obj_t *fire_button = display->add_button(parent, "fire", 100, 100);
  lv_obj_align(fire_button, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_event_cb(fire_button, [](lv_obj_t *btn, lv_event_t event) {
    static_view_pointer->event_cb(ACTION_FIRE, btn, event);
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