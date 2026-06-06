# RGB Lightstrip Optimizations

These strategies convert microphone features into stable, colorful LED strip behavior.

## 1. Center-out volume bar

Use `smoothed` to decide how many LEDs are lit from the center toward both ends. This gives a readable volume shape even when the strip is moving or viewed from an angle.

## 2. Beat flash accents

Use `beat` as a short-lived flash layer instead of replacing the base effect. The sample firmware uses a decay variable so impact flashes fade naturally.

## 3. Energy-to-gradient color mapping

Map low, medium, and high energy into different color zones:

- low: blue / cyan base
- medium: green / yellow transition
- high: red / magenta intensity

This makes loudness visible through both length and color.

## 4. Bass / mid / treble style band mapping

Even before a full FFT, a visualizer can reserve spatial regions for different music roles:

- bass-like energy near the base or center
- mid-like smoothed motion across the body
- treble-like sparkle or edge accents

Future FFT support can feed these channels directly.

## 5. Hue-flow and rainbow phase modulation

Use audio energy to modulate hue speed, saturation, or brightness rather than only turning LEDs on and off. This keeps colorful animations alive during quiet passages.

## 6. Sparkle and star accents

Use `onset` and `beat` to add sparse white or pastel accents. This works better than random sparkle alone because the eye connects accents to the music.

## 7. Shape/group-aware propagation

For non-linear props, divide LEDs into groups such as base, side, outline, and logo. Audio can then propagate through groups instead of treating the strip as a single line.

## 8. Concert-style phases

For choreographed effects, combine audio features with timed phases:

- charge
- burst
- comet
- settle

The audio layer can drive intensity inside each phase while the phase machine controls the overall visual story.

## Practical tuning rules

- Keep a dim base layer so the strip never looks dead during quiet input.
- Use fast attack and slower decay for readability.
- Clamp brightness during early tests to avoid power and heat surprises.
- Make beat accents additive, not destructive.
- Log raw, floor, amplitude, level, and peak over serial during tuning.
