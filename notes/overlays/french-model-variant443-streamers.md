# French MODEL443 streamers helper

`func_8013E668` / `func_8017E668` (MODEL443 family, image offset `0x3668`,
0x888 bytes) builds three thirteen-point streamers and draws them in three
passes. **No release had matching C for this body before.** It is reachable
from the entry.

It follows the accepted MODEL458 streamers helper (`variant458_streamers.c`)
over a MODEL443 view, `variant443_streamers.h`:
- streamers at `+0x11CC`, sheets at `+0x2A68`, the `POLY_FT4` pair at
  `+0x34BC` and positions at `+0x3570`;
- the axis at `+0x3608`, flags at `+0x3620`, and length and rotation at
  `+0x3680..+0x368C`.

Measured differences from MODEL458:
- the flags are unsigned: the phase is 0 when `(flags & 3) < 2`, and the
  rotation boost uses an unsigned `% 10`;
- three passes instead of seven. Each is translated directly to `positions[j]`
  and scaled by `sheets[j].size`, instead of an origin plus a scaled delta and
  one shared scale.

The helper is registered in both French MODEL443 images (model 62, stages 9
and 10). `RotTransPers` was added to the French MODEL443 binding file at its
French resident address. All French overlay images rebuild exactly. The
[attempt ledger](french-model-variant443-streamers-attempts.csv) records both
probes.
