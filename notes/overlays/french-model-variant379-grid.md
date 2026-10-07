# French MODEL379 grid helper

`func_8013BFD0` / `func_8017BFD0` (image offset `0xFD0`, 0x528 bytes) scales,
fades and draws five 9×17 point sheets as gouraud quads.
**No release had matching C for this body before.**

The local view in `variant379_grid.h` holds:
- five 0x4F8-byte sheets at `+0`, each with a 9×17 point grid, nine
  colours at `+0x4C8`, the size at `+0x4EC` and a done flag at `+0x4F4`;
- five 0x250-byte pulse records from `+0x18D8`, with progress, offset and
  state at `+0x180`;
- the `POLY_GT4` at `+0x3E38` and the positions at `+0x4078`;
- the flags at `+0x4164`, the step at `+0x4170`, a pulse value at `+0x4198`
  and the phase at `+0x41EC`.

Measured shape:
- **Scale:** each sheet is scaled by its size, plus `pulse / 32` when flag
  bit 0 is clear.
- **Colours:** the nine colours are copied, or faded by
  `(8192 - size) / 4096` once the size reaches 4096.
- **Drawing:** 8×16 quads go through `RotTransPers4`. They are sorted only
  while the matching pulse progress is at least 1024.
- **Growth:** after that point the size grows by `step * 192` up to 8192.
  It then restarts the pulse (state 1) before phase 5, or finishes it
  (state 3, done) from phase 5. Pulses still in progress at phase 5 also
  count as done.
- **Completion:** five done sheets move phase 5 to phase 6.

Matching notes:
- The pulse records are walked with a pointer whose fields all sit at
  non-zero offsets, so loop.c eliminates the pointer and bases the reduced
  register on the last-stored state field (`+0x1A60`).
- The flag test is written in its negated form.

The helper is registered in both French MODEL379 images (stage 7 slot 0 and
stage 8 slot 1); the existing French MODEL479 binding file already covers
every call. All French overlay images rebuild exactly. The
[attempt ledger](french-model-variant379-grid-attempts.csv) records the
probes.
