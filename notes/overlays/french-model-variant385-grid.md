# French MODEL385 grid helper

`func_8013C04C` / `func_8017C04C` (image offset `0x104C`, 0x560 bytes)
scales, fades and draws five 9×17 point sheets as gouraud quads.
**No release had matching C for this body before.**

It is the MODEL385 counterpart of the French MODEL379 grid helper
(`variant379_grid.c`). The control flow is the same. The differences:
- two discarded `ratan2` calls on the axis at `+0x3B60`;
- 0x22C-byte pulse records (progress, offset and state at `+0x180`, from
  `+0x18D8`);
- the `POLY_GT4` at `+0x38F4`, sorted with `GsSortPoly`;
- the positions at `+0x3B10`, the flags at `+0x3BDC`, the step at
  `+0x3BE8`, the pulse value at `+0x3C08` and the phase at `+0x3C5C`.

The local view lives in `variant385_grid.h`. The helper is registered in
both French MODEL385 images that carry it (stage 9 slot 0 and stage 10
slot 1). The existing French MODEL473 binding file already covers every
call. All French overlay images rebuild exactly. The
[attempt ledger](french-model-variant385-grid-attempts.csv) records the
probes.
