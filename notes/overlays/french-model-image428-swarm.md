# French header-428 image swarm helper

`func_8013C308` / `func_8017C308` (image offset `0x1308`, 0x584 bytes)
grows, fades and orbits 32 quads.
**No release had matching C for this body before.**

The body appears in the two French raw images of model 507 that also hold
the header-428 quads helper (`french-model-image428-quads.md`), directly
before it.

The control flow follows the header-429 swarm helper (#7206), with these
differences:
- 32 quads in a 0x714-byte group at `+0x2FC`, with directions at `+0x2524`;
- the orbit angle of quad `j` is `j * 4096 / 3`;
- quads are drawn only for sizes above zero;
- growth of `step * 8`.

The local view lives in `variant428_swarm.h`, and the resident calls are
covered by `model_variant428_linker_symbols.txt`. The
[attempt ledger](french-model-image428-swarm-attempts.csv) records the probe,
which matched on the first build.
