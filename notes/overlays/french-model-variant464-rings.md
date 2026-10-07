# French MODEL464 rings helper

`func_8013C368` / `func_8017C368` (MODEL464 family, image offset `0x1368`,
0x584 bytes) rebuilds the first row of three rings, draws each ring as
sixteen framebuffer-textured `POLY_GT4` strips and advances them. **No
release had matching C for this body before.**

It shares its drawing pass with the accepted Spanish MODEL443 rings helper
(`variant443_rings.c`), over a local view in `variant464_rings.h`:
- three 0x1E4-byte rings at `+0x114C`, each with `SVECTOR points[3][17]`,
  the progress at `+0x1DC` and a completion state at `+0x1E0`;
- the `POLY_GT4` at `+0x1AE8`, the origin at `+0x1BD8`, the direction at
  `+0x1BEC`, the step at `+0x1C24` and the phase at `+0x1C60`.

Measured shape:
- above progress 2048 the row is lifted by `(progress - 2048) / 16` with
  radius `lift + 256`; otherwise it is a flat 256-unit circle;
- strips are white on the inner edge and `(0, 255, 255)` on the outer
  edge, held in one `full` byte set in the draw-loop initialiser;
- `size` is assigned at the end of both point-building branches, which
  the register allocation needs;
- a ring that reaches 4096 wraps (state 1) before phase 3 and stops
  there (state 2) afterwards; once the states add up to 6 in phase 3,
  the phase moves to 5.

The helper is registered in all six French images that carry this body
(models 175, 182 and 244, both slots). `GsGetActiveBuff`, `GetTPage`,
`SetPolyGT4`, `SetSemiTrans` and `SetShadeTex` were added to the French
MODEL464 binding file at their French resident addresses. All French
overlay images rebuild exactly. The
[attempt ledger](french-model-variant464-rings-attempts.csv) records the
four materially distinct probes.
