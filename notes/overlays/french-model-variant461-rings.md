# French MODEL461 rings helper

`func_8013D498` / `func_8017D498` (MODEL461 family, image offset `0x2498`,
0x43C bytes) draws the 24-segment textured ring. **No release had matching C
for this body before.** It is reachable directly from the entry.

It follows the accepted MODEL469 rings helper (`variant469_rings.c`) over a
MODEL461 layout, `variant461_rings.h`:
- the ring starts at `+0xB90` and the `POLY_FT4` at `+0x2EB8`;
- origin, probe and delta are at `+0x2F2C`/`+0x3030`/`+0x303C`;
- step, angle, scale, wave and phase are at
  `+0x3364`/`+0x3380`/`+0x3384`/`+0x3388`/`+0x33AC`.

Two statements differ from MODEL469:
- the per-segment pulse adds a fixed `rsin(wave) * 512 >> 12` instead of a
  size/8 fraction;
- the scale fade starts in phase 6 instead of 4.

The helper is registered in both French MODEL461 images (model 705, both
slots). `ratan2`, `rcos` and `rsin` were added to the French MODEL461 binding
file at their French resident addresses. All French overlay images rebuild
exactly. The [attempt ledger](french-model-variant461-rings-attempts.csv)
records the experiment.
