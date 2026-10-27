# Spanish MODEL441 entry

Loader entry `+0x0004..+0x11BC` is matching C. It is 4,536 bytes in the ten
Spanish MODEL441-family images (MODEL166, 360, 487 and 590 stages 7/8;
MODEL709 stages 9/10) and the ten French images that carry the identical
body at the same offsets. `variant441_entry.c` is compiled once per slot with
`gcc_2_8_1_g0_split`. The slot-1 wrapper only renames the descriptor base
`D_8013F754` and the entry and helper symbols to their `0x8017....`
addresses. This is a new decompilation; no region had accepted C for it.
The helpers at `+0x2A48` and `+0x414C` remain assembly.

## Behaviour

A non-negative command initialises the work area. It reads the active slot,
selects the 48-byte record at descriptor `+0xFC`, resolves three slot parts,
places the target at `(0, -300, -450)` (slot 0) or `(0, -300, 450)`
(slot 1) and stores the direction from part 0. It packs six texture images
(CLUT x 640, y 244) and initialises the primitives. It then initialises:

- three hoops with a repeating eight-colour sequence;
- the tube's nine fixed colours;
- the webs (coloured from the record);
- the ribbon, the twelve glows and the two sheets;
- five curtains and four eight-spoke sets.

A negative command runs one frame. It derives the view angles. Until the
clock reaches the record's `end` word, it copies the opposite slot's
position into the target and rebuilds the direction from part 0. It then
projects part 0 and the target for the screen delta. For each of the
record's `count` parts it recomputes the direction from that part and calls:

- `+0x2A48` and `+0x414C` once the clock reaches the record start;
- the tube `+0x173C` from phase 1.

The framebuffer rings `+0x3060` run from phase 2. The tint goes to
`Model_SetSlotAmbient`, and phases 6..8 drive the fade and the return
codes.

The MODEL441 binding lists gain `GetClut`, `GsGetLwUnit`, `SetPolyFT4`,
`SetPolyG3`, `SetPolyG4`, `Square0` and `SquareRoot0`.

## What reproduces the target

- Hoop colours use `r`, `g` and `b` locals that live across the
  trigonometry calls. The tube colours use separate locals, so they stay in
  temporaries.
- The screen delta stores `vx` before computing `dy`.
- `work->index = 0` precedes the delta stores, and the per-part loop is
  `if (count != 0) do { ... } while (++index < count)`. A plain `for`
  either stores the index after the delta or reloads it for the entry
  test.
- `frame` is signed; both clock comparisons use `slt`.

The ledger `spanish-model-variant441-entry-attempts.csv` records the 20
exact full-image results.
