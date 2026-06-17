# TWK_KBE58_SSI Unofficial Arduino and ESP32 Library for Rohde & Schwarz RD130

Code for the `TWK_KBE58_SSI` Arduino library.

## Unofficial Project Disclaimer

This is an unofficial, independent open source project.

This project is not developed, published, approved, endorsed, sponsored or supported by Rohde & Schwarz.

There is no partnership, cooperation or official relationship between this project and Rohde & Schwarz.

The names "Rohde & Schwarz", "R&S" and "RD130" are used only to identify the equipment this library was developed and tested for.

All trademarks, product names and company names belong to their respective owners.

## Description

This library reads the SSI absolute encoder inside a Rohde & Schwarz RD130 rotor.

The encoder is assumed to be a TWK KBE 58 - K 4096 G K E06 with:

- Gray code output
- 12 useful position bits
- 13 SSI clock pulses
- 4096 positions per revolution
- Single-turn absolute position

The library reads and provides:

- SSI raw value
- Gray value
- Binary position
- DATA idle level
- Trailing bits
- Angle in degrees

The angle output can be rounded to 0.1 degrees in the example sketch.

## Supported Boards

The library is written for Arduino-compatible boards and can be used with ESP32, ESP32-S3, Arduino and other microcontrollers, as long as the board can generate a stable SSI clock and read a digital input.

The Arduino `library.properties` file may use:

```ini
architectures=*
```

This makes the library visible for all Arduino architectures.

## Logic Level and RS422 Interface

The encoder does not use direct TTL or CMOS logic signals.

The SSI interface uses differential RS422/RS485-style signals:

- CLOCK IN +
- CLOCK IN -
- DATA OUT +
- DATA OUT -

Never connect these differential encoder signals directly to Arduino, ESP32 or other microcontroller GPIO pins.

A suitable full duplex RS422 transceiver is required.

## Ready-Made RS422 Module for Testing

For a quick test setup, you can use a ready-made RS422 TTL full duplex module.

Search for:

```text
MAX490 RS422 TTL Full Duplex Modul
```

A module based on the MAX490 usually provides one differential driver and one differential receiver. This is suitable for the SSI interface because the microcontroller must send the SSI clock to the encoder and receive the SSI data from the encoder at the same time.

Typical module pin names:

```text
TTL side:
VCC
GND
TXD or DI
RXD or RO

RS422 side:
TX+
TX-
RX+
RX-
```

Typical connection to the RD130 encoder:

```text
Microcontroller clock output -> TXD / DI

TX+ -> RD130 data connector pin 2 CLOCK IN+
TX- -> RD130 data connector pin 3 CLOCK IN-

RX+ -> RD130 data connector pin 4 DATA OUT+
RX- -> RD130 data connector pin 5 DATA OUT-

RXD / RO -> Microcontroller data input
GND      -> Microcontroller GND and RD130 data connector pin 6 GND
```

For 5 V Arduino boards, a MAX490 module is a practical choice because it is intended for 5 V logic.

For ESP32 or other 3.3 V boards, check the module carefully. Many MAX490 modules use 5 V logic and are not directly compatible with 3.3 V GPIO pins. For 3.3 V boards, use the ADM3490ARZ circuit shown below or a 3.3 V compatible RS422 TTL full duplex module.

Do not use simple MAX485 RS485 modules for this SSI interface. Most MAX485 modules are half duplex and provide only one A/B pair. The SSI encoder needs full duplex operation with two differential pairs: one pair for CLOCK and one pair for DATA.

## Recommended Transceiver for 3.3 V Boards

For ESP32, ESP32-S3 and other 3.3 V Arduino-compatible boards, the recommended transceiver is:

```text
ADM3490ARZ
```

The ADM3490ARZ is a 3.3 V full duplex RS422 transceiver. It can send the SSI clock to the encoder and receive the SSI data from the encoder at the same time.

## Recommended Transceiver for 5 V Arduino Boards

For classic 5 V Arduino boards such as Arduino Uno, Arduino Nano or Arduino Mega, do not use the ADM3490ARZ directly with 5 V logic.

For 5 V boards, use a 5 V full duplex RS422 transceiver, for example:

```text
MAX490
```

The MAX490 is suitable for 5 V logic and provides one differential driver and one differential receiver. It can be used for the SSI clock and data lines.

Typical use with 5 V Arduino boards:

- Arduino digital output -> MAX490 driver input -> CLOCK+/CLOCK-
- DATA+/DATA- -> MAX490 receiver input -> Arduino digital input

If a 3.3 V RS422 transceiver is used with a 5 V Arduino board, proper level shifting is required.

## ADM3490ARZ Connection for ESP32 / 3.3 V Boards

```text
                 ADM3490ARZ
          +----------------------+
 +3V3 ----| 1 VCC            A 8 |---- DATA+  -> Encoder Pin 3
 IO9  <---| 2 RO             B 7 |---- DATA-  -> Encoder Pin 4
 IO8  --->| 3 DI             Z 6 |---- CLOCK- -> Encoder Pin 2
 GND  ----| 4 GND            Y 5 |---- CLOCK+ -> Encoder Pin 1
          +----------------------+
```

Default example pin assignment:

```text
IO8  -> SSI clock output
IO9  -> SSI data input
```

## Connection at the Rohde & Schwarz RD130 Rotor

### Data Connector

```text
Data connector:

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
Motor connector:

1 ---> Motor 1 +
2 ---> Motor 1 -
3 ---> Filter 1 ground
4 ---> Motor 1 -
5 ---> Motor 1 +
6 ---> Filter 2 ground
```

## Important Notes

```text
Important notes:
- This is an unofficial project and is not affiliated with Rohde & Schwarz.
- Never connect CLOCK+/CLOCK- or DATA+/DATA- directly to Arduino or ESP32 GPIO pins.
- Use the ADM3490ARZ for 3.3 V boards or another suitable full duplex RS422 transceiver.
- For 5 V Arduino boards, use a 5 V full duplex RS422 transceiver such as the MAX490.
- For quick tests, search for a ready-made MAX490 RS422 TTL Full Duplex Modul.
- Connect microcontroller GND, RS422 transceiver GND and encoder 0 V together.
- Keep CLOCK+/CLOCK- and DATA+/DATA- as twisted or closely coupled pairs.
- Use a 100 nF decoupling capacitor close to the RS422 transceiver VCC and GND pins.
- For the DATA pair, a 120 ohm termination close to the receiver is recommended.
- For longer CLOCK lines, a 120 ohm termination at the encoder side may be required.
```

## Example Output

The example sketch prints diagnostic values to the serial console:

```text
DATA-Idle: 1 | Raw: 0b0110111110110 | Trailing: 0 | Gray: 0b011011111011 | Position: 1197 / 4096 | Angle: 105.2 deg
```

The output contains:

- `DATA-Idle`: logic level on the data input before the SSI telegram starts
- `Raw`: complete SSI raw value
- `Trailing`: trailing bits after the useful data bits
- `Gray`: extracted Gray code value
- `Position`: binary position after Gray-to-binary conversion
- `Angle`: calculated angle in degrees, rounded to 0.1 degrees

## Encoder Resolution

The assumed encoder resolution is:

```text
4096 positions per revolution
```

This means:

```text
0      -> 0.0 degrees
1024   -> 90.0 degrees
2048   -> 180.0 degrees
3072   -> 270.0 degrees
4095   -> just below 360.0 degrees
```

## License

Copyright (C) 2026 Joerg Koerner DK8DE

This library is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
