# French header-462 image sheets helper

`func_8013CEF8` / `func_8017CEF8` (image offset `0x1EF8`, 0x534 bytes)
draws and updates the header-462 sheet set.
**No release had matching C for this body before.**

The body appears in the two French raw images of model 457 that also hold
the header-462 rings helper (`french-model-image462-rings.md`), directly
before it.

The control flow follows the Spanish header-461 sheets helper
(`variant461_sheets.c`), with these differences:
- 0x2B0-byte primaries (active at `+0x1C8`), which shift the rest of the
  layout by `+0x1E0`;
- an unweighted sheet scale (`size + extra` on every axis);
- timed phases: sheets grow from the elapsed time between two timing marks
  (phase 0 to 1) and fade between two later marks (phase 5 to 6).

The local view lives in `variant462_sheets.h`, and the resident calls are
covered by `model_variant462_linker_symbols.txt`. The
[attempt ledger](french-model-image462-sheets-attempts.csv) records the
probe, which matched on the first build.
