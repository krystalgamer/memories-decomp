# USA MODEL headers 752 and 849

Thirty North American MODEL variant images share one entry, at `+0x4` and
`0x197C` bytes long. No French or Spanish source reproduces it. It is a
sibling of the [header-754 entry](usa-model-variant754.md), and the new North
American C in
[`variant752_entry.c`](../../src/overlays/model_variant/variant752_entry.c)
follows that source. It is built under `gcc_2_8_1_g0_split` like the French
entries.
[`variant752_entry_slot1.c`](../../src/overlays/model_variant/variant752_entry_slot1.c)
covers the images loaded at `0x8017B000`.

| Header | Stage | Models |
|---:|---|---|
| 752 | `pos0_slot0` | 31, 80, 99, 104, 117, 570, 572, 616 |
| 752 | `stage9_slot0` | 5, 24, 46, 59, 359, 564, 610 |
| 849 | `stage8_slot1` | 31, 80, 99, 104, 247, 570, 572, 616 |
| 849 | `stage10_slot1` | 5, 24, 46, 59, 359, 564, 610 |

All thirty rebuild byte-identical to retail.

## What the entry does

The setup command takes its row of the 0x44-byte command table. Like header
754, it gives the particles random velocities, lays out a ring of 16
segments, copies the burst colour and uploads three textures. Each of the 64
sparkles also starts grey with a zero scale.

The camera presets, the ambient-level dip and the tint behave as in header
754, with three tint modes: all parts, parts 7 to 9, or parts 1 to 18. Once
the burst starts, a growing number of small four-point sparkles is drawn,
each growing and fading on its own. The ring, the four-point star, the embers
and the debris follow as in header 754. There is no camera shake and no
flash.

The entry returns 1 when the burst starts, 2 when the ambient level is back,
the last sparkle has faded out and so has the burst colour, and 0 otherwise.

## Layout and bindings

The entry's `.rodata` is the unit scale vector `D_8013C980`, at `0x1980`. The
rest of the image is one raw tail that starts at `0x1990`. Inside it, the
layout names the three texture descriptors `D_8013C990` and the command table
`D_8013C9E4`. The `0x8017B000` images use the same offsets. The resident
bindings are in `config/slus_01411/overlays/variant752_linker_symbols.txt`.
