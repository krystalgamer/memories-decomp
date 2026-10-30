# French MODEL variant 425 ribbon helper

`func_8013CDA4` in the French and Spanish `model_variant_0_stage9_slot0`
images and `func_8017CDA4` in the `model_variant_0_stage10_slot1` images
(offset `0x1DA4`, 0x938 bytes) draw a single swaying ribbon.
`french_model_variant/variant425_ribbon_slot1.c` renames the body for the
slot-1 images.

- **Shape.** It is the single-ribbon body of the family 426 helper at offset
  `0x1DF0`, with these changes:
  - the header-0 stage 9/10 work offsets;
  - the edge length is `fade * 80 / 1024` or `fade * 96 / 1024`, depending
    on frame parity;
  - each edge is pushed out by an extra sway term,
    `rcos/rsin(sway) * (fade / 64) >> 12`, where the sway starts at the
    negated phase and steps by 768 per point;
  - the tail advances the phase by `step * 384`.

  The view lives in `french_model_variant/variant425_ribbon.h`.
- **Register choice.** The width is computed in both arms of the length
  branch, and the sway is set in the inner loop's initialiser. The
  point-index copies and the dead `if (i < 0)` branch match the family 426
  helper.
- **Bindings.** All four images reuse their region's
  `model_variant425_linker_symbols.txt`.

All four images rebuild exactly. The
[attempt ledger](french-model-variant425-ribbon-attempts.csv) records the
probes.
