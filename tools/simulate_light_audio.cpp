#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

#include "../firmware/mic_visual_effect/light_audio.h"

struct SampleFrame {
  uint32_t avg;
  uint32_t peak;
};

uint8_t parseByteArg(int argc, char **argv, const char *flag, uint8_t fallback) {
  for (int i = 1; i + 1 < argc; ++i) {
    if (std::string(argv[i]) == flag) {
      const long value = std::strtol(argv[i + 1], nullptr, 10);
      if (value < 0) return 0;
      if (value > 255) return 255;
      return static_cast<uint8_t>(value);
    }
  }
  return fallback;
}

int main(int argc, char **argv) {
  LightAudio audio;
  const uint8_t sensitivity = parseByteArg(argc, argv, "--sensitivity", 192);
  const uint8_t noiseReduction = parseByteArg(argc, argv, "--noise-reduction", 120);
  const SampleFrame frames[] = {
      {900, 1400},
      {1200, 1800},
      {8000, 16000},
      {18000, 36000},
      {42000, 90000},
      {120000, 300000},
      {38000, 70000},
      {9000, 16000},
      {1300, 1900},
  };

  std::cout << "# sensitivity=" << static_cast<int>(sensitivity)
            << ",noiseReduction=" << static_cast<int>(noiseReduction) << "\n";
  std::cout << "index,avg,peak,level,smoothed,onset,beat\n";
  for (uint8_t i = 0; i < sizeof(frames) / sizeof(frames[0]); ++i) {
    const auto result = audio.update(frames[i].avg, frames[i].peak, sensitivity, noiseReduction);
    std::cout << static_cast<int>(i) << ","
              << frames[i].avg << ","
              << frames[i].peak << ","
              << static_cast<int>(result.level) << ","
              << static_cast<int>(result.smoothed) << ","
              << static_cast<int>(result.onset) << ","
              << (result.beat ? 1 : 0) << "\n";
  }
}
