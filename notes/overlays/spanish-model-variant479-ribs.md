# Spanish MODEL479 ribs

Helper `+0x1F68..+0x2B90` is matching C. It is 3,112 bytes in both
header-479/629 MODEL loads (MODEL379, stages 7/8) of the Spanish release and in
the same two French images, which carry the identical body at the same offsets.
`variant479_ribs.c` is compiled once per slot with `gcc_2_8_1_g0_split`; the
slot-1 wrapper only renames `func_8013CF68` to `func_8017CF68`. This is a new
decompilation; no region had accepted C for it.

## Behaviour

Five 0x250-byte rib records start at `+0x18D8`. For each rib with a positive
size, nine spine points run either from the origin toward the anchor at
`+0x40F8 + j * 16` scaled by size/1024 (state below 2), or from fade/1024 of
the anchor to the anchor (state 2 and above). Each point is bent by
`rsin(len)` along the rib angle, and a copy is moved along the view by the
spread (`+0x4190 / 16`, or `* 72 / 1024` when bit 0 of `+0x4164` is set). Both
rows are projected at the position `+0x4024 + (j % 3) * 32` and every segment
draws two POLY_GT4 halves offset by the projected width where the depth and
the projection flag are not negative.

Size grows by `step * 32` to 0x400, setting state 2 and moving phase 2 to 3.
A state-2 rib snaps to fade 0x400 and state 3. The ripples at
`+0x41DC/+0x41E0` advance by `step * 850` and `step * 128`. Once the clock at
`+0x4168` passes the timing record's `+0x24`, `+0x4190` falls linearly from
0x400 over `+0x28 - +0x24`, moving phase 4 to 5 at zero.

## What reproduces the target

- The fade branch writes size, then fade, then state, so loop strength
  reduction bases the rib-gate pointer on `&state`.
- `n = i` before the spine `setVector` and `a[k = n]` with `dz * k` give the
  target's index copy pair.
- The state-2 branch computes `dx`, `dz`, `dy` in that order with `len` last,
  which fixes the spill choices.
- The clock test is written `timing[0x24] <= work[0x4168]`. With the operands
  the other way round, sched1 keeps the second step load after the `+0x41DC`
  store.

The ledger `spanish-model-variant479-ribs-attempts.csv` records the candidates
and the four exact full-image results.
