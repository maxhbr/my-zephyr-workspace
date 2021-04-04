/*
 * Copyright (c) 2018 Google LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * This is mainly a parse test that verifies that Zephyr header files
 * compile in C++ mode.
 */

#include <string.h>
#include <zephyr.h>
#include <zephyr/types.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <init.h>
#include <device.h>
#include <kernel.h>
#include <net/buf.h>
#include <sys/byteorder.h>
#include <sys/crc.h>
#include <sys/crc.h>
#include <sys/printk.h>

#include <console/console.h>

#include <lvgl.h>

#include <drivers/display.h>
#include <drivers/sensor.h>
// #include <drivers/gpio.h>
// #include <drivers/led_strip.h>
// #include <drivers/spi.h>
// #include <drivers/uart.h>
// #include <usb/usb_device.h>
// #include <usb/class/usb_hid.h>
// #include <drivers/watchdog.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(app);

class GyroWaiter {
	const struct device *mpu6050;
	lv_obj_t *label;
	int num_of_samples = 4;
	double boundary = 0.1 * num_of_samples;
	int sleep_msec = 50;
	int bail_count = 40;
public:
	GyroWaiter() {
		label = lv_label_create(lv_scr_act(), NULL);
		lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

		const char *const mpu_label = DT_LABEL(DT_INST(0, invensense_mpu6050));
		mpu6050 = device_get_binding(mpu_label);
	};
	int wait() {
		char gyro_str[40] = {0};
		struct sensor_value gyro[3];

		double cyclic_buffer[num_of_samples * 3] = {
			1,1,1
		};
		int idx = 1;
		int count = 0;
		int rc = 0;

		printk("wait for stall...\n");
		while (1) {
			rc = sensor_sample_fetch(mpu6050);
			if (rc == 0) {
				rc = sensor_channel_get(mpu6050, SENSOR_CHAN_GYRO_XYZ, gyro);
			}
			if (rc != 0) {
				return rc;
			}
			sprintf(gyro_str, "%f\n%f\n%f",
					sensor_value_to_double(&gyro[0]),
					sensor_value_to_double(&gyro[1]),
					sensor_value_to_double(&gyro[2])
					);
			printk("... %s\n", gyro_str);
			if (label) {
				lv_label_set_text(label, gyro_str);
				lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
			}

			cyclic_buffer[idx * 3] = sensor_value_to_double(&gyro[0]);
			cyclic_buffer[idx * 3 + 1] = sensor_value_to_double(&gyro[1]);
			cyclic_buffer[idx * 3 + 2] = sensor_value_to_double(&gyro[2]);

			double sum = 0;
			for(int i=0; i < num_of_samples * 3; i++) {
				if (cyclic_buffer[i] > 0) {
					sum += cyclic_buffer[i];
				} else {
					sum -= cyclic_buffer[i];
				}
			}

			printk("... sum = %f ...\n", sum);
			if (sum < boundary) {
				printk("... stalled\n");
				return rc;
			}

			if (count > bail_count) {
				printk("... failed\n");
				return -1;
			}

			idx = (++idx) % num_of_samples;
			k_sleep(K_MSEC(sleep_msec));
			++count;
		}
	};
};

class Rail {
	int pos = 0;
	int target_pos = 0;
	lv_obj_t *label;
	int sleep_msec = 1;
	bool current_to_left = true;

	void print_to_label() {
		char pos_str[11] = {0};
		sprintf(pos_str, "%i --> %i", target_pos, pos);
		lv_label_set_text(label, pos_str);
		lv_task_handler();
	}
	void flip_dir() {
		current_to_left = !current_to_left;
	};
	void step(bool to_left) {
		if (to_left != current_to_left) {
			flip_dir();
		}
		if (to_left) {
			++pos;
		} else {
			--pos;
		}
		print_to_label();
	};
public:
	Rail() {
		label = lv_label_create(lv_scr_act(), NULL);
		lv_obj_align(label, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
		print_to_label();
	};
	int go(int relative) {
		target_pos += relative;
		print_to_label();
		return pos;
	};
	void run() {
		while (1) {
			if (pos != target_pos) {
				run_to_target();
			}
			// k_sleep(K_MSEC(sleep_msec));
		}
	};
	void run_to_target() {
		while(1) {
			if (pos < target_pos) {
				step(true);
			} else if (pos > target_pos) {
				step(false);
			} else {
				break;
			}
			k_sleep(K_MSEC(sleep_msec));
		}
	}
};

void display_init() {
	const struct device *display_dev;
	display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);

	if (display_dev == NULL) {
		LOG_ERR("device not found.  Aborting test.");
		return;
	}
	display_blanking_off(display_dev);
}

#define CONSOLE_HELP  \
    "help\n"  \
    "----\n"  \
    "   s -> do a single shot\n" \
    "   $int -> move relative\n"


void main(void) {
	int rc = 0;

	display_init();
	console_getline_init();
	GyroWaiter waiter;
	Rail rail;

	lv_task_handler();

	printk(CONSOLE_HELP);
	while (1) {
		char *s = console_getline();
		printk("<-- %s\n", s);

		if (s[0] == 's') {
			waiter.wait();
			if (rc != 0) {
				printk("!!! returned rc=%i", rc);
			}
		} else if (s[0] == 'r') {
			rail.run_to_target();
		} else {
			int x = atoi(s);
			rail.go(x);
		}

		lv_task_handler();
		k_sleep(K_MSEC(10));
	}
}
