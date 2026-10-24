# USA MODEL headers 118 and 248

Twenty-five North American MODEL variant images share one entry, at `+0x4` and
`0x1040` bytes long. No French or Spanish source reproduces it. The entry is
new North American C in
[`variant118_entry.c`](../../src/overlays/model_variant/variant118_entry.c),
built under `gcc_2_8_1_g0_split` like the French entries.
[`variant118_entry_slot1.c`](../../src/overlays/model_variant/variant118_entry_slot1.c)
covers the images loaded at `0x8017B000`.

| Header | Stage | Models |
|---:|---|---|
| 118 | `pos0_slot0` | 16, 17, 59, 358, 378, 452, 484, 564 |
| 118 | `stage9_slot0` | 51, 390, 510, 511 |
| 248 | `stage8_slot1` | 16, 17, 59, 224, 358, 378, 452, 484, 564 |
| 248 | `stage10_slot1` | 51, 390, 510, 511 |

All twenty-five rebuild byte-identical to retail.

## What the entry does

The setup command takes its row of the 0x20-byte command table. It clears the
three trail positions, gives each of 64 fan triangles a random scale, and puts
64 dots at random points of a shell around the origin, at the row's radius plus
a random amount up to half its height. It uploads two textures and sets the camera step
counter to two. Commands from 100 up also set a tint flag.

After the row's delay, the entry runs three phases from the same start time:

- Three textured quads follow the camera part's last three positions, each
  starting two frames after the one before.
- Two rings of 64 shaded triangles fan out from a point in front of the active
  slot and fade over the fan time.
- The 64 dots grow from that same point as 2x2 boxes and fade over the box time.

With the tint flag set, the entry also queues a tint for every part of the slot
on two frames out of three during the first half of the slot animation window.

Each time the slot animation passes the middle of that window, the entry takes
one camera step. The first two steps set an orbit preset and restart both slot
animations and the timer. After those, each pass asks for a camera move to the
slots (`Model_RequestCameraMoveToSlots`).
The entry returns 1 when the phases start, 2 when the box phase is over, and 0
otherwise.

The box loop is a `goto` loop. With a `for` loop, gcc's loop pass turns the
depth and flag reads into a moving pointer, and retail recomputes each address
from the counter.

## Layout and bindings

The entry has no `.rodata`. The rest of the image is one raw tail that starts
at `0x1044`. Inside it, the layout names the unit scale vector `D_8013C044`,
the two texture descriptors `D_8013C054` and the command table `D_8013C08C`.
The `0x8017B000` images use the same offsets. The resident bindings are in
`config/slus_01411/overlays/variant118_linker_symbols.txt`.
