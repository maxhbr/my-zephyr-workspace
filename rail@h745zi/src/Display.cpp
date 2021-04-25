#include "Display.h"
#include <logging/log.h>
LOG_MODULE_REGISTER(display);

static const lv_font_t *font_subtitle;
static const lv_font_t *font_normal;
static const lv_font_t *font_small;

Display::Display() {
  display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);

  if (display_dev == NULL) {
    LOG_ERR("device not found...");
  }
  display_blanking_off(display_dev);

    font_subtitle  =  &lv_font_montserrat_24;
    font_normal    =  &lv_font_montserrat_16;
    font_small     = &lv_font_montserrat_12;
  //   // lv_theme_default_init(NULL, *palette_primary, palette_secondary,
  //   false, font_small, font_normal, font_subtitle);

  positionLabel = lv_label_create(lv_scr_act(), NULL);
  lv_obj_align(positionLabel, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 20);
  secondaryLabel = lv_label_create(lv_scr_act(), NULL);
  lv_obj_align(secondaryLabel, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
}
