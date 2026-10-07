# French MODEL417 tube helper

`func_8013C778` / `func_8017C778` (MODEL417, image offset `0x1778`, 0x6E8
bytes) is the MODEL417 member of the tube helper family. **No release had
matching C for this body before.**

It follows the accepted MODEL441 tube (`variant441_tube.c`) over a MODEL417
state in which every field is 0x5AC bytes lower; for example, the tubes start
at `+0x0` and the `POLY_G4` is at `+0x1FC0`. The one semantic difference is
the radius base: MODEL441 uses `width / 16`, while MODEL417 uses
`width * (timing->width_scale / 3) / 1024`, which reads a timing word at
`+0x30`. Both the pulsed and the plain radius use it. The shared Spanish
header and body are not modified. `variant417_tube.{c,h}` are full MODEL417
copies, and `variant417_tube_slot1.c` renames the function.

The helper is registered in all eight French MODEL417 images (models 134, 232,
354 and 535, both slots), and all French overlay images rebuild exactly. The
[attempt ledger](french-model-variant417-tube-attempts.csv) records the
experiment.
