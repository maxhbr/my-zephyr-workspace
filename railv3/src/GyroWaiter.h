#ifndef GYRO_WAITER_H
#define GYRO_WAITER_H
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr.h>
#include <zephyr/types.h>

#include <device.h>
#include <init.h>
#include <kernel.h>
#include <net/buf.h>
#include <sys/byteorder.h>
#include <sys/crc.h>
#include <sys/printk.h>

#include <console/console.h>

#include <lvgl.h>

#include <drivers/gpio.h>
#include <drivers/sensor.h>

#define MPU6050_LABEL DT_LABEL(DT_INST(0, invensense_mpu6050))

class GyroWaiter {
  const struct device *mpu6050 = device_get_binding(MPU6050_LABEL);
  lv_obj_t *label;
  int num_of_samples = 4;
  double boundary = 0.1 * num_of_samples;
  int bail_count = 400;
  int sleep_msec = 50;

public:
  GyroWaiter(){};
  int wait();
};
#endif /* GYRO_WAITER_H */
