#include <BlinkControl.h>

BlinkControl led(15);

bool btnState = true;
unsigned long lastPressedAt = 0;

void setup() {
  pinMode(12, INPUT);
  
  led.begin();

  int timing[] = {500,1000,60,1000,500,1000,60,1000};
  led.blink(timing, sizeof(timing)/sizeof(int));
}

void loop() {
  led.loop();

  if (btnState) led.resume();
  else led.pause();

  if (digitalRead(12) == HIGH && millis()-lastPressedAt >= 1000) {
    lastPressedAt = millis();
    btnState = !btnState;
  }
}
