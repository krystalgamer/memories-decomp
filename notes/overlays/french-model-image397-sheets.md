# French header-397 image sheets helper

`func_8013CA9C` / `func_8017CA9C` (image offset `0x1A9C`, 0x5D0 bytes)
draws and animates the header-397 sheet set.
**No release had matching C for this body before.**

The body appears in four French raw images: models 258 and 21, slot 0
(header `0x18D`) and slot 1 (header `0x223`). Each layout gains a C segment
at `0x1A9C`, carved out of the unclassified image.

The structure follows the Spanish header-459 sheets helper
(`variant459_sheets.c`):
- `timing->count` sheets from `+0xE24` (`ModelVariantSheetSet`);
- below phase 4, sheets sit at 32-byte anchors (`+0x19DC`), scaled by
  `size / timing->divisor`; from phase 4, at the VECTOR positions (`+0x1A70`);
- the depth is `RotTransPers4(...) * 8 / 10`;
- the phase machine:
  - phase 0: timed grow;
  - phase 1: shrink once `time` passes `timing->fade_at`;
  - phase 4: pulse, toggling `shown`;
  - phase 5: shrink, setting `shown` to 2;
- the summed `shown` flags move phase 5 to phase 6.

The quad pointer is assigned after the ot call; assigning it at the
declaration ties the loop-entry count load to the wrong register. The local
view lives in `variant397_sheets.h`, and
`model_variant397_linker_symbols.txt` covers the resident calls. The
[attempt ledger](french-model-image397-sheets-attempts.csv) records six
measured probes.
