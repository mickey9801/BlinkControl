#include "Arduino.h"
#include "BlinkControl.h"

BlinkControl::BlinkControl(int pin) {
  this->_pin = pin;
  pinMode(this->_pin, OUTPUT);
}

BlinkControl::BlinkControl(Shifty *sh, int shiftRegPin, int bitCount) {
  this->_shiftReg = sh;
  this->_pin = shiftRegPin;
  this->_shiftRegBitCount = bitCount;
}

void BlinkControl::begin() {
  this->_offOne();
  this->_timingCursor = 0;
  this->_pinOn = false;
  this->_state = BC_STATE_OFF;
  this->_lastAction = 0;
}

void BlinkControl::loop() {
  if (this->_state == BC_STATE_BLINK) {
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
}

void BlinkControl::_onOne(bool shiftRegOffOthers) {
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
  if (this->_state == BC_STATE_BLINK) {
    this->pause();
  }
  this->_onOne(shiftRegOffOthers);
  this->_state = BC_STATE_ON;
}

void BlinkControl::_offOne() {
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

// set state to off only, not to touch the timings array
// so that it can be restart again
void BlinkControl::pause() {
  if (this->_state != BC_STATE_BLINK) return;
  this->_offOne();
  this->_timingCursor = 0;
  this->_pinOn = false;
  this->_lastAction = 0;
  this->_prevState = this->_state;
  this->_state = BC_STATE_OFF;
}

void BlinkControl::resume() {
  if (this->_prevState != BC_STATE_BLINK) return;
  this->_state = this->_prevState;
  this->_prevState = BC_STATE_OFF;
}

int BlinkControl::getState() {
  return this->_state;
}

bool BlinkControl::isOff() {
  if (this->_state == BC_STATE_OFF) {
    return true;
  } else return false;
}

void BlinkControl::blink(int *timings, int timingCount) {
  if (this->_state == BC_STATE_ON) {
    this->off();
  }
  this->_blinkTiming = timings;
  this->_timingCount = timingCount;
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

void BlinkControl::clearBlink() {
  this->_blinkTiming = NULL;
  this->_timingCount = 0;
  this->_pinOn = false;
  this->_prevState = BC_STATE_OFF;
  this->_state = BC_STATE_OFF;
  this->_lastAction = 0;
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
