# French header-463 image rings helper

`func_8013E7D4` / `func_8017E7D4` (image offset `0x37D4`, 0x434 bytes)
grows and orbits 24 ring quads.
**No release had matching C for this body before.**

The body appears in four French raw images: models 461 and 424, slot 0
(header `0x1CF`) and slot 1 (header `0x265`). Each layout gains a C segment
at `0x37D4`, carved out of the unclassified image.

The control flow follows the Spanish header-461 rings helper
(`variant461_rings.c`), with these differences:
- no vertical ripple: `t[1]` is the origin;
- the wave advances by 768 per ring and adds `rsin(wave) * 256 >> 12`;
- fixed texture coordinates (192/255 by 64/127) are written each frame;
- quads are sorted for any depth above zero, without the flag test;
- a different state layout (ring at `+0x2710`, polygon at `+0x4420`).

The local view lives in `variant463_rings.h`. The resident calls are covered
by `model_variant463_linker_symbols.txt`. All French overlay images rebuild
exactly. The [attempt ledger](french-model-image463-rings-attempts.csv)
records the probe, which matched on the first build.
