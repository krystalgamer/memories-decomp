# USA MODEL headers 164 and 294

Fourteen North American MODEL variant images share one entry, at `+0x4` and
`0x1E5C` bytes long. No French or Spanish source reproduces it. The entry is new
North American C in
[`variant164_entry.c`](../../src/overlays/model_variant/variant164_entry.c),
built under `gcc_2_8_1_g0_split` like the French entries.
[`variant164_entry_slot1.c`](../../src/overlays/model_variant/variant164_entry_slot1.c)
covers the images loaded at `0x8017B000`.

| Header | Stage | Models |
|---:|---|---|
| 164 | `pos0_slot0` | 169, 178, 272, 494, 591, 636 |
| 164 | `stage9_slot0` | 280 |
| 294 | `stage8_slot1` | 169, 178, 272, 494, 591, 636 |
| 294 | `stage10_slot1` | 280 |

All fourteen rebuild byte-identical to retail.

## What the entry does

The setup command picks a `0x24`-byte row of the command table by
`command % 100` and keeps `command / 100` as the mode. It scatters one target
point per particle through a sphere of random radius, copies the other slot's
position into the gather centre at `y = -350`, and builds two 33-point circles
(the ring radius and the glow size) for the ring. It uploads three textures.
Like the work-size entries in the [French-entry note](usa-model-french-entries.md),
it prints the work-area size, 0x638 bytes, with
`printf("%d (0x%x) Byte\n", sizeof(state), sizeof(state))`.

Each particle starts its own spacing after the previous one. Until it starts,
it follows one of the slot's parts. Over the rise duration it grows to full
size as a textured quad and moves out towards its target. Over the fade
duration it then blinks and fades. Once it has faded, it waits at its target
around the gather centre. From the second delay until the start time, it is
drawn again with its colour ramping up over the pulse duration. Its size
depends on the mode: a sine pulse (mode 2), a two-frame flicker (modes 1 and 3,
and mode 3 also sets the other slot's playback direction), or plain.

From the start time, for the gather duration, the particles spin about the
view axis and shrink onto the gather centre. After that, for the burst
duration:

- in mode 3 the particles fly back out past their targets and fade;
- the entry sets the other slot's playback direction;
- it draws a growing, fading glow quad and the ring at the centre;
- it covers the screen with a flat grey quad that fades from white to black.

The entry returns 0 until the burst starts, 1 on the first burst frame, 0 for
the rest of the burst, and 2 after it.

## Layout and bindings

The entry's `.rodata` is the unit scale vector and the format string, `0x20`
bytes at `0x1E60`. The rest of the image is one raw tail that starts at
`0x1E80`. Inside it, the layout names the texture descriptors `D_8013CE80` and
the command table `D_8013CED4`. The `0x8017B000` images use the same offsets.
The resident bindings are in
`config/slus_01411/overlays/variant164_linker_symbols.txt`.

## Source shape

One local, `value`, carries every scale and fade factor and the second loop's
wait test, so that it lives in `s1` and the other values spill where retail
spills them. The ring loop walks explicit cursors over the screen points, the
depths and the clip flags, and reads the first flag into its own local. Each
"follow the part" branch stores `vz` before the padding halfword, so the
`vz` store is shared with the rise branch's last store. The fades compute
green before blue. The completion test returns early when the flag is already
set.
