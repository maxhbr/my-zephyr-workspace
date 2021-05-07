#include "GPIOs.h"
#include <logging/log.h>
LOG_MODULE_REGISTER(gpios);

GPIO::GPIO(const char *label, int _pin, int flags) {
  LOG_MODULE_DECLARE(gpios);
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
}
void GPIO::set(bool value) {
  LOG_MODULE_DECLARE(gpios);
  if (ret == 0) {
    if (value != cur_value) {
      cur_value = value;

      gpio_pin_set(dev, pin, value);
    }
  } else {
    LOG_WRN("Do nothing, since ret=%i", ret);
  }
}
