# Spanish MODEL388 entry

Loader entry `+0x0004..+0x0CD4` is matching C. It is 3,280 bytes in ten
Spanish images (MODEL8 stages 7/8; MODEL43, MODEL235, MODEL269 and MODEL706
stages 9/10) and the eight French images that carry the identical body
(the same set without MODEL269). `variant388_entry.c` is compiled once per
slot with `gcc_2_8_1_g0_split`. The slot-1 wrapper only renames the
descriptor base `D_8013DE7C` and the entry and helper symbols to their
`0x8017....` addresses. This is a new decompilation; no region had accepted
C for it.

## Behaviour

A non-negative command initialises the 0x18B0-byte work area. It reads the
active slot, selects the 36-byte record at descriptor `+0xFC` and resolves
parts 0..2. It places the target at `(0, -300, -450)` (slot 0) or
`(0, -300, 450)` (slot 1), reads part 0's matrix and stores the direction.

It packs five texture images and initialises the same primitives as the
header-385 entry: a G3, a G4, two GT4s on page `(1, 1, 896, 0)` with CLUT
`(640, 244)`, two FT4s and two more GT4s. It then initialises:

- one tube record with unit scales and colour `(255, 0, 255)`;
- twelve petal arms at `+0x1050`, coloured on the first point only;
- two sheets at `+0xF8` with `±128` corners;
- five feedback groups at `+0x5C8`: three 17-point rows (radii `>> 4`,
  `* 3 >> 5` and `* 7 >> 6`) and progress `-(n * 2048 / 5)`;
- four eight-point spokes at `+0xE10` (`>> 4` and `* 5 >> 6`), each
  starting at `n * 256`.

A negative command runs one frame. It derives the view angles and copies the
opposite slot's position into the target. Before record word `+0x1C` it
reads part 0's matrix; the direction is always target minus origin. It
projects part 0 and the target for the screen delta.

For each of the record's `+0x0C` parts it then:

- reads that part's matrix, again only before word `+0x1C`, and refreshes
  the direction;
- calls the petals `+0x240C`;
- from word `+0x10` on, calls the sheets `+0x19A4`;
- in phases above 0, calls the ribbons `+0x1344` and the strips `+0xCD4`.

From phase 2 on it calls the feedback helper `+0x1E9C`. Tint, fade and
return codes follow the header-385 entry: phase 6 returns 1, phase 7 fades
in and phase 8 returns 2.

The MODEL388 binding lists gain `GetClut`, `GsGetLwUnit`, `SetPolyFT4`,
`SetPolyG3`, `SetPolyG4`, `Square0` and `SquareRoot0`.

## What reproduces the target

- Colours are written as byte stores; `setColor` would be a call here.
- The sheet corner loop has its own counter. The spoke loop reuses the
  arm/feedback counters with the outer and inner roles swapped.
- The part loop is a `for` loop that zeroes the stored index. It must come
  directly before the loop test, so the duplicated test still knows the
  index is 0 and keeps the descriptor pointer in one register.
- The projected screen words are scalar `PSXLONG`s read as
  `(u16)sxy` and `(s16)(sxy >> 16)`. As scalar frame loads they do not
  alias the work area, so the index store is scheduled ahead of them.

The ledger `spanish-model-variant388-entry-attempts.csv` records the 18
exact full-image results.
