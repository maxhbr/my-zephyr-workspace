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

  positionLabel = lv_label_create(lv_scr_act(), NULL);
  lv_obj_align(positionLabel, NULL, LV_ALIGN_IN_TOP_MID, 20, 20);
  lv_obj_add_style(positionLabel, 0, &style_normal);

  secondaryLabel = lv_label_create(lv_scr_act(), NULL);
  lv_obj_add_style(secondaryLabel, 0, &style_normal);
  lv_obj_align(secondaryLabel, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
}
