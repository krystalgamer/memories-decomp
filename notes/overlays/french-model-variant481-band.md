# French header-481 band renderer

`FrenchModelVariant481_DrawBand` (image offset `0xE38`, 0x8E0 bytes) is
the third function of the French header-481/631 images (models 248 and 624,
slot 0 and slot 1). It grows, projects and draws the entry's 33-step band.
It builds from
[`variant481_band.c`](../../src/overlays/french_model_variant/variant481_band.c)
in both slots under `gcc_2_8_1_g0_split`. All four French images
rebuild byte-identical to retail.

The US header-464/614 images at the same sectors carry the same function at
`+0xE3C` (0x8CC bytes). The same source builds it under `gcc_2_7_2_cdk_g0`,
and all four US images rebuild byte-identical too. Each image binds the name
at its own address (`0x8013BE38`/`0x8017BE38` in the PAL images,
`0x8013BE3C`/`0x8017BE3C` in the North American and Japanese images). Earlier notes called this function the funnel renderer; it draws the
band.

## Behaviour

The band is the entry's 0x8EC-byte record at context `+0x230`. The local
view in [`variant481_band.h`](../../src/overlays/french_model_variant/variant481_band.h)
keeps the entry's `amount`/`offset` names for `+0x420`/`+0x4A4`.

- **Growth:** each step's `offset` starts at `-128 * step`. It is clamped
  to 0 for the shape and grows by `step * 32` until it caps at 0x800.
  Step 0 reaching 0x400 raises the phase to 2; step 32 reaching 0x800 in
  phase 2 moves it to 6.
- **Shape:** the spine point is `direction * offset / 1024`, lifted by
  `rsin(offset * 2) * 512 >> 12`. The edge point sits beside it at the
  view yaw plus 3072. The width rises from 16 to 96 over the first 512
  units of the offset.
- **Projection:** under an unrotated, unscaled matrix at the entry's part
  position, each step projects a spine segment with `RotTransPers4` and the
  edge point with `RotTransPers`. The last step reuses its previous segment.
  The half-width vector `(dx, dy)` follows the screen-space direction to the
  target, scaled by the projected edge distance.
- **Drawing:** 32 GT4 quads take the step colours. They are mirrored
  according to the slot side and whether the view angle lies in
  [-1024, 1024). The first quad uses the first beam packet and the rest the
  second.

## Matching notes

Retail keeps a copy of the unclamped offset that it never uses. The source
spells it as an `unclamped` copy tested by `if (unclamped < 0)` after the
width computation. The branch body is a re-read of
`state->bands[j].offset[i]` into the copy.

- Flow deletes that dead body after loop optimisation, and post-reload jump
  optimisation deletes the empty branch. That leaves retail's dead
  `move v1,s0` in the clamp test's delay slot.
- The body's `j * 0x8EC` address enlarges loop 1 enough that GCC keeps
  `offset[0]`/`offset[32]` addressed from the band pointer instead of
  strength-reducing them into a separate register.
- The copy must stay live across the width test's `slti v0` to land in `v1`.

Other required shapes:
- **Width rise:** the 80-unit rise is a `s16` variable, so the multiply uses
  a hoisted register.
- **Screen words:** screens are packed words read as `(u16)` and `>> 16`,
  like the entry's projection.
- **Discarded call:** the second `ratan2` result is discarded.

The [attempt ledger](french-model-variant481-band-attempts.csv) records the
measured probes.
