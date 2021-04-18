/*
 * Copyright (c) 2018 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <device.h>
#include <drivers/display.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr.h>

#define LOG_LEVEL LOG_LEVEL_DEBUG
#include <logging/log.h>
LOG_MODULE_REGISTER(app);

void main(void)
{
	uint32_t count = 0U;
	char count_str[11] = {0};
	const struct device *display_dev;
	lv_obj_t *hello_world_label;
	lv_obj_t *count_label;

	display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);

	if (display_dev == NULL) {
		LOG_ERR("device not found.  Aborting test.");
		return;
	}

	if (IS_ENABLED(CONFIG_LVGL_POINTER_KSCAN)) {
		lv_obj_t *hello_world_button;

		hello_world_button = lv_btn_create(lv_scr_act(), NULL);
		lv_obj_align(hello_world_button, NULL, LV_ALIGN_CENTER, 0, 0);
		lv_btn_set_fit(hello_world_button, LV_FIT_TIGHT);
		hello_world_label = lv_label_create(hello_world_button, NULL);
	} else {
		hello_world_label = lv_label_create(lv_scr_act(), NULL);
	}
	LOG_ERR("1");

	lv_label_set_text(hello_world_label, "Hello world!");
	LOG_ERR("2");
	lv_obj_align(hello_world_label, NULL, LV_ALIGN_CENTER, 0, 0);
	LOG_ERR("3");

	count_label = lv_label_create(lv_scr_act(), NULL);
	LOG_ERR("4");
	lv_obj_align(count_label, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
	LOG_ERR("5");

	lv_task_handler();
	LOG_ERR("6");
	display_blanking_off(display_dev);
	LOG_ERR("7");

	while (1) {
		if ((count % 100) == 0U) {
			LOG_ERR("%d", count/100U);
			sprintf(count_str, "%d", count/100U);
			lv_label_set_text(count_label, count_str);
		}
		lv_task_handler();
		k_sleep(K_MSEC(10));
		++count;
	}
}
