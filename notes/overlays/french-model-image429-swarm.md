# French header-429 image swarm helper

`func_8013C33C` / `func_8017C33C` (image offset `0x133C`, 0x578 bytes)
grows, fades and orbits six quads.
**No release had matching C for this body before.**

The body sits immediately before the header-429 quads helper
(`french-model-image429-quads.md`), in the same six French raw images:
models 275, 371 and 517, slot 0 (header `0x1AD`) and slot 1 (header
`0x243`). Each layout gains a C segment at `0x133C`, carved out of the
unclassified head.

The control flow follows the French MODEL478 swarm helper
(`variant478_swarm.c`), with these differences:
- six quads in a 0x164-byte group at `+0x2FC`, with the colour at `+0xF0`,
  sizes at `+0x104`, done flags at `+0x11C` and reset flags at `+0x14C`;
- a separate direction vector per quad at `+0x1DD4`;
- quads are drawn for sizes of 0 and above;
- growth of `step * 32`;
- the reset flag is cleared when a quad wraps;
- completion moves phase 2 to phase 4.

The local view lives in `variant429_swarm.h`. The calls are covered by
`model_variant429_linker_symbols.txt`. All French overlay images rebuild
exactly. The [attempt ledger](french-model-image429-swarm-attempts.csv)
records the probe, which matched on the first build.
