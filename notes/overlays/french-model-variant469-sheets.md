# French MODEL469 sheets helper

`SpanishModelVariant469_DrawSheets` (`0x8013DB30` / `0x8017DB30`, MODEL469 family, image offset `0x2B30`,
0x538 bytes) draws the primary and secondary sheet sets. **No release had
matching C for this body before.**

It follows the accepted MODEL461 sheets helper (`h444_sheets_pal.c`) over the
MODEL469 layout. It was first matched through a private view,
`h452_sheets_2b30.h`; since #9563 it takes `EntryState469` and
`EntryConfig469` directly:
- the primaries at `+0xA84` are the entry's `ribbons`, whose `state` and
  `length` the view called `active` and `size`; the sheets at `+0x26E8` are
  `quads` and the polygon pair at `+0x3388` is `textured`;
- the positions at `+0x3558` are `path_origins`/`path_targets`, and frame,
  time and step are in `clock`, `0x62C` bytes after their MODEL461 offsets;
- the timing is `config`, whose `fade_end` is the config's `matrix_end`.

Four statements differ from MODEL461:
- secondary sheets scale x and z by `size * 2 + extra`;
- polygons are sorted whenever the depth is positive, with no flag test;
- a newly shown secondary sheet starts at size 8192;
- a visible secondary sheet follows `primary->length * 8`.

The helper is registered in all four French MODEL469 images (models 129 and
582, both slots). All French overlay images rebuild exactly. The
[attempt ledger](french-model-variant469-sheets-attempts.csv) records both
probes.
