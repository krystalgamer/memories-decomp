# French MODEL417 ribbon helper

`SpanishModelVariant441_DrawRibbon` (`0x8013CE60` / `0x8017CE60`, MODEL417, image offset `0x1E60`, 0x620
bytes) is the MODEL417 member of the ribbon helper family. **No release had
matching C for this body before.** It is a closed stack-prologue helper, and no
call from the entry was observed.

The accepted MODEL441 ribbon body (`h424_ribbon_pal.c`) reproduces it
exactly. Every MODEL417 state field is 0x5AC bytes lower than in MODEL441: the
ribbon at `+0xA2C` instead of `+0xFD8`, the `POLY_GT4` at `+0x1FC0`, and
origin, direction, time, timing, index, progress, width and phase likewise.
Both bodies take their entry record: MODEL441's `Variant441EntryState` and
MODEL417's `Variant417EntryState` name these members identically, so the
MODEL417 wrapper maps one type to the other, skips the MODEL441 entry header and
includes the MODEL441 body.

The helper is registered in all eight French MODEL417 images (models 134, 232,
354 and 535, both slots). `RotTransPers3` was added to the French MODEL417
binding file at the French resident function `0x80087898`. All French overlay
images rebuild exactly. The
[attempt ledger](french-model-variant417-ribbon-attempts.csv) records the
experiment.
