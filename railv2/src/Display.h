#ifndef __DISPLAY_H_
#define __DISPLAY_H_

#include <drivers/display.h>
#include <lvgl.h>
#include <map>

class Display {
  void init_styles();
  lv_obj_t *tabview;
  void init_tabview(lv_obj_t *parent);
  std::map<int, lv_obj_t *> index_to_tile;
  void init_tileview(lv_obj_t *parent);

  const lv_font_t *font_title = &lv_font_montserrat_28;
  const lv_font_t *font_subtitle = &lv_font_montserrat_24;
  const lv_font_t *font_normal = &lv_font_montserrat_16;
  const lv_font_t *font_small = &lv_font_montserrat_12;

public:
  Display(const struct device *display_dev);

  lv_style_t style_normal;
  lv_style_t style_button;
  lv_style_t style_box;

  lv_obj_t *make_tab(const char *title);
  lv_obj_t *get_tile(int index) {
    return index_to_tile.at(index);
  };
  lv_obj_t *add_label(lv_obj_t *parent);
  lv_obj_t *add_panel(lv_obj_t *parent);
  lv_obj_t *add_button(lv_obj_t *parent, const char *label_text, int width, int heigth);
};

#endif // __DISPLAY_H_
