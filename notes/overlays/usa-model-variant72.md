# USA MODEL headers 72 and 202

Twenty-five North American MODEL variant images share one entry, at `+0x4`
and `0x1D64` bytes long. No French or Spanish source reproduces it. The entry
is new North American C in
[`variant72_entry.c`](../../src/overlays/model_variant/variant72_entry.c),
built under `gcc_2_8_1_g0_split` like the French entries.
[`variant72_entry_slot1.c`](../../src/overlays/model_variant/variant72_entry_slot1.c)
covers the images loaded at `0x8017B000`.

| Header | Stage | Models |
|---:|---|---|
| 72 | `pos0_slot0` | 30, 137, 233, 234, 617 |
| 72 | `stage9_slot0` | 9, 45, 104, 176, 186, 572, 592, 595 |
| 202 | `stage8_slot1` | 30, 137, 233, 234, 617 |
| 202 | `stage10_slot1` | 45, 104, 176, 186, 572, 592, 595 |

All twenty-five rebuild byte-identical to retail.

## What the entry does

The setup command picks a `0x32`-byte row of the command table by
`command % 100`; `command / 100` switches the sparks on. The setup clears a
grid of up to 192 points, shades one colour per grid row from the row's
start colour to its end colour, builds the fan's inner ring and a jagged
outer ring, scatters 48 spark offsets, uploads the texture and sets the clock
to minus twice the lead time.

While the clock is negative the entry only records where the row's eight
parts are, into every grid row. After that it records the parts once a frame
into the next row, so the grid holds the last few frames of their path, and
draws it as a ribbon of gouraud quads that fades towards the last column.
From the row's start frame two fans of triangles open in front of the slot
and fade, a sprite plays eight frames of the texture as it shrinks, and the
sparks, when they are on, fly out and fade.

The entry returns 0 until the start frame, 1 once on the first frame after
it, 2 after the row's end frame, and 0 otherwise.

## Layout and bindings

The entry's `.rodata` is the unit scale vector, `0x10` bytes at `0x1D68`.
The rest of the image is one raw tail that starts at `0x1D78`. Inside it,
the layout names the texture descriptor `D_8013CD78` and the command table
`D_8013CD94`. The `0x8017B000` images use the same offsets. The resident
bindings are in `config/slus_01411/overlays/variant72_linker_symbols.txt`.

## Source shape

Retail keeps the second triangle's far corner, `i + 18`, in an induction
register of its own. The source reads it as `t + 17` with `t = i + 1` set at
the top of the loop body, which gives that index enough weight to be reduced
the way retail's is. Several locals also hold more than one value: the
`elevation` that is passed as the fans' point count is also the ribbon's
row offset and the index of a quad's lower-left corner, as in retail's
register use.
