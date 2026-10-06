# Spanish MODEL388 ribbons

Helper `+0x1344..+0x19A4` is matching C. It is 1,632 bytes in all ten
header-388/538 MODEL loads registered by the MODEL388 feedback change, for
16,320 instruction bytes from one unique routine:

- MODEL8, stages 7/8
- MODEL43, stages 9/10
- MODEL235, stages 9/10
- MODEL269, stages 9/10
- MODEL706, stages 9/10

An independent second-slot compilation also matches. The screen checked 6,223
configured regional C entries. Two had the same size, and neither had the same
shape, so no region has accepted C for this routine.

## Relationship to the other ribbons

This routine draws the same kind of ribbons as the shared
`model_variant/variant418_ribbons.c` routine. That accepted source was used only
as structural evidence. The following changes were recovered from the retail
displacements and instructions:

- **Context offsets.** The work fields are 0x300 lower, and the ribbons start at
  `+0x228`. The timing record is at `+0xF8` and the primitive at `+0x1680`.
- **Matrix translation.** The translation scales each direction by the
  half-word at `+0x1890`.
- **Projection flags.** Each `RotTransPers4` flag is stored in the ribbon at
  `+0x5C`. The draw pass submits a ribbon only when both its depth and its flag
  are non-negative.
- **Frame test.** The test compares the word at `+0x1880` with the frame count
  at `+0xC` of the timing pointer at `+0x186C`. The fan then turns by the step
  at `+0x1864` times 32.

The 0x74-byte record is declared as `Ribbon388` in `variant388_ribbons.h`,
because this family's flag words are the only part of the shared record's
unknown region that it uses.

## Integration

The modules already carry the accepted MODEL388 feedback strips at `+0x1E9C`.
This change converts the `+0x1344` helper from ASM to C. As recorded in the
existing inventory, it is a closed contiguous stack-prologue function that is
direct-entry reachable. The MODEL388 binding list gains two SDK aliases whose
addresses are already bound as `func_spanish_*` names:

- `rcos`
- `RotTransPers`

## Acceptance evidence

All ten complete 20KiB images match using `gcc_2_8_1_g0_split`. The 12-row
ledger keeps the exact source, the second-slot compilation and all ten terminal
full-image results.

Regressions cover:

- the record layout and packet size, including repeated header inclusion
- relocations and callees against every image
- C segment order, statuses, totals and bindings in the MODEL388 feedback suite

Terminal dependency fingerprints hash the body, the ribbon header, the shared
model-variant header and `gpu_packets.h`, in that order.
