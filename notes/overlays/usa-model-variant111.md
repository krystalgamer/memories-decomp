# USA MODEL headers 111 and 241

Ten North American MODEL variant images share one entry, at `+0x4` and
`0xCF4` bytes long, that no French or Spanish source reproduces. The closest
French entry, `variant129_entry`, is 1,017 instructions against 829: it has two
textures and a different command row. The entry is new North American C in
[`variant111_entry.c`](../../src/overlays/model_variant/variant111_entry.c),
built under `gcc_2_8_1_g0_split` like the French entries, with
[`variant111_entry_slot1.c`](../../src/overlays/model_variant/variant111_entry_slot1.c)
for the images loaded at `0x8017B000`.

| Header | Load address | Images |
|---:|---|---|
| 111 | `0x8013B000` | `model_variant_466_pos0_slot0`, `483_pos0_slot0`, `548_pos0_slot0`, `247_stage9_slot0`, `486_stage9_slot0` |
| 241 | `0x8017B000` | `model_variant_466_stage8_slot1`, `483_stage8_slot1`, `548_stage8_slot1`, `247_stage10_slot1`, `486_stage10_slot1` |

All ten rebuild byte-identical to retail.

## What the entry does

A setup command (`command >= 0`) selects a 0x1C-byte row of the command table
and uploads one texture. The row holds the colour, the source part, the quad half
size, a depth, an extra depth for the last leg, the leg duration, the spacing,
the count and the start delay. Until the delay runs out, every quad follows the
model part. After that, quad `i` starts `spacing * i` frames late and runs three
legs of `travel_duration` frames each, toward fixed depths. Each leg has its own
colour fade and scale ramp, and the quad is drawn as four textured quarters. The
entry returns 0 until the first quad has finished its second leg, then 4, and
then 1 once (2 afterwards) when the last quad has finished its third leg.

Two details of the C come from the retail code rather than from intent. The setup
loop zeroes the first position `count` times without advancing; the French
header-129 entry does the same. And the `GsGLINE` local is never drawn: retail
only writes its attribute word (`0x50000000`), and the type was chosen because
its size gives retail's 0x170-byte frame.

## Layout and bindings

Each image keeps everything after the entry as one raw tail. The layout names
three symbols inside it that the entry reads: the scale vector `D_8013BCF8`, the
texture descriptor `D_8013BD08` and the command table `D_8013BD24` (the
`0x8017B000` images use the same offsets). The resident bindings are read from
each image's own `jal` targets, in
`config/slus_01411/overlays/variant111_linker_symbols.txt`, which follows the
French-wrapper convention of one binding file per source.
