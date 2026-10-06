# Spanish MODEL423 spiral

Helper `+0x12A8..+0x1CB4` is matching C. It is 2,572 bytes in both
header-423/573 MODEL loads (MODEL385, stages 7/8), for 5,144 instruction bytes
from one unique routine. An independent second-slot compilation also matches.
The screen checked 6,223 configured regional C entries and found no body of the
same size, so no region has accepted C for this routine.

## Behaviour

This is the stage-7/8 counterpart of the MODEL473 spiral that the same model
loads at stages 9/10. While the phase at `+0x1E0C` is not negative, the
routine:

1. Builds twelve two-point arms from `+0x84C`, on a spiral whose radius is the
   size at `+0x1DDC`.
2. Moves a copy of each arm along the view by `(k * 48 + 8) * spread / 1024`.
   Before phase 3 the spread is `0x400` minus the half-word at `+0x1DDE`; from
   phase 3 on it is one eighth of the timing record's size.
3. Projects each arm and draws it as two POLY_GT4 halves where both the depth
   and the projection flag are not negative.

The size grows by step × 64 up to 0x400; reaching it moves phase 1 to phase 2.
From phase 2 on, the sweep at `+0x1DD8` advances by 48.

## Relationship to the other spirals

The source follows the MODEL473 spiral recovered in the companion change.
Relative to MODEL473, the offsets and the arm-length step were recovered from
this retail body:

- The work fields are 0x1E50 lower.
- The translation fields are 0x1DB0 lower.
- The polygon is 0x1D60 lower.
- The arms and the timing record are 0x1B68 lower.
- The length uses a step of 48 instead of 32.

The accepted shared `variant448_spiral.c` and `variant425_spiral.c`, and the
arm record in `variant425_spiral.h`, were used only as structural evidence. The
first candidate missed only the three translation-field offsets; it is kept in
the ledger.

## Integration

This change converts the `+0x12A8` helper from ASM to C, next to the accepted
MODEL423 mesh and sheets. As recorded in the existing inventory, it is
direct-entry reachable. The MODEL423 binding list gains the SDK alias
`RotTransPers`, whose address was already bound as `func_spanish_80087868`.

## Acceptance evidence

Both complete 20KiB images match using `gcc_2_8_1_g0_split`. The ledger keeps
the rejected candidate, the exact source, the second-slot compilation and both
terminal full-image results.

Regressions cover:

- the arm record and packet layouts, including repeated header inclusion
- relocations and callees against both images
- C segment order, statuses, owners and bindings in the MODEL423 and sheet suites

Terminal dependency fingerprints hash the body, the shared spiral header, the
shared model-variant header and `gpu_packets.h`, in that order.
