# USA MODEL headers 654 and 721

Thirty-three North American MODEL variant images share one entry, at `+0x4`
and `0x236C` bytes long. No French or Spanish source reproduces it. The entry
is new North American C in
[`variant654_entry.c`](../../src/overlays/model_variant/variant654_entry.c),
built under `gcc_2_8_1_g0_split` like the French entries.
[`variant654_entry_slot1.c`](../../src/overlays/model_variant/variant654_entry_slot1.c)
covers the images loaded at `0x8017B000`.

| Header | Stage | Models |
|---:|---|---|
| 654 | `pos0_slot0` | 204, 503, 515, 526, 532, 547, 566, 589, 604 |
| 654 | `stage9_slot0` | 122, 487, 509, 538, 578, 585, 613 |
| 721 | `stage8_slot1` | 204, 503, 515, 526, 532, 547, 566, 589, 604 |
| 721 | `stage10_slot1` | 122, 151, 487, 509, 538, 578, 585, 613 |

All thirty-three rebuild byte-identical to retail.

## What the entry does

The entry belongs to the family of headers 627 and 647: it clears a
`0x3F9C`-byte work area, prints its size, and keeps its effects in the
`0x4C`-byte burst records of header 647. The setup command picks one of
fifteen rows of the `0xC`-byte command table and one of five grid shapes:
up to three rows of bursts per column, three to twelve columns, and five or
eight ring quads per column. Every burst and ring quad gets the row's
colour, a frame of the shared texture, and a direction around the column;
every column gets one sphere in the row's second colour. The setup also
fills four quad corners from the row's radius and a 9x9 grid of points on a
sphere five times that radius.

After the row's delay the command decides how the effects start:

- commands 0, 9 and 14 send one column of bursts every four frames from a
  part of the slot (part 8, or part 6 for command 14; command 9 moves the
  start fifty units further in front);
- commands 2, 3, 6, 7, 8 and 11 start the first column's ring quads and every
  sphere at once, in front of the slot and 350, 150 or 250 units up;
- commands 1, 4, 5, 10, 12 and 13 start one column's ring quads and its
  sphere every ten frames in front of the slot.

Each frame the bursts grow from nothing as they fly out, turned towards the
camera, and step through seven frames of the texture. When a column's first
burst finishes, it starts that column's ring quads and sphere where it ended.
The ring quads fly out in a circle and grow to twice their size; the spheres
grow from nothing, each turned by its column's angle. Both fade over the
second half of their lifetime. Each sphere is drawn as gouraud lines whose
two ends are the same grid point, so it shows as a dotted sphere; retail does
the same.

The entry returns 4 while a ring quad is drawn, 1 on the first frame with
everything finished and 2 on the frames after, and 0 otherwise.

## Layout and bindings

The entry's `.rodata` is the work-size format string and the two jump tables
of the command switches, `0x94` bytes at `0x2370`. The rest of the image is
one raw tail that starts at `0x2404`. Inside it, the layout names the texture
descriptor `D_8013D404` and the command table `D_8013D420`. The `0x8017B000`
images use the same offsets. The resident bindings are in
`config/slus_01411/overlays/variant654_linker_symbols.txt`.

## Source shape

The quad corners' negations go through a `static inline` with an `int`
parameter. Written inline, the front end narrows `-radius` to halfword
arithmetic and gcc loads the radius twice; through the parameter it keeps
retail's three loads and the zero-extension before the third negation.
