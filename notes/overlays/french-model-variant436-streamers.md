# French MODEL436 streamers helper

`func_8013D6C8` / `func_8017D6C8` (MODEL436 family, image offset `0x26C8`,
0x8F4 bytes) builds three thirteen-point streamers and draws them in one
pass. **No release had matching C for this body before.** It is reachable
from the entry.

It follows the accepted MODEL458 streamers helper (`variant458_streamers.c`)
over a MODEL436 view, `variant436_streamers.h`:
- streamers at `+0x93C`, the quads at `+0x14A8` and one origin `MATRIX` at
  `+0x1508`;
- the factor, lift and scale at `+0x1590..+0x1598` and one delta at `+0x15A0`;
- axis, flags, length and rotation at `+0x15C4..+0x1630`.

Instead of seven origin/delta passes, the loop runs once. It moves the one
origin along the delta by the factor and lowers y by
`drop = lift / 4 * (1024 - factor) / 1024`. The target computes `drop` before
the translation, which needs the separate local; writing it inline in
`m.t[1]` reorders the code. Everything else (phase, reach 96, wobble, the
local flag table and the draw test) is unchanged from MODEL458.

The helper is registered in both French MODEL436 images (model 707, stages 9
and 10). `RotTransPers`, `rcos` and `rsin` were added to the French MODEL436
binding file at their French resident addresses. All French overlay images
rebuild exactly. The [attempt ledger](french-model-variant436-streamers-attempts.csv)
records both probes.
