/*
  Loud Detector - Firmware v1.0
  Target: ATtiny85 @ 1 MHz
  Sensor: KY-037 (analog out)

  Features:
  - Sound detection via peak-to-peak window (robust)
  - Sensitivity (threshold) via RV1 ADC
  - Volume is 100% analog (RV2 in series with buzzer)
  - Buzzer + LED share PB1 (digital ON/OFF)
  - Mic power gating via PB0 (high-side PNP typical => logic inverted)
*/

#include "hardware.h"
#include "control-logic.h"

bool activeTrigger = false;

void setup()
{
  hardwareInit();
  delay(200);
}

void loop()
{
  activeTrigger = checkBuzzLastTrigger();
  if (activeTrigger) { return; }

  checkBuzzStop();

  checkLoudEnvironment();
  delay(50); // sampling interval (for v1)
}
