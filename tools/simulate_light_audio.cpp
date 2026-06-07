#include <cstdint>
#include <iostream>

#include "../firmware/mic_visual_effect/light_audio.h"

struct SampleFrame {
  uint32_t avg;
  uint32_t peak;
};

int main() {
  LightAudio audio;
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

  std::cout << "index,avg,peak,level,smoothed,onset,beat\n";
  for (uint8_t i = 0; i < sizeof(frames) / sizeof(frames[0]); ++i) {
    const auto result = audio.update(frames[i].avg, frames[i].peak);
    std::cout << static_cast<int>(i) << ","
              << frames[i].avg << ","
              << frames[i].peak << ","
              << static_cast<int>(result.level) << ","
              << static_cast<int>(result.smoothed) << ","
              << static_cast<int>(result.onset) << ","
              << (result.beat ? 1 : 0) << "\n";
  }
}
