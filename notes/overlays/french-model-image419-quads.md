# French header-419 image quads helper

`func_8013C770` / `func_8017C770` (image offset `0x1770`, 0x4DC bytes)
grows, fades and draws ten orbiting quads.
**No release had matching C for this body before.**

The body occurs in twelve French MODEL raw images that have no configured
variant layout:
- six slot 0 images with header word `0x1A3` (419): models 69, 93, 95, 163,
  249 and 425;
- the six matching slot 1 images with header word `0x239`.

Each image layout used to be one unclassified block. It is now split into
an unclassified head, the C helper at `0x1770` and an unclassified tail,
following the raw-image convention of #7137. The surrounding bytes stay
unclassified, and module names and physical keys are unchanged.

The control flow is that of the Spanish MODEL470 quads helper
(`variant470_quads.c`). The local view in `variant419_quads.h` holds:
- the quad group at `+0`;
- the `POLY_FT4` at `+0x221C`;
- the origin at `+0x2288`, the direction at `+0x2290` and the projected
  vector at `+0x22A0`;
- the unsigned time at `+0x22C0`, the step at `+0x22C8`, the timing pointer
  at `+0x22D0` (limit at `+0x24`) and the phase at `+0x22F4`.

The quads orbit their origin with radius 128.

The new `model_variant419_linker_symbols.txt` binds the twelve resident
calls at their French addresses. Each split image names it in the overlay
manifest. All French overlay images rebuild exactly. The
[attempt ledger](french-model-image419-quads-attempts.csv) records the
probe, which matched on the first build.
