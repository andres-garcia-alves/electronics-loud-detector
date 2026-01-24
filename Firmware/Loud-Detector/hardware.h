#pragma once

// pin mapping (ATtiny85)
#define PIN_MIC_ENABLE    PB0     // PB0 -> controls Q1 high-side
#define PIN_BUZZ_LED      PB1     // PB1 -> Q2 base + LED1
#define PIN_LED2          PB3     // PCB -> LED2
#define PIN_MIC_ADC       A1      // PB2 (ADC1) -> KY-038 sensor, pin AO
#define PIN_THR_ADC       A2      // PB4 (ADC2) -> RV1 sensitivity control

// inverse logic (Q1: BC557)
#define MIC_ON            LOW
#define MIC_OFF           HIGH

// audio sensor parameters (KY-037)
#define SENSOR_WARMUP_MS  20UL    // 20ms
#define SENSOR_WINDOW_MS  40UL    // 30-60ms works well

// threshold mapping (RV1 -> peak-to-peak threshold)
#define THR_MIN_P2P       8       // very sensitive
#define THR_MAX_P2P       220     // very strict

enum LoudLevel {LEVEL1, LEVEL2};

void inline micPowerOn()   { digitalWrite(PIN_MIC_ENABLE, MIC_ON); delay(SENSOR_WARMUP_MS); }
void inline micPowerOff()  { digitalWrite(PIN_MIC_ENABLE, MIC_OFF); }


void hardwareInit()
{
  pinMode(PIN_MIC_ENABLE, OUTPUT);

  pinMode(PIN_MIC_ADC, INPUT);
  micPowerOff();

  pinMode(PIN_BUZZ_LED, OUTPUT);
  digitalWrite(PIN_BUZZ_LED, LOW);

  pinMode(PIN_LED2, OUTPUT);
  digitalWrite(PIN_LED2, LOW);
}


void buzzStart(LoudLevel loudLevel)
{
  // digital ON (volume handled by analog trimmer RV2)
  tone(PIN_BUZZ_LED, 2000);

  if (loudLevel == LEVEL2) { digitalWrite(PIN_LED2, HIGH); }
}

void buzzStop()
{
  noTone(PIN_BUZZ_LED);
  digitalWrite(PIN_LED2, LOW);
}

int micMeasurePeakToPeak()
{
  int minRaw = 1023;
  int maxRaw = 0;
  unsigned long start = millis();

  while (millis() - start < SENSOR_WINDOW_MS)
  {
    int value = analogRead(PIN_MIC_ADC);

    if (value < minRaw) { minRaw = value; }
    if (value > maxRaw) { maxRaw = value; }
  }

  return maxRaw - minRaw;
}


int adcThresholdPeakToPeak()
{
  int thresholdRaw = analogRead(PIN_THR_ADC);   // 0..1023
  int threshold = map(thresholdRaw, 0, 1023, THR_MIN_P2P, THR_MAX_P2P);

  // Safety clamp
  if (threshold < THR_MIN_P2P) { threshold = THR_MIN_P2P; }
  if (threshold > THR_MAX_P2P) { threshold = THR_MAX_P2P; }

  return threshold;
}
