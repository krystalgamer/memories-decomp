# French MODEL388 strips helper

`func_8013BCD4` / `func_8017BCD4` (MODEL388 family, image offset `0xCD4`,
0x670 bytes) builds two five-point strips, projects them with
`RotTransPers3`, draws them, and then advances the grow and shrink timing.
**No release had matching C for this body before.** It is written from
scratch over a local 0xF8-byte unit, `Strip388Unit` in
`variant388_strips.h`.

Measured shape:
- screen coordinates are `PSXLONG` words (x in the low half, y in `>> 16`).
  The width is `s16`;
- the `POLY` cursor is assigned right after `func_80058F10()`, before
  `ratan2`;
- the first timing test is written `grow_start <= time`;
- the second strip's head (the xy loads and the otz clamp) reads through
  `col`/`next` copies of the unit pointer. The target keeps the
  `move a0,s0` / `move v1,s2` copies there but uses the originals in the
  flag/sort tail. GCC 2.8.1 only keeps them when both copies are reset to
  `work` before `unit->flag[k] = 0` **and** after the draw loops. Leaving
  out either reset changes the registers or drops the copies.

The helper is registered in all eight French MODEL388 images (models 8, 43,
235 and 706, both slots) and in the ten byte-identical Spanish images
(also MODEL269). `RotTransPers3` was added to both MODEL388 binding files,
and `ReadRotMatrix` and `SetRotMatrix` to the Spanish one, at their resident
addresses. Every affected image rebuilds exactly. The
[attempt ledger](french-model-variant388-strips-attempts.csv) records the
probes.
