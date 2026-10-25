# USA MODEL headers 110 and 240

Twenty-four North American MODEL variant images share one entry, at `+0x4`
and `0x1914` bytes long. No French or Spanish source reproduces it. The entry
is new North American C in
[`variant110_entry.c`](../../src/overlays/model_variant/variant110_entry.c),
built under `gcc_2_8_1_g0_split` like the French entries.
[`variant110_entry_slot1.c`](../../src/overlays/model_variant/variant110_entry_slot1.c)
covers the images loaded at `0x8017B000`.

| Header | Stage | Models |
|---:|---|---|
| 110 | `pos0_slot0` | 28, 191, 393, 467, 479, 599 |
| 110 | `stage9_slot0` | 112, 250, 445, 533, 574, 625 |
| 240 | `stage8_slot1` | 28, 191, 393, 467, 479, 599 |
| 240 | `stage10_slot1` | 112, 250, 445, 533, 574, 625 |

All twenty-four rebuild byte-identical to retail.

## What the entry does

The setup command picks a `0x26`-byte row of the command table. The setup
builds the fan's inner ring and a jagged outer ring, scatters the row's dots
and sparks over spheres of random radius, and uploads the texture.

After the row's delay, each part runs for its own duration. Two fans of
triangles open in front of the slot and fade, and a sprite plays eight frames
of the texture as it shrinks. The dots grow out of the slot as one-pixel
boxes, and the sparks fly out as textured quads. All four fade as they go.
Retail sets each spark's `x` instead of adding to it, so every spark starts
at `x = 0`.

The entry returns 0 during the delay, 1 once on the first frame after it,
2 when the longest of the four durations has passed, and 0 otherwise.

## Layout and bindings

The entry's `.rodata` is the unit scale vector, `0x10` bytes at `0x1918`.
The rest of the image is one raw tail that starts at `0x1928`. Inside it,
the layout names the texture descriptor `D_8013C928` and the command table
`D_8013C944`. The `0x8017B000` images use the same offsets. The resident
bindings are in `config/slus_01411/overlays/variant110_linker_symbols.txt`.

## Source shape

Like [header 72's entry](usa-model-variant72.md), the fans read the second
triangle's far corner as `t + 17` with `t = i + 1` set at the top of the
loop body. With that, the corner gets an induction register of its own, as
in retail. The second triangle's flag is also copied into the scale clamp's
`size`. Each colour's blue channel has a name of its own. These choices put
the clamps and the colour arithmetic in retail's registers.
