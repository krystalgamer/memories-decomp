# Spanish MODEL478 orbiting quads

Helper `+0x1898..+0x1DA0` is independently recovered matching C: 1,288 bytes in
all four configured header-478/628 MODEL loads (MODEL272 and MODEL636, stages
9/10), for 5,152 instruction bytes from one unique routine. An independent
second-slot compilation also matches.

Screening 6,217 accepted regional C entries compared five same-size bodies. All
are unrelated dispatch code, and none has this normalized shape. No region has
accepted C for this routine.

The accepted MODEL470 quads were used only as structural evidence. With that
form, the routine is 44 bytes short and 237 words differ. The state layout,
orbit radius and shrink-completion gate were recovered from the retail body.

The modules already carry the accepted MODEL478 petal rings at `+0x1DA0`. This
change converts the entry-reachable `+0x1898` helper from ASM to C. Every SDK
call was already bound in the MODEL478 binding list.

## Behavior

The state starts with one 0x2FC-byte quad group. The group holds:

- four 10-element corner arrays
- a color at `+0x190`
- sizes, angles, done flags and reset flags at `+0x194`, `+0x1BC`, `+0x1E4` and
  `+0x234`

The `POLY_FT4` is at context `+0x2948`. Two `ratan2` results, taken over the
direction at `+0x29BC` and the projection at `+0x29CC`, are discarded.

Each of the ten quads with a non-negative size is drawn as follows:

- It orbits the `SVECTOR` origin at `+0x29B4` with radius 192. The base angle
  advances by 1300, the ripple by 1700, and each quad adds its own angle
  offset.
- It is uniformly scaled by its size. Above 3072, it fades out over the last
  1024 units.
- It is submitted while depth and flag are non-negative and the quad is not
  done.

Sizes grow by `step * 256`:

- On reaching 4096, a quad becomes done once the descriptor time limit is
  reached and the phase at `+0x2A24` is at least 3. Otherwise it restarts with
  its angle advanced by 600.
- A non-positive size becomes done at 4096 only when the phase is at least 3
  and the limit has been reached.
- When every quad is done in phase 3, the phase becomes 5.

## Source detail that reproduces the target

The polygon pointer is initialized in its declaration. Assigning it later
leaves the polygon and inner-index registers swapped. The other layouts that
were tried also left that swap; all of them are kept in the ledger.

The 16-byte gap after the rotation is reproduced by the same unreferenced
`VECTOR` used in the accepted MODEL470 quads.

## Acceptance evidence

All four complete 20KiB images match using `gcc_2_8_1_g0_split`. The 12-row
ledger records:

- the six rejected experiments
- the exact source compilation and the second-slot compilation
- all four terminal full-image results

Regressions cover:

- the group, timing and state layouts, including repeated header inclusion
- relocations and callees against every image
- C segment order, statuses and totals in the existing MODEL478 suite

Terminal dependency fingerprints hash the body, the private header, the shared
model-variant header and `gpu_packets.h`, in that order.
