# Spanish MODEL479 entry

Loader entry `+0x0004..+0x0FD0` is matching C. It is 4,044 bytes in both
header-479/629 MODEL loads (MODEL379, stages 7/8) of the Spanish release and in
the same two French images, which carry the identical body at the same offsets.
`variant479_entry.c` is compiled once per slot with `gcc_2_8_1_g0_split`; the
slot-1 wrapper only renames the descriptor base `D_8013ED20` and the entry and
helper symbols to their `0x8017....` addresses. This is a new decompilation;
no region had accepted C for it.

## Behaviour

A non-negative command initialises the work area: it reads the active slot,
selects the 44-byte timing record at descriptor `+0xFC`, resolves three slot
parts, places the target at `(0, -300, -450)` (slot 0) or `(0, -300, 450)`
(slot 1), and packs the texture pages for the frame, flat and strip
primitives (CLUT x 640, y 244 in every image).

A negative command runs one frame. It derives the view angles from the camera
buffer with `Square0`, `SquareRoot0` and `ratan2`, copies the other slot's
position into the target, rebuilds five points on a 512-unit ring around it,
and calls the helpers by phase: curtains `+0x3860` and the assembly helper
`+0x0FD0` in phases 3..5, ribs `+0x1F68` in 2..4, spiral `+0x14F8` in
1..4 and sheets `+0x2B90` once the clock reaches the record start before phase 5. It then passes
the tint to `func_800595C8` and advances phases 6..8.

## What reproduces the target

- `POLY_GT4 *textured` is declared directly after the spiral-arm pointer, so
  its pseudo number lets global allocation keep it in `s7`.
- `frame_page = packed >> 16` is assigned before `frame_texture = packed`.
- The point loop uses its own counter `k`; reusing `i` moves `s0`.

The ledger `spanish-model-variant479-entry-attempts.csv` records the four
exact full-image results.
