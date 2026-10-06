# French MODEL438 line helper

`func_8013F300` / `func_8017F300` (image offset `0x4300`, 0x568 bytes) is the
MODEL438 member of the "lines" helper family. **No release had matching C for
this body before.** It was chosen because no C existed for it in any release,
with `J`/`JAL` destination fields masked when comparing bodies across releases.

The accepted MODEL417 lines helper was the structural starting point. Like that
helper, this one makes four unused `ratan2` calls and walks three groups of
4×6 line segments projected with `RotTransPers4`. Phase-dependent size and
colour fades come before each `GsSortGLine` call. MODEL438 differs in its state
layout and in phase 0. In phase 0, each group's size follows the timing window
at `timing+0x1C`/`+0x20`:
`i * 4096 / 3 - cycle`, where
`cycle = ((time - start) * 3 << 12) / (end - start) - 4096`,
wrapping by +4096 while non-positive. The named intermediate is required;
GCC otherwise folds the expression into a different add/subtract order.
Clamping happens at `phase >= 4`, followed by the advance to phase 5.

The C is registered in all 12 French MODEL438 images (models 147, 211, 263,
525, 610 and 632, both slots), where this function was already split out as
assembly. `GsSortGLine` was added to the French MODEL438 binding file at the
French resident function `0x800840B8`. All 3,594 French overlay images rebuilt
exactly. The [attempt ledger](french-model-variant438-lines-attempts.csv)
records the five source experiments.

The same images exist in the Spanish, German and Italian archives. This C can be
registered there without decompiling the function again.
