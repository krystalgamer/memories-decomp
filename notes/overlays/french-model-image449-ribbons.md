# French header-449 image ribbons helper

The `0x95C` helper at image offset `0x1240` is first-ever game code: no release
had matching C for its body. Four French images carry it: MODEL715 stages 9/10
(`169940`, `169950`) and MODEL370 stages 7/8 (`88500`, `88510`). Slot 0 owns
`func_8013C240` from `variant449_ribbons.c`; slot 1 owns `func_8017C240`
through a rename-only wrapper.

Four `0x380`-byte ribbons start at work offset `0`. Each record keeps the
MODEL400 ribbon fields: points, projected words, angles, offset points,
widths, colour, position/delta, depths, and screen offsets at `32C/34E`.
It then adds `start`, `end`, `state` and `count` words at `370..37C`. The view
reuses the header-449 fields:
- two `POLY_FT4` packets at `1AA8` and the transform centre at `1B54`;
- a direction vector at `1B80` and axis words at `1BC4`;
- `elapsed`, `step` and the timing pointer at `1BE0/1BE8/1BF4`;
- `spin` at `1C1C` and two ripple phases at `1C34/1C38`.

The first pass builds 17 points per ribbon:
- each point sits at `direction * k / 16`;
- its `y`/`z` are displaced by a bend: an alternating-parity sway angle (base
  `i * 1024`, `k * 384` steps) times a +1300 phase;
- offset points use the turn angle `ratan2(axis_z, axis_x) + 3072` at length 16.

One transform from the centre projects all ribbons in the MODEL400 shape
(points 15/16 for the end point). Each ribbon then draws from `max(start, 0)`
to `max(end, start)` through alternating packets. A per-ribbon state machine
follows: even ribbons grow `end` then `start`, odd ribbons shrink `start`
then `end`, by `step` up to 16 or down to 0. On completion each restarts and
counts while `elapsed` is below the timing record's `scale_end`; otherwise it
settles in state 2. The two ripple phases and `spin` advance afterwards.

Exactness depended on three source details:
- the draw limit is written `if (end < first) first else end`;
- the odd ribbons use an else-if chain;
- the timing test is written `scale_end <= elapsed` with the settling branch
  first.

The last choice keeps the shared `count` increment as the final field
reference, which the retail induction base reflects.
