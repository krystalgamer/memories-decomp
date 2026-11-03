# French MODEL469 ribbons helper

`FrenchModelVariant469_DrawRibbons` (`0x8013D1F4` / `0x8017D1F4`, image offset `0x21F4`, 0x93C bytes) in the
model_variant_129 and model_variant_582 stage 9 slot 0 and stage 10 slot 1
images is a five-ribbon form of the header-475 ribbon body
(`spanish_model_variant/variant475_ribbons.c`). Each `Variant469Ribbon` is
0x250 bytes at `work + 0xA84`. It holds eleven spine points with no per-point
projection flag. The layout lives in `french_model_variant/variant469_ribbons.h`.

Changes from the 475 body:

- **Spine.** Point `k` is the ribbon's end point times `k / 10`. The view copy
  is pushed by a 0x30 length.
- **Projection.** Both projections share the scalar `flag`. A segment is
  sorted only when `otz > 0`.
- **Bend.** The second wave uses the projected width unscaled. The bend phase
  is `i * 6144 / 5 - k * 6144 / 10`.
- **Lifecycle.** The drawn length grows to 10, then the view offset shrinks by
  `step * 64`. A restart resets the count to -50. The retire signal fires
  when the state sum reaches 10.

The template's per-ribbon `turn` selection and the first loop's `bend` are
still computed and never used. Removing them changes the output:

- Without `turn`, combine folds `(s16)i << 4` into one `sll`/`sra` pair at
  the top of the outer loop.
- Without the dead `bend`, the spare sign extension of `k` disappears from
  the first loop.

`RotTransPers` (`0x80087868`) was added to the French and Spanish MODEL469
linker-symbol files. The helper is registered in the four French images and
their byte-identical Spanish twins. All eight images rebuild exactly. The
[attempt ledger](french-model-variant469-ribbons-attempts.csv) records the
probes.
