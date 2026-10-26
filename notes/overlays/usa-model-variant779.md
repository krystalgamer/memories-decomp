# USA MODEL headers 779 and 876

Seventy-one North American MODEL variant images share one entry, at `+0x4`
and `0x20DC` bytes long. No French or Spanish source reproduces it. The entry
is new North American C in
[`variant779_entry.c`](../../src/overlays/model_variant/variant779_entry.c),
written after the [header-756 entry](usa-model-variant756.md) and built under
`gcc_2_8_1_g0_split` like the French entries.
[`variant779_entry_slot1.c`](../../src/overlays/model_variant/variant779_entry_slot1.c)
covers the images loaded at `0x8017B000`.

| Header | Stage | Models |
|---:|---|---|
| 779 | `pos0_slot0` | 5, 24, 49, 53, 98, 120, 154, 157, 177, 196, 221, 473, 559, 577, 586, 588 |
| 779 | `stage9_slot0` | 49, 80, 86, 124, 210, 265, 273, 376, 387, 402, 474, 559, 570, 580, 609, 620, 634, 637 |
| 876 | `stage8_slot1` | 5, 24, 49, 53, 92, 98, 120, 136, 154, 157, 177, 196, 221, 473, 559, 577, 586, 588 |
| 876 | `stage10_slot1` | 49, 80, 86, 124, 210, 238, 265, 273, 376, 387, 402, 474, 559, 570, 580, 609, 620, 634, 637 |

All seventy-one rebuild byte-identical to retail.

## What the entry does

The setup command takes its row of the 0x2A-byte command table. It clears
four trail edges, gives every trail segment the row's trail colour, and points
up to 16 sparks and 16 flares in random directions at the row's speed. Each
flare also gets a star with the row's star colour and a unit scale for both
bursts. The setup then uploads three textures.

Each frame the entry lowers the slot's ambient level until the second burst
and raises it again afterwards. After the row's delay:

- two trails each follow two parts of the slot inside their own time window.
  Each trail records up to 64 positions as a ring and draws gouraud bands
  between them, layered by the row's layer count and spacing, that fade
  towards the tail. The first trail clears the green channel when the red
  one is spent, as retail does.
- from the row's burst time, a burst grows a few sparks and flares a frame,
  draws the sparks through eight frames of their texture and the flares
  through four, and draws a growing, fading four-point star over the sparks;
- from the row's second burst time, a second burst does the same, with its
  own frames and with its stars at the flares.

The entry returns 4 when the first burst starts, 1 when the second starts, 2
once the ambient level is back and the row's end time has passed, and 0
otherwise.

## Layout and bindings

The entry's `.rodata` is the unit scale vector `D_8013D0E0`, at `0x20E0`. The
rest of the image is one raw tail that starts at `0x20F0`. Inside it, the
layout names the three texture descriptors `D_8013D0F0` and the command table
`D_8013D144`. The `0x8017B000` images use the same offsets. The resident
bindings are in `config/slus_01411/overlays/variant779_linker_symbols.txt`.

## French images

French headers 796 and 893 hold the same entry. The French wrappers in
`src/overlays/french_model_variant/variant796_entry{,_slot1}.c` set
`MODEL_VARIANT779_CLUT_X` to 640, the PAL burst palette; see
[French MODEL entries reusing North American C](french-model-usa-entry-reuse.md).
