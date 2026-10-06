# Spanish MODEL474 coiled streamers

Helper `+0x30D0..+0x38BC` is matching C. It is 2,028 bytes in all four
configured MODEL474 loads, for 8,112 instruction bytes from one unique routine:

- MODEL121, stages 7/8
- MODEL431, stages 9/10

An independent second-slot compilation also matches. Screening 6,217 accepted
regional C entries found no same-sized body, so no region has accepted C for
this routine.

## Relationship to the accepted MODEL475 streamers

Structurally, this routine is a close sibling of the accepted MODEL475
streamers in `french_model_variant/variant475_streamers.c`. That source is
configured for both the French and the Spanish MODEL475 modules. The two
routines share the shared `Variant458Streamer` record and the overall coil,
projection and draw structure.

The MODEL475 source compiled unchanged here is 44 bytes too long and differs
in 78 words. Reproducing this routine required the following changes, all
recovered from the retail displacements and instructions:

- every context offset moves; for example, the streamers are at `+0x12C8`, the
  polygon at `+0x1ADC` and the phase at `+0x1C1C`
- the coil reach is 320 rather than 160
- submission requires only positive depth, with no upper bound of 0x800
- the final block differs: from phase 5 on, the length word at `+0x1BF0` is
  reset from the word at `+0x1BD4`; the sibling's phase-4 shrink is absent

The function is therefore a distinct routine. The sibling served only as
structural evidence; this is not a regional port of the same function.

## Integration

The modules already carry the accepted MODEL474 mesh and sheets. This change
converts the `+0x30D0` helper from ASM to C. As recorded in the existing
inventory, no entry-reachable call to it has been observed; it is a closed
retained helper.

The MODEL474 binding list gains the SDK aliases `RotTransPers`, `rsin` and
`rcos`. Their addresses are already bound under `func_spanish_*` names.

## Acceptance evidence

All four complete 20KiB images match using `gcc_2_8_1_g0_split`. The
seven-row ledger keeps the unchanged-sibling experiment, the exact source and
second-slot compilations, and all four terminal full-image results.

Regressions cover:

- the streamer and packet sizes, including repeated header inclusion
- relocations and callees against every image
- C segment order, statuses, totals and bindings in the existing MODEL474 suite

Terminal dependency fingerprints hash, in order:

1. the body
2. the shared streamer header
3. the shared model-variant header
4. `gpu_packets.h`
