# BlinkControl
Arduino/ESP32 module for easily manage multiple LED blinking or Buzzer beats with different timing in same circuit.  

This module can work with digitial pin, or 74HC595 shift register via johnnyb's [Shifty](https://github.com/johnnyb/Shifty).  

Each instance of the module control one pin. Module provide some predefined blink timing. You may control blink timing by providing a timing array.  

## Usage
1. Include BlinkControl module
   
   ```cpp
   #include <BlinkControl.h>
   ```
   
2. Create a BlinkControl object with pin number or [Shifty](https://github.com/johnnyb/Shifty) instance.
   
   ```cpp
   BlinkControl led = BlinkControl(15);
   ```
   
3. In setup(), initial the BlinkControl object.
   
   ```cpp
   void setup() {
     led.begin();
   }
   ```
   
4. In loop() of the sketch, run the object's loop() method.
   
   ```cpp
   void loop() {
     led.loop();
   }
   ```   

5. Set blink timing
   
   ```cpp
   led.blink1(); // Blink once pre second using predefined timing
   ```
   
   or just turn it on
   
   ```cpp
   led.on();
   ```

## Example

Control 1 LED connected to digital pin, 4 LED connected to 74HC595 shift register and a buzzer "blink" with different timing. When the button connect to PIN 12 pressed, pause the buzzer, turn off LED in PIN 0 of 74HC595 and change the blink timing of LED in PIN 2 of 74HC595.  

We use evert-arias' [EasyButton](https://github.com/evert-arias/EasyButton) to handle button actions.  

```cpp
#include <EasyButton.h>
#include <Shifty.h>
#include <BlinkControl.h>

#define SHIFTY_BIT_COUNT  8
Shifty shift;

BlinkControl led(15);
BlinkControl sled1(&shift, 0, SHIFTY_BIT_COUNT);
BlinkControl sled2(&shift, 2, SHIFTY_BIT_COUNT);
BlinkControl sled3(&shift, 5); // You may ignore the bit count parameter if the shift register is 8-bit
BlinkControl sled4(&shift, 7);
BlinkControl buzzer(14);

EasyButton button(12, 35, false, false);
void btnOnPressed() {
  if (!buzzer.isOff()) {
    sled1.off();     // Turn off led
    sled2.blink1();  // Blink once per second
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
  shift.setPins(17, 32, 33);
  
  led.begin();     // Initial led
  led.blink1();    // Blink once per second
  
  sled1.begin();
  sled1.on();      // Turn on led
  
  sled2.begin();
  sled2.blink3();  // Blink three times per second
  
  sled3.begin();
  int sled3Timing[] = {1000,200,60,200,1000,200,60,1000}; // Custom blink timing
  sled3.blink(sled3Timing, sizeof(sled3Timing)/sizeof(int)); // Set custom blink timing
  
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

- **Build Object with Digitial Pin**
   
   ```cpp
   BlinkControl(int pin);
   ```

- **Build Object with 74HC595 Shift Register Pin, work with [Shifty](https://github.com/johnnyb/Shifty) library**
   
   ```cpp
   BlinkControl(Shifty* sh, int shiftRegPin, int bitCount=8);
   ```

### Setup methods

- **void begin ()**
  
  Initial object parameters. Should be placed inside setup() of the sketch.  
  
  ```cpp
  begin();
  ```
  
- **void loop ()**
  
  Module loop. Should be placed inside loop() of the sketch.  
    
  ```cpp
  loop();
  ```

### Simple On/Off

- **void on (** bool **shiftRegOffOthers**=false **)**
  
  Set pin to **HIGH**. If work with shift register, you may turn off all other shift register pins with `shiftRegOffOthers` option set to **ture**.  
  
  ```cpp
  led.on(); // digital  pin
  shiftRegisterPin.on(false); // shift register pin
  ```
  
- **void off()**
  
  Set pin to **LOW**.  
  
  ```cpp
  led.off();
  ```
  
- **void offAll ()**
  
  *Only work for shift register.* Set all pins of same shift register to **LOW**.  
  
  ```cpp
  shiftRegisterPin.offAll();
  ```

### Blink related

- **void pause ()**
  
  Pause blinking.

  ```cpp
  led.pause();
  ```

- **void resume ()**
  
  Resume blinking from pause.  
  
  ```cpp
  led.resume();
  ```

- **void blink (** int **timings[],** int **timingCount )**
  
  Customize blink timing by providing a timing array. Unit of each value is millisecond. Pin will be set to **HIGH** and **LOW** alternatively based on those timing, starting from **HIGH**.  
  
  ```cpp
  int blinkTiming[] = {1000,200,60,200,1000,200,60,1000};
  led.blink(blinkTiming, sizeof(blinkTiming)/sizeof(int));
  ```
  
- **Convenient methods for blinking**
  
  ```cpp
  void blink1(); // blink once per second
  void blink2(); // blink twice per second
  void blink3(); // blink three times per second
  void blink4(); // blink four times per second
  ```

- **void clearBlink ()**
  
  Reset the object and delete blink timing.
  
  ```cpp
  led.clearBlink();
  ```

### Status related

- **int getState ()**
  
  Retrieve the state of pin. Possible return values are:  
  
  Constant          |Value|Description
  ------------------|-----|-----------
  BC\_STATE\_OFF    |  0  | Pin level set to **LOW**
  BC\_STATE\_ON     |  1  | Pin level set to **HIGH** and not blinking
  BC\_STATE\_BLINK  |  2  | Blinking
  *BC\_STATE\_BREATH* |  3  | *[under construction]* Breathing LED (for PWM pin only)

- **bool isOff ()**
  
  Check if the LED is off (set to **LOW**).

## What's Next

- Build a Shify Manager to handle bit writing operation across multiple BlinkControl instances for faster operation.
- PWM Breathing LED
