# French header-449 image streamers helper

The `0x8BC` helper at image offset `0x1B9C` is first-ever game code: no release
had matching C for its body. It sits in the same four header-449 images as the
ribbons and bands helpers. Both slots build
`variant449_streamers.c`; each image's symbols file binds
`FrenchModelVariant449_DrawStreamers` (formerly `func_8013CB9C`/`func_8017CB9C`)
at its own address.

The body is the MODEL458 streamer helper in the header-449 layout:
- three `0x274`-byte, 13-point streamers at `F3C`, directly after the band;
- one `POLY_FT4` at `1AA8`, the origin matrix at `1B14` and the delta at
  `1BB0`;
- `factor`/`scale` at `1B90/1B98`, the axis words, the flags at `1BDC`, and
  length/rotation at `1C24..1C30`.

It differs from MODEL458 in three measured ways:
- the coil radius is 144 instead of 96;
- there is a single transform pass;
- that pass reads the one origin and delta directly.

Indexing them by the pass counter spills the view pointer instead of the
polygon pointer and does not match.
