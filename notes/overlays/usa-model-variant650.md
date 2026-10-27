# USA MODEL headers 650 and 717

Ten North American MODEL variant images share one entry, at `+0x4` and
`0x1B04` bytes long. The entry is new North American C in
[`variant650_entry.c`](../../src/overlays/model_variant/variant650_entry.c),
built under `gcc_2_8_1_g0_split` (the CDK GCC 2.7.2 profile leaves a large
register-allocation diff).
[`variant650_entry_slot1.c`](../../src/overlays/model_variant/variant650_entry_slot1.c)
covers the images loaded at `0x8017B000`.

| Header | Stage | Models |
|---:|---|---|
| 650 | `stage7_slot0` | 237, 416, 417, 619 |
| 650 | `stage9_slot0` | 518 |
| 717 | `stage8_slot1` | 237, 416, 417, 619 |
| 717 | `stage10_slot1` | 518 |

The French headers 667 and 734 carry the same entry without the `E_DATA`
work-size printf (`0x1AF4` bytes). Their
[`french_model_variant/variant667_entry.c`](../../src/overlays/french_model_variant/variant667_entry.c)
and `_slot1.c` wrappers define `VERSION_FRENCH` and rename the two data tables
`0x28` lower. All twenty images rebuild byte-identical to retail.

## Layout

| Range | Contents |
|---|---|
| `0x0000..0x0004` | module header |
| `0x0004..0x1B08` | entry (French `..0x1AF8`) |
| `0x1B08..0x1B20` | `"E_DATA size = %x byte\n"` as compiled `.rodata` (US only) |
| `0x1B20..0x1B58` | two `GsIMAGE` records (French `0x1AF8`) |
| `0x1B58..` | `0xC`-byte command rows (French `0x1B30`), then raw data |

Unlike header 673, the bytes after the format string's terminator are zero in
both slots, so the literal compiles into the module's own `.rodata`.

## What the entry does

Setup clears the `0x1D20`-byte work area, reads one command row (start delay,
radius, quad colour and sphere/fan colour), and packs the textures' page and
CLUT. It places 8 sideways and 8 vertical arc records, up to 8 spinning quads,
4 spheres and 64 fans around the slot, a four-corner square of half the
radius, a 17-by-9 sphere grid of ten times the radius and a 17-point ring of
fifteen times it. After the delay, the command picks the quads' starting
model-slot data entry (command 2 switches entries once 0x87 frames have
passed and opens half of the spheres and fans at once, the rest later). Each
frame the active records grow, rotate, fade by their remaining time and draw
as flat textured quads or Gouraud fans. The entry returns 0 while waiting, 4
while any sphere is still active, 1 on the first frame every record is
inactive (command 2 also waits for 0x8D frames) and 2 on later frames.
