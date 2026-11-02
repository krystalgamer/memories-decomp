# French MODEL582 sheets helper

`FrenchModelVariant582_DrawSheets` (`0x8013CB98` / `0x8017CB98`, image offset `0x1B98`, 0x65C bytes)
animates and draws five three-row sheets as semi-transparent `POLY_GT4`
quads with a masking `POLY_F4`. **No release had matching C for this body
before.**

The local view in `variant582_sheets.h` holds:
- five 0x174-byte sheets at `+0x340`, each with points `[3][13]`, progress
  at `+0x16C` and a state word at `+0x170`;
- the `POLY_F4` mask at `+0x3370` and the `POLY_GT4` quad at `+0x33F0`;
- the positions at `+0x3508`, the direction at `+0x3648`, the step at
  `+0x3990` and the phase at `+0x39D8`.

Measured shape:
- **Points:** above progress 2048 the top row is a 13-point circle of radius
  `(progress - 2048) * 96 / 2048 + 384`, otherwise of radius 384.
- **Drawing:** sheets with positive progress are scaled by progress and
  placed at their position. Twelve quads per sheet are projected from rows 0
  and 2 to derive UVs (texture page by screen half and active buffer), then
  from rows 0 and 1 for the drawn quad, coloured 192/64/192 and sorted with
  the mask packet.
- **Progress:** progress below 4096 grows by `step * 96`. Reaching 4096
  either wraps it (phase below 3, state 1) or clamps it (state 2). A state
  total of 10 in phase 3 moves to phase 4.

Matching notes:
- The yaw from `ratan2` is clamped by a dead three-way `if`/`else if`. The
  extra dead block lengthens the sheet pointer's live range before the loop
  so global-alloc ranks it under the `&sheet->state` induction pointer,
  giving retail's s7/s6. A two-way clamp leaves the two registers swapped.
- `(radius = lift + 384)` inside the `rcos` multiply puts the `+384` after
  the call, so the radius shift fills the delay slot.

The helper is registered in the four French images that carry this body
(models 582 and 129, stage 9 slot 0 and stage 10 slot 1) and in their four
byte-identical Spanish twins. `GetTPage`, `SetPolyGT4`, `SetSemiTrans`,
`SetShadeTex` and `GsGetActiveBuff` were added to the French and Spanish
MODEL469 binding files at their resident addresses. All eight images rebuild
exactly. The [attempt ledger](french-model-variant582-sheets-attempts.csv)
records the probes.
