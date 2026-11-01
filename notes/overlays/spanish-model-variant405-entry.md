# Spanish MODEL405 entry

Loader entry `+0x0004..+0x0D00` is matching C. It is 3,324 bytes in the
eight Spanish MODEL405-family images (MODEL149 and MODEL419 stages 7/8;
MODEL57 and MODEL562 stages 9/10) and the eight French images that carry the
identical body at the same offsets. `variant405_entry.c` is compiled once
per slot with `gcc_2_8_1_g0_split`. Both slots compile it directly
(see [Names](#names)). This is a new decompilation; no region had accepted
C for it. The `model_variant/variant405_entry.c` source is an unrelated
North American body with the same header number.
per slot with `gcc_2_8_1_g0_split`. The slot-1 wrapper only renames the
descriptor base `D_8013CFBC` and the entry and helper symbols to their
`0x8017....` addresses. This is a new decompilation; no region had accepted
C for it. The North American header-405 entry is an unrelated body with the
same header number (`french_model_variant/variant422_entry.c`).

## Behaviour

A non-negative command initialises the 0x326C-byte work area. It reads the
active slot, selects the 32-byte record at descriptor `+0x38` and resolves
part 0. It places the target at `(0, -300, -450)` (slot 0) or
`(0, -300, 450)` (slot 1), reads part 0's matrix and stores the direction.

It packs one texture image and initialises the primitives: two GT4s on
page `(1, 1, 896, 0)` with CLUT `(640, 244)`, two FT4s and sixteen glyph
GT4s that step the packed page down in 16-pixel rows. It then initialises:

- three rings;
- a staggered angle in each of three 0xA1C-byte trail records at `+0xF0C`;
- six sheets with `±512` corners;
- two sets of three 17-point curtains (radii `>> 4`/`* 3 >> 5` and
  `>> 3`/`>> 2`);
- four eight-point spokes and two ribbons.

A negative command runs one frame. It derives the view angles. Before
record word `+0x1C` it copies the opposite slot's position into the target
and reads part 0's matrix. Until word `+0x10` it also refreshes the aim
origin from that matrix; the direction is always target minus origin.
It projects part 0 and the target for the screen delta and calls the outer
bands `+0x1B64`, the `+0x12CC` helper and the pulses `+0xD00`.

The tint goes to `Model_SetSlotAmbient`. Record words `+0x10`, `+0x14` and
`+0x18` start phase 1 and select the draw mode (1, 2, then record `+0x0C`).
Phases 2..6 drive the fade and the return codes.

The MODEL405 binding lists gain `GetClut`, `GetTPage`, `GsGetLwUnit`,
`RotTransPers`, `SetPolyFT4`, `SetPolyG3`, `SetPolyG4`, `SetPolyGT4`,
`SetSemiTrans`, `SetShadeTex`, `Square0`, `SquareRoot0` and `ratan2`.

## Names

Both slots compile the shared sources without renaming wrappers. The images
define `SpanishModelVariant405_HandleRequest`, `SpanishModelVariant405_DrawPulses`
(`0xD00`), `FrenchModelVariant562_DrawShards` (`0x12CC`),
`SpanishModelVariant405_DrawBands` (`0x1774`),
`SpanishModelVariant405_DrawOuterBands` (`0x1B64`) and the descriptor base
`gSpanishModelVariant405_D1FBC`. The US header-388 images build the same
sources with the palette column 512 instead of 640; there and in the
Japanese images of the same models the shards,
bands, outer bands and data sit 4, 0x10, 0x48 and 0x8C bytes higher. Each image binds the names at its own addresses (see
[Per-image symbol bindings](model-variants.md#per-image-symbol-bindings)).

## What reproduces the target

- The FT4 and glyph CLUT lives in a 64-byte stack record at `sp + 0x4C`
  (`Variant405EntryStack`), as in the header-415 entry.
- The glyph counters are zeroed and the glyph pointer is set right after
  the init `GsGetLwUnit` call. That keeps the zero in the counter's register
  for the `GetTPage` and `SetShadeTex` arguments.
- The glyph, sheet-corner and spoke counters are three separate variables,
  and the curtains use their own angle variable. Sharing them moves the
  flat pointer, the glyph row and the curtain angle into other registers.
- The ring's `+0x48` byte is cleared after the three scale words, and the
  spoke angle step comes before the counter step.

The ledger `spanish-model-variant405-entry-attempts.csv` records the 16
exact full-image results.
