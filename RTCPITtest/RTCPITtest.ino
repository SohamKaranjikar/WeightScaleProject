#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/cpufunc.h>

void RTC_init(void);
void SLPCTRL_init(void);
void SLPCTRL_init(void)
{
    SLPCTRL.CTRLA |= SLPCTRL_SMODE_PDOWN_gc;
    SLPCTRL.CTRLA |= SLPCTRL_SEN_bm;
}
void RTC_init(void)
{
    cli();
    uint8_t temp;
    RTC.CLKSEL |= B00000001;
    RTC.PITCTRLA |= B01001001;
    /* Initialize RTC: */
    while (RTC.STATUS > 0)
    {
        ; /* Wait for all register to be synchronized */
        Serial.println("Waiting for RTC.Status to be > 0");
    }

    RTC.PITINTCTRL = 1; /* Periodic Interrupt: enabled */
    sei();
}

ISR(RTC_PIT_vect)
{
    /* Clear flag by writing '1': */
    RTC.PITINTFLAGS = RTC_PI_bm;
    Serial.println("Wake up");
    
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  Serial.println("Hello, World!");
  RTC_init();
  SLPCTRL_init();
  sei();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Going to sleep");
  sleep_cpu();
  while(1);

}
