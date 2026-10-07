# French header-471 image quads helper

`func_8013CA34` / `func_8017CA34` (image offset `0x1A34`, 0x4DC bytes)
grows, fades and orbits ten quads.
**No release had matching C for this body before.**

The body appears in two French raw images of model 82: slot 0 (header `0x1D7`, stage 9) and slot 1 (header `0x26D`, stage 10). The layout gains a
C segment at `0x1A34`, carved out of the unclassified image.

The control flow is the header-429 quads helper (#7206), with these differences:
- the groups start at `+0x794`, with the polygon at `+0x231C` and the origin at `+0x23B0`;
- the orbit offsets are scaled by 256 instead of 128;
- completion moves phase 3 to phase 5.

The local view lives in `variant471_quads.h`, and `model_variant471_linker_symbols.txt`
covers the resident calls. The
[attempt ledger](french-model-image471-quads-attempts.csv) records the
measured probes.
