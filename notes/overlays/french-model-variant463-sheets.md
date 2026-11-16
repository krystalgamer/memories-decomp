# French header-463 sheets helper

`FrenchModelVariant463_DrawSheets` (`0x8013E2E8` / `0x8017E2E8`, image offset
`0x32E8`, 0x4EC bytes) draws and sizes up to ten four-quad sheets. One source
serves both slots; each image's symbols file binds the name at its own
address.

The body appears in eight images:

| Region | Slot 0 | Slot 1 |
| --- | --- | --- |
| French | `model_variant_424_stage9_slot0`, `model_variant_461_stage9_slot0` | raw `model_image_model_103434_8017b000`, `model_image_model_113646_8017b000` (new C segment at `0x32E8`, beside the rings helper) |
| Spanish | `model_image_model_103424_8013b000`, `model_image_model_113636_8013b000` | `model_image_model_103434_8017b000`, `model_image_model_113646_8017b000` |

- **Shape.** The control flow follows the Spanish header-461 sheets helper
  (`variant461_sheets.c`), with these differences:
  - the sheet count is the timing record's first count;
  - each sheet scales uniformly;
  - sheets below the count follow the primary records;
  - later sheets grow by timed division, and fade once the phase reaches 5.
- **Paired test.** The count is read in both arms of a paired-timing test.
  The arms are identical, but the branch keeps the polygon advance and sheet
  base out of CSE, matching the target.
- **Record and bindings.** The helper reads `Variant446EntryState`, the
  record the entry passes (a view in `variant463_sheets.h` until it was
  folded). The primaries are the five `0x2B0`-byte `trails` at
  `work+0xD70`; the primary walk, which runs up to the config's first
  count, stays inside that array. The sheets are `panels` at `0x20F8`. The French
  images reuse `model_variant463_linker_symbols.txt`. The Spanish images now
  bind `model_variant461_linker_symbols.txt`.

All eight images rebuild exactly. The
[attempt ledger](french-model-variant463-sheets-attempts.csv) records the
probes.
