# Spanish MODEL421 palette spiral

Helper `+0x1860..+0x247C` is matching C. It is 3,100 bytes in all twelve
header-421/571 MODEL loads, for 37,200 instruction bytes from one unique
routine:

- MODEL84 and MODEL162, stages 7/8
- MODEL88, MODEL114, MODEL184 and MODEL369, stages 9/10

An independent second-slot compilation also matches. Excluding this change, the
screen checked 7,712 configured regional C entries and found no body of the
same size, so no region has accepted C for this routine.

## Behaviour

While the phase at `+0x1960` is not negative, the routine:

1. Builds twelve two-point arms from `+0x4E0`, on a spiral of half the size at
   `+0x1934`.
2. Moves a copy of each arm along the view by the spread.
3. Projects each arm, keeping per-arm status words.
4. Draws each arm as two POLY_GT4 halves from `+0x1778`. The inner and outer
   colours come from an eight-entry palette selected by `(flags + arm) & 7`.
   Both halves require a non-negative depth and status word.

The sweep advances by 16 a frame. Once the frame counter passes the bound of
the timing record at `+0x1070`, the size grows by step × 64 up to 0x400 and the
secondary phase becomes 2.

## Relationship to the other spirals

This is the MODEL438 palette spiral (#7145) with MODEL421 work offsets. The
only structural difference is the first-half gate: MODEL438 tests for a
positive depth, while MODEL421 tests for a non-negative depth and status word,
which accounts for the 24 extra bytes. The `Variant425SpiralArm` record of the
accepted shared header is reused. The ledger keeps every rejected form.

## Integration

This change converts the `+0x1860` helper from ASM to C, beside the accepted
MODEL421 helpers. The entry calls it directly at `+0x1068`, so it is
direct-entry reachable. All required SDK aliases were already bound in the
MODEL421 binding list.

## Acceptance evidence

All twelve complete 20KiB images match using `gcc_2_8_1_g0_split`.

Regressions cover:

- the arm record and packet layouts, including repeated header inclusion
- relocations and callees against every image
- C segment order, statuses and reachability notes in the MODEL421 suite

Terminal dependency fingerprints hash the body, the shared spiral header, the
shared model-variant header and `gpu_packets.h`, in that order.
