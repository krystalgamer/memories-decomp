# Spanish MODEL456 coiled streamers

Helper `+0x34D4..+0x3D00` is matching C. It is 2,092 bytes in all six
header-456/606 MODEL loads registered by the MODEL456 lines change, for 12,552
instruction bytes from one unique routine:

- MODEL163, stages 7/8
- MODEL460, stages 9/10
- MODEL536, stages 9/10

An independent second-slot compilation also matches. Screening 6,217 accepted
regional C entries found no same-sized body, so no region has accepted C for
this routine.

## Relationship to the other streamers

This routine shares the `Variant458Streamer` record and the coil, projection
and draw structure with two other streamer routines:

- the accepted MODEL475 streamers, which are configured for Spanish modules
- the MODEL474 streamers, pending as #7129

Those sources were used only as structural evidence. The following changes
were recovered from the retail displacements and instructions:

- **Context offsets.** Every context offset differs. For example, the streamers
  are at `+0x1AC0`, the polygon at `+0x2204` and the phase at `+0x236C`.
- **Shape.** The coil reach is 192, and the narrow-width override is 1 rather
  than 2.
- **Depth gate.** Submission keeps the positive-depth gate bounded by 0x800.
- **Length.** A pointer to the pulse record at `+0x1580` is taken once. Below
  phase 5, a positive length word at `+0x2348` follows one eighth of the pulse
  size at `+0x88`, clamped at zero.

The pulse pointer is assigned after the reach and step constants. Assigning it
earlier schedules it ahead of the polygon pointer, and the result is 4 or 8
bytes long; those layouts are kept in the ledger.

## Integration

The modules already carry the accepted MODEL456 lines at `+0x315C`. This change
converts the `+0x34D4` helper from ASM to C. As recorded in the existing
inventory, it is a closed contiguous stack-prologue function that is not
entry-reachable. The MODEL456 binding list gains the SDK alias `RotTransPers`,
whose address is already bound as `func_spanish_80087868`.

## Acceptance evidence

All six complete 20KiB images match using `gcc_2_8_1_g0_split`. The 13-row
ledger keeps the five rejected experiments, the exact source and second-slot
compilations, and all six terminal full-image results.

Regressions cover:

- the streamer and packet sizes, including repeated header inclusion
- relocations and callees against every image
- C segment order, statuses, totals and bindings in the MODEL456 lines suite

Terminal dependency fingerprints hash the body, the shared streamer header,
the shared model-variant header and `gpu_packets.h`, in that order.
