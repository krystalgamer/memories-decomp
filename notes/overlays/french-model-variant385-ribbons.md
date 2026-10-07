# French MODEL385 ribbons helper

`func_8013CB00` / `func_8017CB00` (MODEL385 family, image offset `0x1B00`,
0x6B8 bytes) builds and draws the eight-ribbon fan. **No release had matching
C for this body before.** It is reachable directly from the entry.

It follows the MODEL402 ribbons helper with the MODEL446 changes (#7164):
- the fan is built only while the progress is positive;
- the radii come from the configuration's halfword at `+0x24`;
- the outer depth is 140.

The local `variant385_ribbons.h` maps the work fields to their MODEL385
offsets. It also declares the 0x60-byte `Ribbon385`, which keeps two
projection flags between the widths and the depths.

Three statements differ from MODEL446:
- `RotTransPers4` writes its flag to `ribbon->flag[i]`, indexed by the outer
  ribbon counter as in the target;
- `RotTransPers` keeps writing the local flag;
- primitives are sorted when `otz >= 0 && ribbon->flag[i] >= 0`.

The helper is registered in all eight French MODEL385 images (models 170,
406, 407 and 513, both slots). `RotTransPers` was added to the French MODEL385
binding file at its French resident address. All French overlay images
rebuild exactly. The [attempt ledger](french-model-variant385-ribbons-attempts.csv)
records both probes.
