# Connection Notes

This document records the connections represented by the current ESP32 sketch.

> The photographs show the assembled prototype, but they are not a substitute for a verified schematic. The table below is therefore based on the code rather than claiming that every physical connection has been independently verified from the photographs.

## ESP32 connections

| Component | Pin / signal | ESP32 |
|---|---|---:|
| DHT22 | DATA | GPIO 4 |
| FSR | Analog output | GPIO 34 |
| PIR | OUT | GPIO 14 |
| HC-SR04 | TRIG | GPIO 32 |
| HC-SR04 | ECHO | GPIO 33 |
| NeoPixel strip | DIN | GPIO 27 |
| I2S microphone | WS/LRCLK | GPIO 13 |
| I2S microphone | DATA | GPIO 35 |
| I2S microphone | SCK/BCLK | GPIO 12 |
| DFPlayer Mini | module TX → ESP32 RX | GPIO 16 |
| DFPlayer Mini | ESP32 TX → module RX | GPIO 17 |

## I²C devices

The sketch calls:

```cpp
Wire.begin();
```

and therefore uses the ESP32 board's default I²C pins unless changed by the board/core configuration.

The MLX90614 and MPU6050 are both I²C devices and can share the I²C bus when their addresses do not conflict.

## Current implementation notes

- One FSR analogue channel is currently read.
- MPU6050 is initialized but its readings are not used by `detectEmotion()`.
- The photodiode is not referenced by the current sketch.
- GSM/SMS is not implemented.
- Dedicated O₂/CO₂ sensing is not implemented.
