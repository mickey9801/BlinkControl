# BlinkControl
Arduino/ESP32 module for easily manage multiple LED blinking or Buzzer beats with different timing in same circuit.

This module can work with digitial pin or shift 74HC595 registers through johnnyb's [Shifty](https://github.com/johnnyb/Shifty).

## Constructors

**Digitial Pin**

```cpp
BlinkControl(int pin);
```

**Pin of 74HC595 Shift Register**

```cpp
BlinkControl(Shifty* sh, int shiftRegPin, int bitCount=8);
```

## Usage
1. Include BlinkControl module
   
   ```cpp
   #include <BlinkControl.h>
   ```
   
2. Create a BlinkControl object with pin numnber or Shifty instance.
   
   ```cpp
   BlinkControl led = BlinkControl(15);
   ```
   
3. In setup(), setup the BlinkControl object.
   
   ```cpp
   led.begin();
   ```

4. In setup(), set blinking timing.
   
   ```cpp
   led.blink1(); // Blink once pre second
   ```
   
5. In loop(), run the loop() script of the BlinkControl instance.
   
   ```cpp
   led.loop();
   ```   

## Example

LEDs and buzzer blink with different timing. When the button in PIN 12 pressed, pause the buzzer and LED in PIN 0 of 74HC595, and change the blinking timing of LED in PIN 2 of 74HC595. We use evert-arias' [EasyButton](https://github.com/evert-arias/EasyButton) to handle button actions.

```cpp
#include <EasyButton.h>
#include <Shifty.h>
#include <BlinkControl.h>

#define SHIFTY_BIT_COUNT  8
Shifty shift;

BlinkControl led = BlinkControl(15);
BlinkControl sled1 = BlinkControl(&shift, 0, SHIFTY_BIT_COUNT);
BlinkControl sled2 = BlinkControl(&shift, 2, SHIFTY_BIT_COUNT);
BlinkControl sled3 = BlinkControl(&shift, 7); // You may ignore the bit count parameter if the shift register is 8-bit
BlinkControl buzzer = BlinkControl(14);

EasyButton button(12, 35, false, false);
void btnOnPressed() {
  if (!buzzer.isOff()) {
    sled1.off();
    sled2.blink1();
    buzzer.pause();
  } else {
    sled1.on();
    sled2.blink3();
    buzzer.resume();
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
  
  led.begin();
  led.blink1();
  sled1.begin();
  sled1.on();
  sled2.begin();
  sled2.blink3();
  sled3.begin();
  sled3.blink2();
  buzzer.begin();
  buzzer.blink4();

  button.begin();
  button.onPressed(btnOnPressed);
  
  Serial.println("--BEGIN--");
}

void loop() {
  led.loop();
  sled1.loop();
  sled2.loop();
  sled3.loop();
  buzzer.loop();
  
  button.read();
}
```


## APIs




## What's Next

- Build a Shify Manager to handle bit writing operation across different BlinkControl object for faster operation.
