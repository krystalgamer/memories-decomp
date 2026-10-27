# Spanish MODEL438 warp rings

Helper `+0x1614..+0x1B5C` is matching C. It is 1,352 bytes in each of the
twelve MODEL438 loads of the Spanish release (MODEL147, MODEL211 and MODEL610
at stages 7/8; MODEL263, MODEL525 and MODEL632 at stages 9/10). The same twelve
French images carry the identical body at the same offsets. That makes 24
instances and 32,448 instruction bytes from one routine. `variant438_warp.c`
is compiled once per slot with `gcc_2_8_1_g0_split`. The slot-1 wrapper only
renames the function to `func_8017C614`. This is a new decompilation; no region
had accepted C for it.

## Behaviour

Five 0x1A8-byte rings, each with seventeen outer, inner and source points,
travel along the work block's direction vector (`+0x226C`) from its origin
(`+0x2258`). For each live ring, the helper:

- builds a `GsCOORDINATE2` at `origin + direction * size / 2048`, rotated to
  face the direction (yaw/pitch from `ratan2`);
- projects each source edge with `RotTransPers4` to get UVs that sample the
  framebuffer page not being drawn (left half x 320/0, right half x 448/0
  minus 128, chosen by `GsGetActiveBuff`);
- projects the outer/inner edge into the same `POLY_GT4` (`+0x2174`) and sorts
  it semi-transparent, with the inner and outer colours as a gradient.

A ring grows by `step << 6` (`+0x22A4`) per frame. Before phase 3
(`+0x22E8`) it wraps at 2048; after that it clamps there and stops drawing.
The routine is a sibling of the MODEL388 screen-feedback rings. The
[MODEL459 warp rings](spanish-model-variant459-warp.md) compile the same body
under `VARIANT438_WARP_LAYOUT_459`.

## What reproduces the target

- `tpage` is `u32`, which gives the target's `andi` on the stored page.
- `yaw = -yaw + 1024` (not `1024 - yaw`) keeps the target's operand order.
- Locals are declared `i, j`, then `buffer`, then `yaw, pitch`.
- A dead `s32 depth = 0;` sits between the `state` and `ring` declarations.
  Without it, cse2 swaps the state/ring register copies (cse.c's
  class-head copy swap) and two to six instructions differ.

All 24 complete images match. The ledger
[`spanish-model-variant438-warp-attempts.csv`](spanish-model-variant438-warp-attempts.csv)
records the full-image results. The Spanish 438 linker symbols gained
`GetTPage`, `GsGetActiveBuff`, `SetPolyGT4`, `SetSemiTrans` and
`SetShadeTex` at the same addresses the French file already binds.
