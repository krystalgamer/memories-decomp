# French MODEL470 quad pairs helper

`func_8013C510` / `func_8017C510` (MODEL470 family, image offset `0x1510`,
0x4E4 bytes) draws and advances two groups of ten growing quads. **No
release had matching C for this body before.**

It follows the shape of the accepted Spanish MODEL470 quads helper
(`variant470_quads.c`, which is the third group's helper at `0x19F4` in the
same images), over a local view in `variant470_quad_pairs.h`:
- two 0x4C4-byte groups at `+0`. Each has quads `a`..`d`, the colour at
  `+0x190`, sizes at `+0x1A4`, done flags at `+0x1CC`, reset flags at
  `+0x21C`, 32-byte anchors at `+0x258` and drift directions at `+0x424`;
- the `POLY_FT4` at `+0x2538`, plus the same direction, time, step, timing
  and phase fields as the Spanish view.

Measured shape:
- each visible quad fades by `(1024 - size) / 512` above size 512;
- `rcos(size)` is called and its result discarded, before the rotation is
  cleared;
- each quad is translated by `anchor + direction * size / 512` and drawn
  with a fixed scale of 768;
- sizes grow by `step * 24`. The first quad reaching 512 moves phase 0 to 2,
  and a quad that reaches 1024 either finishes or wraps and clears its reset
  flag, depending on the timing limit;
- once all done flags are set in phase 2, the phase moves to 3.

The helper is registered in both French images that carry this body
(model 425, stage 7 slot 0 and stage 8 slot 1). No new bindings were
needed. All French overlay images rebuild exactly. The
[attempt ledger](french-model-variant470-quad-pairs-attempts.csv) records
both probes.
