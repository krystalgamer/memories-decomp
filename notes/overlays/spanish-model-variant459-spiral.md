# Spanish MODEL459 palette spiral

Helper `+0x2604..+0x3224` is matching C. It is 3,104 bytes in all twelve
header-459/609 MODEL loads, for 37,248 instruction bytes from one unique
routine:

- MODEL47, MODEL238, MODEL411 and MODEL620, stages 7/8
- MODEL231 and MODEL417, stages 9/10

An independent second-slot compilation also matches. Excluding this change, the
screen checked 7,781 configured regional C entries and found no body of the
same size, so no region has accepted C for this routine.

## Behaviour

While the phase at `+0x30DC` is not negative, the routine:

1. Builds twelve two-point arms from `+0x1188`, on a spiral of half the size at
   `+0x3098`.
2. Moves a copy of each arm along the view by the spread, which is always an
   eighth of the selected timing record's `+0x88` word. The timing record is
   the `+0x130`-byte entry at `+0x2568` indexed by the descriptor's count.
3. Projects each arm, keeping per-arm status words.
4. Draws each arm as two POLY_GT4 halves from `+0x2D60`. The inner and outer
   colours come from an eight-entry palette selected by `(flags + arm) & 7`.
   Both halves require a non-negative depth and status word.

The sweep advances by 16 a frame. Once the time reaches the descriptor's
`+0x38` bound, while the secondary phase at `+0x30E0` is zero, the size grows
by step × 32 up to 0x400 and the secondary phase becomes 2.

## Relationship to the other spirals

This is the routine of the accepted MODEL438 and MODEL421 palette spirals with
MODEL459 work offsets, both gates of MODEL421 and its own timing details. The
`Variant425SpiralArm` record of the accepted shared header is reused. The
ledger keeps every rejected form. Three details were required:

- **Pointer order.** The context, arm pointer and indexed timing pointer are
  set before the order-table call.
- **Unconditional spread.** The spread is read from the timing record without
  a phase test.
- **Tail.** The timing bound is at `+0x38` and the size step is step × 32.

## Integration

This change converts the `+0x2604` helper from ASM to C, beside the accepted
MODEL459 bands and sheets. As recorded in the existing inventory, it is
entry-reachable. The MODEL459 binding list gains the SDK alias `RotTransPers`,
whose address was already bound as `func_spanish_80087868`.

## Acceptance evidence

All twelve complete 20KiB images match using `gcc_2_8_1_g0_split`.

Regressions cover:

- the arm record and packet layouts, including repeated header inclusion
- relocations and callees against every image
- C segment order, statuses and bindings in the MODEL459 suite

Terminal dependency fingerprints hash the body, the shared spiral header, the
shared model-variant header and `gpu_packets.h`, in that order.
