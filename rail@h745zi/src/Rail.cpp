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

#include "Rail.h"

void Rail::set_dir(bool to_left) {
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
void Rail::step(bool to_left) {
    k_sem_take(threadRail_sem, K_FOREVER);
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
    k_sem_give(threadRail_sem);
    print_to_label();
};
bool Rail::is_in_pos() {
    return pos == target_pos;
}
void Rail::run_to_target() {
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
void Rail::print_to_label() {
    char pos_str[21] = {0};
    sprintf(pos_str, "%i > %i", target_pos, pos);
    lv_label_set_text(label, pos_str);
    lv_task_handler();
}
Rail::Rail(struct k_sem *_threadRail_sem) {
    threadRail_sem = _threadRail_sem;
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
void Rail::loop() {
    while (1) {
        if (! is_in_pos()) {
            run_to_target();
        }
        print_to_label();
        k_sleep(K_MSEC(sleep_msec));
    }
}
int Rail::go(int relative) {
    target_pos += relative;
    print_to_label();
    return pos;
};
