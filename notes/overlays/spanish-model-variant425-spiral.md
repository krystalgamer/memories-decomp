# Spanish MODEL425 spiral

Helper `+0x1370..+0x1DA4` is matching C. It is 2,612 bytes in both
header-425/575 MODEL loads (MODEL0, stages 9/10), for 5,224 instruction bytes
from one unique routine. An independent second-slot compilation also matches.
The screen checked 6,223 configured regional C entries and found no body of the
same size, so no region has accepted C for this routine.

## Behaviour

While the phase at `+0x180C` is not negative, the routine:

1. Builds twelve two-point arms from `+0x84C`, on a spiral whose radius is
   three quarters of the size at `+0x17DC`.
2. Moves a copy of each arm along the view by `(k * 16 + 4) * spread / 1024`.
   Before phase 3 the spread is `0x400` minus the half-word at `+0x17DE`; from
   phase 3 on it is one eighth of the timing record's size.
3. Projects each arm and draws it as two POLY_GT4 halves in the arm's own
   colours, where both the depth and the projection flag are not negative.

The size grows by step × 64 up to 0x400; reaching it moves phase 1 to phase 2.
From phase 2 on, the sweep at `+0x17D8` advances by 48.

## Relationship to the other spirals

The source follows the MODEL423 and MODEL473 spirals recovered in companion
changes. The following were recovered from this retail body:

- **Offsets.** Work fields below `+0x1DD8` in the MODEL423 form are 0x604 lower
  here, and the size, sweep and phase are 0x600 lower.
- **Radius.** The radius is `size * 768 / 1024`, and the length step is 16 with
  a base of 4.
- **Colours.** Each arm's colours are read from its record, in the
  `Variant448SpiralArm` layout of the accepted shared
  `model_variant/variant448_spiral.h`, instead of fixed shades.

The accepted shared spiral sources were used only as structural evidence. The
first candidate was exact.

## Integration

This change converts the `+0x1370` helper from ASM to C, next to the accepted
MODEL425 mesh and sheets. As recorded in the existing inventory, it is
direct-entry reachable. The MODEL425 binding list gains the SDK alias
`RotTransPers`, whose address was already bound as `func_spanish_80087868`.

## Acceptance evidence

Both complete 20KiB images match using `gcc_2_8_1_g0_split`. The ledger keeps
the exact source, the second-slot compilation and both terminal full-image
results.

Regressions cover:

- the arm record and packet layouts, including repeated header inclusion
- relocations and callees against both images
- C segment order, statuses, owners and bindings in the MODEL425 and sheet
  suites

Terminal dependency fingerprints hash the body, the shared `variant448_spiral.h`
header, the shared model-variant header and `gpu_packets.h`, in that order.
