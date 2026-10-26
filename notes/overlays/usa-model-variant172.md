# USA MODEL headers 172 and 302

Twenty North American MODEL variant images share one entry, at `+0x4` and
`0xCF4` bytes long. No French or Spanish source reproduces it. The entry is new
North American C in
[`variant172_entry.c`](../../src/overlays/model_variant/variant172_entry.c),
built under `gcc_2_8_1_g0_split` like the French entries.
[`variant172_entry_slot1.c`](../../src/overlays/model_variant/variant172_entry_slot1.c)
covers the images loaded at `0x8017B000`.

| Header | Stage | Models |
|---:|---|---|
| 172 | `stage9_slot0` | 7, 56, 157, 179, 228, 505, 552, 588, 593, 615 |
| 302 | `stage10_slot1` | 7, 56, 157, 179, 228, 505, 552, 588, 593, 615 |

All twenty rebuild byte-identical to retail.

## What the entry does

The setup command picks a `0x1A`-byte row of the command table by
`command % 100`. It scatters target points through a sphere of random radius
around the other slot's `x` and `z` at `y = -350`, and uploads the texture.
Like the work-size entries in the [French-entry note](usa-model-french-entries.md),
it prints the work-area size, 0x2014 bytes, with
`printf("%d (0x%x) Byte\n", sizeof(state), sizeof(state))`. Retail fills 513
targets into a 512-entry array. The last one overwrites the texture word and
the completion flag, and the setup sets both afterwards, so the source keeps
the bound and says so.

The points run in groups of the row's group size, each group starting its own
spacing after the previous one. Until a group starts, its dots follow one of
the slot's parts with a random jitter. Then, for the gather duration, they
move onto their targets as one-pixel boxes. After that, for the fly duration,
each target draws a textured quad that plays eight frames of the texture.

The entry returns 0 until the first group has gathered, 4 while the groups
fly, 1 once after the last group has flown, and 2 after that.

## Layout and bindings

The entry's `.rodata` is the unit scale vector and the format string, `0x20`
bytes at `0xCF8`. The rest of the image is one raw tail that starts at
`0xD18`. Inside it, the layout names the texture descriptor `D_8013BD18` and
the command table `D_8013BD34`. The `0x8017B000` images use the same offsets.
The resident bindings are in
`config/slus_01411/overlays/variant172_linker_symbols.txt`.

## Source shape

The draw pass sets the unit scale through `j = 4096` before `j` becomes the
loop counter, and reads the second pass's frame index back into `j`. Its inner
loop counts with `n1`. The quad colour is read through `frame`, `u` and `n2`,
and the first gather step through `v`. These choices put the frame arithmetic,
the colour bytes and the step in retail's registers.
