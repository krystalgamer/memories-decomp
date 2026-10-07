# French MODEL478 swarm helper

`func_8013C34C` / `func_8017C34C` (MODEL478 family, image offset `0x134C`,
0x54C bytes) draws and advances a swarm of 32 growing quads. **No release
had matching C for this body before.** It is a different body from the
accepted Spanish MODEL478 quads helper (`variant478_quads.c`, 0x508 bytes).

It follows the shape of the Spanish MODEL470 quads helper over a local
view in `variant478_swarm.h`:
- one 32-quad group at `+0xA28`, with corners `a`..`d`, the colour at
  `+0x500`, sizes at `+0x514` and done flags at `+0x594`;
- the `POLY_FT4` at `+0x2948`, the origin at `+0x29A8`, the direction at
  `+0x29BC`, the time at `+0x29EC`, the step at `+0x29F4`, the timing
  pointer at `+0x29FC` (limit at `+0x24`) and the phase at `+0x2A24`.

Measured shape:
- visible quads (size above 0) fade by `(1024 - size) / 512` above 512;
- each quad orbits at `angle - 1024`, where the angle advances 1300 per
  quad, with radius `256 - (rcos(size) * 256 >> 12)`;
- each quad drifts by `direction * size / 512`;
- each quad is scaled by `(rsin(size) * 3596 >> 12) + 500`;
- sizes grow by `step * 8`. The first quad reaching 512 moves phase 0 to 2,
  and once every quad is done in phase 2, the phase moves to 5.

The helper is registered in all four French images that carry this body
(models 272 and 636, both slots). No new bindings were needed. All French
overlay images rebuild exactly. The
[attempt ledger](french-model-variant478-swarm-attempts.csv) records the
probe.
