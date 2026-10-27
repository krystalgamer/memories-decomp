# Spanish MODEL459 warp rings

Helper `+0x13E0..+0x19AC` is matching C. It is 1,484 bytes in the twelve
Spanish MODEL459 loads (MODEL47, MODEL238, MODEL411 and MODEL620 at stages
7/8; MODEL231 and MODEL417 at stages 9/10) and in the eleven French images
that carry the identical body. French MODEL238 has no stage-8 image. That makes
23 instances and 34,132 instruction bytes. The helper has no observed
entry-reachable call.

## Shared source

The body is the [MODEL438 warp rings](spanish-model-variant438-warp.md) with
a different work-block layout. `variant459_warp.c` defines
`VARIANT438_WARP_LAYOUT_459` and renames the function, then includes
`variant438_warp.c`. The layout macro selects three things:

- **State.** `Warp459State` keeps the five 0x1A8-byte rings at `0` and moves
  the polygon to `+0x2DC8`, the origin to `+0x2EE0`, the direction to
  `+0x3054`, the step to `+0x308C` and the phase to `+0x30DC`.
- **Colours.** Each inner and outer component is scaled by
  `(2048 - size) / 2048`, so a ring fades as it grows.
- **Sorting.** Quads sort whenever depth and flag are non-negative. MODEL438
  also requires `size < 2048`.

The first candidate was exact. The MODEL438 build of the shared body is
unchanged and still matches in all 24 of its images.

All 23 complete images match using `gcc_2_8_1_g0_split`. The ledger
[`spanish-model-variant459-warp-attempts.csv`](spanish-model-variant459-warp-attempts.csv)
records the full-image results. Both 459 linker symbol files gained
`GetTPage`, `GsGetActiveBuff`, `SetPolyGT4`, `SetSemiTrans` and
`SetShadeTex`.
