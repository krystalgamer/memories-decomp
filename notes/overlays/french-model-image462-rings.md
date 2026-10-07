# French header-462 image rings helper

`func_8013D42C` / `func_8017D42C` (image offset `0x242C`, 0x434 bytes)
grows and orbits 24 ring quads.
**No release had matching C for this body before.**

The body appears in two French raw images of model 457: slot 0 (header
`0x1CE`, stage 7) and slot 1 (header `0x264`, stage 8). Each layout gains a
C segment at `0x242C`, carved out of the unclassified image.

The code is the header-463 rings helper (`french-model-image463-rings.md`).
Its state follows the Spanish header-461 rings layout shifted by `+0x1E0`
(ring at `+0xD70`, polygon at `+0x3098`, origin at `+0x310C`). The local view
lives in `variant462_rings.h`, and `model_variant462_linker_symbols.txt`
covers the resident calls. The
[attempt ledger](french-model-image462-rings-attempts.csv) records the probe,
which matched on the first build.
