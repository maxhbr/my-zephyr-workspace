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

/* size of stack area used by each thread */
#define STACKSIZE 8192
/* scheduling priority used by each thread */
#define PRIORITY 7
/* delay between updates (in ms) */
#define SLEEPTIME 500

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

K_SEM_DEFINE(threadRail_sem, 1, 1);
class Rail {
	int pos = 0;
	int target_pos = 0;
	lv_obj_t *label;
	int sleep_msec = 1;
	bool current_to_left = true;

	void flip_dir() {
		current_to_left = !current_to_left;
	};
	void step(bool to_left) {
		k_sem_take(&threadRail_sem, K_FOREVER);
		if (to_left != current_to_left) {
			flip_dir();
		}
		if (to_left) {
			++pos;
		} else {
			--pos;
		}
		k_sem_give(&threadRail_sem);
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
	bool is_in_pos() {
		return pos == target_pos;
	}
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
	void print_to_label() {
		char pos_str[21] = {0};
		sprintf(pos_str, "%i > %i", target_pos, pos);
		lv_label_set_text(label, pos_str);
		lv_task_handler();
	}
};

K_THREAD_STACK_DEFINE(threadRail_stack_area, STACKSIZE);
static struct k_thread threadRail_data;
void threadRail(void *railV, void *dummy2, void *dummy3)
{
	ARG_UNUSED(dummy2);
	ARG_UNUSED(dummy3);
	Rail* rail = static_cast<Rail *>(railV);

	while (1) {
		if (! rail->is_in_pos()) {
			rail->run_to_target();
		}
		rail->print_to_label();
		k_sleep(K_MSEC(10));
		// k_sleep(K_MSEC(sleep_msec));
	}
}

#define CONSOLE_HELP  \
    "help\n"  \
    "----\n"  \
    "   s -> do a single shot\n" \
    "   $int -> move relative\n"
K_THREAD_STACK_DEFINE(threadConsole_stack_area, STACKSIZE);
static struct k_thread threadConsole_data;
void threadConsole(void *railV, void *waiterV, void *dummy3) {
	ARG_UNUSED(dummy3);
	Rail* rail = static_cast<Rail *>(railV);
	GyroWaiter* waiter = static_cast<GyroWaiter *>(waiterV);
	int rc = 0;

	printk(CONSOLE_HELP);
	while (1) {
		char *s = console_getline();
		printk("<-- %s\n", s);

		if (s[0] == 's') {
			rc = waiter->wait();
			if (rc != 0) {
				printk("!!! returned rc=%i", rc);
			}
		} else {
			int x = atoi(s);
			rail->go(x);
		}

		lv_task_handler();
		k_sleep(K_MSEC(10));
	}
}

void display_init() {
	const struct device *display_dev;
	display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);

	if (display_dev == NULL) {
		LOG_ERR("device not found.  Aborting test.");
		return;
	}
	display_blanking_off(display_dev);
}


void main(void) {

	display_init();
	console_getline_init();
	GyroWaiter waiter;
	Rail rail;
	lv_task_handler();

	k_tid_t my_tid_console = k_thread_create(&threadConsole_data, threadConsole_stack_area,
			K_THREAD_STACK_SIZEOF(threadConsole_stack_area),
			threadConsole, &rail, &waiter, NULL,
			PRIORITY, 0, K_NO_WAIT);
	k_thread_name_set(&threadConsole_data, "thread_r");

	k_tid_t my_tid_rail = k_thread_create(&threadRail_data, threadRail_stack_area,
			K_THREAD_STACK_SIZEOF(threadRail_stack_area),
			threadRail, &rail, NULL, NULL,
			PRIORITY, 0, K_NO_WAIT);
	k_thread_name_set(&threadRail_data, "thread_r");

	k_thread_start(&threadConsole_data);
	k_thread_start(&threadRail_data);
}
