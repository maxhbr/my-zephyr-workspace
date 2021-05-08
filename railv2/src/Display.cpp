#include "Display.h"
#include <logging/log.h>
LOG_MODULE_REGISTER(display);

void Display::init_styles() {
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
}

void Display::init_tabview(lv_obj_t *parent) {
  tabview = lv_tabview_create(parent, NULL);
  lv_obj_align(tabview, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
  lv_obj_add_style(tabview, 0, &style_normal);
}

Display::Display(const struct device *display_dev) {
  LOG_INF("initialize display...");
  init_styles();
  init_tabview(lv_scr_act());
  LOG_INF("...initialize display done");
  display_blanking_off(display_dev);
}

lv_obj_t *Display::make_tab(const char *title) {
  return lv_tabview_add_tab(tabview, title);
}

lv_obj_t *Display::add_label(lv_obj_t *parent) {
  lv_obj_t *lbl = lv_label_create(parent, NULL);
  lv_obj_add_style(lbl, 0, &style_normal);
  return lbl;
}

lv_obj_t *Display::add_panel(lv_obj_t *parent) {
  lv_obj_t *panel = lv_obj_create(parent, NULL);
  lv_obj_add_style(panel, 0, &style_box);
  return panel;
}

lv_obj_t *Display::add_button(lv_obj_t *parent, const char *label_text,
                              int width, int heigth) {
  lv_obj_t *btn = lv_btn_create(parent, NULL);
  lv_obj_set_height(btn, heigth);
  lv_obj_set_width(btn, width);
  lv_obj_add_style(btn, 0, &style_button);
  lv_obj_t *label = lv_label_create(btn, NULL);
  lv_label_set_text(label, label_text);
  lv_obj_add_style(label, 0, &style_normal);
  return btn;
}
