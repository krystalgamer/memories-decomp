# USA MODEL headers 673 and 740

Ten North American MODEL variant images share one entry, at `+0x4` and
`0x1B2C` bytes long. The entry is new North American C in
[`variant673_entry.c`](../../src/overlays/model_variant/variant673_entry.c),
built under `gcc_2_8_1_g0_split` (not the CDK GCC 2.7.2 profile, which leaves
a large register-allocation diff).
[`variant673_entry_slot1.c`](../../src/overlays/model_variant/variant673_entry_slot1.c)
covers the images loaded at `0x8017B000`.

| Header | Stage | Models |
|---:|---|---|
| 673 | `stage9_slot0` | 215, 276, 503, 515, 611 |
| 740 | `stage10_slot1` | 215, 276, 503, 515, 611 |

The French headers 690 and 757 carry the same entry without the `E_DATA`
work-size printf (`0x1B1C` bytes). Their
[`french_model_variant/variant690_entry.c`](../../src/overlays/french_model_variant/variant690_entry.c)
and `_slot1.c` wrappers define `VERSION_FRENCH` and rename the two data tables
`0x28` lower. All twenty images rebuild byte-identical to retail.

## Layout

| Range | Contents |
|---|---|
| `0x0000..0x0004` | module header |
| `0x0004..0x1B30` | entry (French `..0x1B20`) |
| `0x1B30..0x1B48` | `"E_DATA size = %x byte\n"` and one pad byte (US only) |
| `0x1B48..0x1B9C` | three `GsIMAGE` records (French `0x1B20`) |
| `0x1B9C..` | `0xC`-byte command rows (French `0x1B74`), then raw data |

The pad byte after the format string's terminator is `0x01` in slot 0 and
`0x08` in slot 1, so a compiled `.rodata` string (zero-padded by the linker
script's 4-byte alignment) cannot reproduce it. The string therefore stays in
the unclassified data tail and the entry prints through
`extern char D_8013CB30[]`; the instructions are the same `lui`/`addiu` pair a
literal would produce.

## What the entry does

Setup clears the `0x3A50`-byte work area, reads one command row (start delay,
radius, rise height, quad colour and fan colour), and packs the three
textures' page and CLUT. It fills 64 rising quads, 64 falling quads and 64
fans around the slot, a four-corner square of the row's radius, a 17-point
ring at three times that radius, and the fan's centre and corners. After the
delay, commands 0-3 release the records two or four frames apart for a
command-specific time; each frame they move along their growth vectors and
draw as flat textured quads or Gouraud fans. The entry returns 0 while
waiting, 4 while any record is still active, 1 on the first frame every record
is inactive and 2 on later frames.
