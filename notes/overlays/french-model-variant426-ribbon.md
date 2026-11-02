# French MODEL variant 426 ribbon helper

`FrenchModelVariant426_DrawRibbon`, at `0x8013CDF0` in the French and Spanish
`model_variant_0_stage7_slot0` images and `0x8017CDF0` in the
`model_variant_0_stage8_slot1` images (offset `0x1DF0`, 0x8E0 bytes), draws a
single ribbon. Both slots compile the same source; each image's symbol file
binds the name at its own address.

- **Shape.** The ribbon's 17 points are placed along a direction vector that
  is scaled by the growth amount, and a copy is moved along the view by the
  edge width. Both are projected, and each segment's screen angle, width and
  offsets are stored. The draw loop sorts two `POLY_GT4` halves per segment,
  blue on the outer edges and grey along the spine. The tail grows the
  amount over the timing record's grow window, raising the stage to 3, and
  fades the width over the fade window, moving stage 4 to 5. The view lives in
  `french_model_variant/variant426_ribbon.h`.
- **Register choice.** The outer one-pass loop starts with a dead
  `if (i < 0) dy = 0;`. Jump optimisation deletes it after reload, which
  leaves the target's dead reload of the counter. Fresh temporaries hold the
  per-point amplitude terms. The point index is copied through
  `n = k = j;` before `setVector`, which then indexes with `n`, reads
  `(k = n)` in the y term and `j` in the z term. Every side effect is
  sequenced: the macro repeats its pointer argument, so assigning `n` inside
  the index would be unsequenced against the y/z reads. This spelling gives
  the target's a1-then-copy-to-a0 order; a plain `n = j;` statement before an
  `n`-reading z term drops the copy.
- **Bindings.** All four images reuse their region's
  `model_variant426_linker_symbols.txt`.

All four images rebuild exactly. The
[attempt ledger](french-model-variant426-ribbon-attempts.csv) records the
probes.
