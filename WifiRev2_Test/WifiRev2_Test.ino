//WifiRev2 Try

#include "HX711.h"
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/io.h>
#include <avr/cpufunc.h>
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 5;

HX711 scale;

void RTC_init(void)
{
    cli();
    uint8_t temp;

    /* Initialize 32.768kHz Oscillator: */
    /* Disable oscillator: */
    //temp = CLKCTRL.INT1K;
    RTC.CLKSEL = INT1K;
    //temp &= ~CLKCTRL_ENABLE_bm;
    /* Writing to protected register */
    //CLKCTRL.XOSC32KCTRLA = temp;

//    while(CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm)
//    {
//        ; /* Wait until XOSC32KS becomes 0 */
//    }

    /* SEL = 0 (Use External Crystal): */
//    temp = CLKCTRL.XOSC32KCTRLA;
//    temp &= ~CLKCTRL_SEL_bm;
    /* Writing to protected register */
    //CLKCTRL.XOSC32KCTRLA = temp;

    /* Enable oscillator: */
//    temp = CLKCTRL.XOSC32KCTRLA;
//    temp |= CLKCTRL_ENABLE_bm;
//    /* Writing to protected register */
//    CLKCTRL.XOSC32KCTRLA = temp;
    RTC.PITCTRLA = CYC1024;

    /* Initialize RTC: */
    while (RTC.STATUS > 0)
    {
        ; /* Wait for all register to be synchronized */
        Serial.println("Waiting for RTC.Status to be > 0");
    }

    /* 32.768kHz External Crystal Oscillator (XOSC32K) */
    //RTC.CLKSEL = RTC_CLKSEL_TOSC32K_gc;

    /* Run in debug: enabled */
    //RTC.DBGCTRL = ~RTC_DBGRUN_bm;

    RTC.PITINTCTRL = 1; /* Periodic Interrupt: enabled */
    
//    RTC.PITCTRLA = RTC_PERIOD_CYC16384_gc /* RTC Clock Cycles 32768 */
//                 | RTC_PITEN_bm; /* Enable: enabled */
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

  // Set sleep to full power down.  Only external interrupts or
  // the watchdog timer can wake the CPU!
  //set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  // Turn off the ADC while asleep.
  //  power_adc_disable();

  while (nbr_remaining > 0){ // while some cycles left, sleep!
    delay(10);
    // Enable sleep and enter sleep mode.
    sleep_cpu();

    // CPU is now asleep and program execution completely halts!
    // Once awake, execution will resume at this point if the
    // watchdog is configured for resume rather than restart

    // When awake, disable sleep mode
    //sleep_disable();

    // we have slept one time more
    nbr_remaining = nbr_remaining - 1;
  }

  // put everything on again
  //  power_all_enable();
  scale.power_up();
}


void setup()
{
  /* Enable Global Interrupts */
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
