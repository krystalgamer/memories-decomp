# Duel-effect color and fullscreen helpers

Two complete groups add six functions / 860 bytes with the existing
`gcc_2_8_1_g0_split` profile:

| Group | Range | Functions | Bytes |
|---|---|---:|---:|
| `color_transition.c` | `0x80153F28..0x80154084` | 3 | 348 |
| `screen_draw.c` | `0x801556F4..0x801558F4` | 3 | 512 |

The first routine decrements each channel only when it exceeds `step + 1`;
otherwise it writes zero. Do not replace this with a conventional saturating
subtraction: the boundary behavior differs, including for a zero step.
The second advances toward three caller-supplied channels, clamps each to
its target, and returns one only when all three targets have been reached.
The third packs three bytes into bits 16, 8 and 0 of the existing resident
word `D_8009B300`, using its declaration in `src/game/sorted_entry.h` and the
Spanish resident binding `0x8009C688`. It does not create overlay storage.

The fullscreen helpers retain the actual PAL `320 x 256` rectangle, SDK
`POLY_F4`/`POLY_G4` layouts, the existing ordering table and unsigned-halfword
priority. One draws a flat polygon before decrementing the caller's color;
one grades from a black top edge to the supplied bottom-edge color; one
draws a flat polygon without changing the color. Only the first has the
post-submission update.

## Expression and allocation experiments

Five functions matched immediately. The target-color routine required both
the halfword threshold expression and explicit completion branches:

| Candidate | Target-color result |
|---|---|
| `color < target + 1 - step`, direct conjunction return | 192 instead of 196 bytes; 34 differing words, including a factored `step - 1` temporary and compact boolean return. |
| `color <= target - step`, direct conjunction return | 180 bytes; 47 differing words and inverted comparison branches. |
| `color < target - step + 1`, direct conjunction return | 192 bytes; 21 differing words, with threshold arithmetic ordered differently. |
| Original threshold with explicit `if (...) return 1; return 0;` | Correct 196-byte size; 23 differing words remain from threshold factoring/allocation. |
| `color < (u16)(target + 1) - step`, explicit completion branches | All six functions match at exact sizes with unchanged compiler settings. |

The halfword cast does not truncate a valid target-plus-one: an unsigned
byte plus one is in `1..256`. It preserves the recovered expression boundary
without register pins, barriers, inline assembly or one-off flags.
Candidate sources, headers and precise comparisons remain under
`tmp/color-probe/`.

A private complete-bank link adds these two groups to the independently
verified 29-function batch and reproduces the exact 90,112-byte Spanish
image: 35 actual C functions / 5,824 bytes, leaving 50 provisional
boundaries as assembly. Production verification additionally covers every
terrain copy and all seven configured module images, with exact C object
and linked-ELF symbol ownership. No other release is claimed to match these
new fullscreen constants without its own complete-image proof.
