/*
  RD130_ADM3490_Diagnose.ino

  Example for the TWK_KBE58_SSI Arduino library.

  This example reads the SSI absolute encoder inside a Rohde & Schwarz RD130
  rotor. The encoder is assumed to be a TWK KBE 58 - K 4096 G K E06 with Gray
  code, 12 useful position bits and 13 SSI clocks.

  The interface between the ESP32 and the encoder uses one ADM3490ARZ full
  duplex RS422 transceiver.

  Copyright (C) 2026 Joerg Koerner DK8DE

  This example is free software: you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or any later version.

  This example is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  ADM3490ARZ connection:

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
  - Never connect CLOCK+/CLOCK- or DATA+/DATA- directly to ESP32 GPIO pins.
  - Use the ADM3490ARZ or another suitable full duplex RS422 transceiver.
  - Connect ESP32 GND, ADM3490 GND and encoder 0 V together.
  - Keep CLOCK+/CLOCK- and DATA+/DATA- as twisted or closely coupled pairs.
  - Use a 100 nF decoupling capacitor close to the ADM3490 VCC and GND pins.
  - For the DATA pair, a 120 ohm termination close to the ADM3490 receiver is recommended.
*/

#include <Arduino.h>
#include <TWK_KBE58_SSI.h>

// ESP32 GPIO connected to ADM3490 DI.
const uint8_t PIN_SSI_CLOCK = 8;

// ESP32 GPIO connected to ADM3490 RO.
const uint8_t PIN_SSI_DATA = 9;

// Encoder configuration for TWK KBE 58 - K 4096 G K E06.
const uint8_t SSI_USEFUL_BITS = 12;
const uint8_t SSI_TOTAL_CLOCKS = 13;
const uint8_t SSI_TRAILING_BITS = 1;

// Create the encoder reader object.
TWK_KBE58_SSI encoder(PIN_SSI_CLOCK, PIN_SSI_DATA, SSI_USEFUL_BITS, SSI_TOTAL_CLOCKS, SSI_TRAILING_BITS);

void printBoth(const char *text)
{
  Serial.print(text);
  Serial0.print(text);
}

void printlnBoth(const char *text)
{
  Serial.println(text);
  Serial0.println(text);
}

void printUIntBoth(uint32_t value)
{
  Serial.print(value);
  Serial0.print(value);
}

void printIntBoth(int value)
{
  Serial.print(value);
  Serial0.print(value);
}

void printFloatBoth(float value, uint8_t digits)
{
  Serial.print(value, digits);
  Serial0.print(value, digits);
}

void printBinaryBoth(uint32_t value, uint8_t bitCount)
{
  for (int8_t bit = bitCount - 1; bit >= 0; bit--)
  {
    uint8_t bitValue = (value >> bit) & 1;
    Serial.print(bitValue);
    Serial0.print(bitValue);
  }
}

void printStartupInfo()
{
  Serial.println();
  Serial0.println();

  printlnBoth("Rohde & Schwarz RD130 SSI encoder diagnosis started");
  printlnBoth("Library: TWK_KBE58_SSI");
  printlnBoth("License: GPL");
  printlnBoth("Encoder: TWK KBE 58 - K 4096 G K E06, Gray SSI");
  printlnBoth("ESP32 CLOCK GPIO: IO8 -> ADM3490 Pin 3 DI");
  printlnBoth("ESP32 DATA  GPIO: IO9 <- ADM3490 Pin 2 RO");
  printlnBoth("ADM3490 Pin 5 Y -> RD130 CLOCK+ / data connector pin 2");
  printlnBoth("ADM3490 Pin 6 Z -> RD130 CLOCK- / data connector pin 3");
  printlnBoth("ADM3490 Pin 8 A <- RD130 DATA+  / data connector pin 4");
  printlnBoth("ADM3490 Pin 7 B <- RD130 DATA-  / data connector pin 5");
  printlnBoth("Angle output is rounded to 0.1 degree");
  Serial.println();
  Serial0.println();
}

void setup()
{
  // Serial is used for native USB CDC on many ESP32-S3 boards.
  Serial.begin(115200);

  // Serial0 is used for the UART0 USB bridge on many ESP32-S3 boards.
  Serial0.begin(115200);

  // Give the PC time to open the USB serial port after reset.
  delay(3000);

  // Initialize the encoder pins.
  encoder.begin();

  // Use a safe default timing for the TWK SSI encoder.
  encoder.setHalfPeriodUs(5);
  encoder.setFramePauseUs(80);
  encoder.setClockIdleHigh(true);
  encoder.setInvertData(false);

  printStartupInfo();
}

void loop()
{
  // Read all currently available diagnostic values from the library.
  TWK_KBE58_SSI::Reading reading = encoder.read();

  printBoth("DATA-Idle: ");
  printIntBoth(reading.dataIdleLevel);

  printBoth(" | Raw: 0b");
  printBinaryBoth(reading.rawValue, encoder.totalClocks());

  printBoth(" | Trailing: ");
  printUIntBoth(reading.trailingBits);

  printBoth(" | Gray: 0b");
  printBinaryBoth(reading.grayValue, encoder.usefulBits());

  printBoth(" | Position: ");
  printUIntBoth(reading.position);

  printBoth(" / ");
  printUIntBoth(encoder.stepsPerRevolution());

  printBoth(" | Angle: ");
  printFloatBoth(reading.angleDegRounded, 1);
  printlnBoth(" deg");

  delay(200);
}
