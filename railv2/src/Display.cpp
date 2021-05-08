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

void Display::init_tileview(lv_obj_t *parent) {
  static lv_point_t valid_pos[] = {{0, 0}, {1, 0}};
  lv_obj_t *tileview;
  tileview = lv_tileview_create(parent, NULL);
  lv_tileview_set_valid_positions(tileview, valid_pos, 3);
  lv_tileview_set_edge_flash(tileview, true);

  lv_obj_t *tile0 = lv_obj_create(tileview, NULL);
  lv_obj_set_size(tile0, LV_HOR_RES, LV_VER_RES);
  lv_tileview_add_element(tileview, tile0);

  lv_obj_t *tile1 = lv_obj_create(tileview, NULL);
  lv_obj_set_size(tile1, LV_HOR_RES, LV_VER_RES);
  lv_tileview_add_element(tileview, tile1);
  lv_obj_set_pos(tile1, LV_HOR_RES, 0);

  index_to_tile.insert_or_assign(0, tile0);
  index_to_tile.insert_or_assign(1, tile1);
}

Display::Display(const struct device *display_dev) {
  LOG_INF("initialize display...");
  init_styles();
  init_tileview(lv_scr_act());
  LOG_INF("...initialize display done");
  display_blanking_off(display_dev);
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
  if (label_text != NULL) {
    lv_obj_t *label = lv_label_create(btn, NULL);
    lv_label_set_text(label, label_text);
    lv_obj_add_style(label, 0, &style_normal);
  }
  return btn;
}
