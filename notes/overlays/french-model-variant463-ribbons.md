# French header-463 ribbon helpers

Two helpers in the header-463 images draw five ribbons, one per primary
record:

- `FrenchModelVariant463_DrawRibbons` (`0x8013C984` / `0x8017C984`, image
  offset `0x1984`, 0x994 bytes), `variant463_ribbons.c`;
- `FrenchModelVariant463_DrawTrailRibbons` (`0x8013D860` / `0x8017D860`,
  image offset `0x2860`, 0xA88 bytes), `variant463_trail_ribbons.c`.

One source serves both slots; each image's symbols file binds the name at its
own address. They are built into the same eight images as
the [header-463 sheets helper](french-model-variant463-sheets.md). In the
French raw slot-1 images, new C segments at `0x1984` and `0x2860` replace
the remaining raw bytes before the paired sheets helper and the sheets
helper.

- **Shape.** Each 0x2B0-byte ribbon record holds thirteen points. Per record:
  - the points are spread along the record's direction and offset by a
    fixed length (48 for the ribbons, 96 for the trailing ribbons);
  - they are projected with RotTransPers4, with the last point reusing
    segment 11;
  - widths and offsets come from the record scale, and each point sways by
    `rsin(phase)` and the wave;
  - the shown count is drawn as alternating FT4 quads, gated on the depth
    and on a local flag table.

  The records then grow and fade through a small state machine.
- **Trailing ribbons.** These differ in:
  - the work layout;
  - a last quad that ends on the bare spine point;
  - phase steps 1 to 2 and 6 to 7;
  - a fade-out gate on the word at `0x4A8C`, which also decays from phase 4;
  - advancing only the wave.
- **Register shape.** Both bodies open the outer loop with a dead `i < 0`
  test. The inner point loop opens with nested dead `j < 0` and `j == -1`
  tests. Branch deletion removes these tests after reload, leaving the
  index loads and extensions that the target keeps.
- **View and bindings.** The view lives in `variant463_ribbons.h`.
  `model_variant463_linker_symbols.txt` gains `RotTransPers` for the French
  raw slot-1 images.

All eight images rebuild exactly. The
[attempt ledger](french-model-variant463-ribbons-attempts.csv) records the
probes.
