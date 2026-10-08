# French header-389 image sheets helper

`func_8013C864` / `func_8017C864` (image offset `0x1864`, 0x5E8 bytes)
draws and updates the header-389 sheet sets.
**No release had matching C for this body before.**

The body appears in eight French raw images: models 184, 269, 409 and 32,
in slot 0 (header `0x185`) and slot 1 (header `0x21B`). Each layout gains a
C segment at `0x1864`, carved out of the unclassified image.

The structure follows the Spanish header-461 sheets helper
(`variant461_sheets.c`):
- **Sets:** a first set of eight sheets is placed by MATRIX translations
  (`transforms[i].t`, `+0x2A5C`); the paired set uses VECTOR positions
  (`+0x2B5C`). The count is 9 or 16, depending on `timing->paired`.
- **Drawing:** sheets are drawn while their size is positive, with depth
  `RotTransPers4(...) * 8 / 10`. `timing->single` hides all but the first
  sheet of the first set.
- **First set:** grows and fades over timed windows, moving to phases 1 and 3.
- **Paired set:** follows the 0x3E4-byte primaries. The pointer is reset at
  `i == first` and advances in both update arms, as in the target. The set
  pulses until every `shown` flag reaches 2, which moves phase 4 to phase 5.

The [attempt ledger](french-model-image389-sheets-attempts.csv) records six
measured probes.
