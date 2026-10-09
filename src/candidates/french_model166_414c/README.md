# French MODEL166 `func_8013F14C` probes

The 27 C files in this directory are retained source variants for the
1,544-byte helper at module offset `+0x414C` (`0x8013F14C`). None is an exact
match or eligible for integration.

`candidate8.c` is the strongest overall result: its object has the exact
`0x608` extent and 312/386 exact relocation-masked words. Its remaining
mismatches are concentrated in early geometry/matrix code. `candidate19.c`
is the strongest signed-division variant: 99/386 masked words at `0x614`
bytes; its high-progress geometry aligns through relative `+0x134` and
diverges in the simple branch at `+0x138`. These scores are diagnostics, not
acceptance evidence.

`candidate8.c` views the root as three `0x1E4` records at offsets `0`,
`0x1E4`, and `0x3C8`. Each record has three `SVECTOR[17]` rows at `+0`,
`+0x88`, and `+0x110`, `CVECTOR[17]` at `+0x198`, progress at `+0x1DC`, and a
still-uncertain word at `+0x1E0`. Treat this as a local candidate view, not
an accepted shared type; other attempts use different hypotheses.

Caller analysis was corrected after the earlier probe notes: the
nonnegative path of `func_8013B004` initializes all three records through
saved-root aliases, including their rows, per-point RGB values, progress
values `0`, `-1365`, and `-2730`, and zero completion counters. Do not spend
restart time searching for a producer based on the earlier mistaken
`s6`-only store scan. The remaining useful work is exact reconstruction of
the helper's geometry and compiler lifetime/order behavior.

The archived variants are `candidate.c`, `candidate6.c` through
`candidate24.c`, and `candidate26.c` through `candidate32.c`. Assembly-only
experiments and extracted target data are intentionally not included.
