# USA MODEL headers 647 and 714

Forty-one North American MODEL variant images share one entry, at `+0x4` and
`0xE54` bytes long, that no French or Spanish source reproduces. The entry is
new North American C in
[`variant647_entry.c`](../../src/overlays/model_variant/variant647_entry.c),
built under `gcc_2_8_1_g0_split` like the French entries, with
[`variant647_entry_slot1.c`](../../src/overlays/model_variant/variant647_entry_slot1.c)
for the images loaded at `0x8017B000`.

| Header | Stage | Models |
|---:|---|---|
| 647 | `pos0_slot0` | 67, 115, 142, 289, 435, 439, 477, 575 |
| 647 | `stage9_slot0` | 123, 206, 271, 299, 381, 382, 393, 397, 415, 579, 606, 635, 649 |
| 714 | `stage8_slot1` | 67, 115, 142, 289, 435, 439, 477, 575 |
| 714 | `stage10_slot1` | 123, 206, 299, 381, 382, 393, 397, 415, 579, 606, 635, 649 |

All forty-one rebuild byte-identical to retail.

## What the entry does

The setup command clears the 0x1204-byte work area, prints its size with
`printf("E_DATA size = %x byte\n", ...)` and uploads one texture twice. Its
row of the 0xC-byte command table gives the start delay, the ring radius and
height, and a colour. Commands 14 and 15 use two bursts and the others one.
Each burst gets its rotation and position from the command, and the camera
coordinate unit as its parent. The setup then fills two sets of four rings and
two sets of eight rings, 17 vertices each, from `rsin`/`rcos` and the radius.

After the delay, the bursts start: all of them at once, or for command 15
the second one 41 frames after the first. Every frame each active burst
scales up with its age (`0x6000 * elapsed / duration`, duration 64) and fades
out over its second half. It is drawn as 64 semi-transparent textured quads
and 128 textured triangles. The entry returns 1 when the bursts start (4 for
command 15's first burst, and 1 again for its second) and 2 once none is
active; otherwise it returns 0.

Two details of the C come from the retail code rather than from intent. The
entry copies eight bytes of `Model_GetViewMetricsBuffer()` to the stack and
never reads them. And the burst's local coordinate unit starts from a copy of
the resident matrix at `0x800FE168` (`D_800FE168`). It is zero in the
executable file; `GsSortSprite`, `GsSortBg` and `Gssub_make_matrix` also read
it, so it is presumably one of the library's identity matrices filled at run
time, but nothing here names it.

## Layout and bindings

The entry's `.rodata` follows it at `0xE58`: the format string and the three
`switch` jump tables. The rest of the image is one raw tail. The layout names
the texture descriptor `D_8013BF5C` and the command table `D_8013BF78` inside
it (the `0x8017B000` images use the same offsets). The resident bindings,
including `D_800FE168`, are in
`config/slus_01411/overlays/variant647_linker_symbols.txt`.
