# French MODEL variant 423 ribbon helper

`func_8013CCB4` in the French and Spanish `model_variant_385_stage7_slot0`
images and `func_8017CCB4` in the `model_variant_385_stage8_slot1` images
(offset `0x1CB4`, 0x8B0 bytes) draw a single ribbon.
`french_model_variant/variant423_ribbon_slot1.c` renames the body for the
slot-1 images.

- **Shape.** It is the single-ribbon body of the family 426 helper at offset
  `0x1DF0`, with these changes:
  - the header-385 work offsets;
  - a blue level of 64 instead of 255 on the outer edges;
  - the amplitude grows by `step * 64` up to 0x400, moving stage 2 to 3,
    instead of following a timing window;
  - the fade window is read at timing offsets 0x24 and 0x28.

  The point-index copy shape and the dead `if (i < 0)` branch are the same
  as in that helper. The view lives in
  `french_model_variant/variant423_ribbon.h`.
- **Bindings.** All four images reuse their region's
  `model_variant423_linker_symbols.txt`.

All four images rebuild exactly. The
[attempt ledger](french-model-variant423-ribbon-attempts.csv) records the
result.
