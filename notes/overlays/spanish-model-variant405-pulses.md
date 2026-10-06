# Spanish MODEL405 paired pulses

Helper `+0xD00..+0x12CC` is independently reconstructed matching C: 1,484
bytes in all eight configured header-405/555 loads (MODEL57, 149, 419 and 562,
stages 7-10), for 11,872 instruction bytes from one unique routine. An
independent second-slot compilation also matches. Screening 6,191 accepted
regional C entries found only the unrelated NTSC MODEL450 quads at the same size
(both slots), and they do not match; no region has accepted C for this routine.

The modules already carry the accepted bands at `+0x1774` and outer bands at
`+0x1B64`. This change converts the `+0xD00` helper from generated ASM to C.
The entry (`+4`), the helper at `+0x12CC` and the raw tail remain unchanged.
The entry calls the helper directly from `entry+0xB1C`.

## Behavior

The state holds three 0x74-byte primary records at context `+0`, followed by
six `ModelVariantSheetSet` groups at `+0x15C`. `2 * count` groups are processed,
in pairs. Each visible group draws four quads through the `POLY_GT4` at
`+0x2DD4`.

- Even groups sit at the origin at `+0x31D0`; odd groups sit at the `SVECTOR`
  target at `+0x31DC`.
- On odd frames, the uniform scale pulses by one eighth of the size.
- Even groups fade linearly above 2048 over the next 2048 size units. Odd groups
  fade above 4096 over the next 4096. Inner colors set vertices 0-2 and the
  outer color sets vertex 3.
- Depth is computed as `depth * 8 / 10`. Submission requires non-negative
  depth and flag.
- Even groups grow by `step * 512` to 4096. They set the paired primary's
  `started` flag at 2048 and are marked shown at the clamp.
- Odd groups grow by `step * 512` to 8192 once the paired primary's progress
  reaches 1024.

## Stack frame

The target frame is 288 bytes, and nothing in the routine accesses the 16
bytes directly after the 8-byte rotation. Without a declaration there, every
other instruction word already matches: the only differences are 76
same-register stack displacements shifted by 16 bytes, because the frame is
272 bytes.

GCC 2.8.1 keeps the frame slot of an unreferenced aggregate local. Any used
local in that position would add stores that retail does not have. An
unreferenced `VECTOR` declared after the rotation therefore reproduces the
original frame. The same construct is used in accepted sources:
`model_exodia/entry.c`, `model_exodia/spokes.c` and `variant432_draw.c`.
There are no forced registers, inline assembly or profile changes.

## Acceptance evidence

All eight complete 20KiB images match using `gcc_2_8_1_g0_split`. The
fourteen-row ledger preserves the four rejected experiments, the exact source
and second-slot compilations, and all eight terminal full-image results.

Regressions cover:

- the state and record layouts, including repeated header inclusion
- selected relocations and callees against every image
- C segment order, function statuses and per-module totals in the existing
  MODEL405 suite
- final function owners

Terminal dependency fingerprints hash the body, the private header, the shared
model-variant header and `gpu_packets.h`, in that order.
