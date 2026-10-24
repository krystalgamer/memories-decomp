# USA MODEL headers 756 and 853

Forty-eight North American MODEL variant images share one entry, at `+0x4`
and `0x1FFC` bytes long. No French or Spanish source reproduces it. It extends
the [header-766 entry](usa-model-variant766.md), and the new North American C
in [`variant756_entry.c`](../../src/overlays/model_variant/variant756_entry.c)
follows that source. It is built under `gcc_2_8_1_g0_split` like the French
entries.
[`variant756_entry_slot1.c`](../../src/overlays/model_variant/variant756_entry_slot1.c)
covers the images loaded at `0x8017B000`.

| Header | Stage | Models |
|---:|---|---|
| 756 | `pos0_slot0` | 11, 128, 132, 172, 179, 235, 251, 255, 285, 380, 415, 553, 593, 626, 644 |
| 756 | `stage9_slot0` | 65, 99, 132, 244, 245, 256, 284, 540, 629, 643 |
| 853 | `stage8_slot1` | 11, 128, 132, 179, 235, 255, 285, 380, 415, 553, 593, 626, 644 |
| 853 | `stage10_slot1` | 65, 99, 132, 244, 245, 256, 284, 540, 629, 643 |

All forty-eight rebuild byte-identical to retail.

## What the entry does

The setup command takes its row of the 0x34-byte command table. Like header
766, it clears the two trail edges, gives the light columns random rotations,
and gives the sparks random velocities. It also gives random velocities to 16
pieces of debris, 64 smoke puffs and 64 embers, and uploads three textures.

The trail, the ambient-level dip and the two four-point stars, the sparks and
the light columns of the burst behave as in header 766. Once the burst starts,
the entry also:

- draws a growing number of smoke puffs, each through six frames of its
  texture;
- draws the debris through eight frames of its texture while it flies out;
- draws the embers flying out of the burst before the stars.

Every puff takes its texture row from the second puff's frame, as retail
does. The entry returns 1 when the burst starts, 2 when the ambient level is
back, every colour has faded out and the last smoke puff and the debris have
finished, and 0 otherwise.

## Layout and bindings

The entry's `.rodata` is the unit scale vector `D_8013D000`, at `0x2000`. The
rest of the image is one raw tail that starts at `0x2010`. Inside it, the
layout names the three texture descriptors `D_8013D010` and the command table
`D_8013D064`. The `0x8017B000` images use the same offsets. The resident
bindings are in `config/slus_01411/overlays/variant756_linker_symbols.txt`.
