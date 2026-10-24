# USA MODEL headers 766 and 863

Fifty-six North American MODEL variant images share one entry, at `+0x4` and
`0x1834` bytes long. No French or Spanish source reproduces it, but its
closest relative is the French header-770 entry
([`variant770_entry.c`](../../src/overlays/french_model_variant/variant770_entry.c)),
which the new source follows. The entry is new North American C in
[`variant766_entry.c`](../../src/overlays/model_variant/variant766_entry.c),
built under `gcc_2_8_1_g0_split` like the French entries.
[`variant766_entry_slot1.c`](../../src/overlays/model_variant/variant766_entry_slot1.c)
covers the images loaded at `0x8017B000`.

| Header | Stage | Models |
|---:|---|---|
| 766 | `pos0_slot0` | 65, 83, 90, 94, 109, 110, 114, 126, 160, 208, 230, 245, 298, 381, 404, 444, 468, 470, 478, 608, 633, 648 |
| 766 | `stage9_slot0` | 224, 226, 233, 450, 614, 617 |
| 863 | `stage8_slot1` | 65, 83, 90, 94, 109, 114, 126, 160, 208, 230, 245, 298, 381, 404, 444, 468, 470, 478, 608, 633, 648 |
| 863 | `stage10_slot1` | 97, 224, 226, 233, 450, 614, 617 |

All fifty-six rebuild byte-identical to retail.

## What the entry does

The setup command takes its row of the 0x30-byte command table. It clears the
two trail edges, gives the light columns random rotations and the sparks random
velocities, copies the trail, burst and flash colours, and uploads two
textures. The flash square sits in front of the active slot.

Each frame the entry lowers the slot's ambient level while the trail runs and
raises it again afterwards. After the row's delay it records the positions of
two parts of the slot, up to 32 times, and draws a gouraud trail between them
that fades towards its tail. Once the burst starts:

- a flash square, tilted by the row's angle, widens and is drawn twice;
- a burst draws two four-point stars, one of the row's size and one of a
  fixed size at three quarters of the scale;
- the sparks fall from the burst under the row's gravity, each with a random
  quarter of the spark texture;
- a fan of light columns turns through the random rotations.

Every colour fades by 15 a frame once its phase is running. The entry returns 1
when the burst starts, 2 when the ambient level is back and every colour has
faded out, and 0 otherwise.

## Layout and bindings

The entry's `.rodata` is the unit scale vector `D_8013C838`, at `0x1838`. The
rest of the image is one raw tail that starts at `0x1848`. Inside it, the
layout names the two texture descriptors `D_8013C848` and the command table
`D_8013C880`. The `0x8017B000` images use the same offsets. The resident
bindings are in `config/slus_01411/overlays/variant766_linker_symbols.txt`.
