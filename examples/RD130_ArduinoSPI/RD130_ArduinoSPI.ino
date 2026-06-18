/*
  RD130_ArduinoSPI.ino

  Portable Arduino SPI example for the TWK_KBE58_SSI library.

  The encoder is read continuously so SCK pulses stay visible on a scope.
  Serial output is printed every 200 ms only.

  Hardware SPI needs a full duplex RS422 transceiver between the MCU and
  the RD130 encoder, for example a MAX490 TTL module on 5 V Arduino boards
  or an ADM3490ARZ on 3.3 V ESP32 boards.

  ---------------------------------------------------------------------------
  ESP32 / ESP32-S3 (example pin assignment, change if needed)

    IO8  (SCK)  -> ADM3490 DI       -> encoder CLOCK+/-
    IO9  (MISO) <- ADM3490 RO       <- encoder DATA+/-
    GND         -> ADM3490 GND      -> encoder GND

  ---------------------------------------------------------------------------
  Arduino Uno / Arduino Nano (ATmega328P, 5 V)

    D13 (SCK)   -> MAX490 DI / TXD  -> encoder CLOCK+/-
    D12 (MISO)  <- MAX490 RO / RXD  <- encoder DATA+/-
    D10 (SS)    must be OUTPUT, kept HIGH (SPI master enable)
    D11 (MOSI)  not connected for SSI

  ---------------------------------------------------------------------------
  Arduino Mega 2560 (ATmega2560, 5 V)

    D52 (SCK)   -> MAX490 DI / TXD  -> encoder CLOCK+/-
    D50 (MISO)  <- MAX490 RO / RXD  <- encoder DATA+/-
    D53 (SS)    must be OUTPUT, kept HIGH (SPI master enable)
    D51 (MOSI)  not connected for SSI

  On classic Arduino boards the sketch uses the board default SPI pins
  through the Arduino symbols SCK, MISO and SS.

  SPI note:
  Portable Arduino SPI usually generates 16 clock pulses. The library keeps
  the 13 relevant SSI bits with setSpiRightShift(3).

  Copyright (C) 2026 Joerg Koerner DK8DE
  License: GPLv3 or later
*/

#include <SPI.h>
#include <TWK_KBE58_SSI.h>

const uint32_t SERIAL_PRINT_INTERVAL_MS = 200;

#if defined(ESP32)

// ESP32: free SPI pins via FSPI, connected to ADM3490 or another RS422 module.
const uint8_t PIN_SSI_CLOCK = 8;
const uint8_t PIN_SSI_DATA = 9;

SPIClass ssiSPI(FSPI);
TWK_KBE58_SSI encoder(PIN_SSI_CLOCK, PIN_SSI_DATA);

#else

// Arduino Uno / Nano: D13=SCK, D12=MISO, D10=SS
// Arduino Mega 2560:  D52=SCK, D50=MISO, D53=SS
const uint8_t PIN_SSI_CLOCK = SCK;
const uint8_t PIN_SSI_DATA = MISO;
const uint8_t PIN_SSI_SPI_SS = SS;

TWK_KBE58_SSI encoder(PIN_SSI_CLOCK, PIN_SSI_DATA);

#endif

TWK_KBE58_SSI::Reading g_lastReading;
unsigned long g_lastPrintMs = 0;

void setup()
{
  Serial.begin(115200);

#if defined(ESP32)
  encoder.beginSPI(ssiSPI, PIN_SSI_CLOCK, PIN_SSI_DATA, 100000);
#else
  encoder.beginSPI(SPI, PIN_SSI_CLOCK, PIN_SSI_DATA, 100000);
  encoder.setSpiDummySsPin(PIN_SSI_SPI_SS);
#endif

  encoder.setSpiMode(SPI_MODE2);
  encoder.setSpiTransferBits(16);
  encoder.setSpiRightShift(3);
  encoder.setFramePauseUs(80);

  Serial.println("RD130 Arduino SPI started");

#if defined(ESP32)
  Serial.print("ESP32 SCK:  IO");
  Serial.println(PIN_SSI_CLOCK);
  Serial.print("ESP32 MISO: IO");
  Serial.println(PIN_SSI_DATA);
#elif defined(__AVR_ATmega2560__)
  Serial.println("Mega2560 SCK:  D52 -> MAX490 DI");
  Serial.println("Mega2560 MISO: D50 <- MAX490 RO");
  Serial.println("Mega2560 SS:   D53 (OUTPUT, HIGH)");
#else
  Serial.println("Uno/Nano SCK:  D13 -> MAX490 DI");
  Serial.println("Uno/Nano MISO: D12 <- MAX490 RO");
  Serial.println("Uno/Nano SS:   D10 (OUTPUT, HIGH)");
#endif

  Serial.println("Continuous SPI reads, Serial every 200 ms");
  Serial.println("SPI sends 16 clock pulses per read (13 used after shift)");
}

void loop()
{
  g_lastReading = encoder.read();

  if (!g_lastReading.valid)
  {
    return;
  }

  unsigned long nowMs = millis();

  if (nowMs - g_lastPrintMs < SERIAL_PRINT_INTERVAL_MS)
  {
    return;
  }

  g_lastPrintMs = nowMs;

  Serial.print("Raw: ");
  for (int8_t bit = encoder.totalClocks() - 1; bit >= 0; bit--)
  {
    Serial.print((g_lastReading.rawValue >> bit) & 1);
  }

  Serial.print(" Position: ");
  Serial.print(g_lastReading.position);
  Serial.print(" Angle: ");
  Serial.println(g_lastReading.angleDegRounded, 1);
}
