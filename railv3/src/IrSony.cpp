#include "IrSony.h"

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

/* ATtiny85 Sony NEX/Alpha Remote Control

   David Johnson-Davies - www.technoblogy.com - 12th April 2015
   ATtiny85 @ 1 MHz (internal oscillator; BOD disabled)
   
   CC BY 4.0
   Licensed under a Creative Commons Attribution 4.0 International license: 
   http://creativecommons.org/licenses/by/4.0/
*/
// const int top = 24;    // 1000000/25 = 40kHz
// const int match = 18;  // pulses with approx 25% mark/space ratio

// // Remote control
// const int Address = 0x1E3A;
// const int ShutterCode = 0x2D;
// const int TwoSecsCode = 0x37;
// const int VideoCode = 0x48;

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
   LOG_INF("Calibrating for channel %d...\n", PWM_IR_CHANNEL);
	max_period = MAX_PERIOD_USEC;
	while (pwm_pin_set_usec(pwm, PWM_IR_CHANNEL,
				max_period, max_period / 2U, PWM_IR_FLAGS)) {
		max_period /= 2U;
		if (max_period < (4U * MIN_PERIOD_USEC)) {
			LOG_ERR("Error: PWM device "
			       "does not support a period at least %u\n",
			       4U * MIN_PERIOD_USEC);
			return;
		}
	}
}

void IrSony::shoot() {
  LOG_MODULE_DECLARE(irsony);
  uint32_t pulse_width = MIN_PULSE_USEC;
  int ret = pwm_pin_set_usec(pwm, PWM_IR_CHANNEL,
               PERIOD_USEC, pulse_width, PWM_IR_FLAGS);
   if (ret < 0) {
      LOG_ERR("Error %d: failed to set pulse width\n", ret);
      return;
   }
}