# Effect Strategy Matrix

This matrix connects audio features from `LightAudio` to RGB lightstrip behaviors.

| Audio feature | Best visual use | Notes |
| --- | --- | --- |
| `level` | immediate brightness, fast meter | Good for direct response, but can flicker if used alone. |
| `smoothed` | center-out bar, glow size, hue speed | Best default driver for readable motion. |
| `onset` | sparkle density, comet spawn, edge flash | Captures sharp changes without requiring FFT. |
| `beat` | accent flash, phase trigger, burst start | Use as additive layer so the base effect keeps continuity. |

## Recommended combinations

### Live prop default

- `smoothed` controls visible length.
- `level` adds brightness.
- `beat` adds sparse white or magenta accents.

### Concert burst

- `beat` starts the burst.
- `onset` controls sparkle count.
- `smoothed` controls fade duration.

### Rainbow audio flow

- `smoothed` controls hue velocity.
- `level` controls saturation.
- `onset` adds short comet heads.

### Quiet ambience

- Keep a low base layer.
- Let `smoothed` drive slow color breathing.
- Ignore `beat` until level rises above the room-noise profile.

## Implementation rule

Treat audio as a control signal, not as raw LED brightness. The most stable effects combine one slow feature, one fast feature, and one transient trigger.
