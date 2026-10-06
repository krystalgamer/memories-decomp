# Spanish MODEL446 paired pulses

Helper `+0x1FE8..+0x24BC` is independently recovered matching C: 1,236 bytes
in both configured MODEL146 header-446/596 loads (stages 9/10), for 2,472
instruction bytes from one unique routine. An independent second-slot
compilation also matches.

Screening 6,217 accepted regional C entries compared one same-size body,
unrelated dialog code. No region has accepted C for this routine.

The accepted MODEL385 pulse was used as structural evidence only. Compiled with
that form, the routine is 40 bytes too long and differs in 184 words. The state
layout, raw depth handling and fade range were recovered from the retail body.

The modules already carry the accepted MODEL446 lines and strip. This change
converts the entry-reachable `+0x1FE8` helper from ASM to C. Every SDK call was
already bound.

## Behavior

Two 0x90-byte pulse groups at context `+0x1130` each hold a 4x4 `SVECTOR` grid
and a size. They draw four quads each through the `POLY_GT4` at `+0x19E4`.

- The first group sits at the origin at `+0x1B30`. The second is advanced by
  the halfword progress along the direction at `+0x1B64`.
- The uniform scale pulses by one eighth of the size on odd frames.
- Colors are blue-gray on three corners and gray on the fourth.
- Projected depth is used raw. A quad is submitted only at positive depth,
  with an ordering bias of -32. The flag is not tested.

On the last timing slot, the group sizes are updated as follows.

**First group:**

- In phase zero, it grows across the descriptor's start/end window to 4096 and
  advances the phase to 1.
- From the fade start, it shrinks linearly from 1024 over the unsigned fade
  window. At zero, phase 3 advances to 4.

**Second group** follows the phases:

- Phases up to 0: size 0.
- Phase 1: size 4096.
- Phase 2: grows by `step * 1024` to 8192, then advances to phase 3.
- Phase 4: shrinks by `step * 64` to 0, then advances to phase 5.

## Acceptance evidence

Both complete 20KiB images match using `gcc_2_8_1_g0_split`, with the lines,
strip and pulse in C. The five-row ledger keeps:

- the rejected sibling-structure experiment
- the exact source and second-slot compilations
- both terminal full-image results

Regressions cover:

- the group, timing and state layouts, including repeated header inclusion
- relocations and callees against both images
- C segment order, statuses and totals in the existing MODEL446 suite

Terminal dependency fingerprints hash the body, the private header, the shared
model-variant header and `gpu_packets.h`, in that order.
