# French header-383 image strip helper

`func_8013BCBC` / `func_8017BCBC` (image offset `0xCBC`, 0x548 bytes)
projects and draws the header-383 strip.
**No release had matching C for this body before.**

The body appears in two French raw images of model 363: slot 0 (header
`0x17F`, stage 9) and slot 1 (header `0x215`, stage 10). Each layout gains a
C segment at `0xCBC`, carved out of the unclassified image.

The code is the French header-373 strip helper (`variant373_strip.c`) over a
different layout: the strip at `+0`, the quad at `+0x138C`, the origin at
`+0x14E4` and the phase at `+0x1580`. Because the strip now starts at the
context itself, the strip pointer is assigned inside the phase block, which
reproduces the target's register copy. The local view lives in
`variant383_strip.h`, and `model_variant383_linker_symbols.txt` covers the
resident calls. The [attempt ledger](french-model-image383-strip-attempts.csv)
records five measured probes.
