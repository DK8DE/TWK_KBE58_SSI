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

void printBinary(uint32_t value, uint8_t bitCount)
{
  // Print a fixed-width binary value.
  for (int8_t bit = bitCount - 1; bit >= 0; bit--)
  {
    uint8_t bitValue = (value >> bit) & 1;
    Serial.print(bitValue);
  }
}

void printStartupInfo()
{
  // Print startup information to the serial console.
  Serial.println();
  Serial.println("Rohde & Schwarz RD130 SSI encoder diagnosis started");
  Serial.println("Library: TWK_KBE58_SSI");
  Serial.println("License: GPLv3 or later");
  Serial.println("Encoder: TWK KBE 58 - K 4096 G K E06, Gray SSI");
  Serial.println("Default clock pin: IO8 -> RS422 driver input");
  Serial.println("Default data  pin: IO9 <- RS422 receiver output");
  Serial.println("3.3 V boards: ADM3490ARZ can be used");
  Serial.println("5 V boards: use a 5 V full duplex RS422 transceiver such as MAX490");
  Serial.println("Angle output is rounded to 0.1 degree");
  Serial.println();
}

void setup()
{
  // Start the serial console.
  Serial.begin(115200);

  // Give boards with native USB some time after reset.
  delay(3000);

  // Initialize the encoder pins.
  encoder.begin();

  // Use a safe default timing for the TWK SSI encoder.
  encoder.setHalfPeriodUs(5);
  encoder.setFramePauseUs(80);
  encoder.setClockIdleHigh(true);
  encoder.setInvertData(false);

  // Print information about the example and hardware connection.
  printStartupInfo();
}

void loop()
{
  // Read all currently available diagnostic values from the library.
  TWK_KBE58_SSI::Reading reading = encoder.read();

  // Print DATA idle level.
  Serial.print("DATA-Idle: ");
  Serial.print(reading.dataIdleLevel);

  // Print complete SSI raw value.
  Serial.print(" | Raw: 0b");
  printBinary(reading.rawValue, encoder.totalClocks());

  // Print trailing bits.
  Serial.print(" | Trailing: ");
  Serial.print(reading.trailingBits);

  // Print extracted Gray value.
  Serial.print(" | Gray: 0b");
  printBinary(reading.grayValue, encoder.usefulBits());

  // Print binary encoder position.
  Serial.print(" | Position: ");
  Serial.print(reading.position);

  // Print maximum position count.
  Serial.print(" / ");
  Serial.print(encoder.stepsPerRevolution());

  // Print angle rounded to 0.1 degree.
  Serial.print(" | Angle: ");
  Serial.print(reading.angleDegRounded, 1);
  Serial.println(" deg");

  // Slow down the serial output.
  delay(200);
}
