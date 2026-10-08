# French header-447 image swarm helper

`func_8013C0F8` / `func_8017C0F8` (image offset `0x10F8`, 0x6B8 bytes)
grows, fades, shrinks and resets 32 orbiting quads.
**No release had matching C for this body before.**

The body appears in two French raw images of model 278: slot 0 (header
`0x1BF`, stage 9) and slot 1 (header `0x255`, stage 10). Each layout gains a
C segment at `0x10F8`, carved out of the unclassified image.

The structure combines the 32-quad group of the header-428 swarm with a
phase machine:
- **Before phase 2:** quads fade over sizes 129–383, are black from 384, and
  are lifted by 40 units.
- **From phase 2:** quads use the usual fade above 512.
- **Growth:** quads below 1024 grow by `step * 8` while the phase is
  positive; slot 0 reaching 512 moves the phase to 3.
- **Shrink:** otherwise quads shrink by `step * 24`; the last one reaching
  zero sets phase 1.
- **Reset:** phase 1 resets every size to `-(j * 32)` and moves to phase 2.
- **Completion:** all quads done moves phase 3 to 5.

As in the header-455 swarm, the orbit argument must be written
`spread - 1024 + angle`. The local view lives in `variant447_swarm.h`, and
`model_variant447_linker_symbols.txt` covers the resident calls. The
[attempt ledger](french-model-image447-swarm-attempts.csv) records five
measured probes.
