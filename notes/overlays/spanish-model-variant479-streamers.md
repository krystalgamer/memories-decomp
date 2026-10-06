# Spanish MODEL479 coiled streamers

Helper `+0x309C..+0x3860` is matching C. It is 1,988 bytes in both
header-479/629 MODEL loads (MODEL379, stages 7/8), for 3,976 instruction bytes
from one unique routine. An independent second-slot compilation also matches.
Excluding this change, the screen checked 6,263 configured regional C entries
and found no body of the same size, so
no region has accepted C for this routine.

## Behaviour

The routine builds two seventeen-point streamers that coil around the variant's
axis with a reach of 192. It projects each spine and a copy of it moved along
the view, and draws each segment as a POLY_G4 as wide as the projected offset
where the depth is positive. The spin at `+0x41C8` advances by 0x20 a frame and
by 0x514 every tenth frame.

## Relationship to the other streamers

The same routine, with different context offsets, is also carried by MODEL423,
MODEL427 and MODEL473 at stages 7–10 of MODEL379 and MODEL385. The source
follows the coiled-streamer structure recovered for MODEL474, which is pending
as #7129 and uses the same 0x334-byte `Variant458Streamer` record. That source
was used only as structural evidence. The following were recovered from the
retail body:

- this family's offsets
- the 192 reach
- the positive-depth gate
- the absence of any shrinking tail

## Integration

This change converts the `+0x309C` helper from ASM to C, next to the accepted
MODEL479 sheets and curtains. As recorded in the existing inventory, it is a
closed retained helper with no observed entry-reachable call. The MODEL479
binding list gains two SDK aliases whose addresses were already bound as
`func_spanish_*` names:

- `ratan2`
- `RotTransPers`

## Acceptance evidence

Both complete 20KiB images match using `gcc_2_8_1_g0_split`. The ledger keeps
the exact source, the second-slot compilation and both terminal full-image
results.

Regressions cover:

- the streamer and packet sizes, including repeated header inclusion
- relocations and callees against both images
- C segment order, statuses, owners, totals and bindings in the MODEL479 and
  sheet suites

Terminal dependency fingerprints hash the body, the shared streamer header, the
shared model-variant header and `gpu_packets.h`, in that order.
