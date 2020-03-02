#include "HX711.h"
#include <avr/sleep.h>

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 5;

HX711 scale;

void setup()
{
  Serial.begin(57600);
  Serial.println("Hello, World!");

  pinMode(2,INPUT_PULLUP);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(-22800);
  scale.tare();
}

volatile int sleepCount = 0;

void loop()
{
  delay(100);
  if (scale.is_ready()) {
    float x = scale.get_units(10);
    Serial.println(x);
    if (x > 2) {
      sleepCount = 0;
      return;
    }
    sleepCount++;
    sleepCount%=5;
    if (sleepCount == 0) {
      Serial.println("Going to sleep");
      attachInterrupt(digitalPinToInterrupt(2), wakeUp, LOW);
      sleep_enable();
      set_sleep_mode(SLEEP_MODE_PWR_DOWN);
      sleep_cpu();
    }
  } else {
    Serial.println("HX711 not found.");
  }
}

void wakeUp()
{
  Serial.println("Interrupt Fired");
  sleep_disable();
  detachInterrupt(digitalPinToInterrupt(2));
}
