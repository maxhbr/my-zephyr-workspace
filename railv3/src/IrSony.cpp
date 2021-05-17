#include "IrSony.h"

/*
   contains parts from:

   ATtiny85 Sony NEX/Alpha Remote Control

   David Johnson-Davies - www.technoblogy.com - 12th April 2015
   ATtiny85 @ 1 MHz (internal oscillator; BOD disabled)

   CC BY 4.0
   Licensed under a Creative Commons Attribution 4.0 International license:
   http://creativecommons.org/licenses/by/4.0/
*/

#include <logging/log.h>
LOG_MODULE_REGISTER(irsony);

// Remote control
const unsigned long shutter_code = 0x2D;
const unsigned long two_secs_code = 0x37;
const unsigned long video_code = 0x48;

const int BASE = 635;
const int ZERO_DURATION = BASE;
const int ONE_DURATION = BASE * 2;
const int START_DURATION = BASE * 4;

void IrSony::send_pulse(int duration) {
  if (pwm_pin_set_usec(pwm, PWM_IR_CHANNEL, PERIOD_USEC, PULSE_USEC,
                       PWM_IR_FLAGS) < 0) {
    LOG_ERR("failed to start carrier");
    return;
  }
  k_busy_wait(duration);
  if (pwm_pin_set_usec(pwm, PWM_IR_CHANNEL, PERIOD_USEC, 0, PWM_IR_FLAGS) < 0) {
    LOG_ERR("failed to stop carrier");
    return;
  }
  k_busy_wait(BASE);
}

void IrSony::send_start() {
  LOG_MODULE_DECLARE(irsony);
  send_pulse(START_DURATION);
}
void IrSony::send_bit(bool is_one) {
  LOG_MODULE_DECLARE(irsony);
  if (is_one) {
    send_pulse(ONE_DURATION);
  } else {
    send_pulse(ZERO_DURATION);
  }
}
void IrSony::send_code(unsigned long code) {
  send_start();
  // Send 20 bits
  for (int bit = 0; bit < 20; bit++) {
    send_bit(code & ((unsigned long)1 << bit));
  }
}
void IrSony::send_command(unsigned long command) {
  unsigned long address = 0x1E3A;
  LOG_MODULE_DECLARE(irsony);
  unsigned long code = (unsigned long)address << 7 | command;
  LOG_INF("send command=0x%08lu code=0x%08lu", command, code);
  send_code(code);
  k_busy_wait(11000);
  send_code(code);
}

IrSony::IrSony() {
  LOG_MODULE_DECLARE(irsony);
  if (pwm_pin_set_usec(pwm, PWM_IR_CHANNEL, PERIOD_USEC, 0, PWM_IR_FLAGS) < 0) {
    LOG_ERR("failed to stop carrier");
    return;
  }
}

void IrSony::shoot() {
  LOG_MODULE_DECLARE(irsony);
  send_command(shutter_code);
}