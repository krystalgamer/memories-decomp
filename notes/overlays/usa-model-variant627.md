# USA MODEL headers 627 and 694

Fourteen North American MODEL variant images share one entry, at `+0x4` and
`0x1368` bytes long. No French or Spanish source reproduces it. The entry is
new North American C in
[`variant627_entry.c`](../../src/overlays/model_variant/variant627_entry.c),
built under `gcc_2_8_1_g0_split` like the French entries.
[`variant627_entry_slot1.c`](../../src/overlays/model_variant/variant627_entry_slot1.c)
covers the images loaded at `0x8017B000`.

| Header | Stage | Models |
|---:|---|---|
| 627 | `pos0_slot0` | 489, 505 |
| 627 | `stage9_slot0` | 13, 286, 421, 482, 541 |
| 694 | `stage8_slot1` | 489, 505 |
| 694 | `stage10_slot1` | 13, 286, 421, 482, 541 |

All fourteen rebuild byte-identical to retail.

## What the entry does

The setup clears the 0x95C-byte work area and prints its size, like the
[header-647 entry](usa-model-variant647.md), whose 0x4C-byte burst record it
reuses. Commands 0 to 6 set up two rings and ten fans of lines from their row
of the 0xE-byte command table. Each one gets the row's colour and lifetime, the
camera unit, and a place in front of the active slot. The rings and fans get
smaller scales one after another, and each fan is also turned a further step
around the cone's axis.

After the row's delay, all of them start together. Each frame, a ring is drawn
as 32 shaded quads between an inner circle and an outer circle. The circle
radii move from the row's inner radius towards its outer radius as the ring
grows. A fan is drawn as 32 shaded lines between the same two circles and grows
faster the higher its index. During the second half of its lifetime each one
fades, by a fixed amount per frame derived from the row's colour. The entry
returns 4 on the first frame, 1 on the second, 2 once every ring and fan has
ended, and 0 otherwise.

## Layout and bindings

The entry's `.rodata` is the work-size format string, at `0x136C`. The rest of
the image is one raw tail that starts at `0x1384` with the command table
`D_8013C384`. The `0x8017B000` images use the same offsets. The resident
bindings are in `config/slus_01411/overlays/variant627_linker_symbols.txt`.
