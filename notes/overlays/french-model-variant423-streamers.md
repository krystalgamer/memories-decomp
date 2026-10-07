# French MODEL423 and MODEL473 streamers helper

`func_8013DA50` / `func_8017DA50` (MODEL423 family, image offset `0x2A50`) and
`func_8013E0A8` / `func_8017E0A8` (MODEL473 family, offset `0x30A8`) contain
the same 0x7C4-byte body. **No release had matching C for this body before.**
No call from the entry was observed.

The body is the accepted MODEL475 streamers helper (`variant475_streamers.c`)
over the shared `variant458_streamers.h` layout, with three measured
differences:
- the reach is 192 instead of 160;
- segments are sorted whenever the depth is positive, without the
  `< 0x800` cap;
- the phase-4 shrink tail is absent.

`variant423_streamers.c` writes the work offsets through four bases:
`STREAMERS_LIST` (`0xF4C`), `STREAMERS_POLY` (`0x1CD8`), `STREAMERS_ORIGIN`
(`0x1D58`) and `STREAMERS_FIELDS` (`0x1D74`). `variant473_streamers.c`
overrides them with `0x2D14`, `0x3A38`, `0x3B08` and `0x3BC4`, and renames the
function.

The helper is registered in all four French images of model 385 (stages 7
and 8 use MODEL423 bindings, stages 9 and 10 MODEL473). `RotTransPers` was
added to both binding files, and `ratan2`, `rcos` and `rsin` to the MODEL473
file, all at their French resident addresses. All French overlay images
rebuild exactly. The [attempt ledger](french-model-variant423-streamers-attempts.csv)
records the experiments.
