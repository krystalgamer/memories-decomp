# Spanish MODEL443 spinning framebuffer rings

Helper `+0x23AC..+0x298C` is independently reconstructed matching C: 1,504
bytes in both configured header-443/593 MODEL62 loads (stages 9/10), for 3,008
instruction bytes from one unique routine. An independent second-slot
compilation also matches. Screening 6,217 accepted regional C entries found no
same-sized body, and no region has accepted C for this routine.

The accepted MODEL441 framebuffer rings were used as structural evidence only.
Compiled with this family's state, that form is 16 bytes short and differs in
336 words. The state, point grids, spin, colors, transparency, submission gate
and phase numbers were all recovered from the retail body.

The modules already carry the accepted MODEL443 mesh at `+0x298C`. This change
converts the entry-reachable `+0x23AC` helper from ASM to C. The MODEL443
binding list gains seven SDK aliases. Their addresses were already bound under
`func_spanish_*` names:

- `GsGetActiveBuff`
- `ScaleMatrix`
- `GetTPage`
- `SetPolyGT4`
- `SetSemiTrans`
- `SetShadeTex`
- `GsSortPoly`

## Behavior

The state holds five 0x70-byte groups at context `+0xA50`. Each group has a 3x4
`SVECTOR` grid and a progress word at `+0x68`.

The rings are oriented by two angles from `ratan2` over the direction at
`+0x35B8`:

- yaw, folded around 2048 as in the MODEL441 rings;
- pitch, negated into `rotation.vx`.

The spin word at `+0x365C` supplies `rotation.vz`. After all groups are
processed, it decreases by `step * 16`.

A group draws only while its progress is positive:

- It scales by `rsin(progress)` and moves along the direction by
  `(progress - 1024) / 2048` from the `SVECTOR` target at `+0x35B0`.
- Three quads per group project the grid twice. The first projection takes
  front-buffer UVs from a 128-pixel-wide half selected by screen x. The second
  supplies the drawn positions.
- Quads are semi-transparent and untextured-shade. The near vertices are gray
  128. The far vertices take one of eight colors selected by the unsigned frame
  at `+0x3620`.
- Submission requires non-negative depth and flag.

Progress advances by `step * 32`. At 2048 it wraps, or clamps once the phase at
`+0x3698` reaches 6. When all five groups are complete in phase 6, the phase
advances to 7.

## Acceptance evidence

Both complete 20KiB images match using `gcc_2_8_1_g0_split`, with the mesh and
rings in C. The five-row ledger preserves the rejected sibling-structure
experiment, the exact source and second-slot compilations, and both terminal
full-image results.

Regressions cover:

- the state and group layouts, including repeated header inclusion
- relocations and callees against both images
- C segment order, statuses, totals and bindings in the existing MODEL443 suite

Terminal dependency fingerprints hash the body, the private header, the shared
model-variant header and `gpu_packets.h`, in that order.
