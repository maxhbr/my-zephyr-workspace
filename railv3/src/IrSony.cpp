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

// // based on http://solderer.tv/diy-intervalometer-for-sony-nex/
// void pulseON(int pulseTime) {
//   unsigned long endPulse = micros() + pulseTime;        // create the microseconds to pulse for
//   while( micros() < endPulse) {
//     digitalWrite(pinIRLED, HIGH);                       // turn IR on
//     delayMicroseconds(13);                              // half the clock cycle for 38Khz - e.g. the 'on' part of our wave
//     digitalWrite(pinIRLED, LOW);                        // turn IR off
//     delayMicroseconds(13);                              // delay for the other half of the cycle to generate wave/ oscillation
//   }
// }
// void pulseOFF(unsigned long startDelay) {
//   unsigned long endDelay = micros() + startDelay;       // create the microseconds to delay for
//   while(micros() < endDelay);
// }
// void takePicture() {
//   for (int i=0; i < 2; i++) {    // impulse send twice
//     pulseON(2336);
//     pulseOFF(646);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(572);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(572);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(572);
//     pulseOFF(646);
//     pulseON(572);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(572);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(572);
//     pulseOFF(621);
//     pulseON(572);
//     pulseOFF(646);
//     pulseON(572);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(11008);
//     pulseON(2336);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(572);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(572);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(572);
//     pulseOFF(646);
//     pulseON(572);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(572);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(572);
//     pulseOFF(621);
//     pulseON(572);
//     pulseOFF(646);
//     pulseON(572);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(11008);
//     pulseON(2336);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(572);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(572);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(572);
//     pulseOFF(646);
//     pulseON(572);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(572);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(1168);
//     pulseOFF(621);
//     pulseON(1093);
//     pulseOFF(696);
//     pulseON(572);
//     pulseOFF(621);
//     pulseON(572);
//     pulseOFF(621);
//     pulseON(572);
//     pulseOFF(621);
//     pulseON(572);
//     pulseOFF(1218);
//     pulseON(497);
//     pulseOFF(1292);
//     pulseON(422);
//     pulseOFF(1367);
//     pulseON(373);
//     pulseOFF(11803);
//     pulseON(298);
//     pulseOFF(2659);
//     pulseON(199);
//     pulseOFF(1590);
//     pulseON(174);
//     pulseOFF(1019);
//     pulseON(174);
//     pulseOFF(1615);
//     pulseON(174);
//     pulseOFF(1615);
//     pulseON(149);
//     pulseOFF(1044);
//     pulseON(149);
//     pulseOFF(1640);
//     pulseON(124);
//     pulseOFF(1093);
//     pulseON(149);
//     pulseOFF(1044);
//     pulseON(124);
//     pulseOFF(1665);
//     pulseON(124);
//     pulseOFF(1068);
//     pulseON(124);
//     pulseOFF(1665);
//     pulseON(99);
//     pulseOFF(1690);
//     pulseON(99);
//     pulseOFF(1690);
//     pulseON(99);
//     pulseOFF(1093);
//     pulseON(99);
//     pulseOFF(1118);
//     pulseON(99);
//     pulseOFF(1093);
//     pulseON(99);
//     pulseOFF(1690);
//     pulseON(99);
//     pulseOFF(1690);
//     pulseON(75);
//     pulseOFF(1715);
//     pulseON(75);
//     pulseOFF(12101);
//     pulseON(149);
//     pulseOFF(2833);
//     pulseON(75);
//     pulseOFF(1715);
//     pulseON(75);
//     pulseOFF(1118);
//     pulseON(75);
//     pulseOFF(1715);
//     pulseON(75);
//     pulseOFF(1715);
//     pulseON(75);
//     pulseOFF(1118);
//     pulseON(75);
//     pulseOFF(1715);
//     pulseON(75);
//     pulseOFF(1118);
//     pulseON(99);
//     pulseOFF(1093);
//     pulseON(99);
//     pulseOFF(1690);
//     pulseON(99);
//     pulseOFF(1093);
//     pulseON(99);
//     pulseOFF(1690);
//     pulseON(99);
//     pulseOFF(1690);
//     pulseON(99);
//     pulseOFF(1690);
//     pulseON(99);
//     pulseOFF(1093);
//     pulseON(99);
//     pulseOFF(1118);
//     pulseON(99);
//     pulseOFF(1093);
//     pulseON(99);
//     pulseOFF(1690);
//     pulseON(99);
//     pulseOFF(1690);
//     pulseON(99);
//     pulseOFF(1690);
//     pulseON(99);
//     pulseOFF(646);
//   } 
// }

const int top = 24;    // 1000000/25 = 40kHz
const int match = 18;  // pulses with approx 25% mark/space ratio

// Remote control
const int shutter_code = 0x2D;
const int two_secs_code = 0x37;
const int video_code = 0x48;

const int BASE = 610;
const int ZERO_TIME = BASE;
const int ONE_TIME = BASE*2;
const int START_TIME = BASE*4;

K_TIMER_DEFINE(my_sync_timer, NULL, NULL);

void IrSony::send_pulse(int duration) {
   if (pwm_pin_set_usec(pwm, PWM_IR_CHANNEL,
               PERIOD_USEC, PULSE_USEC, PWM_IR_FLAGS) < 0) {
      LOG_ERR("failed to start carrier");
      return;
   }
   k_busy_wait(duration);
   if (pwm_pin_set_usec(pwm, PWM_IR_CHANNEL,
               PERIOD_USEC, 0, PWM_IR_FLAGS) < 0) {
      LOG_ERR("failed to stop carrier");
      return;
   }
   k_busy_wait(BASE);
}


void IrSony::send_start() {
  LOG_MODULE_DECLARE(irsony);
   send_pulse(START_TIME);
}
void IrSony::send_bit(bool is_one) {
  LOG_MODULE_DECLARE(irsony);
   if (is_one) {
      send_pulse(ONE_TIME);
   } else {
      send_pulse(ZERO_TIME);
   }
}
void IrSony::send_code(unsigned long code) {
   send_start();
  // Send 20 bits
  for (int bit=0; bit<20; bit++) {
     send_bit(code & ((unsigned long) 1<<bit));
  }
}
void IrSony::send_command(int command) {
  int address = 0x1E3A;
  LOG_MODULE_DECLARE(irsony);
   unsigned long code = (unsigned long) address<<7 | command;
  LOG_INF("send command=0x%08x code=0x%08x", command, code);
   send_code(code);
   k_busy_wait(11000));
   send_code(code);
}

// void Pulse (int carrier, int gap) {
//   int count = carrier;
//   OCR0B = match;  // Generate pulses  
//   for (char i=0; i<2; i++) {
//     for (int c=0; c<count; c++) {
//       do ; while ((TIFR & 1<<TOV0) == 0);
//       TIFR = 1<<TOV0;
//     }
//   count = gap;
//   OCR0B = top;
//   }
// }

// // The routine SendSony() outputs a complete code, consisting of a start pulse and 20 bits of data:
// void SendSony (unsigned long code) {
//   TCNT0 = 0;             // Start counting from 0
//   // Send Start pulse
//   Pulse(96, 24);
//   // Send 20 bits
//   for (int Bit=0; Bit<20; Bit++) {
//     if (code & ((unsigned long) 1<<Bit)) Pulse(48, 24); else Pulse(24, 24);
//   }
// }

// void Transmit (int address, int command) {
//   unsigned long code = (unsigned long) address<<7 | command;
//   SendSony(code);
//   delay(11);
//   SendSony(code);
// }


IrSony::IrSony() {
  LOG_MODULE_DECLARE(irsony);

   // if (pwm_pin_set_usec(pwm, PWM_IR_CHANNEL,
   //             PERIOD_USEC, PULSE_USEC, PWM_IR_FLAGS) < 0) {
   //    LOG_ERR("failed to start carrier");
   //    return;
   // }
   // k_sleep(K_MSEC(100));
   if (pwm_pin_set_usec(pwm, PWM_IR_CHANNEL,
               PERIOD_USEC, 0, PWM_IR_FLAGS) < 0) {
      LOG_ERR("failed to stop carrier");
      return;
   }
}

void IrSony::shoot() {
  LOG_MODULE_DECLARE(irsony);
  send_command(shutter_code);
}