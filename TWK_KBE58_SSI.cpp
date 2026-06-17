/*
  TWK_KBE58_SSI.cpp

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

#include "TWK_KBE58_SSI.h"

TWK_KBE58_SSI::TWK_KBE58_SSI(uint8_t clockPin, uint8_t dataPin)
{
  _clockPin = clockPin;
  _dataPin = dataPin;

  _usefulBits = 12;
  _totalClocks = 13;
  _trailingBits = 1;

  _clockIdleHigh = true;
  _invertData = false;

  _halfPeriodUs = 5;
  _framePauseUs = 80;
}

TWK_KBE58_SSI::TWK_KBE58_SSI(uint8_t clockPin, uint8_t dataPin, uint8_t usefulBits, uint8_t totalClocks, uint8_t trailingBits)
{
  _clockPin = clockPin;
  _dataPin = dataPin;

  _usefulBits = usefulBits;
  _totalClocks = totalClocks;
  _trailingBits = trailingBits;

  _clockIdleHigh = true;
  _invertData = false;

  _halfPeriodUs = 5;
  _framePauseUs = 80;
}

void TWK_KBE58_SSI::begin()
{
  pinMode(_clockPin, OUTPUT);
  pinMode(_dataPin, INPUT);

  if (_clockIdleHigh)
  {
    digitalWrite(_clockPin, HIGH);
  }
  else
  {
    digitalWrite(_clockPin, LOW);
  }
}

void TWK_KBE58_SSI::setHalfPeriodUs(uint32_t halfPeriodUs)
{
  _halfPeriodUs = halfPeriodUs;
}

void TWK_KBE58_SSI::setFramePauseUs(uint32_t framePauseUs)
{
  _framePauseUs = framePauseUs;
}

void TWK_KBE58_SSI::setClockIdleHigh(bool clockIdleHigh)
{
  _clockIdleHigh = clockIdleHigh;

  if (_clockIdleHigh)
  {
    digitalWrite(_clockPin, HIGH);
  }
  else
  {
    digitalWrite(_clockPin, LOW);
  }
}

void TWK_KBE58_SSI::setInvertData(bool invertData)
{
  _invertData = invertData;
}

TWK_KBE58_SSI::Reading TWK_KBE58_SSI::read()
{
  Reading reading;

  reading.dataIdleLevel = digitalRead(_dataPin);
  reading.rawValue = readRaw();

  uint32_t trailingMask = 0;

  if (_trailingBits > 0)
  {
    trailingMask = (1UL << _trailingBits) - 1;
  }

  reading.trailingBits = reading.rawValue & trailingMask;

  reading.grayValue = reading.rawValue >> _trailingBits;
  reading.grayValue &= _usefulBitMask();

  reading.position = grayToBinary(reading.grayValue);
  reading.stepsPerRevolution = stepsPerRevolution();

  reading.angleDeg = ((float)reading.position * 360.0f) / (float)reading.stepsPerRevolution;
  reading.angleDegRounded = ((float)_roundAngleTo01(reading.angleDeg)) / 10.0f;

  return reading;
}

uint32_t TWK_KBE58_SSI::readRaw()
{
  uint32_t rawValue = 0;

  if (_clockIdleHigh)
  {
    digitalWrite(_clockPin, HIGH);
  }
  else
  {
    digitalWrite(_clockPin, LOW);
  }

  delayMicroseconds(_framePauseUs);

  for (uint8_t bitIndex = 0; bitIndex < _totalClocks; bitIndex++)
  {
    digitalWrite(_clockPin, LOW);
    delayMicroseconds(_halfPeriodUs);

    digitalWrite(_clockPin, HIGH);
    delayMicroseconds(_halfPeriodUs);

    int dataBit = digitalRead(_dataPin);

    if (_invertData)
    {
      dataBit = !dataBit;
    }

    rawValue <<= 1;

    if (dataBit)
    {
      rawValue |= 1;
    }
  }

  delayMicroseconds(_framePauseUs);

  return rawValue;
}

uint32_t TWK_KBE58_SSI::grayToBinary(uint32_t grayValue) const
{
  uint32_t binaryValue = grayValue;

  while (grayValue >>= 1)
  {
    binaryValue ^= grayValue;
  }

  return binaryValue;
}

uint8_t TWK_KBE58_SSI::usefulBits() const
{
  return _usefulBits;
}

uint8_t TWK_KBE58_SSI::totalClocks() const
{
  return _totalClocks;
}

uint8_t TWK_KBE58_SSI::trailingBits() const
{
  return _trailingBits;
}

uint32_t TWK_KBE58_SSI::stepsPerRevolution() const
{
  return 1UL << _usefulBits;
}

uint32_t TWK_KBE58_SSI::_usefulBitMask() const
{
  return stepsPerRevolution() - 1;
}

uint32_t TWK_KBE58_SSI::_roundAngleTo01(float angleDeg) const
{
  return (uint32_t)((angleDeg * 10.0f) + 0.5f);
}
