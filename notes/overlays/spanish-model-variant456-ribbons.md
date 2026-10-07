# Spanish MODEL456 ribbons

Helper `+0x1834..+0x1F58` is matching C. It is 1,828 bytes in all six
header-456/606 MODEL loads registered by the MODEL456 lines change, for 10,968
instruction bytes from one unique routine:

- MODEL163, stages 7/8
- MODEL460, stages 9/10
- MODEL536, stages 9/10

A separate second-slot compilation also matches. Screening 7,948 accepted
regional C entries found no body of this size, so no region has accepted C
for this routine. The routine is a closed contiguous function with a stack
prologue, and the loader entry does not reach it.

## Behaviour

The routine handles two ribbons, each a `Ribbon456` record of 0xE0 bytes at
`+0x280`.

1. **Placement.** Each ribbon's three points sit at `i / 2` of the offset at
   `+0x22F0`. A copy of each point is moved 48 units along the view angle.
2. **Projection.** Both spines are projected with `RotTransPers4` and
   `RotTransPers`. This gives a screen angle and a projected width for each
   point. The width is scaled by the record's `+0xDC` word.
3. **Sway.** Every point is pushed along its screen normal by a sway. The
   sway combines the phase at `+0x235C` with the spine wave of the last
   placement step. The end points use a zero phase, so only the wave moves
   them.
4. **Drawing.** Each of the two segments is written into one of two
   `POLY_FT4` packets at `+0x2204` and sorted when its depth is positive.

The phase base is never assigned. The retail code reads its stack slot as it
is, and the source keeps that behaviour.

## Method

The routine was reconstructed from the retail instructions. The parked helper
at `+0x101C` was used only as structural evidence. That helper draws the same
projection and sway, but over clamped, growing beams. Both helpers differ from
the accepted streamers in shape and record layout.

Two source details decide the exact layout:

- **Last segment.** Its angle takes `sa[j]` and `sa[j - 1]`. With constant
  indices, the allocator spills the address of `a[1]` instead of the
  ribbon's scale pointer.
- **Packet toggle.** The toggle tests `!(j & 1)` first.

## Integration

This change converts the `+0x1834` helper from assembly to C in all six
modules. The accepted MODEL456 lines (`+0x315C`) and streamers (`+0x34D4`)
sit alongside it. Every callee already had a binding.

## Acceptance evidence

All six complete 20 KiB images match using `gcc_2_8_1_g0_split`. The 10-row
ledger keeps:

- the two rejected experiments
- the exact source and its second-slot compilation
- all six terminal full-image results

Regressions cover:

- record offsets, including repeated inclusion of the header
- relocations and callees against every image
- C segment order, statuses and totals in the MODEL456 lines suite

Terminal dependency fingerprints hash, in order:

1. the body
2. the ribbon header
3. the shared model-variant header
