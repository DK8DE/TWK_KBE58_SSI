/*
  TWK_KBE58_SSI.h

  Arduino library for reading a TWK KBE 58 - K 4096 G K E06 absolute encoder
  inside a Rohde & Schwarz RD130 rotor using SSI over an RS422 transceiver.

  Copyright (C) 2026 Joerg Koerner DK8DE

  This library is free software: you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
*/

#ifndef TWK_KBE58_SSI_H
#define TWK_KBE58_SSI_H

#include <Arduino.h>

class TWK_KBE58_SSI
{
public:
  struct Reading
  {
    // Logic level at the DATA input before the SSI telegram starts.
    int dataIdleLevel;

    // Full SSI word as read from the encoder, including trailing bits.
    uint32_t rawValue;

    // Trailing bits after the useful position bits.
    uint32_t trailingBits;

    // Useful Gray-coded position bits.
    uint32_t grayValue;

    // Binary position after Gray-to-binary conversion.
    uint32_t position;

    // Calculated angle in degrees.
    float angleDeg;

    // Calculated angle rounded to 0.1 degree.
    float angleDegRounded;
  };

  TWK_KBE58_SSI(
    uint8_t clockPin,
    uint8_t dataPin,
    uint8_t usefulBits = 12,
    uint8_t totalClocks = 13,
    uint8_t trailingBits = 1
  );

  // Configure the clock and data pins.
  void begin();

  // Read all diagnostic and calculated values.
  Reading read();

  // Read the complete SSI raw word only.
  uint32_t readRaw();

  // Convert a Gray-coded value to binary.
  uint32_t grayToBinary(uint32_t gray) const;

  // Return the number of positions per revolution.
  uint32_t stepsPerRevolution() const;

  // Return the bit mask for the useful encoder bits.
  uint32_t usefulBitMask() const;

  // Convert a binary encoder position to degrees.
  float positionToDegrees(uint32_t position) const;

  // Round an angle to 0.1 degree.
  float roundAngleToOneDecimal(float angleDeg) const;

  // Set half of the SSI clock period in microseconds.
  void setHalfPeriodUs(uint32_t halfPeriodUs);

  // Set the pause between SSI frames in microseconds.
  void setFramePauseUs(uint32_t framePauseUs);

  // Invert the received DATA bit if the receiver polarity is reversed.
  void setInvertData(bool invertData);

  // Set the SSI clock idle level.
  void setClockIdleHigh(bool clockIdleHigh);

  uint8_t clockPin() const;
  uint8_t dataPin() const;
  uint8_t usefulBits() const;
  uint8_t totalClocks() const;
  uint8_t trailingBitsCount() const;

private:
  uint8_t _clockPin;
  uint8_t _dataPin;
  uint8_t _usefulBits;
  uint8_t _totalClocks;
  uint8_t _trailingBits;

  bool _clockIdleHigh;
  bool _invertData;

  uint32_t _halfPeriodUs;
  uint32_t _framePauseUs;
};

#endif
