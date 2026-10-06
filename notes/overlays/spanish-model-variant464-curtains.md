# Spanish MODEL464 curtains

Helper `+0x2838..+0x2DAC` is matching C. It is 1,396 bytes in all six
header-464/614 MODEL loads registered by the MODEL464 sheets change, for 8,376
instruction bytes from one unique routine:

- MODEL175, stages 7/8
- MODEL182, stages 9/10
- MODEL244, stages 7/8

An independent second-slot compilation also matches. The screen checked
6,223 configured regional C entries. Fifty-four had the same size, and none had
the same shape, so no region has accepted C for this routine.

## Relationship to the other curtains

This routine draws the same kind of curtains as the shared
`model_variant/variant398_curtains.c` routine. That accepted source was used only
as structural evidence. The following changes were recovered from the retail
displacements and instructions:

- **Context offsets.** Fields up to `+0x1A00` in the header-398 form are 0x22C
  higher here, and the angle, colours and phase are 0x230 higher.
- **Records.** The curtains start at `+0x16F8`, the sheet at `+0x774` and the
  polygon at `+0x1B1C`.
- **Curtain count.** There are two curtains rather than three. The phase-4
  completion therefore tests the second curtain.
- **Draw gate.** A strip is drawn only when its depth is positive. There is no
  projection-flag test, and the depth is passed as a 16-bit value.

Three earlier independent reconstructions, which used a local state record,
were 12 to 24 bytes short. They are kept in the ledger.

## Integration

The modules already carry the accepted MODEL464 advancing quads at `+0x18EC`
and sheets at `+0x1E94`. This change converts the `+0x2838` helper from ASM to
C. As recorded in the existing inventory, it is a closed retained helper with no
observed entry-reachable call. Every callee was already bound for MODEL464.

## Acceptance evidence

All six complete 20KiB images match using `gcc_2_8_1_g0_split`. The 11-row
ledger keeps the three rejected experiments, the exact source, the second-slot
compilation and all six terminal full-image results.

Regressions cover:

- the curtain, sheet and packet layouts, including repeated header inclusion
- relocations and callees against every image
- C segment order, statuses and totals in the MODEL464 suite

Terminal dependency fingerprints hash the body, the shared model-variant header
and `gpu_packets.h`, in that order.
