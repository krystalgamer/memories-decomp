# French header-447 image rings helper

`func_8013E520` / `func_8017E520` (image offset `0x3520`, 0x42C bytes) in
the French and Spanish `model_image_model_109772_8013b000` (slot 0) and
`model_image_model_109782_8017b000` (slot 1) images (header 447) is the
header-463 rings body. It grows, scales and draws 24 `POLY_FT4` rings.
Nothing in the loader or the resident executable reaches it.

- **Layout.** The work area matches the header-463 rings layout, except that
  the pad after the polygon at `0x4420` is 0x54 bytes instead of 0x4C. Every
  later field is 8 bytes higher. The view lives in
  `french_model_variant/variant447_rings.h`.
- **Compiler.** Under gcc 2.8.1 the source is two words longer, uses
  different reload registers and fills the epilogue delay slot. Under
  `gcc_2_7_2_cdk_g0` it matches exactly, including the
  `addiu sp; jr ra; nop` epilogue. This is the same compiler evidence as the
  [MODEL variant helpers](model-variants.md#compiler).
- **Bindings.** Its calls go to an older resident layout that does not match
  any shipped executable: `rsin` at `0x80053D1C`, `GsSortPoly` at
  `0x80057B1C`, and so on. The names come from the identical call sequence of
  the header-463 body. They live in `model_image447_rings_linker_symbols.txt` in both regions;
  the slot 0 and slot 1 copies call the same absolute addresses.

All four French and Spanish images rebuild exactly. The
[attempt ledger](french-model-image447-rings-attempts.csv) records the probes.
