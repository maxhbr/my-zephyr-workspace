#ifndef GYRO_WAITER_H
#define GYRO_WAITER_H
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

class GyroWaiter {
	const struct device *mpu6050;
    struct k_sem *threadRail_sem;
	lv_obj_t *label;
	int num_of_samples = 4;
	double boundary = 0.1 * num_of_samples;
	int sleep_msec = 50;
public:
	GyroWaiter(struct k_sem *_threadRail_sem);
	int wait();
};
#endif /* GYRO_WAITER_H */