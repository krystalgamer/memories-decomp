# USA MODEL headers 634 and 701

Thirteen North American MODEL variant images share one entry, at `+0x4`
and `0x135C` bytes long. No French or Spanish source reproduces it. The entry
is new North American C in
[`variant634_entry.c`](../../src/overlays/model_variant/variant634_entry.c),
built under `gcc_2_8_1_g0_split` like the French entries.
[`variant634_entry_slot1.c`](../../src/overlays/model_variant/variant634_entry_slot1.c)
covers the images loaded at `0x8017B000`.

| Header | Stage | Models |
|---:|---|---|
| 634 | `pos0_slot0` | 256, 409, 605, 629 |
| 634 | `stage9_slot0` | 47, 476 |
| 701 | `stage8_slot1` | 48, 256, 409, 605, 629 |
| 701 | `stage10_slot1` | 47, 476 |

All thirteen rebuild byte-identical to retail.

## What the entry does

The entry belongs to the family of headers 627, 647 and 654: it clears a
work area, prints its size, and keeps its effects in the `0x4C`-byte burst
records of header 647. The setup command picks one of seven rows of the
`0xC`-byte command table, uploads two textures, and sets up eight, sixteen or
twenty rings and thirty-two sparks. Each ring gets the row's colour and its
own tilt; the sparks sit in a cluster around the front of the slot, up to
three half-radii out from its centre. The setup also fills two circles of
band vertices, at the radius and four times it, and four quad corners.

After the row's delay the entry opens one more ring a frame and starts every
spark on the first frame. Each ring is sixteen gouraud bands between the two
circles, shaded along a sine; it grows to six times its size over its
lifetime. The sparks are textured quads turned towards the camera, stepping
through four frames of the second texture. Both fade over the second half of
their lifetime.

The entry returns 4 while sparks are drawn and none has finished yet, 1 on
the frame the first spark finishes, 2 once everything has finished, and 0
otherwise.

## Layout and bindings

The entry's `.rodata` is the work-size format string and the jump table of
the setup's command switch, `0x34` bytes at `0x1360`. The rest of the image
is one raw tail that starts at `0x1394`. Inside it, the layout names the two
texture descriptors `D_8013C394` and the command table `D_8013C3CC`. The
`0x8017B000` images use the same offsets. The resident bindings are in
`config/slus_01411/overlays/variant634_linker_symbols.txt`.

## Source shape

As in header 654's entry, the quad corners' negations go through a
`static inline` with an `int` parameter, which keeps retail's three loads of
the radius and the zero-extension before the third negation.
