# USA MODEL headers 176 and 306

Twenty-seven North American MODEL variant images share one entry, at `+0x4`
and `0x13A0` bytes long. No French or Spanish source reproduces it. The
entry is new North American C in
[`variant176_entry.c`](../../src/overlays/model_variant/variant176_entry.c),
built under `gcc_2_8_1_g0_split` like the French entries.
[`variant176_entry_slot1.c`](../../src/overlays/model_variant/variant176_entry_slot1.c)
covers the images loaded at `0x8017B000`.

| Header | Stage | Models |
|---:|---|---|
| 176 | `pos0_slot0` | 13, 52, 127, 388, 462, 506, 581 |
| 176 | `stage9_slot0` | 177, 213, 467, 479, 544, 608 |
| 306 | `stage8_slot1` | 13, 52, 127, 388, 462, 506, 581 |
| 306 | `stage10_slot1` | 177, 208, 213, 467, 479, 544, 608 |

All twenty-seven rebuild byte-identical to retail.

## What the entry does

The setup command takes its row of the 0x22-byte command table. It builds
two 33-vertex rings from `ccos`/`csin`: an outer one at the row's depth, its
sign set by the active slot, and an inner one at zero. It clears up to 128
dots and gives each dot a random velocity. It also gives up to 32 sprites a random outward
velocity, uploads two textures and prints the work-area size.

After the row's delay, three phases run, each over its own duration:

- The dots are projected with `RotTransPersN` and drawn as box fills while
  their colour fades. Each frame they move by half their velocity, and the
  vertical velocity is pulled towards `y = 350`.
- Three semi-transparent textured strips are cut from the two rings and
  grow with the elapsed time.
- The sprites fly outwards from the camera part, or from a fixed point when
  the part is 0xFF.

The entry returns 1 once after the delay, 2 when the longest phase is over,
and 0 otherwise.

## Layout and bindings

The entry's `.rodata` follows it at `0x13A4`: the unit scale vector, then the
16-byte format string at `0x13B4`. The string is `"%d (%x) Byte.\n"`, and
retail stores `0x0C` in the byte after its terminator, the last byte before
the data. The C spells the string as a 16-byte array with that byte. The
image tail starts at `0x13C4`. Inside it, the layout names the two texture
descriptors `D_8013C3C4` and the command table `D_8013C3FC`. The
`0x8017B000` images use the same offsets. The resident bindings are in
`config/slus_01411/overlays/variant176_linker_symbols.txt`.
