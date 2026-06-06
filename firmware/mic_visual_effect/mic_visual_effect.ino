#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "driver/i2s_std.h"

#include "light_audio.h"

static constexpr uint8_t LED_PIN = 4;
static constexpr uint16_t LED_COUNT = 50;
static constexpr uint8_t BASE_FLOOR = 3;
static constexpr uint8_t MAX_BRIGHTNESS = 170;

static constexpr int PIN_I2S_BCLK = 5;
static constexpr int PIN_I2S_WS = 3;
static constexpr int PIN_I2S_DIN = 1;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
static i2s_chan_handle_t rxChannel = nullptr;
static LightAudio audio;
static uint8_t beatFlash = 0;
static uint32_t noiseEstimate = 1800;

static uint32_t colorForLevel(uint8_t level, uint8_t pixel, uint8_t litPixels) {
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
  if (level < 90) {
    r = 0;
    g = map(level, 0, 90, 32, 180);
    b = map(level, 0, 90, 130, 255);
  } else if (level < 180) {
    r = map(level, 90, 180, 0, 255);
    g = 220;
    b = map(level, 90, 180, 255, 40);
  } else {
    r = 255;
    g = map(level, 180, 255, 220, 35);
    b = map(level, 180, 255, 40, 145);
  }

  if (litPixels > 0) {
    uint8_t edge = abs(static_cast<int>(pixel) - static_cast<int>(LED_COUNT / 2));
    uint8_t sparkle = edge % 3 == 0 ? 18 : 0;
    r = min<int>(255, r + sparkle);
    g = min<int>(255, g + sparkle);
    b = min<int>(255, b + sparkle);
  }
  return strip.Color(r, g, b);
}

static bool setupMic() {
  i2s_chan_config_t chanConfig = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
  esp_err_t err = i2s_new_channel(&chanConfig, nullptr, &rxChannel);
  if (err != ESP_OK) {
    Serial.printf("i2s_new_channel failed: %d\n", err);
    return false;
  }

  i2s_std_config_t stdConfig = {
      .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(16000),
      .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO),
      .gpio_cfg = {
          .mclk = I2S_GPIO_UNUSED,
          .bclk = static_cast<gpio_num_t>(PIN_I2S_BCLK),
          .ws = static_cast<gpio_num_t>(PIN_I2S_WS),
          .dout = I2S_GPIO_UNUSED,
          .din = static_cast<gpio_num_t>(PIN_I2S_DIN),
          .invert_flags = {
              .mclk_inv = false,
              .bclk_inv = false,
              .ws_inv = false,
          },
      },
  };
  stdConfig.slot_cfg.slot_mask = I2S_STD_SLOT_BOTH;

  err = i2s_channel_init_std_mode(rxChannel, &stdConfig);
  if (err != ESP_OK) {
    Serial.printf("i2s init failed: %d\n", err);
    return false;
  }

  err = i2s_channel_enable(rxChannel);
  if (err != ESP_OK) {
    Serial.printf("i2s enable failed: %d\n", err);
    return false;
  }
  return true;
}

static LightAudioFrame readAudioFrame(uint32_t *avgOut, uint32_t *peakOut) {
  static int32_t samples[512];
  size_t bytesRead = 0;
  esp_err_t err = i2s_channel_read(rxChannel, samples, sizeof(samples), &bytesRead, pdMS_TO_TICKS(80));
  if (err != ESP_OK || bytesRead == 0) {
    *avgOut = 0;
    *peakOut = 0;
    return audio.update(0, 0);
  }

  size_t count = bytesRead / sizeof(samples[0]);
  uint64_t sumAbs = 0;
  uint32_t peak = 0;
  size_t leftCount = 0;
  for (size_t i = 0; i < count; i += 2) {
    int32_t sample = samples[i] >> 8;
    uint32_t absSample = sample < 0 ? -sample : sample;
    sumAbs += absSample;
    if (absSample > peak) peak = absSample;
    leftCount++;
  }

  uint32_t avg = leftCount ? static_cast<uint32_t>(sumAbs / leftCount) : 0;
  *avgOut = avg;
  *peakOut = peak;
  return audio.update(avg, peak);
}

static void renderEffect(const LightAudioFrame &frame) {
  uint8_t level = frame.smoothed;
  if (frame.beat) beatFlash = 255;
  uint8_t lit = map(level, 0, 255, 2, LED_COUNT);
  uint8_t center = LED_COUNT / 2;

  strip.clear();
  for (uint8_t i = 0; i < LED_COUNT; ++i) {
    strip.setPixelColor(i, strip.Color(0, BASE_FLOOR, BASE_FLOOR * 5));
  }

  uint8_t half = lit / 2;
  for (uint8_t step = 0; step <= half; ++step) {
    if (center + step < LED_COUNT) strip.setPixelColor(center + step, colorForLevel(level, center + step, lit));
    if (center >= step) strip.setPixelColor(center - step, colorForLevel(level, center - step, lit));
  }

  if (beatFlash > 8) {
    for (uint8_t i = 0; i < LED_COUNT; i += 4) {
      strip.setPixelColor(i, strip.Color(beatFlash, beatFlash / 2, 255));
    }
    beatFlash = static_cast<uint8_t>(beatFlash * 7U / 10U);
  }

  strip.show();
}

void setup() {
  Serial.begin(115200);
  delay(600);
  strip.begin();
  strip.setBrightness(MAX_BRIGHTNESS);
  strip.clear();
  strip.show();

  Serial.println();
  Serial.println("Mic visual effect: INMP441 + 50 LEDs, GPIO4");
  Serial.println("Wiring: SCK=5 WS=3 SD=1 L/R=GND");
  if (setupMic()) {
    Serial.println("MIC_INIT_OK");
  } else {
    Serial.println("MIC_INIT_FAIL");
  }
}

void loop() {
  static uint32_t lastPrint = 0;
  uint32_t avg = 0;
  uint32_t peak = 0;
  LightAudioFrame frame = readAudioFrame(&avg, &peak);
  if (avg < noiseEstimate * 2U) {
    noiseEstimate = (noiseEstimate * 31U + avg) / 32U;
  }
  renderEffect(frame);

  uint32_t now = millis();
  if (now - lastPrint > 120) {
    lastPrint = now;
    uint32_t amplitude = peak > noiseEstimate ? peak - noiseEstimate : 0;
    Serial.printf("MIC,%lu,%lu,%lu,%lu,%u,%u,%lu,%u\n",
                  static_cast<unsigned long>(avg),
                  static_cast<unsigned long>(noiseEstimate),
                  static_cast<unsigned long>(amplitude),
                  static_cast<unsigned long>(noiseEstimate / 32U),
                  frame.level,
                  frame.smoothed,
                  static_cast<unsigned long>(peak),
                  120U);
  }
}
