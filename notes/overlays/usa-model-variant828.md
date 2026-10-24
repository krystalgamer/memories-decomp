# USA MODEL headers 828 and 925

Thirty-five North American MODEL variant images share one entry, at `+0x4`
and `0x1A34` bytes long. No French or Spanish source reproduces it. The entry
is new North American C in
[`variant828_entry.c`](../../src/overlays/model_variant/variant828_entry.c),
built under `gcc_2_8_1_g0_split` like the French entries.
[`variant828_entry_slot1.c`](../../src/overlays/model_variant/variant828_entry_slot1.c)
covers the images loaded at `0x8017B000`.

| Header | Stage | Models |
|---:|---|---|
| 828 | `pos0_slot0` | 50, 79, 122, 396, 397, 414, 513, 536, 560, 569, 578 |
| 828 | `stage9_slot0` | 64, 75, 225, 353, 374, 495, 567 |
| 925 | `stage8_slot1` | 50, 79, 396, 397, 414, 513, 536, 560, 569, 578 |
| 925 | `stage10_slot1` | 64, 75, 225, 353, 374, 495, 567 |

All thirty-five rebuild byte-identical to retail.

## What the entry does

The setup command takes its row of the 0x3C-byte command table. It gives
every light ray a random rotation and the sparks, the debris and the embers
random velocities, uploads four textures, copies the burst colour, and picks
one of four camera sequences at random. Three of them move the camera through
the row's presets; the fourth leaves it alone.

Each frame the entry lowers the slot's ambient level during the row's delay
and raises it again afterwards. After the delay, in front of the slot:

- while the burst colour lasts, a fan of light rays turns through the random
  rotations and stretches as it grows, each ray drawn twice, and the colour
  fades by the row's step;
- one more spark a frame flies out, each through eight frames of its texture;
- while the burst colour lasts, a glow is drawn at two sizes and the embers
  fly out;
- the debris flies out for twenty-four frames, through eight frames of its
  texture.

The entry returns 1 on its first frame after the delay, 2 when the ambient
level is back, the burst colour has faded out and the debris has finished,
and 0 otherwise.

## Layout and bindings

The entry's `.rodata` is the unit scale vector `D_8013CA38`, at `0x1A38`. The
rest of the image is one raw tail that starts at `0x1A48`. Inside it, the
layout names the four texture descriptors `D_8013CA48` and the command table
`D_8013CAB8`. The `0x8017B000` images use the same offsets. The resident
bindings are in `config/slus_01411/overlays/variant828_linker_symbols.txt`.
