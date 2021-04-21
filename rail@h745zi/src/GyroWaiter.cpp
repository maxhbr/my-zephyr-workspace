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


#include <lvgl.h>

#include <drivers/display.h>
#include <drivers/sensor.h>

#include "GyroWaiter.h"

GyroWaiter::GyroWaiter(struct k_sem *_threadRail_sem) {
    threadRail_sem = _threadRail_sem;
    label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

    const char *const mpu_label = DT_LABEL(DT_INST(0, invensense_mpu6050));
    mpu6050 = device_get_binding(mpu_label);
};
int GyroWaiter::wait() {
    char gyro_str[40] = {0};
    struct sensor_value gyro[3];

    double cyclic_buffer[num_of_samples * 3] = {
        1,1,1
    };
    int idx = 1;
    int count = 0;
    int bail_count = 40;
    int rc = 0;

    printk("wait for semathor...\n");
    k_sem_take(threadRail_sem, K_FOREVER);
    printk("wait for stall...\n");
    while (1) {
        rc = sensor_sample_fetch(mpu6050);
        if (rc == 0) {
            rc = sensor_channel_get(mpu6050, SENSOR_CHAN_GYRO_XYZ, gyro);
        }
        if (rc != 0) {
            break;
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

        printk("... %i:sum = %f ...\n", count, sum);
        if (sum < boundary) {
            printk("... stalled\n");
            break;
        }

        if (count > bail_count) {
            printk("... bail count exceeded: %i > %i\n", count, bail_count);
            rc = -1;
            break;
        }

        idx = (++idx) % num_of_samples;
        k_sleep(K_MSEC(sleep_msec));
        ++count;
    }
    k_sem_give(threadRail_sem);
    return rc;
};