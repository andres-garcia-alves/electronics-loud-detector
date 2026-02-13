#pragma once
#include "hardware.h"
#include "low-power.h"

// behavior tuning
#define BUZZ_LEVEL_DELTA  15        // noise level-1 or level-2
#define BUZZ_LEVEL1_MS    5000UL    // 5 seconds
#define BUZZ_LEVEL2_MS    10000UL   // 10 seconds

// state
bool buzzing = false;
unsigned long triggerEnd = 0;
int noiseBase = 0;

bool inline isBuzzing() { return buzzing; }

// if buzzing, keep it for BUZZ_DURATION_MS
void checkBuzzStop()
{
  if (!buzzing) { return; }

  unsigned long now = millis();

  if (now > triggerEnd) {
    buzzing = false;
    buzzStop();
  }

  delay(10);
}

int calculateThresholdLevel2(int threshold1)
{
  int delta = threshold1 / 2;  // +50%
  if (delta < 5)  { delta = 5; }
  if (delta > 30) { delta = 30; }

  return threshold1 + delta;
}

/*int calculateThresholdLevel2(int threshold1)
{
  int delta = BUZZ_LEVEL_DELTA + (threshold1 / 5);  // BUZZ_LEVEL_DELTA (fixed value) + 20% of threshold1
  if (delta < 20) { delta = 20; }                   // clamping minimum
  if (delta > 70) { delta = 70; }                   // clamping maximum
  
  return threshold1 + delta;
}*/

void checkLoudEnvironment()
{
  // turn-on ADC & mic sensor for sensing window
  adcOn();
  micPowerOn();

  // measure sound energy
  int energy = micMeasureEnergy();
  // int energy = micMeasurePeakToPeak();
  
  // actualizar ruido base solo si no está activado
  if (!buzzing)
  {
    // filtro exponencial simple
    noiseBase = (noiseBase * 7 + energy) / 8;
  }

  // read sensitivity knob
  // int threshold1 = adcThresholdEnergy();
  // int threshold1 = adcThresholdPeakToPeak();
  int userMargin = adcThresholdEnergy();   // ahora es margen
  int threshold1 = noiseBase + userMargin;
  int threshold2 = calculateThresholdLevel2(threshold1);

  // turn-off ADC & mic sensor to save power
  adcOff();
  micPowerOff();

  // trigger condition
  if (energy > threshold1) {

    LoudLevel level = (energy > threshold2) ? LEVEL2 : LEVEL1;
    unsigned long extendUntil = millis() + ((energy > threshold2) ? BUZZ_LEVEL2_MS : BUZZ_LEVEL1_MS);
  
    if (!buzzing) {     // if not buzzing, start
      buzzing = true;
      buzzStart(level);
      triggerEnd = extendUntil;
    }
    else {
      // if already buzzing, extend time
      if (buzzing && extendUntil > triggerEnd) { triggerEnd = extendUntil; }

      // if loud rises, turn-on LED2
      if (buzzing && level == LEVEL2) { digitalWrite(PIN_LED2, HIGH); }
    }
  }
}
