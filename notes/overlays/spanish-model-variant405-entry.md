# Spanish MODEL405 entry

Loader entry `+0x0004..+0x0D00` is matching C. It is 3,324 bytes in the
eight Spanish MODEL405-family images (MODEL149 and MODEL419 stages 7/8;
MODEL57 and MODEL562 stages 9/10) and the eight French images that carry the
identical body at the same offsets. Both slots compile the shared
`spanish_model_variant/variant405_entry.c` directly with
`gcc_2_8_1_g0_split`; there is no slot wrapper. Each image binds the entry,
helper and descriptor-base names at its own addresses in its
`*_symbols.txt` (see [Names](#names)). This is a new decompilation; no region
had accepted C for it. The North American header-405 entry is an unrelated
body with the same header number (`french_model_variant/variant422_entry.c`).

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
`track_end` (`+0x1C`) it copies the opposite slot's position into the target
and reads part 0's matrix. Until `phase_start` (`+0x10`) it also refreshes the
aim origin from that matrix; the direction is always target minus origin.
It projects part 0 and the target for the screen delta and calls the outer
bands `+0x1B64`, the `+0x12CC` helper and the pulses `+0xD00`.

The tint goes to `Model_SetSlotAmbient`. `phase_start`, `second_pulse_start`
and `all_pulses_start` start phase 1 and set the work block's `pulse_count`
to 1, 2 and then the record's `count`. Phases 2..6 drive the fade and the
return codes.

### Config record fields

The 32-byte `Variant405EntryConfig` record and one work member had
offset-only names. Each name below follows how the matching C reads the
member; the layout and types did not change.

| Offset | Former name | Name | Evidence |
| --- | --- | --- | --- |
| `+0x0C` | `field_0C` | `count` | Copied into `pulse_count` at `all_pulses_start`. The bands and outer bands read the same word as `Bands405Descriptor.count`, their band-loop bound. |
| `+0x10` | `field_10` | `phase_start` | At this frame phase 0 becomes 1 and `pulse_count` becomes 1. Until it, the aim origin follows part 0's matrix. Same role as header 456's `phase_start`. |
| `+0x14` | `field_14` | `second_pulse_start` | At this frame `pulse_count` becomes 2. |
| `+0x18` | `field_18` | `all_pulses_start` | At this frame `pulse_count` becomes the record's `count`. |
| `+0x1C` | `field_1C` | `track_end` | While the frame is below it, the target copies the opposite slot and part 0's matrix is refreshed. Same role as `track_end` in headers 388, 430 and 456. |
| work `+0x322C` | `field_322C` | `pulse_count` | The pulses (`+0xD00`) read this word as `Pulse405State.count` and draw `count * 2` groups. |

The record bytes `+0x00..+0x0B` (part index and padding) and the trailing
placeholders of the other Spanish config records are never read by matching
C, so they keep their placeholder names.

The MODEL405 binding lists gain `GetClut`, `GetTPage`, `GsGetLwUnit`,
`RotTransPers`, `SetPolyFT4`, `SetPolyG3`, `SetPolyG4`, `SetPolyGT4`,
`SetSemiTrans`, `SetShadeTex`, `Square0`, `SquareRoot0` and `ratan2`.

## Names

Both slots compile the shared sources without renaming wrappers. The images
define `SpanishModelVariant405_HandleRequest`, `SpanishModelVariant405_DrawPulses`
(`0xD00`), `FrenchModelVariant562_DrawShards` (`0x12CC`),
`SpanishModelVariant405_DrawBands` (`0x1774`),
`SpanishModelVariant405_DrawOuterBands` (`0x1B64`) and the descriptor tables
`gSpanishModelVariant405_aImages` and `gSpanishModelVariant405_aConfigs`. The US header-388 images build the same
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
