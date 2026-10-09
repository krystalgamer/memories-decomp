# French MODEL166 `func_8013F14C` candidate

This directory retains only the best available source candidate for the
1,544-byte helper at module offset `+0x414C` (`0x8013F14C`).

`candidate8.c` has the exact `0x608` extent and 312/386 exact
relocation-masked words. Its remaining mismatches are concentrated in early
geometry/matrix code. This score is diagnostic, not acceptance evidence; the
candidate is not exact or eligible for integration.

The private `candidate8.h` view has three `0x1E4` records at offsets `0`,
`0x1E4`, and `0x3C8`. Each record has three `SVECTOR[17]` rows at `+0`,
`+0x88`, and `+0x110`, `CVECTOR[17]` at `+0x198`, progress at `+0x1DC`, and a
still-uncertain word at `+0x1E0`. Treat this as a local candidate view, not
an accepted shared type.

Caller analysis was corrected after the earlier probe notes: the
nonnegative path of `func_8013B004` initializes all three records through
saved-root aliases, including their rows, per-point RGB values, progress
values `0`, `-1365`, and `-2730`, and zero completion counters. Do not spend
restart time searching for a producer based on the earlier mistaken
`s6`-only store scan. The remaining useful work is exact reconstruction of
the helper's geometry and compiler lifetime/order behavior.
