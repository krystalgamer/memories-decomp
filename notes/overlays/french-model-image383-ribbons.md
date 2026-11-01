# French header-383 image ribbons helper

`func_8013C204` / `func_8017C204` (image offset `0x1204`, 0x718 bytes)
builds, projects and draws eight ribbons.
**No release had matching C for this body before.**

Both slots compile `variant383_ribbons.c` itself; it defines `FrenchModelVariant383_DrawRibbons`, which
each image binds at its own address, the same addresses in the North American and Japanese header-366 images (see [Per-image symbol
bindings](model-variants.md#per-image-symbol-bindings)).

The body appears in the two French raw images of model 363 that also hold
the header-383 strip and quads helpers, between them.

The code follows the French header-373 ribbons helper (`variant373_ribbons.c`)
over the header-383 layout (colors at `+0x6C`, ribbons at `+0xF4`, triangle at
`+0x1334`), with these differences:
- each ribbon segment keeps its own clip flag (`flag[8][2]`);
- the edge projection uses a separate flag;
- triangles are drawn when both the depth and that segment's flag are
  non-negative.

The [attempt ledger](french-model-image383-ribbons-attempts.csv) records
three measured probes.
