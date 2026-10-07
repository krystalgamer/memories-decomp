# French MODEL446 ribbons helper

`func_8013C95C` / `func_8017C95C` (MODEL446 family, image offset `0x195C`,
0x68C bytes) builds and draws the eight-ribbon fan. **No release had matching
C for this body before.** It is reachable directly from the entry.

It follows the accepted MODEL402 ribbons helper (`variant402_ribbons.c`). The
local `variant446_ribbons.h` maps the MODEL402-named work fields to their
MODEL446 offsets: ribbons at `+0x1250`, the `POLY_G3` at `+0x1970`, the
configuration pointer at `+0x1BA4` and so on. The measured differences are:
- the fan is only built while the halfword progress is positive;
- the inner and outer radii come from the configuration's halfword at `+0x24`,
  with 64 added for the outer point, instead of the fixed 40 and 150;
- the outer points sit 140 instead of 160 units deep;
- primitives are sorted whenever the depth is positive, without the `0x800`
  cap.

The helper is registered in both French MODEL446 images (model 146, stages 9
and 10). `RotTransPers` was added to the French MODEL446 binding file at its
French resident address. All French overlay images rebuild exactly. The
[attempt ledger](french-model-variant446-ribbons-attempts.csv) records the
experiment.
