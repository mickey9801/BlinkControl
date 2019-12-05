# BlinkControl
**BlinkControl** is an Arduino/ESP32 module designed for easily manage multiple LED blinking or buzzer alarm with different pattern.  

This module can work with digital pins, analog pins, or 74HC595 shift register via johnnyb's [Shifty](https://github.com/johnnyb/Shifty).  

Each instance of the module control one pin. Module provide some predefined blink pattern. You may also control blink pattern by providing a pattern array.  

## Dependencies
[Shifty by Johnnyb](https://github.com/johnnyb/Shifty)


## Installation

1. [Arduino Library Manager (Recommended)](https://www.arduino.cc/en/Guide/Libraries)  
2. [Download or clone this repository into your arduino libraries directory](https://help.github.com/articles/cloning-a-repository/)  


## Basic Usage
1. Include BlinkControl module  
   
   ```cpp
   #include <BlinkControl.h>
   ```
   
2. Create a BlinkControl object with pin number or [Shifty](https://github.com/johnnyb/Shifty) instance.  
   
   ```cpp
   BlinkControl led(15);
   ```
   
   (For detail of create BlinkControl instance for 74HC595, please check [Shift Register 74Hc595](examples/shift_register_74hc595/shift_register_74hc595.ino) example.)  
   
3. In `setup()`, initial the BlinkControl object.  
   
   ```cpp
   void setup() {
     led.begin();
   }
   ```
   
4. In `loop()` of the sketch, run the object's **loop()** method.  
   
   ```cpp
   void loop() {
     led.loop();
   }
   ```   

5. Set blink pattern  
   
   ```cpp
   led.blink1(); // Blink once pre second using predefined pattern
   ```
   
   or just turn it on  
   
   ```cpp
   led.on();
   ```

## Example

Control 1 LED connected to digital pin, 4 LED connected to 74HC595 shift register and a buzzer "blink" with different pattern. When the button connect to PIN 12 pressed, the buzzer will be paused, LED in PIN 0 of 74HC595 will be turned off and blink pattern of LED in PIN 2 of 74HC595 will be changed into breathe pattern.  

We use evert-arias' [EasyButton](https://github.com/evert-arias/EasyButton) to handle button actions.  

```cpp
#include <EasyButton.h>
#include <Shifty.h>
#include <BlinkControl.h>

#define SHIFTY_BIT_COUNT  8
Shifty shift;

BlinkControl led(13);
BlinkControl sled1(&shift, 0, SHIFTY_BIT_COUNT);
BlinkControl sled2(&shift, 2, SHIFTY_BIT_COUNT);
BlinkControl sled3(&shift, 5); // You may ignore the bit count parameter if the shift register is 8-bit
BlinkControl sled4(&shift, 7);
BlinkControl buzzer(14);

EasyButton button(12, 35, false, false);
void btnOnPressed() {
  if (!buzzer.isOff()) {
    sled1.off();     // Turn off led
    sled2.breathe();  // Blink once per second
    buzzer.pause();  // Pause blinking
  } else {
    sled1.on();      // Turn on led
    sled2.blink3();  // Blink three times per second
    buzzer.resume(); // Resume blinking
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  Serial.println();

  shift.setBitCount(SHIFTY_BIT_COUNT);
  shift.setPins(15, 32, 33);
  
  led.begin();     // Initial led
  led.blink1();    // Blink once per second
  
  sled1.begin();
  sled1.on();      // Turn on led
  
  sled2.begin();
  sled2.blink3();  // Blink three times per second
  
  sled3.begin();
  int sled3Pattern[] = {1000,200,60,200,1000,200,60,1000}; // Custom blink pattern
  sled3.blink(sled3Pattern, sizeof(sled3Pattern)/sizeof(int)); // Set custom blink pattern
  
  sled4.begin();
  sled4.blink2();  // Blink twice per second
  
  buzzer.begin();
  buzzer.blink4(); // Blink four times per second

  button.begin();
  button.onPressed(btnOnPressed);
  
  Serial.println("--BEGIN--");
}

void loop() {
  led.loop();
  sled1.loop();
  sled2.loop();
  sled3.loop();
  sled4.loop();
  buzzer.loop();
  
  button.read();
}
```

## APIs
### Constructors

- **Build with Digital Pin**
   
   ```cpp
   BlinkControl(int pin);
   ```

- **Build with 74HC595 Shift Register, work with [Shifty](https://github.com/johnnyb/Shifty) library**
   
   ```cpp
   BlinkControl(Shifty* sh, int shiftRegPin, int bitCount=8);
   ```
   
- **Build for ESP32 breathe LED, work with analog (PWM) pins**
  
  ```cpp
  BlinkControl(int pin, uint8_t channel, double freq=50, uint8_t resolutionBits=8);
  ```

### Setup methods

- void **begin ()**
  
  Initial object parameters. Should be placed inside `setup()` of the sketch.  
  
  ```cpp
  begin();
  ```
  
- void **loop ()**
  
  Module loop. Should be placed inside `loop()` of the sketch.  
    
  ```cpp
  loop();
  ```

### Simple On/Off

- void **on (** bool **shiftRegOffOthers**=false **)**
  
  Set pin to **HIGH**. If work with shift register, you may turn off all other shift register pins with `shiftRegOffOthers` option set to **ture**.  
  
  ```cpp
  led.on(); // digital  pin
  shiftRegisterPin.on(false); // shift register pin
  ```
  
- void **off ()**
  
  Set pin to **LOW**.  
  
  ```cpp
  led.off();
  ```
  
- void **offAll ()**
  
  *Only work for 74HC595 shift register.* Set all pins of same shift register to **LOW**.  
  
  ```cpp
  shiftRegisterPin.offAll();
  ```

### Blink related

- void **pause ()**
  
  Pause blinking or breathing.

  ```cpp
  led.pause();
  ```

- void **resume ()**
  
  Resume blinking/breathing from pause.  
  
  ```cpp
  led.resume();
  ```

- void **blink (** int **timings[],** int **timingCount )**
  
  Customize blink pattern by providing a pattern array. Unit of each element is **millisecond**. Pin will be set to **HIGH** and **LOW** alternatively based on those pattern, starting from **HIGH**.  
  
  ```cpp
  int blinkPattern[] = {1000,200,60,200,1000,200,60,1000};
  led.blink(blinkPattern, sizeof(blinkTiming)/sizeof(int));
  ```
  
- **Convenient methods for blinking**
  
  ```cpp
  void blink1(); // blink once per second
  void blink2(); // blink twice per second
  void blink3(); // blink three times per second
  void blink4(); // blink four times per second
  void fastBlinking(); // continuous blinking with 6.25 times per second
  ```

- void **clearBlink ()**
  
  Reset the object and delete blink/breathe pattern.  
  
  ```cpp
  led.clearBlink();
  ```
  
### Breathe LED

These methods are only for analog (PWM) pins of Arduino and ESP32.   

  ```cpp
  void breathe(uint8_t duration=2000);
  void pulse(uint8_t duration=1500);
  ```
  
For ESP32, you should use the following constructor to create a LED instance for breathe/pulse (Setup PWM channel and attach to the pin):  

```cpp
BlinkControl(int pin, uint8_t channel, double freq=50, uint8_t resolutionBits=8);
```

It is OK for LEDs to both breathe and blink pattern. The module will handle attach/detech operation of assigned pin when switching between analog breathe/pluse and digital blink on ESP32 board.  

### Status related

- int **getState ()**
  
  Retrieve the state of pin. Possible return values are:  
  
  Constant            |Value|Description
  :-------------------|:---:|:----------
  BC\_STATE\_OFF      |  0  | Pin level set to **LOW**
  BC\_STATE\_ON       |  1  | Pin level set to **HIGH** and not blink
  BC\_STATE\_BLINK    |  2  | Blinking
  BC\_STATE\_BREATHE  |  3  | Breathe LED (for PWM pin only)
  BC\_STATE\_PULSE    |  4  | Pulse LED (for PWM pin only)
  
  ```cpp
  int state = led.getState();
  ```

- bool **isOff ()**
  
  Check if the LED is off (set to **LOW**).  
  
  ```cpp
  bool result = led.isOff();
  ```

## What's Next

- Build a Shify Manager to handle bit writing operation across multiple BlinkControl instances for faster operation.  
