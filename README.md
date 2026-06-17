# TWK_KBE58_SSI Arduino and ESP32 Library for Rohde & Schwarz RD130

Code for the `TWK_KBE58_SSI` Arduino library.

This code reads the SSI absolute encoder inside a Rohde & Schwarz RD130 rotor.  
The encoder is assumed to be a TWK KBE 58 - K 4096 G K E06 with Gray code, 12 useful position bits and 13 SSI clocks.

The interface between the ESP32 and the encoder uses one ADM3490ARZ full duplex RS422 transceiver.

## License

Copyright (C) 2026 Joerg Koerner DK8DE

This example is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or any later version.

This example is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

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
- Never connect CLOCK+/CLOCK- or DATA+/DATA- directly to ESP32 GPIO pins.
- Use the ADM3490ARZ or another suitable full duplex RS422 transceiver.
- Connect ESP32 GND, ADM3490 GND and encoder 0 V together.
- Keep CLOCK+/CLOCK- and DATA+/DATA- as twisted or closely coupled pairs.
- Use a 100 nF decoupling capacitor close to the ADM3490 VCC and GND pins.
- For the DATA pair, a 120 ohm termination close to the ADM3490 receiver is recommended.
```
