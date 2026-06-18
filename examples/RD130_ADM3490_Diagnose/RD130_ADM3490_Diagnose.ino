/*
  RD130_ADM3490_Diagnose.ino

  Example for the TWK_KBE58_SSI Arduino library.

  This example reads the SSI absolute encoder inside a Rohde & Schwarz RD130
  rotor. The encoder is assumed to be a TWK KBE 58 - K 4096 G K E06 with Gray
  code, 12 useful position bits and 13 SSI clocks.

  The interface between the microcontroller and the encoder uses one full duplex
  RS422 transceiver. For 3.3 V boards such as ESP32, the ADM3490ARZ can be used.
  For 5 V Arduino boards such as Uno, Nano or Mega, use a 5 V full duplex RS422
  transceiver such as the MAX490.

  Copyright (C) 2026 Joerg Koerner DK8DE

  This example is free software: you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or any later version.

  This example is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  ADM3490ARZ connection for ESP32 or another 3.3 V board:

                   ADM3490ARZ
            +----------------------+
   +3V3 ----| 1 VCC            A 8 |---- DATA+  -> Encoder Pin 3
   IO9  <---| 2 RO             B 7 |---- DATA-  -> Encoder Pin 4
   IO8  --->| 3 DI             Z 6 |---- CLOCK- -> Encoder Pin 2
   GND  ----| 4 GND            Y 5 |---- CLOCK+ -> Encoder Pin 1
            +----------------------+

  Connection at the Rohde & Schwarz RD130 rotor:

  Data connector:

  1 ---> VCC 11-30 V + SET input via push button to VCC -- set encoder to 0
  2 ---> CLOCK IN +
  3 ---> CLOCK IN -
  4 ---> DATA OUT +
  5 ---> DATA OUT -
  6 ---> GND + Code Sense 0 = CW 1 = CCW
  8 ---> Shield

  Motor connector:

  1 ---> Motor 1 +
  2 ---> Motor 1 -
  3 ---> Filter 1 ground
  4 ---> Motor 1 -
  5 ---> Motor 1 +
  6 ---> Filter 2 ground

  Important notes:
  - Never connect CLOCK+/CLOCK- or DATA+/DATA- directly to Arduino or ESP32 GPIO pins.
  - Use the ADM3490ARZ for 3.3 V boards or another suitable full duplex RS422 transceiver.
  - For 5 V Arduino boards, use a 5 V full duplex RS422 transceiver such as the MAX490.
  - Connect microcontroller GND, RS422 transceiver GND and encoder 0 V together.
  - Keep CLOCK+/CLOCK- and DATA+/DATA- as twisted or closely coupled pairs.
  - Use a 100 nF decoupling capacitor close to the RS422 transceiver VCC and GND pins.
  - For the DATA pair, a 120 ohm termination close to the receiver is recommended.
  - For longer CLOCK lines, a 120 ohm termination at the encoder side may be required.
*/

#include <Arduino.h>
#include <TWK_KBE58_SSI.h>

// Default ESP32-S3 GPIO connected to ADM3490 DI.
// Change this pin for other Arduino-compatible boards.
const uint8_t PIN_SSI_CLOCK = 8;

// Default ESP32-S3 GPIO connected to ADM3490 RO.
// Change this pin for other Arduino-compatible boards.
const uint8_t PIN_SSI_DATA = 9;

// Encoder configuration for TWK KBE 58 - K 4096 G K E06.
const uint8_t SSI_USEFUL_BITS = 12;
const uint8_t SSI_TOTAL_CLOCKS = 13;
const uint8_t SSI_TRAILING_BITS = 1;

// Create the encoder reader object.
TWK_KBE58_SSI encoder(PIN_SSI_CLOCK, PIN_SSI_DATA, SSI_USEFUL_BITS, SSI_TOTAL_CLOCKS, SSI_TRAILING_BITS);

void debugBegin(unsigned long baudRate)
{
  Serial.begin(baudRate);

#if defined(ESP32)
  Serial0.begin(baudRate);
#endif
}

void debugPrint(const char *text)
{
  Serial.print(text);

#if defined(ESP32)
  Serial0.print(text);
#endif
}

void debugPrintln(const char *text)
{
  Serial.println(text);

#if defined(ESP32)
  Serial0.println(text);
#endif
}

void debugPrintInt(int value)
{
  Serial.print(value);

#if defined(ESP32)
  Serial0.print(value);
#endif
}

void debugPrintUInt(uint32_t value)
{
  Serial.print(value);

#if defined(ESP32)
  Serial0.print(value);
#endif
}

void debugPrintFloat(float value, uint8_t digits)
{
  Serial.print(value, digits);

#if defined(ESP32)
  Serial0.print(value, digits);
#endif
}

void debugPrintlnEmpty()
{
  Serial.println();

#if defined(ESP32)
  Serial0.println();
#endif
}

void debugPrintBinary(uint32_t value, uint8_t bitCount)
{
  for (int8_t bit = bitCount - 1; bit >= 0; bit--)
  {
    uint8_t bitValue = (value >> bit) & 1;

    Serial.print(bitValue);

#if defined(ESP32)
    Serial0.print(bitValue);
#endif
  }
}

void printStartupInfo()
{
  debugPrintlnEmpty();
  debugPrintln("Rohde & Schwarz RD130 SSI encoder diagnosis started");
  debugPrintln("Library: TWK_KBE58_SSI");
  debugPrintln("License: GPLv3 or later");
  debugPrintln("Encoder: TWK KBE 58 - K 4096 G K E06, Gray SSI");
  debugPrintln("Default clock pin: IO8 -> RS422 driver input");
  debugPrintln("Default data  pin: IO9 <- RS422 receiver output");
  debugPrintln("3.3 V boards: ADM3490ARZ can be used");
  debugPrintln("5 V boards: use a 5 V full duplex RS422 transceiver such as MAX490");
  debugPrintln("Angle output is rounded to 0.1 degree");

#if defined(ESP32)
  debugPrintln("ESP32 detected: output is sent to Serial and Serial0");
#else
  debugPrintln("Non-ESP32 board detected: output is sent to Serial only");
#endif

  debugPrintlnEmpty();
}

void setup()
{
  debugBegin(115200);

  delay(3000);

  encoder.beginBitBang();

  encoder.setHalfPeriodUs(5);
  encoder.setFramePauseUs(80);
  encoder.setClockIdleHigh(true);
  encoder.setInvertData(false);

  printStartupInfo();
}

void loop()
{
  TWK_KBE58_SSI::Reading reading = encoder.read();

  if (!reading.valid)
  {
    delay(200);
    return;
  }

  debugPrint("DATA-Idle: ");
  debugPrintInt(reading.dataIdleLevel);

  debugPrint(" | Raw: 0b");
  debugPrintBinary(reading.rawValue, encoder.totalClocks());

  debugPrint(" | Trailing: ");
  debugPrintUInt(reading.trailingBits);

  debugPrint(" | Gray: 0b");
  debugPrintBinary(reading.grayValue, encoder.usefulBits());

  debugPrint(" | Position: ");
  debugPrintUInt(reading.position);

  debugPrint(" / ");
  debugPrintUInt(encoder.stepsPerRevolution());

  debugPrint(" | Angle: ");
  debugPrintFloat(reading.angleDegRounded, 1);
  debugPrintln(" deg");

  delay(200);
}
