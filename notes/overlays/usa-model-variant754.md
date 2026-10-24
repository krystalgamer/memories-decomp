# USA MODEL headers 754 and 851

Forty-nine North American MODEL variant images share one entry, at `+0x4`
and `0x1958` bytes long. No French or Spanish source reproduces it. The entry
is new North American C in
[`variant754_entry.c`](../../src/overlays/model_variant/variant754_entry.c),
written after the [header-756 entry](usa-model-variant756.md) and built under
`gcc_2_8_1_g0_split` like the French entries.
[`variant754_entry_slot1.c`](../../src/overlays/model_variant/variant754_entry_slot1.c)
covers the images loaded at `0x8017B000`.

| Header | Stage | Models |
|---:|---|---|
| 754 | `pos0_slot0` | 45, 46, 78, 192, 198, 199, 254, 286, 574, 601, 602, 603, 628 |
| 754 | `stage9_slot0` | 10, 11, 158, 160, 193, 281, 396, 481, 553, 589, 641 |
| 851 | `stage8_slot1` | 45, 78, 112, 192, 198, 199, 209, 254, 286, 574, 601, 602, 603, 628 |
| 851 | `stage10_slot1` | 10, 11, 158, 160, 193, 281, 396, 481, 553, 589, 641 |

All forty-nine rebuild byte-identical to retail.

## What the entry does

The setup command takes its row of the 0x48-byte command table. It gives 64
smoke puffs, 8 pieces of debris and 64 embers random velocities, lays out the
two edges of a ring of 16 segments, copies the burst colour and uploads three
textures.

Each frame, the row's camera mode picks the camera preset for the elapsed
time and queues it, and the entry lowers the slot's ambient level until the
burst and raises it again afterwards. After the row's delay, the slot's parts
are tinted every few frames until the row's tint time, through all parts or
through parts 1 to 18 depending on the mode. Once the burst starts:

- a growing number of smoke puffs is drawn, each through eight frames of its
  texture;
- while the burst colour lasts, a widening ring, a four-point star and the
  embers are drawn and fade;
- the debris is drawn for sixteen frames, through eight frames of its
  texture, while it flies out.

When the burst starts the entry queues a camera shake and, if the row asks for
one, a white full-screen flash that fades out. It returns 1 when the burst
starts, 2 when the ambient level is back, the last smoke puff has finished
and every colour has faded out, and 0 otherwise.

## Layout and bindings

The entry's `.rodata` is the unit scale vector `D_8013C95C`, at `0x195C`. The
rest of the image is one raw tail that starts at `0x196C`. Inside it, the
layout names the three texture descriptors `D_8013C96C` and the command table
`D_8013C9C0`. The `0x8017B000` images use the same offsets. The resident
bindings are in `config/slus_01411/overlays/variant754_linker_symbols.txt`.
