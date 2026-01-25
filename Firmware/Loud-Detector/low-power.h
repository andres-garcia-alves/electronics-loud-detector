#pragma once
#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>

// flag set by watchdog interrupt
volatile bool wdtFired = false;

// watchdog interrupt (wake up only)
ISR(WDT_vect) { wdtFired = true; }

// variables + enums
enum WdtInterval { WDT_500MS, WDT_1S };

// funtions declaration + inline functions
void wdtSetup(WdtInterval);
void inline adcOn()       { power_adc_enable(); }
void inline adcOff()      { power_adc_disable(); }


// module initialization
void powerInit()
{
  power_usi_disable();
  wdtSetup(WDT_500MS);
}


// configure WDT to interrupt every ~500/~1000 ms
void wdtSetup(WdtInterval interval)
{
  cli();

  wdt_reset();
  MCUSR &= ~(1 << WDRF);

  // enter WDT change mode
  WDTCR |= (1 << WDCE) | (1 << WDE);

  // interrupt mode only
  switch (interval)
  {
    case WDT_500MS:
      // ~0.5s => WDP2 + WDP0
      WDTCR = (1 << WDIE) | (1 << WDP2) | (1 << WDP0);
      break;

    case WDT_1S:
      // ~1.0s => WDP2 + WDP1
      WDTCR = (1 << WDIE) | (1 << WDP2) | (1 << WDP1);
      break;
  }

  sei();
}

// sleep until watchdog interrupt wakes us
void sleep()
{
  wdtFired = false;

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  // brown-out disable (if supppoted by the ATtiny85 core)
  #if defined(BODS) && defined(BODSE)
    sleep_bod_disable();
  #endif

  sleep_cpu();

  // wake-up here
  sleep_disable();
}
