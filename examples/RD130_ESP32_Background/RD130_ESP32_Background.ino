/*
  RD130_ESP32_Background.ino

  ESP32 background read example for the TWK_KBE58_SSI library.

  Uses BitBang mode by default because it matches the proven RD130 diagnose
  sketch and generates exactly 13 SSI clock pulses.

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

#if defined(ESP32)
  Serial0.begin(115200);
#endif

  encoder.beginBitBang();
  encoder.setHalfPeriodUs(5);
  encoder.setFramePauseUs(80);

#if defined(ESP32)
  if (!encoder.startBackgroundRead(10))
  {
    Serial.println("Background read start failed");
#if defined(ESP32)
    Serial0.println("Background read start failed");
#endif
    while (true)
    {
      delay(1000);
    }
  }

  Serial.println("RD130 background read started (BitBang, 10 ms)");
  Serial0.println("RD130 background read started (BitBang, 10 ms)");
#endif
}

void loop()
{
#if defined(ESP32)
  if (encoder.hasNewReading())
  {
    TWK_KBE58_SSI::Reading reading = encoder.getLastReading();

    if (!reading.valid)
    {
      return;
    }

    Serial.print("Position: ");
    Serial.print(reading.position);
    Serial.print(" / ");
    Serial.print(reading.stepsPerRevolution);
    Serial.print(" Angle: ");
    Serial.print(reading.angleDegRounded, 1);
    Serial.println(" deg");

    Serial0.print("Position: ");
    Serial0.print(reading.position);
    Serial0.print(" Angle: ");
    Serial0.print(reading.angleDegRounded, 1);
    Serial0.println(" deg");
  }
#else
  TWK_KBE58_SSI::Reading reading = encoder.read();
  Serial.println(reading.angleDegRounded, 1);
  delay(200);
#endif
}
