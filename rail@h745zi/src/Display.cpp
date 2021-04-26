#include "Display.h"
#include <logging/log.h>
LOG_MODULE_REGISTER(display);

static lv_style_t style_normal;
static lv_style_t style_button;
static lv_style_t style_box;
static const lv_font_t *font_title = &lv_font_montserrat_28;
static const lv_font_t *font_subtitle = &lv_font_montserrat_24;
static const lv_font_t *font_normal = &lv_font_montserrat_16;
static const lv_font_t *font_small = &lv_font_montserrat_12;

lv_obj_t* addButton(lv_obj_t *parent, const char *label_text, int width) {
  lv_obj_t * btn = lv_btn_create(parent, NULL);
  lv_obj_set_height(btn, 40);
  lv_obj_set_width(btn, width);
  lv_obj_add_style(btn, 0, &style_button);
  lv_obj_t * label = lv_label_create(btn, NULL);
  lv_label_set_text(label, label_text);
  lv_obj_add_style(label, 0, &style_normal);
  return btn;
}

void fillMenuBar(lv_obj_t *panel) {
  lv_obj_t * l_left_btn = addButton(panel, "<<<", 60);
  lv_obj_align(l_left_btn, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 2);
  lv_obj_t * left_btn = addButton(panel, "<<", 60);
  lv_obj_align(left_btn, NULL, LV_ALIGN_IN_TOP_LEFT, 75, 2);
  lv_obj_t * fire_btn = addButton(panel, "<>", 40);
  lv_obj_align(fire_btn, NULL, LV_ALIGN_IN_TOP_MID, 0, 2);
  lv_obj_t * right_btn = addButton(panel, ">>", 60);
  lv_obj_align(right_btn, NULL, LV_ALIGN_IN_TOP_RIGHT, -75, 2);
  lv_obj_t * r_right_btn = addButton(panel, ">>>", 60);
  lv_obj_align(r_right_btn, NULL, LV_ALIGN_IN_TOP_RIGHT, -5, 2);

  lv_obj_t * slider1 = lv_slider_create(panel, NULL);
}

Display::Display() {
  display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);

  if (display_dev == NULL) {
    LOG_ERR("device not found...");
  }
  display_blanking_off(display_dev);

  lv_style_init(&style_normal);
  lv_style_set_text_font(&style_normal, LV_STATE_DEFAULT, font_normal);

  lv_style_init(&style_button);
  lv_style_set_bg_color(&style_button, LV_STATE_DEFAULT, LV_COLOR_GRAY);
  lv_style_set_bg_color(&style_button, LV_STATE_PRESSED, LV_COLOR_RED);
  lv_style_set_radius(&style_button, LV_STATE_DEFAULT, 5);
  lv_style_set_bg_opa(&style_button, LV_STATE_DEFAULT, LV_OPA_COVER);

  lv_style_init(&style_box);
  lv_style_set_bg_color(&style_box, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_style_set_bg_opa(&style_box, LV_STATE_DEFAULT, LV_OPA_COVER);

  lv_obj_t * panel1 = lv_obj_create(lv_scr_act(), NULL);
  lv_obj_align(panel1, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
  lv_obj_set_height(panel1, 70);
  lv_obj_set_width(panel1, 320);
//   lv_obj_set_width(panel1, LV_PCT(100));

  position_label = lv_label_create(panel1, NULL);
  lv_obj_align(position_label, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 20);
  lv_obj_add_style(position_label, 0, &style_normal);
  target_position_label = lv_label_create(panel1, NULL);
  lv_obj_align(target_position_label, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 50);
  lv_obj_add_style(target_position_label, 0, &style_normal);

  secondary_label = lv_label_create(lv_scr_act(), NULL);
  lv_obj_add_style(secondary_label, 0, &style_normal);
  lv_obj_align(secondary_label, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 180);

  lv_obj_t * panel3 = lv_obj_create(lv_scr_act(), NULL);
  lv_obj_align(panel3, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 70);
  lv_obj_set_height(panel3, 70);
  lv_obj_set_width(panel3, 320);
  lv_obj_add_style(panel3, 0, &style_box);
  fillMenuBar(panel3);
}
