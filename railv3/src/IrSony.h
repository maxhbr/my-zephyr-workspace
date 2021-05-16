#ifndef IRSONY_H_
#define IRSONY_H_

#include <device.h>
#include <init.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr.h>
#include <zephyr/types.h>
#include <drivers/pwm.h>

#include <logging/log.h>

#include "GPIOs.h"

#define PWM_IR_NODE DT_ALIAS(pwmir)
#define PWM_IR_CTLR	DT_PWMS_CTLR(PWM_IR_NODE)
#define PWM_IR_CHANNEL	DT_PWMS_CHANNEL(PWM_IR_NODE)
#define PWM_IR_FLAGS	DT_PWMS_FLAGS(PWM_IR_NODE)

/*
 * Unlike pulse width, the PWM period is not a critical parameter for
 * motor control. 20 ms is commonly used.
 */
#define PERIOD_USEC	(20U * USEC_PER_MSEC)
#define STEP_USEC	100
#define MIN_PULSE_USEC	700
#define MAX_PULSE_USEC	2300
#define MIN_PERIOD_USEC	(USEC_PER_SEC / 64U)
#define MAX_PERIOD_USEC	USEC_PER_SEC

class IrSony{
  const struct device *pwm = DEVICE_DT_GET(PWM_IR_CTLR);
  	uint32_t max_period = MAX_PERIOD_USEC;
public:
  IrSony(); 
  void shoot();
};

#endif // IRSONY_H_
