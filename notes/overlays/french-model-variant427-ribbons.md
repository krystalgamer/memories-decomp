# French MODEL variant 427 ribbons helper

`func_8013CFC4` in the French and Spanish `model_variant_379_stage9_slot0`
images and `func_8017CFC4` in the `model_variant_379_stage10_slot1` images
(offset `0x1FC4`, 0x8DC bytes) draw three ribbons.
`french_model_variant/variant427_ribbons_slot1.c` renames the body for the
slot-1 images.

- **Shape.** Each ribbon with a positive amplitude places 17 points along its
  direction vector, scaled by that amplitude, and moves a copy along the view
  by the edge width. Each ribbon is projected through its own anchor matrix.
  The draw loop sorts two `POLY_GT4` halves per segment, green on the outer
  edges and grey along the spine. It then grows each amplitude to 0x400 and
  moves stage 2 to 3. The tail fades the width over the timing record's fade
  window, moving stage 4 to 5. The view lives in
  `french_model_variant/variant427_ribbons.h`.
- **Register choice.** The point index is copied through `n = kk = k;`
  before `setVector`, which indexes with `n`; the y term reads `(kk = n)` and
  the z term reads `k`, giving the target's a1-then-copy-to-a0 order. The
  copies are separate full expressions because the macro repeats its pointer
  argument. The row address is set in the inner loop's
  initialiser, `for (k = 0, v = work + i * 16; ...)`, which schedules
  `move s6,zero` before the row address as in the target.
- **Bindings.** All four images reuse their region's existing linker
  symbols.

All four images rebuild exactly. The
[attempt ledger](french-model-variant427-ribbons-attempts.csv) records the
probes.
