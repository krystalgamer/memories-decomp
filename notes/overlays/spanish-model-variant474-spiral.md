# Spanish MODEL474 spiral

Helper `+0x2240..+0x2BD8` is matching C. It is 2,456 bytes in all four
header-474/624 MODEL loads, for 9,824 instruction bytes from one unique
routine:

- MODEL121, stages 7/8
- MODEL431, stages 9/10

An independent second-slot compilation also matches. The screen checked 6,223
configured regional C entries and found no body of the same size, so no region
has accepted C for this routine.

## Behaviour

While the phase at `+0x1C1C` is not negative, the routine:

1. Builds twelve two-point arms from `+0xBC8`, on a spiral whose radius is half
   the size at `+0x1BEC`.
2. Moves a copy of each arm along the view by the spread. Before phase 3 the
   spread is `0x400` minus the half-word at `+0x1BEE`; from phase 3 on it is one
   eighth of the timing record's size.
3. Projects each arm and draws it as two POLY_GT4 halves where the depth is
   positive.

The size then grows by step × 64 up to 0x400. Only once it is full does the
sweep at `+0x1BE8` advance by 32.

## Relationship to the other spirals

This routine has the same twelve-arm, `0x1000`/`0x1200`-scale form as the
accepted shared `model_variant/variant448_spiral.c`, and the same two-pass
projection as `variant425_spiral.c`. Those sources, and the arm record in
`variant425_spiral.h`, were used only as structural evidence. The phase guard,
spread, offsets, shades and tail were recovered from the retail body.

The ledger keeps 17 rejected experiments. They cover pointer partitioning, declaration
order, guard forms and a named-profile scheduling diagnostic. Three source
details were required:

- The second-point projection indexes the depth, angle and width by `k`, and
  the screen offsets by constant indices. The retail code therefore keeps two
  strength-reduced pointers.
- The projection pass's arm pointer is assigned before the rotation is built.
- The polygon pointer is assigned before the turn is computed.

## Integration

This change converts the `+0x2240` helper from ASM to C, next to the accepted
MODEL474 mesh and sheets. As recorded in the existing inventory, it is a closed
retained helper with no observed entry-reachable call. The MODEL474 binding
list gains three SDK aliases whose addresses were already bound as
`func_spanish_*` names:

- `RotTransPers`
- `rcos`
- `rsin`

## Acceptance evidence

All four complete 20KiB images match using `gcc_2_8_1_g0_split`. The ledger
keeps every experiment, the exact source, the second-slot compilation and all
four terminal full-image results.

Regressions cover:

- the arm record and packet layouts, including repeated header inclusion
- relocations and callees against every image
- C segment order, statuses, totals and bindings in the MODEL474 suite

Terminal dependency fingerprints hash the body, the shared spiral header, the
shared model-variant header and `gpu_packets.h`, in that order.
