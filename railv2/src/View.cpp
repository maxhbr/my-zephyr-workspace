#include "View.h"

#include <functional>

void my_task(lv_task_t *task) {
  CurrentPositionView *cpv =
      static_cast<CurrentPositionView *>(task->user_data);
  cpv->update();
}

CurrentPositionView::CurrentPositionView() {
  lv_task_t *task = lv_task_create(my_task, 500, LV_TASK_PRIO_MID, this);
};

void CurrentPositionView::update() {
  if (cur_pos_label != NULL) {
    char cur_str[11] = {0};
    sprintf(cur_str, "%d", model->get_cur_position());
    lv_label_set_text(cur_pos_label, cur_str);
  }
  if (target_pos_label != NULL) {
    char target_str[11] = {0};
    sprintf(target_str, "%d", model->get_target_position());
    lv_label_set_text(target_pos_label, target_str);
  }
}

static View *static_view_pointer = NULL;

View::View(Display *_display, Stepper *_stepper) {
  display = _display;

  static_view_pointer = this;

  model = new Model(_stepper);
  controller = new Controller(model);

  lv_obj_t *coarse_tab = display->make_tab("coarse");
  fill_coarse_panel(coarse_tab);
  display->make_tab("fine");
  display->make_tab("go");
};

static void static_event_cb(lv_obj_t *obj, lv_event_t event) {
  static_view_pointer->event_cb(obj, event);
}

void View::event_cb(lv_obj_t *obj, lv_event_t event) {
  if (event != LV_EVENT_PRESSED) {
    return;
  }
  int dist = button_to_dist.at(obj);
  printf("go: %i\n", dist);
  controller->go(dist);
}

void View::register_button_to_dist(lv_obj_t *btn, int dist) {
  button_to_dist.insert_or_assign(btn, dist);
  lv_obj_set_event_cb(btn, static_event_cb);
};

void View::fill_nav_panel(lv_obj_t *panel) {
  lv_obj_t *l_left_btn = display->add_button(panel, "<<<", 60);
  lv_obj_align(l_left_btn, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 2);
  register_button_to_dist(l_left_btn, -12800);
  lv_obj_t *left_btn = display->add_button(panel, "<<", 60);
  lv_obj_align(left_btn, NULL, LV_ALIGN_IN_TOP_LEFT, 75, 2);
  register_button_to_dist(left_btn, -1280);
  lv_obj_t *right_btn = display->add_button(panel, ">>", 60);
  lv_obj_align(right_btn, NULL, LV_ALIGN_IN_TOP_RIGHT, -75, 2);
  register_button_to_dist(right_btn, 1280);
  lv_obj_t *r_right_btn = display->add_button(panel, ">>>", 60);
  lv_obj_align(r_right_btn, NULL, LV_ALIGN_IN_TOP_RIGHT, -5, 2);
  register_button_to_dist(r_right_btn, 12800);

  //   lv_obj_t *slider = lv_slider_create(panel, NULL);
  //   lv_obj_align(slider, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -10);
  //   lv_slider_set_range(slider, 0, 100);
  //   lv_slider_set_value(slider, 50, 0);
  //   // lv_obj_set_width(slider, LV_PCT(95))
}

void View::fill_coarse_panel(lv_obj_t *parent) {
  lv_obj_t *panel_nav = display->add_panel(parent);
  lv_obj_align(panel_nav, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 30);
  lv_obj_set_height(panel_nav, 90);
  lv_obj_set_width(panel_nav, 320);
  fill_nav_panel(panel_nav);

  lv_obj_t *header = lv_obj_create(parent, NULL);
  lv_obj_align(header, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
  lv_obj_set_height(header, 20);
  lv_obj_set_width(header, 320);
  //   lv_obj_set_width(header, LV_PCT(100));

  lv_obj_t *label = display->add_label(header);
  lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 2);
  current_position_view.set_cur_pos_label(label);
  lv_obj_t *target_label = display->add_label(header);
  lv_obj_align(target_label, NULL, LV_ALIGN_IN_TOP_MID, 20, 2);
  current_position_view.set_target_pos_label(target_label);
}