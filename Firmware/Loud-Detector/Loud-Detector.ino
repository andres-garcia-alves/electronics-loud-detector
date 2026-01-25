/*
  Loud Detector - Firmware v1.0
  Target: ATtiny85 @ 1 MHz
  Sensor: KY-037/KY-038 (analog out)

  Features:
  - Sound detection via peak-to-peak window
  - Sensitivity (threshold) via RV1 ADC
  - 2 noise levels (LED2 indicates LEVEL2)
  - Volume is 100% analog (RV2 in series with buzzer)
  - Buzzer + LED share PB1 (digital ON/OFF)
  - Buzzer via tone()
  - Mic power gating via PB0 (high-side PNP typical => logic inverted)
  - Sleep mode using Watchdog (~500ms wakeups)
*/

#include "hardware.h"
#include "low-power.h"
#include "control-logic.h"


void setup()
{
  hardwareInit();
  powerInit();
  delay(200);
}

void loop()
{
  if (isBuzzing())
  {
    checkBuzzStop();
    checkLoudEnvironment();
    delay(30);  // fast polling while alarm is active
  }
  else
  {
    checkLoudEnvironment();
    sleep();    // ~500ms low power idle (WDT wake-up)
  }
}
