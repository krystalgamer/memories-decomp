# USA MODEL headers 827 and 924

Thirty-two North American MODEL variant images share one entry, at `+0x4`
and `0x1A2C` bytes long. No French or Spanish source reproduces it. It is a
sibling of the [header-828 entry](usa-model-variant828.md), and the new North
American C in
[`variant827_entry.c`](../../src/overlays/model_variant/variant827_entry.c)
follows that source. It is built under `gcc_2_8_1_g0_split` like the French
entries.
[`variant827_entry_slot1.c`](../../src/overlays/model_variant/variant827_entry_slot1.c)
covers the images loaded at `0x8017B000`.

| Header | Stage | Models |
|---:|---|---|
| 827 | `pos0_slot0` | 4, 91, 150, 200, 294, 383, 403, 516, 521, 527 |
| 827 | `stage9_slot0` | 147, 171, 449, 463, 534 |
| 924 | `stage8_slot1` | 4, 91, 150, 200, 294, 383, 403, 516, 521, 527, 546 |
| 924 | `stage10_slot1` | 147, 171, 260, 449, 463, 534 |

All thirty-two rebuild byte-identical to retail.

## What the entry does

The entry is the header-828 entry with 64 embers instead of 32, which moves
every field after them in the work area by `0x200`, and with grey sparks
drawn from the fourth texture through four 64-pixel frames on each of two
rows instead of red sparks from the third.

## Layout and bindings

The entry's `.rodata` is the unit scale vector `D_8013CA30`, at `0x1A30`. The
rest of the image is one raw tail that starts at `0x1A40`. Inside it, the
layout names the four texture descriptors `D_8013CA40` and the command table
`D_8013CAB0`. The `0x8017B000` images use the same offsets. The resident
bindings are in `config/slus_01411/overlays/variant827_linker_symbols.txt`.
