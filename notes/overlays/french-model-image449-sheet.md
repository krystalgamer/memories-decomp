# French header-449 image sheet helper

`func_8013BD24` / `func_8017BD24` (image offset `0xD24`, 0x51C bytes)
scales, moves, draws and fades one four-quad sheet.
**No release had matching C for this body before.**

The body occurs in four French MODEL raw images that have no configured
variant layout:
- two slot 0 images with header word `0x1C1` (449): models 370 and 715;
- the two matching slot 1 images with header word `0x257`.

Each image layout is split into an unclassified head, the C helper at
`0xD24` and an unclassified tail, following the raw-image convention of
#7137.

The control flow follows the French MODEL458 sheets helper
(`variant458_sheets.c`), with these differences:
- one sheet with one origin matrix and one delta vector, all at fixed
  offsets;
- doubled scale constants:
  - phase 0 ramps the scale to 4096 with `<< 12`;
  - phase 2 grows it by `step << 11` up to 16384;
  - phase 3 fades it by `step << 7`;
- timing fields 4 bytes lower than MODEL458.

The local view lives in `variant449_sheet.h`. The new
`model_variant449_linker_symbols.txt` binds the ten resident calls at
their French addresses, and each split image names it in the overlay
manifest. All French overlay images rebuild exactly. The
[attempt ledger](french-model-image449-sheet-attempts.csv) records both
probes.
