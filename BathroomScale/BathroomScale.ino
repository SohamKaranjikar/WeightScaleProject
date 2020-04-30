//WifiRev2 Try

#include "HX711.h"
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/io.h>
#include <avr/cpufunc.h>

const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 5;

HX711 scale;

void RTC_init(void)
{
    cli();
    uint8_t temp;

    RTC.CLKSEL |= B00000001;
    RTC.PITCTRLA |= B01001001;

    /* Initialize RTC: */
    while (RTC.STATUS > 0)
    {
        Serial.println("Waiting for registers to synchronize");
    }

    RTC.PITINTCTRL = 1; /* Periodic Interrupt: enabled */

    sei();
}


// interrupt raised by the RTC PIT firing
// when the watchdog fires during sleep, this function will be executed
// remember that interrupts are disabled in ISR functions
ISR(RTC_PIT_vect)
{
    /* Clear flag by writing '1': */
    RTC.PITINTFLAGS = 1;
}

void SLPCTRL_init(void)
{
    SLPCTRL.CTRLA |= SLPCTRL_SMODE_PDOWN_gc;
    SLPCTRL.CTRLA |= SLPCTRL_SEN_bm;
}

// Put the Arduino to deep sleep. Only an interrupt can wake it up.
void sleep(int ncycles)
{
  scale.power_down();
  int nbr_remaining = ncycles; // defines how many cycles should sleep

  while (nbr_remaining > 0){
    delay(10);
    sleep_cpu();

    // CPU is now asleep and program execution completely halts!

    nbr_remaining = nbr_remaining - 1;
  }

  scale.power_up();
}


void setup()
{
  Serial.begin(57600);
  Serial.println("Hello, World!");

  RTC_init();
  SLPCTRL_init();

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(-22800);
  scale.tare();
}

volatile int sleepCount = 4;

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
      while(scale.get_units(10) < 2) {
          sleep(2);
      }
    }
  } else {
    Serial.println("HX711 not found.");
  }
}
