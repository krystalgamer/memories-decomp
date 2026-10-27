# Spanish MODEL479 spiral

Helper `+0x14F8..+0x1F68` is matching C. It is 2,672 bytes in both
header-479/629 MODEL loads (MODEL379, stages 7/8) of the Spanish release and in
the same two French images, which carry the identical body at the same offsets.
`variant479_spiral.c` is compiled once per slot with `gcc_2_8_1_g0_split`; the
slot-1 wrapper only renames `func_8013C4F8` to `func_8017C4F8`. A scan of every
extracted MODEL image found the body in these four images only. This is a new
decompilation; no region had accepted C for it.

## Behaviour

While the phase word at `+0x41EC` is not negative, the routine runs three
passes, each turned by a further 768. A pass builds twelve 0xC4-byte arms at
`+0x2468`, each a three-point spine on a spiral of radius `size * 800 / 1024`
(size at `+0x41BC`, sweep at `+0x41B8`) and a copy moved along the view by
`(k * 16 + 16) * spread / 1024`. The spread is `0x400 - work[0x41BE]`, or the
timing record's `+0x88 / 8` from phase 3 on. It projects both rows at the pass
position (`+0x4024 + n * 32`), with scale 0x1000 or 0x1200 by bit 0 of
`+0x4164`, and draws two POLY_GT4 halves per segment where the depth and the
projection flag are not negative. The size grows by `step * 64` to 0x400,
moving phase 1 to 2; from phase 2 on the sweep advances by 48.

The head computes `r * work[0x41BA]` and `(size / 8) * work[0x41BA]` without
using them, as in the header-418 family spirals.

## What reproduces the target

- The MODEL427 spiral's structure, extended to three points per arm: the last
  point's transform pairs `a[k - 1]` with `a[k]`, the others `a[k]` with
  `a[k + 1]`; literal indices reschedule the loop.
- `phi` and `theta` hold only the index and sweep terms. The pass offset is
  added inline at each `rcos`/`rsin` argument, so loop.c hoists `phi + offset`
  into a pseudo created after the arm pointer copy. With the offset folded
  into `phi`, the target's spill slot order (divisor, arm pointer copy, phi)
  cannot be reached by any declaration order or type.
- The arm pointer is reset after `RotMatrix`, which fixes the temporaries of
  the pass-position reads.

The ledger `spanish-model-variant479-spiral-attempts.csv` records the
candidates and the four exact full-image results.
