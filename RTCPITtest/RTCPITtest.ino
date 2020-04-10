#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/cpufunc.h>

void RTC_init(void);
void SLPCTRL_init(void);

//void RTC_init(void)
//{
//    uint8_t temp;
//    
//    /* Initialize 32.768kHz Oscillator: */
//    /* Disable oscillator: */
//    temp = CLKCTRL.XOSC32KCTRLA;
//    temp &= ~CLKCTRL_ENABLE_bm;
//    /* Writing to protected register */
//    CLKCTRL.XOSC32KCTRLA |= temp;
//    
//    while(CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm)
//    {
//        ; /* Wait until XOSC32KS becomes 0 */
//    }
//    
//    /* SEL = 0 (Use External Crystal): */
//    temp = CLKCTRL.XOSC32KCTRLA;
//    temp &= ~CLKCTRL_SEL_bm;
//    /* Writing to protected register */
//    CLKCTRL.XOSC32KCTRLA |= temp;
//    
//    /* Enable oscillator: */
//    temp = CLKCTRL.XOSC32KCTRLA;
//    temp |= CLKCTRL_ENABLE_bm;
//    /* Writing to protected register */
//    CLKCTRL.XOSC32KCTRLA |= temp;
//    
//    /* Initialize RTC: */
//    while (RTC.STATUS > 0)
//    {
//        ; /* Wait for all register to be synchronized */
//    }
//
//    /* 32.768kHz External Crystal Oscillator (XOSC32K) */
//    RTC.CLKSEL = RTC_CLKSEL_TOSC32K_gc;
//
//    /* Run in debug: enabled */
//    RTC.DBGCTRL = RTC_DBGRUN_bm;
//    
//    RTC.PITINTCTRL = RTC_PI_bm; /* Periodic Interrupt: enabled */
//    
//    RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc /* RTC Clock Cycles 32768 */
//                 | RTC_PITEN_bm; /* Enable: enabled */
//}
//
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
