#include <BlinkControl.h>

BlinkControl led(15);

void setup() {
  led.begin();
  led.blink2();
}

void loop() {
  led.loop();
}
