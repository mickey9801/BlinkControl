#include <BlinkControl.h>

#if defined(ESP32)
BlinkControl led(13, 15, 50, 8); // ESP32
#else
BlinkControl led(3); // Arduino
#endif

int btnState = 1;
unsigned long lastPressedAt = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  Serial.println();

  pinMode(12, INPUT);

  led.begin();
  led.breathe(2000);
}

void loop() {
  led.loop();
  
  if (digitalRead(12) == HIGH && millis()-lastPressedAt >= 1000) {
    lastPressedAt = millis();
    btnState++;
    if (btnState > 8) btnState = 1;
    switch (btnState) {
      case 1:
        led.breathe(2000);
        break;
      case 2:
        led.pulse(1500);
        break;
      case 3:
        led.fadeIn(2000);
        break;
      case 4:
        led.fadeOut(2000);
        break;
      case 5:
        led.blink2();
        break;
      case 6:
        led.fastBlinking();
        break;
      case 7:
        led.on();
        break;
      case 8:
        led.off();
        break;
    }
  }
}
