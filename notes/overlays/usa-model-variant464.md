# USA MODEL headers 464 and 614

Four North American MODEL variant images share one module of three
functions: the entry at `+0x4` (`0xE38` bytes), a band renderer at `+0xE3C`
(`0x8CC` bytes) and a ring renderer at `+0x1708` (`0x640` bytes). The entry is
North American C in
[`variant464_entry.c`](../../src/overlays/model_variant/variant464_entry.c),
built under the `gcc_2_7_2_cdk_g0` profile. The ring renderer is the French
header-481 C, [`variant481_rings.c`](../../src/overlays/french_model_variant/variant481_rings.c),
and the band renderer is the French header-481 C,
[`variant481_band.c`](../../src/overlays/french_model_variant/variant481_band.c)
(see [the band note](french-model-variant481-band.md)); the US images build
both sources directly. The same sources build both slots: the shared C
defines `ModelVariant464_HandleRequest`, `FrenchModelVariant481_DrawBand`,
`FrenchModelVariant481_DrawRings` and `gModelVariant464_D1E0C`, and each
image's symbols file binds those names at its own addresses.

| Header | Stage | Models |
|---:|---|---|
| 464 | `pos0_slot0` | 248, 624 |
| 614 | `stage8_slot1` | 248, 624 |

All four rebuild byte-identical to retail.

## French headers 481 and 631

The French images at the same sectors build the same entry under
`gcc_2_8_1_g0_split` through
[`variant481_entry.c`](../../src/overlays/french_model_variant/variant481_entry.c)
in both slots. `VERSION_FRENCH` selects the palette column 640
instead of 512; the code is otherwise the same apart from resident `jal`
targets. The French band renderer is `0x8E0` bytes at `+0xE38` and builds from
`variant481_band.c`, so the rings sit at `+0x1718` and the data label `gModelVariant464_D1E0C`
at `0x8013CE14`.
The four French modules take the US names (`french_model_variant_624_pos0_slot0`,
`_248_pos0_slot0`, `_624_stage8_slot1`, `_248_stage8_slot1`) and all four
rebuild byte-identical to retail.

## What the entry does

The setup command takes its row of the 0x3C-byte command table at
`gModelVariant464_D1E0C + 0x1C`, aims at a point 450 units in front of or behind the
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
