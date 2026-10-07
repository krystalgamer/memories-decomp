# Spanish MODEL417 palette spiral

Helper `+0x3750..+0x43A0` is matching C. It is 3,152 bytes in all eight
MODEL417 loads, for 25,216 instruction bytes from one unique routine:

- MODEL354, stages 7/8
- MODEL134, MODEL232 and MODEL535, stages 9/10

An independent second-slot compilation also matches. Excluding this change, the
screen checked 7,938 configured regional C entries and found no body of the
same size, so no region has accepted C for this routine. The entry calls it
directly, as recorded in the existing inventory.

## Behaviour

While the phase at `+0x2170` is not negative, the routine:

1. Builds twelve two-point arms from `+0x19B0`, on a spiral of half the size at
   `+0x2154`.
2. Moves a copy of each arm along the view by the spread: from phase 3 an
   eighth of the `+0x88` word of the timing record at `+0xB30`.
3. Projects each arm, keeping per-arm status words.
4. Draws each arm as two POLY_GT4 halves from `+0x1FC0`, where the depth and
   status are not negative. The inner and outer colours come from an
   eight-entry palette selected by `(flags + arm) % 8`, with the flags at
   `+0x2124`.

The sweep at `+0x2150` advances by 16 a frame. Once the frame counter at
`+0x2128` reaches the timing record's `+0x20` bound (signed), while the
secondary phase at `+0x2174` is zero, the size grows by step × 64 up to 0x400
and the secondary phase becomes 2.

## Relationship to the other spirals

This is the routine of the accepted MODEL421 palette spiral with MODEL417
offsets, used as structural evidence. The `Variant425SpiralArm` record of the
accepted shared header is reused. Three details differ from MODEL421 and were
recovered from the retail body:

- the palette index is a signed `% 8`, and the eighth entry keeps its explicit
  `== 7` test, which leaves the palette bytes in stack slots
- the tail bound is at `+0x20`
- the tail comparison is signed

## Integration

This change converts the `+0x3750` helper from ASM to C, beside the accepted
MODEL417 lines and rays. The MODEL417 binding list gains the SDK alias
`GsSortPoly`, whose address was already bound as `func_spanish_800842A8`.

## Acceptance evidence

All eight complete 20KiB images match using `gcc_2_8_1_g0_split`.

Regressions cover:

- the arm record and packet layouts, including repeated header inclusion
- relocations and callees against every image
- C segment order, statuses, totals and bindings in the MODEL417 suite

Terminal dependency fingerprints hash the body, the shared spiral header, the
shared model-variant header and `gpu_packets.h`, in that order.
