# French MODEL469 sheets helper

`func_8013DB30` / `func_8017DB30` (MODEL469 family, image offset `0x2B30`,
0x538 bytes) draws the primary and secondary sheet sets. **No release had
matching C for this body before.**

It follows the accepted MODEL461 sheets helper (`variant461_sheets.c`) over a
MODEL469 layout, `variant469_sheets.h`:
- primaries start at `+0xA84`, sheets at `+0x26E8` and the polygon pair at
  `+0x3388`;
- positions start at `+0x3558`, and frame, time, step, timing and phase lie
  `0x62C` bytes after their MODEL461 offsets.

The timing and primary structures are unchanged. Four statements differ from
MODEL461:
- secondary sheets scale x and z by `size * 2 + extra`;
- polygons are sorted whenever the depth is positive, with no flag test;
- a newly shown secondary sheet starts at size 8192;
- a visible secondary sheet follows `primary->size * 8`.

The helper is registered in all four French MODEL469 images (models 129 and
582, both slots). All French overlay images rebuild exactly. The
[attempt ledger](french-model-variant469-sheets-attempts.csv) records both
probes.
