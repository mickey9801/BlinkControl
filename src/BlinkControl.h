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
#ifndef BLINK_CONTROL_H
#define BLINK_CONTROL_H

#include <Arduino.h>
#include <Shifty.h>

#define BC_STATE_OFF    0
#define BC_STATE_ON     1
#define BC_STATE_BLINK  2
#define BC_STATE_BREATH 3

class BlinkControl {
  public:
    BlinkControl(int pin);
    BlinkControl(Shifty* sh, int shiftRegPin, int bitCount=8);
    ~BlinkControl();

    void begin();
    void loop();

    void on(bool shiftRegOffOthers=false); // set pin to constant HIGH
    void offAll(); // for shift register only
    void off();    // set BlinkControl state to OFF only
    void pause();
    void resume();

    int getState();
    bool isOff();

    // convenient methods for blinking
    void blink(int timings[], int timingCount); // custom beat
    void blink1(); // blink once per second
    void blink2(); // blink twice per second
    void blink3(); // blink three times per second
    void blink4(); // blink four times per second
    void clearBlink();
  
  private:
    int _state = BC_STATE_OFF;
    int _prevState = BC_STATE_OFF;
    
    int _pin = -1;
    
    Shifty* _shiftReg;
    int _shiftRegBitCount = 8;

    int blinkTiming1[2] = {100,900};
    int blinkTiming2[4] = {80,150,80,690};
    int blinkTiming3[6] = {80,130,80,130,80,500};
    int blinkTiming4[8] = {80,80,80,80,80,80,80,440};
    
    int* _blinkTiming;
    int _timingCount = 0;
    int _timingCursor = 0;
    unsigned long _lastAction = 0;
    bool _pinOn = false;

    void _onOne(bool shiftRegOffOthers=false);
    void _offOne();
    void _shiftRegAllPinOff();
    void _shiftRegOnePinOnOnly(int pinNum, bool value);
};

#endif /* BLINK_CONTROL_H */
