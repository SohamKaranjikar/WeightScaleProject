#include "HX711.h"
#include <avr/sleep.h>
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 5;

HX711 scale;

void setup() {
  Serial.begin(57600);
  pinMode(2,INPUT_PULLUP);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(-22800);
  scale.tare();
  attachInterrupt(digitalPinToInterrupt(2), wakeUp, LOW);
  sleep_enable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_cpu();
}

void loop() {
  
  
  if (scale.is_ready()) {
    float x = scale.get_units(10);
    Serial.println(x);
  } else {
    Serial.println("HX711 not found.");
  }

  delay(100);
  
}

void wakeUp()
{
  Serial.println("Interrupt Fired");
  sleep_disable();
  detachInterrupt(digitalPinToInterrupt(2));
}

