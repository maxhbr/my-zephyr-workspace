/*
 * Copyright (c) 2018 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <device.h>
#include <drivers/display.h>
#include <drivers/sensor.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr.h>
#include <sys/printk.h>
#include <console/console.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(app);

static const struct device *setup_mpu6050() {
	const char *const label = DT_LABEL(DT_INST(0, invensense_mpu6050));
	const struct device *mpu6050 = device_get_binding(label);

	if (!mpu6050) {
		printf("Failed to find sensor %s\n", label);
		return mpu6050;
	}

	return mpu6050;
}

static int wait_for_stall(const struct device *dev, lv_obj_t *label) {
	char gyro_str[40] = {0};
	struct sensor_value gyro[3];

	double cyclic_buffer[12] = {
		1,1,1,
		1,1,1,
		1,1,1,
		1,1,1,
	};
	int idx = 0;
	int count = 0;
	int rc = 0;

	printk("wait for stall...\n");

	if (rc != 0) {
		return rc;
	}

	while (1) {
		rc = sensor_sample_fetch(dev);
		if (rc == 0) {
			rc = sensor_channel_get(dev, SENSOR_CHAN_GYRO_XYZ,
									gyro);
		}
		if (rc != 0) {
			return rc;
		}
		if (label) {
			sprintf(gyro_str, "%f\n%f\n%f",
					sensor_value_to_double(&gyro[0]),
					sensor_value_to_double(&gyro[1]),
					sensor_value_to_double(&gyro[2])
					);
			printk("... %s\n", gyro_str);
			lv_label_set_text(label, gyro_str);
			lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
		}

		cyclic_buffer[idx] = sensor_value_to_double(&gyro[0]);
		cyclic_buffer[idx + 1] = sensor_value_to_double(&gyro[1]);
		cyclic_buffer[idx + 2] = sensor_value_to_double(&gyro[2]);

		double sum = 0;
		for(int i=0; i<12; i++) {
			if (cyclic_buffer[i] > 0) {
				sum += cyclic_buffer[i];
			} else {
				sum -= cyclic_buffer[i];
			}
		}

		printk("... sum = %f ...\n", sum);
		if (sum < 0.4) {
			printk("... stalled\n");
			return rc;
		}

		if (count > 100) {
			printk("... failed\n");
			return -1;
		}

		idx = (idx + 3) % 12;
		k_sleep(K_MSEC(10));
		++count;
	}
}

void main(void)
{
	const struct device *display_dev;
	lv_obj_t *label;
	lv_obj_t *count_label;

	console_getline_init();


	const struct device *mpu6050 = setup_mpu6050();
	if (!mpu6050) {
		return;
	}


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
		label = lv_label_create(hello_world_button, NULL);
	} else {
		label = lv_label_create(lv_scr_act(), NULL);
	}

    count_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(count_label, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

	lv_task_handler();
	display_blanking_off(display_dev);

	while (1) {
		char *s = console_getline();
		printk("<-- %s\n", s);

		if (s[0] == 's') {
			int rc = wait_for_stall(mpu6050, label);
			if (rc != 0) {
				printk("!!! returned rc=%i", rc);
			}
		}

		int x = atoi(s);


		lv_task_handler();
		k_sleep(K_MSEC(10));
	}
}
