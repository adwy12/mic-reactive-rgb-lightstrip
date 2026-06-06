# Hardware Notes

## Wiring

| Signal | ESP32-C3 pin |
| --- | --- |
| LED data | GPIO 4 |
| INMP441 BCLK/SCK | GPIO 5 |
| INMP441 WS/LRCLK | GPIO 3 |
| INMP441 SD/DOUT | GPIO 1 |
| INMP441 L/R | GND |
| INMP441 VDD | 3.3 V |
| Common ground | ESP32-C3, microphone, and LED strip |

## Notes

- Start with low brightness when testing a full LED strip.
- Do not power a long strip from an undersized USB port.
- Keep microphone wiring short when possible.
- If the microphone reads silence, check L/R, BCLK, WS, and DIN first.
- If LEDs flicker during loud passages, check power injection and common ground.
