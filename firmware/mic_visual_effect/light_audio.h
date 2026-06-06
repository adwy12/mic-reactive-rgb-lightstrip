#pragma once

#include <stdint.h>

struct LightAudioFrame {
  uint8_t level;
  uint8_t smoothed;
  uint8_t onset;
  bool beat;
};

class LightAudio {
 public:
  LightAudioFrame update(uint32_t avg, uint32_t peak, uint8_t sensitivity = 192, uint8_t noiseReduction = 120) {
    uint32_t avgSignal = avg > noiseFloor_ ? avg - noiseFloor_ : 0;
    uint32_t peakSignal = peak > noiseFloor_ ? peak - noiseFloor_ : 0;
    uint32_t signal = avgSignal > peakSignal / 3U ? avgSignal : peakSignal / 3U;
    if (avg < noiseFloor_ * 2U) {
      noiseFloor_ = (noiseFloor_ * 31U + avg) / 32U;
    }

    uint16_t raw = scaleSignal(signal, sensitivity, noiseReduction);
    uint16_t onset = raw > energyAverage_ ? raw - energyAverage_ : 0;
    if (raw > smoothed_) {
      smoothed_ = static_cast<uint16_t>((smoothed_ * 2U + raw * 3U) / 5U);
    } else {
      smoothed_ = static_cast<uint16_t>((smoothed_ * 9U + raw) / 10U);
    }

    uint16_t adaptiveThreshold = 10U + noiseReduction / 9U + energyAverage_ / 10U;
    bool beat = raw > 24U && onset > adaptiveThreshold && raw > lastLevel_ + 8U;
    lastPeak_ = (lastPeak_ * 3U + peak) / 4U;
    lastLevel_ = static_cast<uint8_t>((lastLevel_ * 3U + raw) / 4U);
    energyAverage_ = static_cast<uint16_t>((energyAverage_ * 15U + raw) / 16U);

    return {
        static_cast<uint8_t>(raw > 255U ? 255U : raw),
        static_cast<uint8_t>(smoothed_ > 255U ? 255U : smoothed_),
        static_cast<uint8_t>(onset > 255U ? 255U : onset),
        beat,
    };
  }

 private:
  static uint16_t scaleSignal(uint32_t signal, uint8_t sensitivity, uint8_t noiseReduction) {
    uint32_t gate = 360U + static_cast<uint32_t>(noiseReduction) * 14U;
    uint32_t sensitivityTrim = static_cast<uint32_t>(sensitivity) / 2U;
    gate = gate > sensitivityTrim ? gate - sensitivityTrim : 0U;
    if (signal < gate) return 0;
    uint32_t gain = 96U + static_cast<uint32_t>(sensitivity) * 16U;
    uint32_t scaled = ((signal - gate) * gain) / 65000U;
    return static_cast<uint16_t>(scaled > 255U ? 255U : scaled);
  }

  uint32_t noiseFloor_ = 1800;
  uint32_t lastPeak_ = 0;
  uint16_t smoothed_ = 0;
  uint16_t energyAverage_ = 0;
  uint8_t lastLevel_ = 0;
};
