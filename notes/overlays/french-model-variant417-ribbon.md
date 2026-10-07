# French MODEL417 ribbon helper

`func_8013CE60` / `func_8017CE60` (MODEL417, image offset `0x1E60`, 0x620
bytes) is the MODEL417 member of the ribbon helper family. **No release had
matching C for this body before.** It is a closed stack-prologue helper, and no
call from the entry was observed.

The accepted MODEL441 ribbon body (`variant441_ribbon.c`) reproduces it
exactly. Every MODEL417 state field is 0x5AC bytes lower than in MODEL441: the
ribbon at `+0xA2C` instead of `+0xFD8`, the `POLY_GT4` at `+0x1FC0`, and
origin, direction, time, timing, index, progress, width and phase likewise.
`variant417_ribbon.h` declares that `Ribbon417State`. The two wrappers map
`Ribbon441State` to it, rename the function and include the MODEL441 body. The
shared Spanish header is not modified.

The helper is registered in all eight French MODEL417 images (models 134, 232,
354 and 535, both slots). `RotTransPers3` was added to the French MODEL417
binding file at the French resident function `0x80087898`. All French overlay
images rebuild exactly. The
[attempt ledger](french-model-variant417-ribbon-attempts.csv) records the
experiment.
