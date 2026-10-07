# French header-419 image swarm helper

`func_8013C1DC` / `func_8017C1DC` (image offset `0x11DC`, 0x594 bytes)
grows, fades and orbits a swarm of 32 quads.
**No release had matching C for this body before.**

The body sits immediately before the header-419 quads helper
(`french-model-image419-quads.md`), in the same twelve French raw MODEL
images:
- slot 0, header `0x1A3`: models 69, 93, 95, 163, 249 and 425;
- slot 1, header `0x239`: the same six models.

Each image layout gains a C segment at `0x11DC`, carved out of the
unclassified head. The bytes before it stay unclassified.

The control flow follows the French MODEL478 swarm helper
(`variant478_swarm.c`), over a local view in `variant419_swarm.h`:
- the group at `+0x2FC`;
- the `POLY_FT4` at `+0x221C`;
- the origin at `+0x227C` and the direction at `+0x2290`;
- the timing pointer at `+0x22D0`, with a u16 spin value at `+0x1C` and
  the limit at `+0x24`.

The differences from MODEL478:
- the quad angle is `j * 4096 / 3`;
- the y offset gains `rsin(size * 3 + spin) * 256 >> 12`, where `spin` is
  the timing value shifted left by 11 and read once before the loop.

The calls are covered by the existing `model_variant419_linker_symbols.txt`.
All French overlay images rebuild exactly. The
[attempt ledger](french-model-image419-swarm-attempts.csv) records both
probes.
