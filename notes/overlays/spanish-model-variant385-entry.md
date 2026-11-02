# Spanish MODEL385 entry

Loader entry `+0x0004..+0x0F5C` is matching C. It is 3,928 bytes in the
eight Spanish MODEL385-family images (MODEL170 stages 7/8; MODEL406, 407 and
513 stages 9/10) and the eight French images that carry the identical body
at the same offsets. `variant385_entry.c`
(`SpanishModelVariant385_HandleRequest`) is compiled once per slot with
`gcc_2_8_1_g0_split`; each image's symbols file binds the entry, the helpers
and the descriptor base (`gSpanishModelVariant385_D2EB4`) at its own
addresses. This is a new decompilation; no region had accepted C for it.
With it, every function in these sixteen images is C.

## Behaviour

A non-negative command initialises the 0x1048-byte work area. It reads the
active slot, selects the 76-byte record at descriptor `+0xFC` and resolves
three slot parts. It reads part 0's matrix, places the target at
`(0, -300, -450)` (slot 0) or `(0, -300, 450)` (slot 1), and applies the
record's offset vector at `+0x40` to the translation. Slot 0 adds x and y and
subtracts z; slot 1 adds all three. It then stores the direction.

It packs five texture images (CLUT x 640, y 244) and initialises the
primitives. That includes a GT4 that uses the third pack's page and CLUT
with UVs `(128..255, 128..207)`. It then initialises:

- three grey webs;
- one tube;
- two sheets whose corners are `±size` (record `+0x28`);
- four 17-point curtain rings with radii from record `+0x14` and `+0x18`.

A negative command runs one frame. It derives the view angles, reads part
0's matrix, copies the opposite slot's position into the target and applies
the offset vector. Here slot 0 subtracts x and z and adds y. It then rebuilds
the direction and projects part 0 and the target for the screen delta. For
each of the record's `count` parts it reads that part's matrix (only part 0
gets the offset), recomputes the direction, and calls:

- the pulse `+0x21B8` once the unsigned clock reaches the record start
  (`+0x2C`);
- the ribbons `+0x1B00` and strip `+0x1478` from phase 1.

After the loop, once the start is reached, it calls the rings `+0x26B4`,
and also the lines `+0xF5C` while the phase is still 0. The tint goes to
`Model_SetSlotAmbient`, and phases 6..8 drive the fade and the return codes.

The MODEL385 binding lists gain `GetClut`, `GetTPage`, `GsGetLwUnit`,
`RotTransPers`, `SetPolyFT4`, `SetPolyG3`, `SetPolyG4`, `SetPolyGT4`,
`SetSemiTrans`, `SetShadeTex`, `Square0` and `SquareRoot0`.

## What reproduces the target

- The curtain angle is a loop variable: `for (j = 0, angle = 0; j < 17;
  j++, angle = j * 256)`. Computing `j * 256` at the top of the body moves
  the shift.
- Each sheet corner iteration reloads `work->config->size` once.
- The ribbons and strip calls share the `phase > 0` block. The instruction
  shape is the same with the strip outside it, but the branch offset then
  differs, which only the full-image verification shows.
- `frame` is unsigned; both start comparisons use `sltu`.

The ledger `spanish-model-variant385-entry-attempts.csv` records the 16
exact full-image results.
