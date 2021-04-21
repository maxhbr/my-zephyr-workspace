#ifndef RAIL_H
#define RAIL_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr.h>
#include <zephyr/types.h>

#include <device.h>
#include <init.h>
#include <kernel.h>
/* #include <net/buf.h> */
#include <sys/byteorder.h>
#include <sys/crc.h>
#include <sys/printk.h>

#include <console/console.h>

#include <lvgl.h>

#include <drivers/display.h>
#include <drivers/gpio.h>
#include <drivers/sensor.h>
// #include <drivers/led_strip.h>
// #include <drivers/spi.h>
// #include <drivers/uart.h>
// #include <usb/usb_device.h>
// #include <usb/class/usb_hid.h>
// #include <drivers/watchdog.h>

#include <logging/log.h>

#define LED0_NODE DT_ALIAS(led0)
#define LED0_LABEL DT_GPIO_LABEL(LED0_NODE, gpios)
#define LED0_PIN DT_GPIO_PIN(LED0_NODE, gpios)
#define LED0_FLAGS DT_GPIO_FLAGS(LED0_NODE, gpios)
#define LED1_NODE DT_ALIAS(led1)
#define LED1_LABEL DT_GPIO_LABEL(LED1_NODE, gpios)
#define LED1_PIN DT_GPIO_PIN(LED1_NODE, gpios)
#define LED1_FLAGS DT_GPIO_FLAGS(LED1_NODE, gpios)

#define PULSE_NODE DT_ALIAS(pulse)
#define PULSE DT_GPIO_LABEL(PULSE_NODE, gpios)
#define PULSE_PIN DT_GPIO_PIN(PULSE_NODE, gpios)
#define PULSE_FLAGS DT_GPIO_FLAGS(PULSE_NODE, gpios)
#define DIR_NODE DT_ALIAS(dir)
#define DIR DT_GPIO_LABEL(DIR_NODE, gpios)
#define DIR_PIN DT_GPIO_PIN(DIR_NODE, gpios)
#define DIR_FLAGS DT_GPIO_FLAGS(DIR_NODE, gpios)

class GPIO {
  const struct device *dev;
  int pin;
  int ret;

public:
  GPIO(char *label, int _pin, int flags) {
    LOG_MODULE_DECLARE(rail);
    pin = _pin;
    dev = device_get_binding(label);
    if (dev != NULL) {
      ret = gpio_pin_configure(dev, pin, GPIO_OUTPUT_ACTIVE | flags);
      if (ret != 0) {
        LOG_ERR("Failed to configure %s pin %i", label, pin);
      }
    } else {
      LOG_ERR("Failed to get device %s", label);
    }
  };
  void set(bool value) {
    LOG_MODULE_DECLARE(rail);
    if (ret == 0) {
      gpio_pin_set(dev, pin, value);

      LOG_INF("%s", value ? "true" : "false");
    } else {
      LOG_WRN("Do nothing, since ret=%i", ret);
    }
  };
};

class LED : public GPIO {
public:
  LED(char *label, int _pin, int flags) : GPIO(label, _pin, flags) {
    set(true);
    k_msleep(100);
    set(false);
  };
};

class Rail {
  struct k_sem *threadRail_sem;
  int pos = 0;
  int target_pos = 0;
  lv_obj_t *label;
  int sleep_msec = 1;
  bool current_to_left = true;

  LED *led0;
  LED *led1;
  /* const struct device* pulse_dev = device_get_binding(PULSE); */
  /* const struct device* dir_dev = device_get_binding(DIR); */

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
