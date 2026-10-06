# Spanish MODEL459 timed bands

Helper `+0x3224..+0x3A24` is matching C. It is 2,048 bytes in all twelve
header-459/609 MODEL loads, for 24,576 instruction bytes from one unique
routine:

- MODEL47, MODEL238, MODEL411 and MODEL620, stages 7/8
- MODEL231 and MODEL417, stages 9/10

An independent second-slot compilation also matches. Excluding this change, the
screen checked 7,459 configured regional C entries and found no body of the
same size, so no region has accepted C for this routine.

## Behaviour

The timing record at `+0x309C` supplies:

- the band count, a half-word at `+0x24`
- a size, a half-word at `+0x2A`

For every band, the routine builds nine three-point cross-sections. Their
radius is that record size scaled by the shrinking half-word at `+0x30CC`; when
flag bit 0 is set, the size is first raised by `rsin(0x100)` of itself. Each
band is translated from its own 16-byte position at `+0x2F8C` along its own
direction at `+0x2FEC`, by the growth word at `+0x30D0`. The bands are projected
with `RotTransPers3` and drawn as paired POLY_GT4 strips.

The tail has two stages:

1. During phase 1, it grows the bands until phase 2.
2. It then shrinks them over the record's fade window until phase 3.

## Relationship to the other bands

This is a multi-band form of the accepted Spanish
`spanish_model_variant/variant415_bands.c`, which was used only as structural
evidence; the accepted `ModelVariantBand` record is reused. The following were
recovered from the retail body:

- the timing-record radius and band count
- the per-band translations
- the offsets
- the record's growth and fade windows

The ledger keeps nine rejected forms. Two details were required:

- The angle is declared last, which gives the retail stack slots.
- Each per-band translation is read at `work + (i << 4)`, which gives the
  retail operand order of the base address.

## Integration

This change converts the `+0x3224` helper from ASM to C, next to the accepted
MODEL459 sheets. As recorded in the existing inventory, it is entry-reachable.
The MODEL459 binding list gains three SDK aliases whose addresses were already
bound as `func_spanish_*` names:

- `rcos`
- `rsin`
- `RotTransPers3`

## Acceptance evidence

All twelve complete 20KiB images match using `gcc_2_8_1_g0_split`. The ledger
keeps every experiment, the exact source, the second-slot compilation and all
twelve terminal full-image results.

Regressions cover:

- the band record and packet layouts, including repeated header inclusion
- relocations and callees against every image
- C segment order, statuses, totals and bindings in the MODEL459 suite

Terminal dependency fingerprints hash the body, the shared model-variant header
and `gpu_packets.h`, in that order.
