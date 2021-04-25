#include "Display.h"
#include <logging/log.h>
LOG_MODULE_REGISTER(display);

static lv_style_t style_normal;
static const lv_font_t *font_title = &lv_font_montserrat_28;
static const lv_font_t *font_subtitle = &lv_font_montserrat_24;
static const lv_font_t *font_normal = &lv_font_montserrat_16;
static const lv_font_t *font_small = &lv_font_montserrat_12;

Display::Display() {
  display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);

  if (display_dev == NULL) {
    LOG_ERR("device not found...");
  }
  display_blanking_off(display_dev);

  lv_style_init(&style_normal);
  lv_style_set_text_font(&style_normal, LV_STATE_DEFAULT, font_normal);
  //   lv_obj_set_style_text_font(lv_scr_act(), font_normal, 0);

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
  lv_obj_align(secondary_label, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 80);
}
