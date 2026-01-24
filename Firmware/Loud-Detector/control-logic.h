#pragma once
#include "hardware.h"

// behavior tuning
#define BUZZ_LEVEL_DELTA  25      // noise level-1 or level-2
#define BUZZ_LEVEL1_MS    5000UL  // 5 seconds
#define BUZZ_LEVEL2_MS    10000UL // 10 seconds
#define COOLDOWN_MS       1500UL  // prevents rapid re-triggering

// state
bool buzzing = false;
unsigned long lastTrigger = 0;
unsigned long triggerEndMillis = 0;


// don't trigger again until COOLDOWN_MS
bool checkBuzzLastTrigger()
{
  unsigned long now = millis();

  if (now - lastTrigger < COOLDOWN_MS) { delay(20); return true; }
  return false;
}

// if buzzing, keep it for BUZZ_DURATION_MS
void checkBuzzStop()
{
  if (!buzzing) { return; }

  unsigned long now = millis();

  if (now > triggerEndMillis) {
    buzzing = false;
    lastTrigger = now;
    buzzStop();
  }

  delay(10);
}

void checkLoudEnvironment()
{
  // turn-on mic for sensing window
  micPowerOn();

  // measure sound energy
  int peakToPeak = micMeasurePeakToPeak();

  // read sensitivity knob
  int threshold1 = adcThresholdPeakToPeak();
  int threshold2 = threshold1 + BUZZ_LEVEL_DELTA;

  // turn-off mic to save power
  micPowerOff();

  // trigger condition
  if (peakToPeak > threshold1) {

    LoudLevel level = (peakToPeak > threshold2) ? LEVEL2 : LEVEL1;
    unsigned long extendUntil = millis() + ((peakToPeak > threshold2) ? BUZZ_LEVEL2_MS : BUZZ_LEVEL1_MS);
  
    if (!buzzing) {                       // if not buzzing, start
      buzzing = true;
      buzzStart(level);
      triggerEndMillis = extendUntil;
    }
    else {
      // if already buzzing, then extended time
      if (buzzing && extendUntil > triggerEndMillis) { triggerEndMillis = extendUntil; }
      // if loud rises, turn-on LED2
      if (buzzing && level == LEVEL2) { digitalWrite(PIN_LED2, HIGH); }
    }
  }
}
