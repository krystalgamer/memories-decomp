# French MODEL175 tube helper

`func_8013BDB4` / `func_8017BDB4` (MODEL175 family, image offset `0xDB4`,
0x734 bytes) builds and draws a nine-ring tube.
**No release had matching C for this body before.**

It follows the pattern of the Spanish MODEL417 tube (`variant417_tube.c`).
The local view lives in `variant175_tube.h`:
- one 0x54C-byte tube at `+0`, holding a 9×9 point grid and nine colours at
  `+0x510`;
- the `POLY_G4` at `+0x1420`;
- the origin at `+0x15F8`, the direction at `+0x160C`, the projected
  vector at `+0x161C` and the second direction at `+0x1620`;
- the unsigned time at `+0x163C`, the step at `+0x1644` and the timing
  pointer at `+0x164C`;
- the progress at `+0x165C`, the width at `+0x166C`, the angle at `+0x1670`
  and the phase at `+0x167C`.

Measured shape:
- **Ring offset:** each ring is offset along the direction by
  `direction * progress / 1024 * j / 8`.
- **Ring radius:** the radius is `width * min(progress * j / 32 + 32, 128) / 1024`.
  The ring angle advances by 64 per ring and by 512 per point.
- **Drawing:** the 8×8 quads go through `RotTransPers4`. From phase 6 onwards
  both colour pairs are scaled by `width / 1024`.
- **Progress:** below phase 2, while progress is at most 1024, progress
  follows the expansion window with unsigned division and enters phase 2 at
  1024.
- **Fade:** once the time reaches the fade start, a positive width fades to
  zero, which sets phase 4.
- **Angle:** the angle falls by `step * 32` each frame.

The helper is registered in both French images that carry this body
(model 175, stage 9 slot 0 and stage 10 slot 1). `GsGetActiveBuff`, `rcos`
and `rsin` were added to the shared French MODEL444 binding file at their
French resident addresses. All French overlay images rebuild exactly. The
[attempt ledger](french-model-variant175-tube-attempts.csv) records the
probe, which matched on the first attempt.
