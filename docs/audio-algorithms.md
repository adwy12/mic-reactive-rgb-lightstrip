# Audio Algorithms

The firmware samples an I2S microphone, reduces each read window to average energy and peak energy, then feeds those values into `LightAudio`.

## 1. Adaptive noise floor

The algorithm tracks a slow-moving `noiseFloor_`. Quiet frames update the floor:

```cpp
noiseFloor_ = (noiseFloor_ * 31U + avg) / 32U;
```

This keeps constant room noise, microphone bias, and low-level electrical noise from permanently lighting the strip.

## 2. Sensitivity and noise reduction scaling

The algorithm combines average energy and peak energy:

```cpp
avgSignal = max(avg - noiseFloor, 0)
peakSignal = max(peak - noiseFloor, 0)
signal = max(avgSignal, peakSignal / 3)
```

Then it applies a configurable gate and gain:

- `sensitivity`: raises useful detail from quiet input.
- `noiseReduction`: raises the gate to avoid false triggers.

The output is clamped to `0..255` for LED-friendly rendering.

## 3. Asymmetric smoothing

Audio that lights LEDs directly can flicker. This project uses asymmetric smoothing:

- louder input rises quickly
- falling input decays more slowly

That makes light movement readable without making impacts feel late.

## 4. Onset and beat detection

The algorithm maintains a slow `energyAverage_`. A beat is detected when the instant level rises above recent energy by an adaptive threshold:

```cpp
onset = raw > energyAverage ? raw - energyAverage : 0
beat = raw > 24 && onset > adaptiveThreshold && raw > lastLevel + 8
```

This catches claps, kicks, and sharp transients without requiring a full FFT.

## Output frame

```cpp
struct LightAudioFrame {
  uint8_t level;
  uint8_t smoothed;
  uint8_t onset;
  bool beat;
};
```

- `level`: immediate normalized energy
- `smoothed`: visually stable energy envelope
- `onset`: sudden change amount
- `beat`: transient trigger flag

## Review and simulation

Use the local simulator to inspect how a sequence of synthetic microphone frames maps into visual control values:

```bash
c++ -std=c++17 -Wall -Wextra -I. tools/simulate_light_audio.cpp -o /tmp/simulate_light_audio
/tmp/simulate_light_audio
```

The output is CSV:

```text
index,avg,peak,level,smoothed,onset,beat
```

## Why no FFT yet

The current firmware prioritizes low-latency, low-memory reaction on a small ESP32-C3. The `bass/mid/treble` style mappings can be approximated from energy, smoothed level, and onset. A future FFT stage can be added when the project needs true frequency separation.
