/*
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>
#include <sys/util.h>
#include <string.h>
#include <usb/usb_device.h>
#include <drivers/uart.h>
#include <device.h>
#include <drivers/gpio.h>
#include <inttypes.h>
#include <console/console.h>

#define SLEEP_TIME_MS	1

#define LED0_NODE	DT_ALIAS(led0)
#define LED0_GPIO_LABEL	DT_GPIO_LABEL(LED0_NODE, gpios)
#define LED0_GPIO_PIN	DT_GPIO_PIN(LED0_NODE, gpios)
#define LED0_GPIO_FLAGS	(GPIO_OUTPUT | DT_GPIO_FLAGS(LED0_NODE, gpios))
class LED0 {
	const struct device *led;
public:
	LED0() {
		led = device_get_binding(LED0_GPIO_LABEL);
		gpio_pin_configure(led, LED0_GPIO_PIN, LED0_GPIO_FLAGS);
	};
	void setVal(bool val) {
		gpio_pin_set(led, LED0_GPIO_PIN, val);
	};
};

void button_pressed(const struct device *dev, struct gpio_callback *cb,
		uint32_t pins)
{
	printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
};

#define SW0_NODE	DT_ALIAS(sw0)
#define SW0_GPIO_LABEL	DT_GPIO_LABEL(SW0_NODE, gpios)
#define SW0_GPIO_PIN	DT_GPIO_PIN(SW0_NODE, gpios)
#define SW0_GPIO_FLAGS	(GPIO_INPUT | DT_GPIO_FLAGS(SW0_NODE, gpios))
class BUTTON : public LED0 {
	const struct device *button;
	struct gpio_callback button_cb_data;
public:
	BUTTON() : LED0() {
		button = device_get_binding(SW0_GPIO_LABEL);
		gpio_pin_configure(button, SW0_GPIO_PIN, SW0_GPIO_FLAGS);
		gpio_pin_interrupt_configure(button,
						SW0_GPIO_PIN,
						GPIO_INT_EDGE_TO_ACTIVE);

		gpio_init_callback(&button_cb_data, button_pressed, BIT(SW0_GPIO_PIN));
		gpio_add_callback(button, &button_cb_data);
	};
	void match_led_to_button() {
		bool val;
		val = gpio_pin_get(button, SW0_GPIO_PIN);
		setVal(val);
	};
};

void initConsole() {
	const struct device *dev = device_get_binding(
			CONFIG_UART_CONSOLE_ON_DEV_NAME);
	uint32_t dtr = 0;

	if (usb_enable(NULL)) {
		return;
	}

	/* Poll if the DTR flag was set, optional */
	while (!dtr) {
		uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
	}

	if (strlen(CONFIG_UART_CONSOLE_ON_DEV_NAME) !=
	    strlen("CDC_ACM_0") ||
	    strncmp(CONFIG_UART_CONSOLE_ON_DEV_NAME, "CDC_ACM_0",
		    strlen(CONFIG_UART_CONSOLE_ON_DEV_NAME))) {
		printk("Error: Console device name is not USB ACM\n");

		return;
	}
}

void main(void)
{
	initConsole();

	BUTTON button;
	printk("Press the button\n");

	while (1) {
		button.match_led_to_button();
		// printk("Hello World! %s\n", CONFIG_ARCH);
		k_msleep(SLEEP_TIME_MS);
	}
}
