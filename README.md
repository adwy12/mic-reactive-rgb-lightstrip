# Mic Reactive RGB Lightstrip

ESP32-C3 + INMP441 microphone firmware and audio-reactive RGB lightstrip algorithms.

This repository is a cleaned public snapshot of microphone-driven lightstrip work. It focuses on the reusable audio analysis layer, a minimal 50 LED visualizer firmware, and notes for mapping audio features onto colorful LED strip effects.

## What is included

- I2S microphone capture for INMP441-style digital microphones.
- A compact `LightAudio` algorithm that converts raw microphone frames into:
  - `level`
  - `smoothed`
  - `onset`
  - `beat`
- A 50 LED center-out visualizer with beat flash accents.
- Documentation for RGB lightstrip mapping strategies.
- A host-side C++ test for the audio algorithm.

## Audio algorithm count

The current project has 4 core audio algorithm stages:

1. **Adaptive noise floor**: follows the room noise floor without treating quiet background noise as music.
2. **Signal scaling with sensitivity and noise reduction**: maps microphone energy into a stable 0-255 range.
3. **Asymmetric smoothing**: rises quickly on louder sound and falls slowly for more readable light motion.
4. **Onset / beat detection**: detects sharp energy changes for flashes, pulses, and accent events.

For RGB lightstrip rendering, the current notes describe 8 mapping strategies:

1. Center-out volume bar
2. Beat flash accents
3. Energy-to-gradient color mapping
4. Bass / mid / treble style band mapping
5. Hue-flow and rainbow phase modulation
6. Sparkle and star accents
7. Shape/group-aware propagation
8. Concert-style charge, burst, comet, and settle phases

See [docs/audio-algorithms.md](docs/audio-algorithms.md) and [docs/rgb-lightstrip-optimizations.md](docs/rgb-lightstrip-optimizations.md).

## Hardware target

- MCU: ESP32-C3
- Microphone: INMP441 or compatible I2S microphone
- LED strip: 50 x WS2812 / NeoPixel compatible RGB LEDs
- LED data pin: GPIO 4
- I2S BCLK: GPIO 5
- I2S WS/LRCLK: GPIO 3
- I2S DIN: GPIO 1
- Microphone L/R: GND

## Firmware dependencies

The Arduino sketch uses:

- ESP32 Arduino core
- `Adafruit_NeoPixel`
- ESP-IDF I2S standard driver headers bundled with the ESP32 Arduino core

Compile example:

```bash
arduino-cli compile --fqbn esp32:esp32:esp32c3 firmware/mic_visual_effect
```

## Repository layout

```text
firmware/mic_visual_effect/
  ESP32-C3 + INMP441 + 50 LED visualizer firmware.

docs/
  Algorithm and RGB lightstrip optimization notes.

tests/
  Host-side algorithm checks.
```

## Scope

This repository is for local microphone-reactive LED strip experiments. It does not include private captures, proprietary firmware images, app dumps, music files, credentials, or commercial device protocol research.

## License

MIT License. See [LICENSE](LICENSE).
