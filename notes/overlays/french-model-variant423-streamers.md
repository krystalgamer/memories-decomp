# French MODEL423 and MODEL473 streamers helper

`FrenchModelVariant423_DrawStreamers`, bound per image at `0x2A50` (MODEL423
family, formerly `func_8013DA50` / `func_8017DA50`) and `0x30A8` (MODEL473
family, formerly `func_8013E0A8` / `func_8017E0A8`), contains
the same 0x7C4-byte body. **No release had matching C for this body before.**
No call from the entry was observed.

The body is the accepted MODEL475 streamers helper (`variant475_streamers.c`)
over the shared `variant458_streamers.h` layout, with three measured
differences:
- the reach is 192 instead of 160;
- segments are sorted whenever the depth is positive, without the
  `< 0x800` cap;
- the phase-4 shrink tail is absent.

`variant423_streamers.c` reads `EntryState423` members, and
`variant473_streamers.c` is its own copy over `EntryState473`, whose same
fields sit later (streamers at `0x2D14` instead of `0xF4C`, the packet at
`0x3A38` instead of `0x1CD8`). Both read their two `POLY_FT4` templates as
one `POLY_G4` through the state's `flat` union (`ft4` / `g4`).

The helper is registered in all four French images of model 385 (stages 7
and 8 use MODEL423 bindings, stages 9 and 10 MODEL473). `RotTransPers` was
added to both binding files, and `ratan2`, `rcos` and `rsin` to the MODEL473
file, all at their French resident addresses. All French overlay images
rebuild exactly. The [attempt ledger](french-model-variant423-streamers-attempts.csv)
records the experiments.
