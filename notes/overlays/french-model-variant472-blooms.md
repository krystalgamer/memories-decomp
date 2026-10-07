# French MODEL472 blooms helper

`func_8013C110` / `func_8017C110` (MODEL472 family, image offset `0x1110`,
0x624 bytes) draws and advances eight groups of twelve blooming quads.
**No release had matching C for this body before.**

It shares the quads drawing pattern of the French MODEL478 swarm helper
(`variant478_swarm.c`), over a local view in `variant472_blooms.h`:
- eight 0x2B4-byte groups at `+0`. Each has corners `a`..`d`, the colour at
  `+0x1E0`, sizes at `+0x1F4`, done flags at `+0x224`, an origin at
  `+0x284` and a direction at `+0x2A4`;
- the `POLY_FT4` at `+0x2E38`, the direction at `+0x2EBC`, the time at
  `+0x2EEC`, the step at `+0x2EF4`, the timing pointer at `+0x2EFC` (limit
  at `+0x2C`) and the phase at `+0x2F40`.

Measured shape:
- a quad is drawn while its size is between 1 and 3071, fading by
  `(2048 - size) / 512` above 1536;
- below 1024 the quad scales by `(rsin(size) * 3596 >> 12) + 500`, and by
  4096 otherwise;
- the size splits into growth (0..1024) and spread (the excess above 1024).
  Growth drives the radius `1024 - (rcos(1024 - (rcos(grow) * 1024 >> 12))
  * 1024 >> 12)` along the direction, and half the spread orbits the quad
  at `angle`;
- the angle starts at `i * 512` for each group and advances 700 per quad;
- sizes grow by `step * 8` and wrap at 2048 until the timing limit,
  driving phases 0 to 1, 2 to 3 and 4 to 5.

The helper is registered in both French images that carry this body
(model 719, stage 7 slot 0 and stage 8 slot 1). `ratan2`, `rcos` and
`rsin` were added to the French MODEL472 binding file at their French
resident addresses. All French overlay images rebuild exactly. The
[attempt ledger](french-model-variant472-blooms-attempts.csv) records the
three probes.
