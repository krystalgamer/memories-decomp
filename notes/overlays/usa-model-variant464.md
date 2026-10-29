# USA MODEL headers 464 and 614

Four North American MODEL variant images share one module of three
functions: the entry at `+0x4` (`0xE38` bytes), a funnel renderer at `+0xE3C`
(`0x8CC` bytes) and a ring renderer at `+0x1708` (`0x640` bytes). The entry is
North American C in
[`variant464_entry.c`](../../src/overlays/model_variant/variant464_entry.c),
built under the `gcc_2_7_2_cdk_g0` profile. The ring renderer is the French
header-481 C, [`variant481_rings.c`](../../src/overlays/french_model_variant/variant481_rings.c),
renamed to its US address by
[`variant464_rings.c`](../../src/overlays/model_variant/variant464_rings.c).
The `_slot1.c` wrappers cover the images loaded at `0x8017B000`. The funnel
renderer remains generated assembly.

| Header | Stage | Models |
|---:|---|---|
| 464 | `pos0_slot0` | 248, 624 |
| 614 | `stage8_slot1` | 248, 624 |

All four rebuild byte-identical to retail.

## What the entry does

The setup command takes its row of the 0x3C-byte command table at
`D_8013CE0C + 0x1C`, aims at a point 450 units in front of or behind the
slot, uploads five textures and builds the primitives: two funnels of two
17-point circles, a band of 33 steps, a nine-ring dome, twelve spirals, two
sheets and two streamers.

Each frame the entry projects the part and its target, and after the row's
start time draws the rings and, until phase 6, the funnels. The growth
progress rises to 1024 between the row's two growth times. The ambient level
falls during phases below 6 and then follows the fade; the entry returns 4 in
phases 2 to 4, 1 on entering phase 7 and 2 in phase 8.

## Matching notes

After the funnel circles the entry clears each funnel's size behind a
`k == 1` test whose two arms both store zero. A single store schedules the
loop counter increment ahead of it; the split arms keep the retail order.
