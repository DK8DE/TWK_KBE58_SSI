/*
  TWK_KBE58_SSI.h

  Arduino library for reading a TWK KBE 58 - K 4096 G K E06 SSI absolute
  encoder inside a Rohde & Schwarz RD130 rotor.

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
    int dataIdleLevel;
    uint32_t rawValue;
    uint32_t trailingBits;
    uint32_t grayValue;
    uint32_t position;
    uint32_t stepsPerRevolution;
    float angleDeg;
    float angleDegRounded;
  };

  TWK_KBE58_SSI(uint8_t clockPin, uint8_t dataPin);
  TWK_KBE58_SSI(uint8_t clockPin, uint8_t dataPin, uint8_t usefulBits, uint8_t totalClocks, uint8_t trailingBits);

  void begin();

  void setHalfPeriodUs(uint32_t halfPeriodUs);
  void setFramePauseUs(uint32_t framePauseUs);
  void setClockIdleHigh(bool clockIdleHigh);
  void setInvertData(bool invertData);

  Reading read();

  uint32_t readRaw();
  uint32_t grayToBinary(uint32_t grayValue) const;

  uint8_t usefulBits() const;
  uint8_t totalClocks() const;
  uint8_t trailingBits() const;

  uint32_t stepsPerRevolution() const;

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

  uint32_t _usefulBitMask() const;
  uint32_t _roundAngleTo01(float angleDeg) const;
};

#endif
