# Spanish MODEL438 entry

Loader entry `+0x0004..+0x1614` is matching C. It is 5,648 bytes in the
twelve Spanish MODEL438-family images (MODEL147, 211 and 610 stages 7/8;
MODEL263, 525 and 632 stages 9/10) and the twelve French images that carry
the identical body at the same offsets. `variant438_entry.c` is compiled
once per slot with `gcc_2_8_1_g0_split`. The slot-1 wrapper only renames the
descriptor base `D_8013F868` and the entry and helper symbols to their
`0x8017....` addresses. This is a new decompilation; no region had accepted
C for it. With the entry in C, every game-owned function in these images is
now C.

## Behaviour

A non-negative command initialises the work area. It reads the active slot,
selects the 72-byte record at descriptor `+0xFC`, resolves three slot parts,
and places the target at `(0, -300, -450)` (slot 0) or `(0, -300, 450)`
(slot 1). It packs five texture images (CLUT x 640, y 244), then
initialises curtains, ring, band, star, sheets, webs, spokes, glows and
petals. Curtain, ring and spoke geometry scales with the record's `size`,
and the sheets scale with its `sheet_size`.

A negative command runs one frame. It copies the opposite slot's position
into the target, derives the view angles, and projects the target for the
screen delta. It then offsets the light-space translation by the record's
`offset` (its sign depends on the slot) and recomputes the direction. It
calls the helpers:

- `+0x3DE8` once the clock reaches the record start;
- `+0x1B5C` every frame;
- `+0x4300`, `+0x3638` and `+0x1614` from phase 1;
- `+0x2A34` from phase 2.

The tint goes to `Model_SetSlotAmbient`.

The MODEL438 binding lists gain `GetClut`, `GsGetLwUnit`, `SetPolyFT4`,
`SetPolyG3`, `SetPolyG4`, `Square0` and `SquareRoot0`.

## What reproduces the target

- `ray` is declared before `plain` in the work area.
- The spoke loop restarts from the base angle each step
  (`angle = ang + n * 512`), while the petal loop accumulates
  `angle += 512`; using a separate spin local rotates `s3`/`s5`.
- `spoke->angle` is stored before `field_8C`, which fixes the giv base
  register.
- The slot-specific `Model_CopySlotPosition` call is an `if`/`else` with a
  constant slot argument.

The ledger `spanish-model-variant438-entry-attempts.csv` records the 24
exact full-image results.
