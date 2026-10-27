# French MODEL461 ribbons helper

`func_8013C508` / `func_8017C508` (image offset `0x1508`, 0xA4C bytes) in the
model_variant_705 stage 9 slot 0 and stage 10 slot 1 images is the
[header-469 ribbon body](french-model-variant469-ribbons.md) with the header-461
work offsets. It reuses `Variant469Ribbon` from
`french_model_variant/variant469_ribbons.h` and differs in four ways:

- **Ribbon base.** The five 0x250-byte ribbons start at `work + 0`.
- **Projection flags.** `RotTransPers4` writes into a local
  `PSXLONG flag[5][11]` table, one row per ribbon. `RotTransPers` keeps a
  scalar flag.
- **Sort gate.** A segment is sorted only when `otz[k] >= 0` and
  `flag[i][k] >= 0`.
- **Last segment.** The final drawn segment (`k + 1 == count`) ends on the
  bare projected spine point `sa[count]`, with no width offsets.

The ribbon pointer must be assigned after the `func_80058F10` call. Assigning
it earlier keeps `work` in `s6` straight from `a0`; retail reloads it from
the stack home after the call.

`RotTransPers` was added to the French and Spanish MODEL461 linker-symbol
files. The Spanish file also gained the `ratan2`, `rcos` and `rsin` bindings
that the French file already had. The helper is registered in the two French
images and their byte-identical Spanish twins. All four rebuild exactly. The
[attempt ledger](french-model-variant461-ribbons-attempts.csv) records the
probes.
