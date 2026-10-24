# USA MODEL headers 119 and 249

Twenty North American MODEL variant images share one entry, at `+0x4` and
`0xA18` bytes long. No French or Spanish source reproduces it. The entry is
new North American C in
[`variant119_entry.c`](../../src/overlays/model_variant/variant119_entry.c),
built under `gcc_2_8_1_g0_split` like the French entries.
[`variant119_entry_slot1.c`](../../src/overlays/model_variant/variant119_entry_slot1.c)
covers the images loaded at `0x8017B000`.

| Header | Stage | Models |
|---:|---|---|
| 119 | `pos0_slot0` | 3, 25, 369, 705 |
| 119 | `stage9_slot0` | 16, 17, 96, 155, 436, 717 |
| 249 | `stage8_slot1` | 3, 25, 369, 705 |
| 249 | `stage10_slot1` | 16, 17, 96, 155, 436, 717 |

All twenty rebuild byte-identical to retail.

## What the entry does

The setup command takes its row of the 0x1A-byte command table and builds one
textured blade, a quad whose outer edge is tilted by the row's depth on the
active slot's side. It uploads one texture and sets the camera step counter to
two. Commands from 100 up also set a tint flag.

After the row's delay, the entry draws the row's number of waves, each starting
a fixed spacing after the previous one. A wave is 16 copies of the blade
turned around the camera part, or around a fixed point when the part is 0xFF.
It grows from nothing to full size and fades out over the row's fade time. A
full-screen flash fades over the same time when the first wave starts. With
the tint flag set, the entry also queues a tint for every part of the slot on
two frames out of three during the first half of the slot animation window.

Each time the slot animation passes the middle of that window, the entry takes
one camera step. The first two steps set an orbit preset and restart both slot
animations and the wave timer. After those, each pass asks for a camera move
to the slots (`Model_RequestCameraMoveToSlots`).
The entry returns 1 when the first wave starts, 2 when the last one is over,
and 0 otherwise.

One local, a three-vector `SVECTOR` array, is never used. Retail's stack frame
has room for it, and without it the other locals move.

## Layout and bindings

The entry has no `.rodata`. The rest of the image is one raw tail that starts
at `0xA1C`. Inside it, the layout names the unit scale vector `D_8013BA1C`,
the texture descriptor `D_8013BA2C` and the command table `D_8013BA48`. The
`0x8017B000` images use the same offsets. The resident bindings are in
`config/slus_01411/overlays/variant119_linker_symbols.txt`.
