# Spanish MODEL426 pulsed sheets

Helper `+0x26D0..+0x2BC0` in the configured MODEL426 images
`spanish_model_variant_0_stage7_slot0` and `spanish_model_variant_0_stage8_slot1`
is independently reconstructed matching C: 1,264 bytes, two physical instances,
2,528 instruction bytes. An independent second-slot compilation also matches.
Screening 6,191 accepted regional C entries found no same-sized body.

These are the same two physical loads that carry the accepted
[MODEL426 nine-row mesh](spanish-model-variant426.md); this change converts one
more of their ASM functions to C and leaves the other five, including the
unreferenced `+0x3440` helper, as generated assembly.

## Behavior

The helper is direct-entry reachable and shares its body plan with the accepted
MODEL423 sheets. Two initialized `0x98`-byte sheets start at context `+0xE1C`
and draw four quads each through the `POLY_GT4` at `+0x1FC4`. The first sheet
stays at the origin; the second is offset by direction times the progress
`/ 1024`. Odd frames add one eighth of the signed size to the scale. Depth is
scaled by eight tenths, then nonnegative depth and flag gate submission.

The first sheet grows to 4096 in phase zero over the unsigned descriptor times
`+0x18..+0x1C`, then sets phase one. The second sheet is fixed at 4096 in phases
one and two and grows by `step * 512` to 8192 in phase three, then sets phase
four. Both sheets shrink linearly over the unsigned descriptor times
`+0x28..+0x2C`, clamping at zero.

## Source details that reproduce the target

MODEL426 differs from the MODEL423 sheets in two ways that the compiler makes
visible:

- the second sheet's fixed size applies to phases one and two, compiled as an
  unsigned range test;
- each shrink computes its unsigned quotient first and then subtracts it from
  the sheet's limit.

The second detail gives the quotient its own lifetime. That lets the compiler
cross-jump the two shrink tails into the shared sequence found in retail. No
unused locals, forced registers or inline assembly are present, and the
256-byte frame is natural.

## Acceptance evidence

Both complete 20KiB images match with the mesh and sheets both compiled from C,
using `gcc_2_8_1_g0_split`. The nine-row ledger preserves all five rejected
experiments, the exact source and second-slot compilations, and both terminal
full-image results. Regressions cover the private state layout, both images'
function inventories and `matching_c` entries, every selected call and
local-jump relocation, and the updated mesh test that now expects both C
segments.
