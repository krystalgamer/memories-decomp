# French header-428 image quads helper

`func_8013C88C` / `func_8017C88C` (image offset `0x188C`, 0x508 bytes)
grows, fades and orbits ten quads.
**No release had matching C for this body before.**

The body appears in two French raw images of model 507: slot 0 (header `0x1AC`, stage 9) and slot 1 (header `0x242`, stage 10). The layout gains a
C segment at `0x188C`, carved out of the unclassified image.

The control flow is the header-429 quads helper (#7206), with these differences:
- the polygon is at `+0x229C`, the origin at `+0x2308` and the time at `+0x2740`;
- the orbit offsets are scaled by 192;
- the `size <= 0` timeout also requires phase 3 or later;
- the polygon pointer is initialised at its declaration, which gives the loop
  counter `j` the earlier saved register.

The local view lives in `variant428_quads.h`, and `model_variant428_linker_symbols.txt`
covers the resident calls. The
[attempt ledger](french-model-image428-quads-attempts.csv) records the
measured probes.
