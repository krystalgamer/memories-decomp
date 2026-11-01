# French MODEL image 12048 sheets helper

`func_8013CBB0` (image offset `0x1BB0`, 0x67C bytes) in the French and
Spanish `model_image_model_12048_8013b000` images draws a set of growing
`POLY_GT4` sheets. The same bytes sit at `func_8017CBB0` in the
`model_image_model_12058_8017b000` images. Both slots compile
`image12048_sheets.c` itself; it defines `FrenchModelVariant403_DrawSheets`,
which each image binds at its own address, `0x8013CB84`/`0x8017CB84` in the
North American and Japanese header-386 images (see [Per-image symbol
bindings](model-variants.md#per-image-symbol-bindings)).

- **Relative.** It follows the
  [header-389 sheets](french-model-image389-sheets.md) loop. It draws
  `ModelVariantSheet` records of 0x98 bytes through the second quad. Before
  phase 4 it draws the timing count of sheets on their transform slots, with
  the size divided by a timing divisor. From phase 4 it draws two sheets: the
  first sits on an `s32` origin and the second on an `SVECTOR` offset. Phase 0
  grows the sheets with time, and phases 1-3 fade them. Phase 4 swells the
  sheets to 8192 and 16384, and phase 5 shrinks them until the timing end.
  The view lives in `french_model_variant/image12048_sheets.h`.
- **Register choice.** The three scale stores have to be written in both
  arms of the `i == 0` branch. GCC merges the copies again, but the extra
  arm moves the quad pointer from `s2` to `s3`, as in the target.
- **Bindings.** Calls use the current resident layout, so all four images
  reuse their region's `model_variant472_linker_symbols.txt`.

All four images rebuild exactly. The
[attempt ledger](french-model-image12048-sheets-attempts.csv) records the
probes.
