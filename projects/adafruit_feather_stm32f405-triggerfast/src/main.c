/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <devicetree.h>

#include <drivers/gpio.h>
#include <drivers/i2c.h>
#include <drivers/pwm.h>


#define SLEEP_TIME_MS   1

#define LED0_PIN DT_GPIO_PIN(DT_ALIAS(led0), gpios)

#define PULSE_DEV_LABEL "GPIOB"
#define PULSE_PIN 9
/* #define PWM_CTLR    DT_PWMS_CTLR(PWM_LED0_NODE) */
/* #define PWM_CHANNEL DT_PWMS_CHANNEL(PWM_LED0_NODE) */
/* #define PWM_FLAGS   DT_PWMS_FLAGS(PWM_LED0_NODE) */

#define DIR_DEV_LABEL "GPIOC"
#define DIR_PIN 3

static const struct device *led0_dev;
static const struct device *pulse_dev;
static const struct device *dir_dev;

void pulse(int num) {
	// TODO: use pwm
	for (int i = 0; i < num; i++) {
		/* pwm_pin_set_usec(pulse_dev, PULSE_PIN, (USEC_PER_SEC), (USEC_PER_SEC) / 2U, 0); */

		gpio_pin_set(pulse_dev, PULSE_PIN, true);
		k_msleep(SLEEP_TIME_MS);
		gpio_pin_set(pulse_dev, PULSE_PIN, false);
		k_msleep(SLEEP_TIME_MS);
	}
}

void setDirection(bool direction) {
	gpio_pin_set(dir_dev, DIR_PIN, direction);

}

void setup() {
	int ret;

	led0_dev = device_get_binding(DT_GPIO_LABEL(DT_ALIAS(led0), gpios));
	if (led0_dev == NULL) {
		return;
	}
	pulse_dev = device_get_binding(PULSE_DEV_LABEL);
	if (pulse_dev == NULL) {
		return;
	}
	dir_dev = device_get_binding(DIR_DEV_LABEL);
	if (dir_dev == NULL) {
		return;
	}

	ret = gpio_pin_configure(led0_dev, LED0_PIN, GPIO_OUTPUT_ACTIVE | DT_GPIO_FLAGS(DT_ALIAS(led0), gpios));
	if (ret < 0) {
		return;
	}
	ret = gpio_pin_configure(pulse_dev, PULSE_PIN, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}
	ret = gpio_pin_configure(dir_dev, DIR_PIN, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}
}

void main(void) {
	bool led_is_on = true;
	setup();

	while (1) {
		gpio_pin_set(led0_dev, LED0_PIN, (int)led_is_on);
		setDirection(led_is_on);
		led_is_on = !led_is_on;

		pulse(1000);
	}
}
