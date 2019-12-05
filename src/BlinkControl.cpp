/**
 * BlinkControl
 * Copyright (c) 2019 Mickey Chan
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * Developed for Arduino-ESP32
 * Created by Mickey Chan (developer AT comicparty.com)
 * 
 */
#include "Arduino.h"
#include "BlinkControl.h"

BlinkControl::BlinkControl(int pin) {
  this->_pin = pin;
  pinMode(this->_pin, OUTPUT);
}

BlinkControl::BlinkControl(Shifty *sh, unsigned int shiftRegPin, unsigned int bitCount) {
  this->_shiftReg = sh;
  this->_pin = shiftRegPin;
  this->_shiftRegBitCount = bitCount;
}

// only for ESP32 ledc library
#if defined(ESP32)
BlinkControl::BlinkControl(int pin, uint8_t channel, double freq, uint8_t resolutionBits) {
  this->_pin = pin;
  this->_pwmChannel = channel;
  this->_pwmFreq = freq;
  this->_pwmResolutionBits = resolutionBits;
  this->_brightnessMax = pow(2, this->_pwmResolutionBits) - 1;
  pinMode(this->_pin, OUTPUT);
  ledcSetup(this->_pwmChannel, this->_pwmFreq, this->_pwmResolutionBits);
}
#endif

BlinkControl::~BlinkControl() {
  delete[] this->_blinkTiming;
}

void BlinkControl::begin() {
  this->_offOne();
  this->_timingCursor = 0;
  this->_pinOn = false;
  this->_state = BC_STATE_OFF;
  this->_lastAction = 0;
  #if defined(ESP32)
  this->_pwmPinAttached = false;
  #endif
}

void BlinkControl::loop() {
  if (this->_state == BC_STATE_BLINK) {
    this->_blinkLoop();
  } else if (this->_state == BC_STATE_BREATHE) {
    this->_breatheLoop();
  } else if (this->_state == BC_STATE_PULSE) {
    this->_pulseLoop();
  } else if (this->_state == BC_STATE_FADE_IN) {
    this->_fadeInLoop();
  } else if (this->_state == BC_STATE_FADE_OUT) {
    this->_fadeOutLoop();
  }
}

void BlinkControl::_blinkLoop() {
  unsigned long curtime = millis();
  if (this->_lastAction == 0) {
    this->_pinOn = true;
    this->_onOne(false);
    this->_lastAction = curtime;
  } else if (curtime - this->_lastAction > this->_blinkTiming[this->_timingCursor]) {
    this->_pinOn = !this->_pinOn;
    if (this->_pinOn) {
      this->_onOne(false);
    } else {
      this->_offOne();
    }
    this->_timingCursor++;
    if (this->_timingCursor >= this->_timingCount) {
      this->_timingCursor = 0;
    }
    this->_lastAction = curtime;
  }
}

void BlinkControl::_breatheLoop() {
  unsigned long curtime = millis();
  if (curtime - this->_lastAction > this->_breatheInterval) {
    this->_lastAction = curtime;
    this->_dutyCycle += this->_brightStep;
    if (this->_dutyCycle <= 0 || this->_dutyCycle >= this->_brightnessMax) {
      this->_brightStep = -this->_brightStep;
    }
    #if defined(ESP32)
    ledcWrite(this->_pwmChannel, this->_dutyCycle);
    #else
    analogWrite(this->_pin, this->_dutyCycle);
    #endif
  }
}

void BlinkControl::_pulseLoop() {
  unsigned long curtime = millis();
  if (curtime - this->_lastAction > this->_breatheInterval) {
    this->_lastAction = curtime;
    this->_dutyCycle -= this->_brightStep;
    if (this->_dutyCycle < 0) {
      this->_dutyCycle = this->_brightnessMax;
      this->_lastAction += 500;
    }
    #if defined(ESP32)
    ledcWrite(this->_pwmChannel, this->_dutyCycle);
    #else
    analogWrite(this->_pin, this->_dutyCycle);
    #endif
  }
}

void BlinkControl::_fadeInLoop() {
  if (this->_dutyCycle >= this->_brightnessMax) return;
  unsigned long curtime = millis();
  if (curtime - this->_lastAction > this->_breatheInterval) {
    this->_lastAction = curtime;
    this->_dutyCycle += this->_brightStep;
    #if defined(ESP32)
    ledcWrite(this->_pwmChannel, this->_dutyCycle);
    #else
    analogWrite(this->_pin, this->_dutyCycle);
    #endif
    if (this->_dutyCycle == this->_brightnessMax) {
      this->on();
    }
  }
}

void BlinkControl::_fadeOutLoop() {
  if (this->_dutyCycle <= 0) return;
  unsigned long curtime = millis();
  if (curtime - this->_lastAction > this->_breatheInterval) {
    this->_lastAction = curtime;
    this->_dutyCycle -= this->_brightStep;
    #if defined(ESP32)
    ledcWrite(this->_pwmChannel, this->_dutyCycle);
    #else
    analogWrite(this->_pin, this->_dutyCycle);
    #endif
    if (this->_dutyCycle == 0) {
      this->off();
    }
  }
}

void BlinkControl::_onOne(bool shiftRegOffOthers) {
  #if defined(ESP32)
  this->_pwmDetachPin();
  #endif
  if (this->_shiftReg == NULL) {
    digitalWrite(this->_pin, HIGH);
  } else {
    if (!shiftRegOffOthers) {
      this->_shiftReg->writeBit(this->_pin, HIGH);
    } else {
      this->_shiftRegOnePinOnOnly(this->_pin, HIGH);
    }
  }
}

void BlinkControl::on(bool shiftRegOffOthers) {
  if (this->_state != BC_STATE_ON && this->_state != BC_STATE_OFF) {
    this->pause();
  }
  this->_onOne(shiftRegOffOthers);
  this->_state = BC_STATE_ON;
}

void BlinkControl::_offOne() {
  #if defined(ESP32)
  this->_pwmDetachPin();
  #endif
  if (this->_shiftReg == NULL) {
    digitalWrite(this->_pin, LOW);
  } else {
    this->_shiftReg->writeBit(this->_pin, LOW);
  }
}

void BlinkControl::offAll() {
  this->_shiftRegAllPinOff();
  this->_prevState = this->_state;
  this->_state = BC_STATE_OFF;
}

void BlinkControl::off() {
  this->_offOne();
  this->_prevState = this->_state;
  this->_state = BC_STATE_OFF;
}

// Turn off only, haven't touch the pattern array
// so that the LED can be resumed
void BlinkControl::pause() {
  if (this->_state != BC_STATE_BLINK && this->_state != BC_STATE_BREATHE && this->_state != BC_STATE_PULSE) {
    return;
  }
  this->_offOne();
  this->_timingCursor = 0;
  this->_pinOn = false;
  this->_prevState = this->_state;
  this->_state = BC_STATE_OFF;
  this->_lastAction = 0;
}

void BlinkControl::resume() {
  if (this->_prevState != BC_STATE_BLINK && this->_prevState != BC_STATE_BREATHE && this->_prevState != BC_STATE_PULSE) {
    return;
  }
  
  // resume from breathe/pulse
  #if defined(ESP32)
  if (this->_prevState == BC_STATE_BREATHE || this->_prevState == BC_STATE_PULSE) {
    this->_pwmAttachPin();
  }
  #endif
  if (this->_prevState == BC_STATE_BREATHE) {
    this->_dutyCycle = 0;
  } else if (this->_prevState == BC_STATE_PULSE) {
    this->_dutyCycle = this->_brightnessMax;
  }
  
  this->_state = this->_prevState;
  this->_prevState = BC_STATE_OFF;
}

void BlinkControl::blink(int timings[], int timingCount) {
  if (this->_state != BC_STATE_BLINK && this->_state != BC_STATE_OFF) {
    this->off();
  }
  
  this->_timingCount = timingCount;
  this->_blinkTiming = new int[this->_timingCount];
  for (int i = 0; i < this->_timingCount; i++) {
    this->_blinkTiming[i] = timings[i];
  }
  
  this->_timingCursor = 0;
  this->_state = BC_STATE_BLINK;
  this->_lastAction = 0;
}

void BlinkControl::blink1() {
  this->blink(this->blinkTiming1, 2);
}

void BlinkControl::blink2() {
  this->blink(this->blinkTiming2, 4);
}

void BlinkControl::blink3() {
  this->blink(this->blinkTiming3, 6);
}

void BlinkControl::blink4() {
  this->blink(this->blinkTiming4, 8);
}

void BlinkControl::fastBlinking() {
  this->blink(this->blinkTiming625, 2);
}

void BlinkControl::breathe(unsigned int duration) {
  if (this->_state != BC_STATE_BREATHE && this->_state != BC_STATE_OFF) {
    this->off();
  }
  #if defined(ESP32)
  this->_pwmAttachPin();
  #endif
  
  this->_dutyCycle = 0;
  this->_breatheInterval = round(duration / (this->_brightnessMax*2));
  this->_state = BC_STATE_BREATHE;
}

void BlinkControl::pulse(unsigned int duration) {
  if (this->_state != BC_STATE_PULSE && this->_state != BC_STATE_OFF) {
    this->off();
  }
  #if defined(ESP32)
  this->_pwmAttachPin();
  #endif
  
  this->_dutyCycle = this->_brightnessMax;
  this->_breatheInterval = round(duration / this->_brightnessMax);
  this->_state = BC_STATE_PULSE;
}

void BlinkControl::fadeIn(unsigned int duration) {
  if (this->_state != BC_STATE_FADE_IN && this->_state != BC_STATE_OFF) {
    this->off();
  }
  #if defined(ESP32)
  this->_pwmAttachPin();
  #endif
  
  this->_dutyCycle = 0;
  this->_breatheInterval = round(duration / this->_brightnessMax);
  this->_state = BC_STATE_FADE_IN;
}

void BlinkControl::fadeOut(unsigned int duration) {
  if (this->_state != BC_STATE_FADE_OUT && this->_state != BC_STATE_OFF) {
    this->off();
  }
  #if defined(ESP32)
  this->_pwmAttachPin();
  #endif
  
  this->_dutyCycle = this->_brightnessMax;
  this->_breatheInterval = round(duration / this->_brightnessMax);
  this->_state = BC_STATE_FADE_OUT;
}

void BlinkControl::clearBlink() {
  this->_offOne();
  delete[] this->_blinkTiming;
  this->_timingCount = 0;
  this->_pinOn = false;
  this->_prevState = BC_STATE_OFF;
  this->_state = BC_STATE_OFF;
  this->_lastAction = 0;
}

int BlinkControl::getState() {
  return this->_state;
}

bool BlinkControl::isOff() {
  if (this->_state == BC_STATE_OFF) {
    return true;
  } else return false;
}

void BlinkControl::_shiftRegAllPinOff() {
  this->_shiftReg->batchWriteBegin();
  for (int i = 0; i < this->_shiftRegBitCount; i++) {
    this->_shiftReg->writeBit(i, LOW);
  }
  this->_shiftReg->batchWriteEnd();
}

void BlinkControl::_shiftRegOnePinOnOnly(int pinNum, bool value) {
  this->_shiftReg->batchWriteBegin();
  for (int i = 0; i < this->_shiftRegBitCount; i++) {
    this->_shiftReg->writeBit(i, LOW);
  }
  this->_shiftReg->writeBit(pinNum, value);
  this->_shiftReg->batchWriteEnd();  
}

// Only for ESP32 ledc library
#if defined(ESP32)
void BlinkControl::_pwmAttachPin() {
  if (!this->_pwmPinAttached) {
    ledcAttachPin(this->_pin, this->_pwmChannel);
    this->_pwmPinAttached = true;
  }
}

void BlinkControl::_pwmDetachPin() {
  if (this->_pwmPinAttached) {
    ledcDetachPin(this->_pin);
    this->_pwmPinAttached = false;
  }
}
#endif