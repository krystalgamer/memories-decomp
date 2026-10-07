# French MODEL427 streamers helper

`func_8013DDF8` / `func_8017DDF8` (MODEL427 family, image offset `0x2DF8`,
0x7C4 bytes) builds, projects and draws two seventeen-point streamers.
**No release had matching C for this body before.** No call from the entry
was observed.

It is the MODEL423 streamers body (#7163): the MODEL475 helper with reach
192, no `< 0x800` depth cap and no phase-4 shrink tail. Two layout
differences made a separate source simpler than another base override:
- the MODEL427 streamer (`variant427_streamers.h`) keeps 0x44 more bytes
  before the depths, giving a 0x378-byte stride;
- the work fields do not share one MODEL423-relative shift: the list is at
  `+0x2388`, the `POLY_G4` at `+0x35C8`, the origin at `+0x3688`, direction
  and frame at `+0x3724..+0x373C`, and the length, rotation and spin at
  `+0x3798..+0x37A4`.

The helper is registered in both French MODEL427 images (model 379, stages 9
and 10). `RotTransPers` and `ratan2` were added to the French MODEL427 binding
file at their French resident addresses. All French overlay images rebuild
exactly. The [attempt ledger](french-model-variant427-streamers-attempts.csv)
records the experiment.
