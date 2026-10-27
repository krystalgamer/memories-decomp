# Spanish MODEL417 entry

Loader entry `+0x0004..+0x1204` is matching C. It is 4,608 bytes in the
eight Spanish MODEL417-family images (MODEL134 and 232 stages 9/10, MODEL354
stages 7/8, MODEL535 stages 9/10) and the eight French images that carry the
identical body at the same offsets. `variant417_entry.c` is compiled once per
slot with `gcc_2_8_1_g0_split`. The slot-1 wrapper only renames the
descriptor base `D_8013F3A0` and the entry and helper symbols to their
`0x8017....` addresses. This is a new decompilation; no region had accepted
C for it. With it, every function in these sixteen images is C.

## Behaviour

The body is the MODEL441-family entry without
the hoops, so the work area is 0x2188 bytes and starts with the tube.

A non-negative command initialises the work area. It reads the active slot,
selects the 68-byte record at descriptor `+0xFC`, resolves three slot parts,
places the target at `(0, -300, -450)` (slot 0) or `(0, -300, 450)`
(slot 1) and stores the direction from part 0. It packs five texture images
(CLUT x 640, y 244), then initialises the primitives, the tube, ribbon,
twelve glows, two sheets, five curtains and four spoke sets. The sheet
corners are `±size`, where `size` is the record halfword at `+0x30`.

A negative command runs one frame. It derives the view angles, copies the
opposite slot's position into the target and rebuilds the direction from
part 0. Before each direction update it applies the record's offset vector
at `+0x38` to the part translation: slot 0 adds all three components, slot 1
subtracts x and z and adds y. It then projects part 0 and the target for the
screen delta. For each of the record's `count` parts it recomputes the
direction and calls:

- the lines `+0x1204` and sheets `+0x2ACC` once the clock reaches the
  record start;
- the tube `+0x1778` from phase 1.

The spiral `+0x3750` runs from phase 2. The tint goes to
`Model_SetSlotAmbient`, and phases 6..8 drive the fade and the return
codes.

The MODEL417 binding lists gain `GetClut`, `GetTPage`, `GsGetLwUnit`,
`SetPolyFT4`, `SetPolyG3`, `SetPolyG4`, `SetPolyGT4`, `SetSemiTrans`,
`SetShadeTex`, `Square0` and `SquareRoot0`.

## What reproduces the target

The MODEL441 entry's choices carry over unchanged. The differences are:

- each sheet corner reloads `work->config->size`, as the target does;
- the curtain `b` ring uses `* 89 >> 10` and the `c` ring `* 3 >> 5`;
- the offset vector is applied inline in both direction updates.

The ledger `spanish-model-variant417-entry-attempts.csv` records the 16
exact full-image results.
