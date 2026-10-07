# French header-383 image quads helper

`func_8013C91C` / `func_8017C91C` (image offset `0x191C`, 0x5AC bytes)
draws and animates the header-383 quad group.
**No release had matching C for this body before.**

The body appears in the two French raw images of model 363, directly after
the header-383 ribbons helper.

The code follows the French header-373 quads helper (`variant373_quads.c`)
over the header-383 layout, with these differences:
- it draws into the second quad of a pair at `+0x138C`;
- the quad colours are written once before the vertex loop;
- the colour update runs from phase 8 onwards;
- the depth is `RotTransPers4(...) * 8 / 10`, sorted when the depth and flag
  are non-negative;
- the configured duration is read at `+0x10`.

The [attempt ledger](french-model-image383-quads-attempts.csv) records two
measured probes.
