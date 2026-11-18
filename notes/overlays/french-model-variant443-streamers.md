# French MODEL443 streamers helper

`FrenchModelVariant443_DrawStreamers` (formerly `func_8013E668` /
`func_8017E668`; MODEL443 family, image offset `0x3668`,
0x888 bytes) builds three thirteen-point streamers and draws them in three
passes. **No release had matching C for this body before.** It is reachable
from the entry.

It follows the accepted MODEL458 streamers helper (`h458_streamers.c`)
over the MODEL443 record, `EntryState443` in `e503_entry.h`:
- `streamers` at `+0x11CC`, `sheets` at `+0x2A68`, the `additional`
  `POLY_FT4` pair at `+0x34BC` and `positions` at `+0x3570`;
- `view_delta` at `+0x3608`, `clock.frame_count` at `+0x3620`, and
  `length` and the rotations at `+0x3680..+0x368C`.

Measured differences from MODEL458:
- the frame count is unsigned: the phase is 0 when `(frame_count & 3) < 2`,
  and the rotation boost uses an unsigned `% 10`;
- three passes instead of seven. Each is translated directly to `positions[j]`
  and scaled by `sheets[j].size`, instead of an origin plus a scaled delta and
  one shared scale.

The helper is registered in both French MODEL443 images (model 62, stages 9
and 10). `RotTransPers` was added to the French MODEL443 binding file at its
French resident address. All French overlay images rebuild exactly. The
[attempt ledger](french-model-variant443-streamers-attempts.csv) records both
probes.
