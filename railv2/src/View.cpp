#include "View.h"

void View::fill_nav_panel(lv_obj_t *panel) {
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

  lv_obj_t *slider = lv_slider_create(panel, NULL);
  lv_obj_align(slider, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -10);
  lv_slider_set_range(slider, 0, 100);
  lv_slider_set_value(slider, 50, 0);
  // lv_obj_set_width(slider, LV_PCT(95))
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
  current_position_view.set_target_pos_label(label);
}