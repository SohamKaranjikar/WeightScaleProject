#include "HX711.h"
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/power.h>

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 5;

HX711 scale;

// interrupt raised by the watchdog firing
// when the watchdog fires during sleep, this function will be executed
// remember that interrupts are disabled in ISR functions
ISR(WDT_vect)
{
        // not hanging, just waiting
        // reset the watchdog
        wdt_reset();
}

// function to configure the watchdog: let it sleep 8 seconds before firing
// when firing, configure it for resuming program execution
void configure_wdt(void)
{
  cli();                           // disable interrupts for changing the registers

  MCUSR = 0;                       // reset status register flags

                                   // Put timer in interrupt-only mode:                                       
  WDTCSR |= 0b00011000;            // Set WDCE (5th from left) and WDE (4th from left) to enter config mode,
                                   // using bitwise OR assignment (leaves other bits unchanged).
  WDTCSR =  0b01000000 | 0b000110; // set WDIE: interrupt enabled
                                   // clr WDE: reset disabled
                                   // and set delay interval (right side of bar) to 8 seconds

  sei();                           // re-enable interrupts

  // reminder of the definitions for the time before firing
  // delay interval patterns:
  //  16 ms:     0b000000
  //  500 ms:    0b000101
  //  1 second:  0b000110
  //  2 seconds: 0b000111
  //  4 seconds: 0b100000
  //  8 seconds: 0b100001
 
}

// Put the Arduino to deep sleep. Only an interrupt can wake it up.
void sleep(int ncycles)
{  
  scale.power_down();
  int nbr_remaining = ncycles; // defines how many cycles should sleep

  // Set sleep to full power down.  Only external interrupts or
  // the watchdog timer can wake the CPU!
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
 
  // Turn off the ADC while asleep.
//  power_adc_disable();
 
  while (nbr_remaining > 0){ // while some cycles left, sleep!
    delay(10);
    // Enable sleep and enter sleep mode.
    wdt_reset();
    sleep_mode();
  
    // CPU is now asleep and program execution completely halts!
    // Once awake, execution will resume at this point if the
    // watchdog is configured for resume rather than restart
   
    // When awake, disable sleep mode
    sleep_disable();
    
    // we have slept one time more
    nbr_remaining = nbr_remaining - 1;
  }
 
  // put everything on again
//  power_all_enable();
  scale.power_up();
}


void setup()
{
  Serial.begin(57600);
  Serial.println("Hello, World!");

  power_adc_disable();
    
  // configure the watchdog
  configure_wdt();
  
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
