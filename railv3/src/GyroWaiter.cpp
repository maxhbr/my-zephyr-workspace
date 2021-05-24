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

#include <lvgl.h>

#include <drivers/sensor.h>

#include "GyroWaiter.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(gyro);

void GyroWaiter::init(){
  LOG_INF("gyro waiter init...");
  offset_0 = 0;
  offset_1 = 0;
  offset_2 = 0;
  if (mpu6050 == NULL) {
    return;
  }

  k_sleep(K_MSEC(500));
  int rc = 0;
  struct sensor_value gyro[3];
  int num_of_samples = 10;
  for(int idx = 1; idx < num_of_samples; ++idx) {
    rc = sensor_sample_fetch(mpu6050);
    if (rc == 0) {
      rc = sensor_channel_get(mpu6050, SENSOR_CHAN_GYRO_XYZ, gyro);
    }

    LOG_DBG("( %f , %f , %f )", sensor_value_to_double(&gyro[0]),
            sensor_value_to_double(&gyro[1]), sensor_value_to_double(&gyro[2]));

    offset_0 += sensor_value_to_double(&gyro[0]);
    offset_1 += sensor_value_to_double(&gyro[1]);
    offset_2 += sensor_value_to_double(&gyro[1]);

    k_sleep(K_MSEC(sleep_msec));
  }
  offset_0 = offset_0 / num_of_samples;
  offset_1 = offset_1 / num_of_samples;
  offset_2 = offset_2 / num_of_samples;
  LOG_INF("gyro waiter offsets: ( %f , %f , %f )", offset_0, offset_1, offset_2);
}

int GyroWaiter::wait() {
  if (mpu6050 == NULL) {
    return 0;
  }

  char gyro_str[40] = {0};
  struct sensor_value gyro[3];

  double cyclic_buffer[num_of_samples * 3] = {1, 1, 1};
  int idx = 1;
  int count = 0;
  int rc = 0;

  LOG_INF("wait for stall...\n");
  while (1) {
    rc = sensor_sample_fetch(mpu6050);
    if (rc == 0) {
      rc = sensor_channel_get(mpu6050, SENSOR_CHAN_GYRO_XYZ, gyro);
    }
    if (rc != 0) {
      break;
    }
    sprintf(gyro_str, "%f\n%f\n%f", sensor_value_to_double(&gyro[0]),
            sensor_value_to_double(&gyro[1]), sensor_value_to_double(&gyro[2]));
    LOG_DBG("... %s\n", gyro_str);
    if (label) {
      lv_label_set_text(label, gyro_str);
    }

    cyclic_buffer[idx * 3] = sensor_value_to_double(&gyro[0]) - offset_0;
    cyclic_buffer[idx * 3 + 1] = sensor_value_to_double(&gyro[1]) - offset_1;
    cyclic_buffer[idx * 3 + 2] = sensor_value_to_double(&gyro[2]) - offset_2;

    double sum = 0;
    for (int i = 0; i < num_of_samples * 3; i++) {
      if (cyclic_buffer[i] > 0) {
        sum += cyclic_buffer[i];
      } else {
        sum -= cyclic_buffer[i];
      }
    }

    LOG_DBG("... %i:sum = %f ...\n", count, sum);
    if (sum < boundary) {
      LOG_INF("... stalled @ sum = %f\n", sum);
      break;
    }

    if (count > bail_count) {
      LOG_WRN("... bail count exceeded: %i > %i\n", count, bail_count);
      rc = -1;
      break;
    }

    idx = (++idx) % num_of_samples;
    k_sleep(K_MSEC(sleep_msec));
    ++count;
  }
  return rc;
};
