#include <BlinkControl.h>

BlinkControl led(15);

bool btnState = true;
unsigned long lastPressedAt = 0;

void setup() {
  pinMode(12, INPUT);
  
  led.begin();
}

void loop() {
  led.loop();

  if (btnState) led.on();
  else led.off();

  if (digitalRead(12) == HIGH && millis()-lastPressedAt >= 1000) {
    lastPressedAt = millis();
    btnState = !btnState;
  }
}