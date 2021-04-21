/*
 * Copyright (c) 2018 Google LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
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
#include <drivers/gpio.h>
// #include <drivers/led_strip.h>
// #include <drivers/spi.h>
// #include <drivers/uart.h>
// #include <usb/usb_device.h>
// #include <usb/class/usb_hid.h>
// #include <drivers/watchdog.h>

#include "GyroWaiter.h"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(app);

/* size of stack area used by each thread */
#define STACKSIZE 8192
/* scheduling priority used by each thread */
#define PRIORITY 7
/* delay between updates (in ms) */
#define SLEEPTIME 500

K_SEM_DEFINE(threadRail_sem, 1, 1);

#define LED0_NODE DT_ALIAS(led0)
#define LED0	DT_GPIO_LABEL(LED0_NODE, gpios)
#define LED0_PIN	DT_GPIO_PIN(LED0_NODE, gpios)
#define LED0_FLAGS	DT_GPIO_FLAGS(LED0_NODE, gpios)
#define LED1_NODE DT_ALIAS(led1)
#define LED1	DT_GPIO_LABEL(LED1_NODE, gpios)
#define LED1_PIN	DT_GPIO_PIN(LED1_NODE, gpios)
#define LED1_FLAGS	DT_GPIO_FLAGS(LED1_NODE, gpios)

#define PULSE_NODE DT_ALIAS(pulse)
#define PULSE	DT_GPIO_LABEL(PULSE_NODE, gpios)
#define PULSE_PIN	DT_GPIO_PIN(PULSE_NODE, gpios)
#define PULSE_FLAGS	DT_GPIO_FLAGS(PULSE_NODE, gpios)
#define DIR_NODE DT_ALIAS(dir)
#define DIR	DT_GPIO_LABEL(DIR_NODE, gpios)
#define DIR_PIN	DT_GPIO_PIN(DIR_NODE, gpios)
#define DIR_FLAGS	DT_GPIO_FLAGS(DIR_NODE, gpios)
class Rail {
	int pos = 0;
	int target_pos = 0;
	lv_obj_t *label;
	int sleep_msec = 1;
	bool current_to_left = true;
	const struct device* led0_dev = device_get_binding(LED0);
	const struct device* led1_dev = device_get_binding(LED1);
	const struct device* pulse_dev = device_get_binding(PULSE);
	const struct device* dir_dev = device_get_binding(DIR);

	void flip_dir() {
		current_to_left = !current_to_left;
	};
	void set_dir(bool to_left) {
		if (to_left) {
			gpio_pin_set(led0_dev, LED0_PIN, true);
			gpio_pin_set(led1_dev, LED1_PIN, false);
		} else {
			gpio_pin_set(led0_dev, LED0_PIN, false);
			gpio_pin_set(led1_dev, LED1_PIN, true);
		}
		if (to_left != current_to_left) {
			// gpio_pin_set(dir_dev, DIR_PIN, to_left);
			current_to_left = to_left;
		}

	}
	void step(bool to_left) {
		k_sem_take(&threadRail_sem, K_FOREVER);
		set_dir(to_left);
		// gpio_pin_set(pulse_dev, PULSE_PIN, true);
		// k_sleep(K_MSEC(10));
		// gpio_pin_set(pulse_dev, PULSE_PIN, false);
		// k_sleep(K_MSEC(10));
		if (to_left) {
			++pos;
		} else {
			--pos;
		}
		k_sem_give(&threadRail_sem);
		print_to_label();
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
				gpio_pin_set(led0_dev, LED0_PIN, false);
				gpio_pin_set(led1_dev, LED1_PIN, false);
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
public:
	Rail() {
		if (led0_dev != NULL) {
			int ret = gpio_pin_configure(led0_dev, LED0_PIN, GPIO_OUTPUT_ACTIVE | LED0_FLAGS);
			if (ret == 0) {
				gpio_pin_set(led0_dev, LED0_PIN, true);
				k_msleep(100);
				gpio_pin_set(led0_dev, LED0_PIN, false);
			}
		}
		if (led1_dev != NULL) {
			int ret = gpio_pin_configure(led1_dev, LED1_PIN, GPIO_OUTPUT_ACTIVE | LED1_FLAGS);
			if (ret == 0) {
				gpio_pin_set(led1_dev, LED1_PIN, true);
				k_msleep(100);
				gpio_pin_set(led1_dev, LED1_PIN, false);
			}
		}

		// if (pulse_dev != NULL) {
		// 	int ret = gpio_pin_configure(pulse_dev, PULSE_PIN, GPIO_OUTPUT_ACTIVE | PULSE_FLAGS);
		// }
		// if (dir_dev != NULL) {
		// 	int ret = gpio_pin_configure(dir_dev, DIR_PIN, GPIO_OUTPUT_ACTIVE | DIR_FLAGS);
		// }



		label = lv_label_create(lv_scr_act(), NULL);
		lv_obj_align(label, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
	};
	void loop() {
		while (1) {
			if (! is_in_pos()) {
				run_to_target();
			}
			print_to_label();
			k_sleep(K_MSEC(sleep_msec));
		}
	}
	int go(int relative) {
		target_pos += relative;
		print_to_label();
		return pos;
	};
};

K_THREAD_STACK_DEFINE(threadRail_stack_area, STACKSIZE);
static struct k_thread threadRail_data;
void threadRail(void *railV, void *dummy2, void *dummy3)
{
	ARG_UNUSED(dummy2);
	ARG_UNUSED(dummy3);
	Rail* rail = static_cast<Rail *>(railV);
	rail->loop();
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
	GyroWaiter waiter(&threadRail_sem);
	Rail rail;

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
