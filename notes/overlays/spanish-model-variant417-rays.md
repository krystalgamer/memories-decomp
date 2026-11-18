# Spanish MODEL417 retained rays

Helper `+0x2480..+0x2ACC` is matching C: 1,612 bytes in all eight configured
header-417/567 MODEL loads (MODEL134, 232 and 535, stages 9/10; MODEL354,
stages 7/8), for 12,896 instruction bytes from one unique routine. An
independent second-slot compilation also matches.

Screening 6,217 accepted regional C entries compared 64 same-size bodies
(MODEL435 and MODEL442 ribbons, MODEL411 sheets). None has this normalized
shape, so no region has accepted C for this routine.

## Relationship to the MODEL441 rays

The body has the same normalized shape as the MODEL441 retained rays (pending
as #7122). Compiled with the MODEL441 state, the independently reconstructed
MODEL441 source reproduces every instruction except 32 state displacements.
Each of those is exactly 0x5AC lower in this family.

This module family therefore needs its own state layout: a 0xA98-byte prefix
in place of 0x1044, with every later field at the same relative offset. The
routine was first matched through a private view, `Rays417State`, and is
duplicated under its own name, as the repository already does for the MODEL417
and MODEL441 lines.

The drawer now takes `Variant417EntryState` directly (#9559), and every view
member sits on an entry member:

- the pulse size is `sheets[0].size` (the view's pulse only named a size at
  +0x88, which is `ModelVariantSheet.size`);
- the rays are the entry's `ModelVariantOffsetRay rays[8]`;
- the timing is the entry's config, read for `config->count`;
- the base angle is `ray_angle`;
- the mirror flag is the entry's `slot`: the rays are drawn mirrored for slot 1.

## Behavior

The behavior is the same as the MODEL441 rays, at these `Variant417EntryState`
offsets:

- eight 0x6C-byte rays at `+0xBC8` (`rays`)
- a pulse size at `+0xB20` (`sheets[0].size`)
- a `POLY_G3` arrowhead at `+0x1F80` (`packets.triangle`)
- origin, direction and secondary direction words at `+0x20E4`, `+0x20F8` and
  `+0x210C` (`matrix.t`, `aim.direction`, `aim.view_delta`)
- frame `+0x2124`, step `+0x2130`, timing `+0x2138`, index `+0x214C`
  (`frame_count`, `step`, `config`, `index`)
- progress `+0x2168`, base angle `+0x216C`, mirror flag `+0x2184`
  (`progress`, `ray_angle`, `slot`)

## Integration

The modules already carry the accepted lines at `+0x1204`. This change
converts the `+0x2480` helper from ASM to C. As recorded in the existing inventory,
it is a closed contiguous stack-prologue function that is not entry-reachable. The MODEL417
binding list gains the SDK aliases `RotTransPers`, `rsin` and `rcos`. Their
addresses are already bound under `func_spanish_*` names, and the MODEL441,
MODEL442 and MODEL445 lists use the same pairing.

## Acceptance evidence

All eight complete 20KiB images match using `gcc_2_8_1_g0_split`. The
eleven-row ledger keeps:

- the shared-state experiment, with 32 displacement differences
- the exact source compilation
- the second-slot compilation
- all eight terminal full-image results

Regressions cover:

- layouts, including repeated header inclusion
- relocations and callees against every image
- C segment order, statuses, totals and bindings in the existing MODEL417
  suite

Terminal dependency fingerprints hash the body, the private header, the shared
model-variant header and `gpu_packets.h`, in that order.
