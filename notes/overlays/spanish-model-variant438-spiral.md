# Spanish MODEL438 palette spiral

Helper `+0x2A34..+0x3638` is matching C. It is 3,076 bytes in all twelve
header-438/588 MODEL loads, for 36,912 instruction bytes from one unique
routine:

- MODEL147, MODEL211 and MODEL610, stages 7/8
- MODEL263, MODEL525 and MODEL632, stages 9/10

An independent second-slot compilation also matches. Excluding this change, the
screen checked 7,712 configured regional C entries and found no body of the
same size, so no region has accepted C for this routine.

## Behaviour

While the phase at `+0x22E8` is not negative, the routine:

1. Builds twelve two-point arms from `+0xE40`, on a spiral of half the size at
   `+0x22B0`.
2. Moves a copy of each arm along the view by the spread.
3. Projects each arm, keeping per-arm status words.
4. Draws each arm as two POLY_GT4 halves. The inner and outer colours come from
   an eight-entry palette selected by `(flags + arm) & 7`. The first half is
   drawn where the depth is positive, and the second where the depth and status
   are not negative.

The sweep advances by 16 a frame. Once the frame counter passes the timing
record's bound, the size grows by step × 64 up to 0x400 and the secondary phase
becomes 2.

## Relationship to the other spirals

The source combines two structures recovered in companion changes:

- the phase-guarded MODEL474 spiral (#7138)
- the status-keeping projection of the MODEL473 spiral (#7134)

The `Variant425SpiralArm` record of the accepted shared header is reused. The
palette, half-word translation, gates, offsets and tail were recovered from the
retail body. The ledger keeps every rejected form. Three details were required:

- **Palette variables.** The six palette bytes are declared in the draw loop's
  block. Retail spills them after the build loop's constants.
- **Second-half gate.** The second half tests the status word as well as the
  depth.
- **Tail comparison.** The tail reads the timing bound before the frame
  counter.

The same routine, with other offsets and gates, is also loaded by MODEL421 and
MODEL459; companion changes cover those families.

## Integration

This change converts the `+0x2A34` helper from ASM to C, next to the accepted
MODEL438 bands and sheets. As recorded in the existing inventory, it is
direct-entry reachable. The MODEL438 binding list gains the SDK alias
`RotTransPers`, whose address was already bound as `func_spanish_80087868`.

## Acceptance evidence

All twelve complete 20KiB images match using `gcc_2_8_1_g0_split`.

Regressions cover:

- the arm record and packet layouts, including repeated header inclusion
- relocations and callees against every image
- C segment order, statuses and bindings in the MODEL438 suite

Terminal dependency fingerprints hash the body, the shared spiral header, the
shared model-variant header and `gpu_packets.h`, in that order.
