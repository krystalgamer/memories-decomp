# Spanish MODEL473 spiral

Helper `+0x15AC..+0x1FB0` is matching C. It is 2,564 bytes in both
header-473/623 MODEL loads (MODEL385, stages 9/10), for 5,128 instruction
bytes from one unique routine. An independent second-slot compilation also
matches. The screen checked 6,223 configured regional C entries. Six had the
same size, and none had the same shape, so no region has accepted C for this
routine.

## Behaviour

While the phase at `+0x3C5C` is not negative, the routine:

1. Builds twelve two-point arms from `+0x23B4`, on a spiral whose radius is the
   size at `+0x3C2C`.
2. Moves a copy of each arm along the view by `(k * 32 + 8) * spread / 1024`.
   Before phase 3 the spread is `0x400` minus the half-word at `+0x3C2E`; from
   phase 3 on it is one eighth of the timing record's size.
3. Projects each arm and draws it as two POLY_GT4 halves where both the depth
   and the projection flag are not negative.

The size grows by step × 64 up to 0x400; reaching it moves phase 1 to phase 2.
From phase 2 on, the sweep at `+0x3C28` advances by 48.

## Relationship to the other spirals

This routine combines the twelve-arm form of the accepted shared
`model_variant/variant448_spiral.c` with the flag-gated projection of
`variant425_spiral.c`. Those sources, and the arm record in
`variant425_spiral.h`, were used only as structural evidence. The guard, spread,
offsets, shades and tail were recovered from the retail body.

The ledger keeps 25 rejected experiments. They cover declaration order, pointer
partitioning, guard forms and the named `no_cse_follow_jumps` profile. The
following source details were required:

- Three shade variables are used.
- The size step is multiplied by 64.
- The projection pass's arm pointer is assigned before the rotation is built.
- The polygon pointer is assigned before the turn is computed.

## Integration

This change converts the `+0x15AC` helper from ASM to C, next to the accepted
MODEL473 sheets. As recorded in the existing inventory, it is direct-entry
reachable. The MODEL473 binding list gains four SDK aliases whose addresses
were already bound as `func_spanish_*` names:

- `RotTransPers`
- `ratan2`
- `rcos`
- `rsin`

## Acceptance evidence

Both complete 20KiB images match using `gcc_2_8_1_g0_split`. The ledger keeps
every experiment, the exact source, the second-slot compilation and both
terminal full-image results.

Regressions cover:

- the arm record and packet layouts, including repeated header inclusion
- relocations and callees against both images
- C segment order, statuses, totals and bindings in the MODEL473 suite

Terminal dependency fingerprints hash the body, the shared spiral header, the
shared model-variant header and `gpu_packets.h`, in that order.
