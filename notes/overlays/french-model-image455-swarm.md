# French header-455 image swarm helper

`func_8013C3B0` / `func_8017C3B0` (image offset `0x13B0`, 0x5C8 bytes)
grows, fades and orbits up to 64 quads.
**No release had matching C for this body before.**

The body appears in four French raw images: models 475 and 9, slot 0
(header `0x1C7`) and slot 1 (header `0x25D`). Each layout gains a C segment
at `0x13B0`, carved out of the unclassified image.

The control flow follows the header-429 swarm helper
(`french-model-image429-swarm.md`), with these differences:
- the quad count, growth rate and time limit come from a timing record at
  `+0x3754` (`count` at `+0x1C`, `rate` at `+0x24`, `limit` at `+0x2C`);
- each quad has its own origin matrix at `+0xE14` and direction vector at `+0x3328`;
- radius `480`, with the orbit angle biased by `j * 1024` and `64` per quad;
- growth of `rate * step / 2`;
- completion moves phase 2 to phase 5.

The trig argument must be written as `spread - 1024 + angle`. Other
groupings, or a separate `angle - 1024` temporary, reassociate the bias or
swap the `a0`/`v1` registers of the two sign-extended operands.

The local view lives in `variant455_swarm.h`. The resident calls are covered
by `model_variant455_linker_symbols.txt`. All French overlay images rebuild
exactly. The [attempt ledger](french-model-image455-swarm-attempts.csv)
records the 13 measured probes.
