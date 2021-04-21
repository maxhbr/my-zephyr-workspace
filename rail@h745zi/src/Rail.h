#ifndef RAIL_H
#define RAIL_H

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
    struct k_sem *threadRail_sem;
	int pos = 0;
	int target_pos = 0;
	lv_obj_t *label;
	int sleep_msec = 1;
	bool current_to_left = true;
	const struct device* led0_dev = device_get_binding(LED0);
	const struct device* led1_dev = device_get_binding(LED1);
	const struct device* pulse_dev = device_get_binding(PULSE);
	const struct device* dir_dev = device_get_binding(DIR);

	void set_dir(bool to_left);
	void step(bool to_left);
	bool is_in_pos();
	void run_to_target();
	void print_to_label();
public:
	Rail(struct k_sem *_threadRail_sem);
	void loop();
	int go(int relative);
};

#endif /* RAIL_H */
