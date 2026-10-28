# French MODEL411 tube helper

`func_8013BE60` / `func_8017BE60` (image offset `0xE60`, 0x71C bytes)
builds and draws a 9x9 point tube as gouraud `POLY_G4` quads. **No release
had matching C for this body before.** It has the same shape as the matched
MODEL175 tube helper (`variant175_tube.c`) with a constant radius.

The local view in `variant411_tube.h` repeats the MODEL175 state layout,
except that progress is at `+0x1658` instead of `+0x165C`:
- one 0x54C-byte tube at `+0` with `SVECTOR points[9][9]` and nine
  `CVECTOR` colours at `+0x510`;
- the quad at `+0x1420`;
- the origin at `+0x15F8`, the direction at `+0x160C`, the projected point
  at `+0x161C` and the second direction at `+0x1620`;
- the unsigned time at `+0x163C`, the step at `+0x1644` and the timing
  pointer at `+0x164C` (expansion start/end at `+0x10`/`+0x14`, fade
  start/end at `+0x18`/`+0x1C`);
- progress at `+0x1658`, width at `+0x166C`, angle at `+0x1670` and phase
  at `+0x167C`.

Measured shape:
- **Points:** the radius is `width * 80 / 1024`. Row `j` starts at angle
  `angle + 64 * j` and is offset along the direction by `progress / 1024 * j / 8`.
  Point `k` turns by `512 * k` around the axis given by the third `ratan2`
  plus 3072.
- **Drawing:** the quads are projected with an identity rotation at the
  origin. In phase 6 and later the colours are scaled by `width / 1024`;
  each quad is submitted through `func_8005B260` when its depth is not
  negative.
- **Progress:** before phase 2, progress follows the expansion window up to
  1024 and then moves to phase 2. After the fade start, width falls from 1024
  to zero and moves to phase 4. The angle then decreases by `step * 32`.

Matching notes:
- Retail keeps the `>> 10` of the radius division separate from the s16
  narrowing, with the product still live in v0. A dead `ratan2` yaw adjust
  guarded by `state->width * 80 > 0`, placed between the division and the
  narrowing, reproduces both. Guards on `size`, on the live yaw or on a
  separate product variable each leave one or two register or copy
  differences.
- The faded colours write all of `setRGB3`. The branch then crossjumps into
  the shared `b3` store; writing only r3/g3 scores the same in a
  target-masked diff but leaves the jump one store late.

The helper is registered in the French MODEL364 and MODEL452 stage 9 slot 0
and stage 10 slot 1 images and in their byte-identical Spanish twins.
`GsGetActiveBuff` and the `func_8005B260` alias of
`Graphics_SubmitDrawModePacket` were added to the French and Spanish
MODEL411 binding files. All eight images rebuild exactly. The
[attempt ledger](french-model-variant411-tube-attempts.csv) records the
probes.

The [French loader entries](french-model-variant411-entry.md) now also
match in all four French images, completing their five inventoried
functions. Spanish entry registrations remain unchanged.
