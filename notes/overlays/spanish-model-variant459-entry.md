# Spanish MODEL459 entry

Loader entry `+0x0004..+0x13E0` is matching C. It is 5,084 bytes in the
twelve Spanish MODEL459-family images (MODEL47, 231, 238, 411, 417 and 620,
stages 7..10) and the eleven French images that carry the identical body at
the same offsets. `variant459_entry.c` is compiled once per slot with
`gcc_2_8_1_g0_split`; the slot-1 wrapper only renames the descriptor base
`D_8013F548` and the entry and helper symbols to their `0x8017....`
addresses. This is a new decompilation; no region had accepted C for it. The
raw `sles_03947`/`sles_03949` MODEL inventory images contain the same body
but are generated inventories and are not wired here.

## Behaviour

A non-negative command initialises the work area: it reads the active slot,
selects the 72-byte record at descriptor `+0xFC`, resolves six slot parts and
places the target at `(0, -300, -450)` (slot 0) or `(0, -300, 450)`
(slot 1). Mode 1 transforms three part vertices into the points; otherwise
the direction from part 0 to the target is stored. It then packs five
texture images (CLUT x 640, y 244) and initialises the triangle, quad,
textured, flat, ring, curtain, band, sheet, web and glow primitives.

A negative command runs one frame. It derives the view angles with `Square0`,
`SquareRoot0` and `ratan2`, rebuilds the points and directions, projects the
target with `RotTransPers` for the screen delta, and calls the helpers:
`+0x3A24` once the clock reaches the record start, `+0x19AC` when curtains
are enabled, `+0x3FE0` and `+0x3224` from phase 1, and `+0x2604` from phase
2. It then passes the tint to `func_800595C8` and advances phases 5..8.

The MODEL459 binding lists gain `GetClut`, `SetPolyG3`, `SetPolyFT4`,
`SetPolyG4`, `SquareRoot0`, `Square0` and `GsGetLwUnit`.

## What reproduces the target

- `ring` is declared before `curtain`.
- Every texture pack result is assigned back to `packed`; the CLUT copies are
  plain `u16` copies. Leaving `packed` dead lets CSE propagate the copy and
  the shift floats to its use.
- The band and glow outer loops use `i`, the sheet inner loop a fresh `k`,
  and the web column loop `j`; swapping these flips `s2`/`s3`.
- The point copy in the non-runtime branch is written as 18 statements.
- Runtime points and directions use `setVector`, and the screen delta goes
  through `s32 dx, dy` locals.

The ledger `spanish-model-variant459-entry-attempts.csv` records the 23 exact
full-image results.
