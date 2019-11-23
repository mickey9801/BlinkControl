#include <BlinkControl.h>
#include <Shifty.h>

#define SHIFTY_BIT_COUNT  8
Shifty shift;

BlinkControl led1(&shift, 0, SHIFTY_BIT_COUNT);
BlinkControl led2(&shift, 7); // You may ignore the bit count parameter if the shift register is 8-bit

void setup() {
  shift.setBitCount(SHIFTY_BIT_COUNT);
  shift.setPins(17, 32, 33);
  
  led1.begin();
  led1.blink1();

  led2.begin();
  led2.blink2();
}

void loop() {
  led1.loop();
  led2.loop();
}
