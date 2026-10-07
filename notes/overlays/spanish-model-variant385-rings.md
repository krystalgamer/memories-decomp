# Spanish MODEL385 rings

Helper `+0x26B4..+0x2EB4` is matching C. It is 2,048 bytes in each of the
eight MODEL385/535 images, for 16,384 instruction bytes from one unique
routine. Every complete 20,480-byte image matches retail, and an independent
second-slot compilation also matches. With this change four of the six
MODEL385 routines are C; the entry and the helper at `+0x1B00` remain ASM.

Excluding this change, the screen checked 7,760 configured regional C entries.
It compared twelve same-size bodies and found no accepted normalized match,
so no region has accepted C for this routine. The source was recovered from
the retail instructions with the `gcc_2_8_1_g0_split` profile.

## Call and context

Entry `+0xDDC` calls the helper directly with the original context. Four
0x118-byte rings start at `+0x98C`. Each holds seventeen inner and seventeen
outer points, a scale and a cycle count, the same layout as the shared
`ModelVariantCurtain` record, which the source reuses.

The routine also reads these context fields, all consistent with the accepted
MODEL385 pulse layout:

- the first pulse group's size at `+0x56C + 0x80`
- the direction at `+0xFC0..+0xFC8`, the frame flag at `+0xFEC`, the time at
  `+0xFF0` and the step at `+0xFF8`
- the timing record at `+0x1000` and the phase at `+0x1030`

The inner and outer colours are bytes at `+0x1018` and `+0x101C`, and the ring
angle is a half-word at `+0x102E`.

## Behaviour

Before phase 2 only ring 3 is drawn; otherwise all four are. For each ring
with a positive scale, the routine:

1. Rebuilds the seventeen point pairs on a circle starting at the ring angle,
   in steps of 256. Ring 3 uses the radii at timing `+0x14`/`+0x18` and sits
   behind the plane; the others use `+0x1C`/`+0x20` and sit in front. The outer
   radius and depth follow an amplitude of 32 × pulse size / 4096 on odd
   frames.
2. Builds the ring's local-screen matrix from the direction's two arctangents,
   the ring's scale, and either the word origin at `+0xFAC` (ring 3) or the
   half-word origin at `+0xFB8`.
3. Draws sixteen POLY_GT4 quads through one packet at `+0xEC8`. Rings past
   half size, other than ring 3, fade by `(0x1000 - scale) / 2048`.

Before phase 3, ring 3 grows as `(time << 12) / end` until it reaches full
size; after that it shrinks over the timing record's fade window. The other
rings grow by step × 128 and wrap, counting cycles, until phase 5. From phase 5
they stop at full size, and when ring 2 does so in a frame where no ring
wrapped, the phase becomes 6. The ring angle advances by step × 80.

## Recovery notes

The attempt ledger keeps every materially distinct form. Three details were
required:

- **Shared point stores.** The points are written with `setVector`, which
  shares the outer point's address between its stores.
- **Colour arms.** The plain colours are written in separate `i == 3` and
  small-ring arms; later cross-jumping merges them. The extra references give
  the colour variables a higher allocation priority than the ring-count
  induction pointer, which retail spills.
- **Prologue order.** The pulse-group and packet pointers are assigned before
  the arctangent calls.

## Acceptance evidence

All eight complete 20KiB images match using `gcc_2_8_1_g0_split`. All
required SDK aliases were already bound.

Regressions cover:

- the ring layout, including repeated header inclusion
- relocations and callees against every image
- C segment order, statuses and totals in the MODEL385 suite

Terminal dependency fingerprints hash the body, the shared model-variant
header and `gpu_packets.h`, in that order.
