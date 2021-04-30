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

lv_obj_t *add_button(lv_obj_t *parent, const char *label_text, int width) {
  lv_obj_t *btn = lv_btn_create(parent, NULL);
  lv_obj_set_height(btn, 60);
  lv_obj_set_width(btn, width);
  lv_obj_add_style(btn, 0, &style_button);
  lv_obj_t *label = lv_label_create(btn, NULL);
  lv_label_set_text(label, label_text);
  lv_obj_add_style(label, 0, &style_normal);
  return btn;
}

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

void Display::fill_nav_panel(lv_obj_t *panel) {
  lv_obj_t *l_left_btn = add_button(panel, "<<<", 60);
  lv_obj_align(l_left_btn, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 2);
  lv_obj_t *left_btn = add_button(panel, "<<", 60);
  lv_obj_align(left_btn, NULL, LV_ALIGN_IN_TOP_LEFT, 75, 2);
  lv_obj_t *right_btn = add_button(panel, ">>", 60);
  lv_obj_align(right_btn, NULL, LV_ALIGN_IN_TOP_RIGHT, -75, 2);
  lv_obj_t *r_right_btn = add_button(panel, ">>>", 60);
  lv_obj_align(r_right_btn, NULL, LV_ALIGN_IN_TOP_RIGHT, -5, 2);

  lv_obj_t *slider = lv_slider_create(panel, NULL);
  lv_obj_align(slider, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -10);
  lv_slider_set_range(slider, 0, 100);
  lv_slider_set_value(slider, 50, 0);
  // lv_obj_set_width(slider, LV_PCT(95))
}

void Display::init_coarse_tab(lv_obj_t *parent) {
  lv_obj_t *panel_nav = lv_obj_create(parent, NULL);
  lv_obj_align(panel_nav, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 30);
  lv_obj_set_height(panel_nav, 90);
  lv_obj_set_width(panel_nav, 320);
  lv_obj_add_style(panel_nav, 0, &style_box);
  fill_nav_panel(panel_nav);

  lv_obj_t *header = lv_obj_create(parent, NULL);
  lv_obj_align(header, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
  lv_obj_set_height(header, 20);
  lv_obj_set_width(header, 320);
  //   lv_obj_set_width(header, LV_PCT(100));

  position_label = lv_label_create(header, NULL);
  lv_obj_align(position_label, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 2);
  lv_obj_add_style(position_label, 0, &style_normal);
  target_position_label = lv_label_create(header, NULL);
  lv_obj_align(target_position_label, NULL, LV_ALIGN_IN_TOP_MID, 20, 2);
  lv_obj_add_style(target_position_label, 0, &style_normal);
}

Display::Display(const struct device *display_dev) {
    LOG_INF("initialize display...");

    init_styles();

    /*Create a Tab view object*/
    lv_obj_t *tabview;
    tabview = lv_tabview_create(lv_scr_act(), NULL);
    lv_obj_align(tabview, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
    lv_obj_add_style(tabview, 0, &style_normal);

    /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
    lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "coarse");
    lv_obj_t *tab2 = lv_tabview_add_tab(tabview, "fine");
    lv_obj_t *tab3 = lv_tabview_add_tab(tabview, "go");

    init_coarse_tab(tab1);

    secondary_label = lv_label_create(tab3, NULL);
    lv_obj_add_style(secondary_label, 0, &style_normal);
    lv_obj_align(secondary_label, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 180);
    LOG_INF("...initialize display done");

    lv_task_handler();
    display_blanking_off(display_dev);
}
