#include <cassert>
#include <cstdint>

#include "../firmware/mic_visual_effect/light_audio.h"

int main() {
  LightAudio audio;

  auto quiet = audio.update(900, 1400);
  assert(quiet.level == 0);
  assert(quiet.smoothed == 0);
  assert(!quiet.beat);

  auto medium = audio.update(18000, 36000);
  assert(medium.level > 0);
  assert(medium.smoothed > 0);
  assert(!medium.beat || medium.onset > 0);

  auto hit = audio.update(120000, 300000);
  assert(hit.level >= medium.level);
  assert(hit.onset > 0);

  for (int i = 0; i < 16; ++i) {
    auto falling = audio.update(1200, 2000);
    assert(falling.level <= 255);
    assert(falling.smoothed <= 255);
  }

  return 0;
}
