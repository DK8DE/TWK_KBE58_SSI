# TWK_KBE58_SSI

Arduino library for reading the SSI absolute encoder inside a Rohde & Schwarz RD130 rotor with an ESP32 and an ADM3490ARZ full-duplex RS422 transceiver.

The library was written for the TWK KBE 58 - K 4096 G K E06 absolute encoder. This encoder uses Gray-coded SSI data with 12 useful position bits, 13 SSI clock pulses, and one trailing bit. The library reads the raw SSI word, extracts the Gray value, converts it to a binary position value, and calculates the rotor angle in degrees.

## Features

- Reads a TWK KBE 58 - K 4096 G K E06 SSI absolute encoder
- Intended for the Rohde & Schwarz RD130 rotor
- Designed for ESP32 boards
- Uses an RS422 transceiver such as the ADM3490ARZ
- Reads the complete SSI raw word
- Extracts trailing bits
- Extracts the Gray-coded position value
- Converts Gray code to binary position
- Calculates the angle in degrees
- Provides the angle rounded to 0.1 degree
- Provides the DATA idle level before each SSI telegram
- Supports configurable SSI timing
- Supports optional DATA inversion
- Supports configurable clock idle state
- Licensed under the GNU General Public License

## Hardware Overview

The encoder interface is SSI over differential RS422 signals. Do not connect the encoder's CLOCK or DATA differential lines directly to ESP32 GPIO pins.

Use a full-duplex RS422 transceiver between the ESP32 and the encoder. The example included with this library uses an ADM3490ARZ.

## ADM3490ARZ Connection

```text
                 ADM3490ARZ
          +----------------------+
 +3V3 ----| 1 VCC            A 8 |---- DATA+  -> Encoder Pin 3
 IO9  <---| 2 RO             B 7 |---- DATA-  -> Encoder Pin 4
 IO8  --->| 3 DI             Z 6 |---- CLOCK- -> Encoder Pin 2
 GND  ----| 4 GND            Y 5 |---- CLOCK+ -> Encoder Pin 1
          +----------------------+
```

## Connection at the Rohde & Schwarz RD130 Rotor

### Data Connector

```text
1 ---> VCC 11-30 V + SET input via push button to VCC -- set encoder to 0
2 ---> CLOCK IN +
3 ---> CLOCK IN -
4 ---> DATA OUT +
5 ---> DATA OUT -
6 ---> GND + Code Sense 0 = CW 1 = CCW
8 ---> Shield
```

### Motor Connector

```text
1 ---> Motor 1 +
2 ---> Motor 1 -
3 ---> Filter 1 ground
4 ---> Motor 1 -
5 ---> Motor 1 +
6 ---> Filter 2 ground
```

## Important Hardware Notes

- The encoder needs its own supply voltage of 11-30 V DC.
- The ESP32 and ADM3490ARZ use 3.3 V logic.
- Connect ESP32 GND, ADM3490ARZ GND, encoder 0 V, and power supply 0 V together.
- Keep CLOCK+ and CLOCK- as a differential pair.
- Keep DATA+ and DATA- as a differential pair.
- Use twisted or closely coupled wiring for the differential pairs.
- Add a 100 nF decoupling capacitor close to the ADM3490ARZ VCC and GND pins.
- A 120 ohm termination resistor between DATA+ and DATA- near the ADM3490ARZ receiver is recommended.
- A 120 ohm termination resistor between CLOCK+ and CLOCK- near the encoder can be useful on longer cables.
- Do not drive the motor connector from the ESP32 directly. Use a suitable motor driver or relay circuit.

## Installation

### Arduino IDE ZIP Installation

1. Download the library ZIP file.
2. Open the Arduino IDE.
3. Select `Sketch -> Include Library -> Add .ZIP Library...`.
4. Select the ZIP file.
5. Open the example from `File -> Examples -> TWK_KBE58_SSI -> RD130_ADM3490_Diagnose`.

### Manual Installation

Copy the complete library folder into your Arduino libraries directory:

```text
Documents/Arduino/libraries/TWK_58_SSI
```

Then restart the Arduino IDE.

## Example

The included example is named:

```text
RD130_ADM3490_Diagnose
```

It reads all available diagnostic values from the encoder and prints them to the serial console.

Example output:

```text
DATA-Idle: 1 | Raw: 0b0110111110110 | Trailing: 0 | Gray: 0b011011111011 | Position: 1197 / 4096 | Angle: 105.2 deg
```

The example prints to both `Serial` and `Serial0`. This is useful on ESP32-S3 boards with two USB ports, where the native USB CDC port and the USB-UART bridge may appear as different serial ports.

## Basic Usage

```cpp
#include <Arduino.h>
#include <TWK_KBE58_SSI.h>

const uint8_t PIN_SSI_CLOCK = 8;
const uint8_t PIN_SSI_DATA = 9;

TWK_KBE58_SSI encoder(PIN_SSI_CLOCK, PIN_SSI_DATA);

void setup()
{
  Serial.begin(115200);

  encoder.begin();
  encoder.setHalfPeriodUs(5);
  encoder.setFramePauseUs(80);
  encoder.setClockIdleHigh(true);
  encoder.setInvertData(false);
}

void loop()
{
  TWK_KBE58_SSI::Reading reading = encoder.read();

  Serial.print("Raw: ");
  Serial.print(reading.rawValue);

  Serial.print(" Position: ");
  Serial.print(reading.position);

  Serial.print(" Angle: ");
  Serial.print(reading.angleDegRounded, 1);
  Serial.println(" deg");

  delay(200);
}
```

## Reading Data Structure

The `read()` function returns a `TWK_KBE58_SSI::Reading` structure.

```cpp
struct Reading
{
  int dataIdleLevel;
  uint32_t rawValue;
  uint32_t trailingBits;
  uint32_t grayValue;
  uint32_t position;
  float angleDeg;
  float angleDegRounded;
};
```

### Fields

| Field | Description |
|---|---|
| `dataIdleLevel` | Logic level at the DATA input before the SSI telegram starts |
| `rawValue` | Complete SSI word including trailing bits |
| `trailingBits` | Trailing bits after the useful position bits |
| `grayValue` | Useful Gray-coded position value |
| `position` | Binary position after Gray-to-binary conversion |
| `angleDeg` | Calculated angle in degrees |
| `angleDegRounded` | Angle rounded to 0.1 degree |

## Default Encoder Configuration

The default constructor configuration matches the TWK KBE 58 - K 4096 G K E06 encoder:

```text
Useful bits:   12
Total clocks:  13
Trailing bits: 1
Resolution:    4096 steps per revolution
Code:          Gray
```

Equivalent constructor call:

```cpp
TWK_KBE58_SSI encoder(8, 9, 12, 13, 1);
```

## Timing Configuration

The SSI timing can be adjusted if required.

```cpp
encoder.setHalfPeriodUs(5);
encoder.setFramePauseUs(80);
```

`setHalfPeriodUs()` sets half of the SSI clock period. A value of 5 microseconds gives approximately 100 kHz SSI clock.

`setFramePauseUs()` sets the pause before and after an SSI telegram. The default value adds margin for the encoder wait time.

For troubleshooting, a slower clock can be used:

```cpp
encoder.setHalfPeriodUs(20);
```

This gives approximately 25 kHz SSI clock.

## Troubleshooting

### The raw value is always `0b1111111111111`

This usually means that the ESP32 reads the DATA input as permanently HIGH.

Check:

- ADM3490ARZ wiring
- DATA+ and DATA- polarity
- Encoder supply voltage
- Common GND connection
- CLOCK+ and CLOCK- wiring
- Whether the encoder receives the SSI clock
- Whether the selected ESP32 GPIO is correct

### The angle jumps occasionally

Check:

- DATA pair termination
- Cable shielding
- Twisted pair wiring
- Common GND connection
- SSI clock speed
- Mechanical noise or motor interference

Try a slower SSI clock:

```cpp
encoder.setHalfPeriodUs(20);
```

### Direction is reversed

Use the RD130 Code Sense connection or invert the position in software.

Software inversion example:

```cpp
uint32_t invertedPosition = (encoder.stepsPerRevolution() - 1) - reading.position;
```

## Library Files

```text
TWK_58_SSI/
├── library.properties
├── LICENSE
├── TWK_KBE58_SSI.h
├── TWK_KBE58_SSI.cpp
└── examples/
    └── RD130_ADM3490_Diagnose/
        └── RD130_ADM3490_Diagnose.ino
```

## License

This project is released under the GNU General Public License.

Copyright (C) 2026 Joerg Koerner DK8DE

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
