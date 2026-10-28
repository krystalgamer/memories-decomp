# French MODEL image 12048 chains helper

`func_8013D22C` (image offset `0x222C`, 0xA6C bytes) in the French and
Spanish `model_image_model_12048_8013b000` images draws three swinging
chains as textured strips. The same bytes sit at `func_8017D22C` in the
`model_image_model_12058_8017b000` images, and
`french_model_variant/image12048_chains_slot1.c` renames the body for that
slot.

- **Shape.** Each 0x1DC-byte chain has nine joints. Loop 1 places joint 0
  from the chain's yaw and pitch, then spreads the other joints along the
  reversed direction with a cosine sway. Each joint also gets an edge point
  24 units along the facing angle. Loop 2 projects joint pairs with
  `RotTransPers4` and the edge points with `RotTransPers`, then derives each
  segment's screen angle, width and `dx`/`dy` offsets. Loop 3 builds one
  of two `POLY_FT4` quads per visible segment and sorts it when the depth
  and the projection flag are not negative. It then moves the visible
  window's end, and then its start, by two joints. When the start passes
  the last joint, the chain resets its window and swings to a new yaw and
  pitch. From phase 6 onward it also advances its stage, and phase 7 starts
  when the third chain reaches stage 4. The view lives in
  `french_model_variant/image12048_chains.h`.
- **Angle.** The angle comes from `ratan2(target[2], target[0])`. A second
  `ratan2(target[1], target[0])` call follows, and its result is discarded.
  The target computes the angle in the second call's delay slot, so it uses
  the first result. Taking the angle from the second call leaves the
  `chain+4` base one call late.
- **Register choice.** Loop 1 indexes as `j[chain->pos]` and builds the edge
  pointer from `pos[18]`. In loop 2, `dx` and `dy` are computed before
  `ratan2` so that the operands load x first. The draw loop resets the quad
  pointer in the loop step. Each of these is needed for the target's
  address forms and allocation.
- **Bindings.** Calls use the current resident layout, so all four images
  reuse their region's `model_variant472_linker_symbols.txt`.

All four images rebuild exactly. The
[attempt ledger](french-model-image12048-chains-attempts.csv) records the
probes.
