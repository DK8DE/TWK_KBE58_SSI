/*
  RD130_BitBang.ino

  Minimal BitBang example for the TWK_KBE58_SSI library.

  Copyright (C) 2026 Joerg Koerner DK8DE
  License: GPLv3 or later
*/

#include <TWK_KBE58_SSI.h>

const uint8_t PIN_SSI_CLOCK = 8;
const uint8_t PIN_SSI_DATA = 9;

TWK_KBE58_SSI encoder(PIN_SSI_CLOCK, PIN_SSI_DATA);

void setup()
{
  Serial.begin(115200);

  encoder.beginBitBang();
  encoder.setHalfPeriodUs(5);
  encoder.setFramePauseUs(80);
}

void loop()
{
  TWK_KBE58_SSI::Reading reading = encoder.read();

  if (!reading.valid)
  {
    return;
  }

  Serial.print("Position: ");
  Serial.print(reading.position);
  Serial.print(" Angle: ");
  Serial.println(reading.angleDegRounded, 1);

  delay(10);
}
