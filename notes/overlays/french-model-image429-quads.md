# French header-429 image quads helper

`func_8013C8B4` / `func_8017C8B4` (image offset `0x18B4`, 0x4DC bytes)
grows, fades and draws ten orbiting quads.
**No release had matching C for this body before.**

The body occurs in six French MODEL raw images that have no configured
variant layout:
- three slot 0 images with header word `0x1AD` (429): models 275, 371 and
  517;
- the three matching slot 1 images with header word `0x243`.

Each image layout is split into an unclassified head, the C helper at
`0x18B4` and an unclassified tail, following the raw-image convention of
#7137 and the header-419 helpers.

The body is the header-419 image quads helper (`variant419_quads.c`) with
three differences:
- the state fields sit 0x530 lower: the `POLY_FT4` at `+0x1CEC`, the
  origin at `+0x1D58`, the time at `+0x1E50`, the step at `+0x1E58`, the
  timing pointer at `+0x1E60` and the phase at `+0x1E88`;
- the timing limit is at `+0x20`;
- completion is tested against phase 4 instead of 3.

The local view lives in `variant429_quads.h`. The new
`model_variant429_linker_symbols.txt` binds the twelve resident calls at
their French addresses, and each split image names it in the overlay
manifest. All French overlay images rebuild exactly. The
[attempt ledger](french-model-image429-quads-attempts.csv) records the
probe, which matched on the first build.
