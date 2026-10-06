# Spanish MODEL411 fading sheets

Helper `+0x1E58..+0x24A4` is independently reconstructed matching C:
1,612 bytes, four physical instances, 6,448 instruction bytes, and one unique
routine. An independent second-slot compilation also matches. Screening 6,191
accepted regional C entries found 60 same-sized bodies, none with the same
normalized shape; no region has accepted C for this routine.

The routine is closely related to the
[MODEL444 fading sheets](spanish-model-variant444-sheets.md) in the same
regional campaign, but it is a distinct function. Its state keeps the movement
progress at `+0x1658` rather than `+0x165C`, and it scales depth by nine tenths
rather than eight tenths. The source declares MODEL411's own state view and was
verified independently against all four MODEL411 retail images.

## Physical loads and ownership

An exhaustive scan identifies four loads with headers 411/561 and four distinct
complete-image hashes:

| Model | Record | Stages | Sectors | Command | Descriptor |
|---:|---:|---:|---:|---:|---:|
| 364 | 314 | 9/10 | 86864/86874 | 577002 | `+0x2B10` |
| 452 | 402 | 9/10 | 111152/111162 | 577001 | `+0x2AE0` |

The entry indexes 48-byte descriptors from `+0x2AB0`. Five closed contiguous
functions begin at `+4`, `+0xE60`, `+0x157C`, `+0x1E58` and `+0x24A4`; raw data
begins at `+0x29B4`. The function at `+0x157C` is not entry-reachable. Only the
selected helper becomes C, leaving sixteen ASM instances.

## Behavior

Entry `+0xCDC` passes the original context captured in `s2` at `+0xC`. The
entry redefines `s2` eight other times, but only the `+0xC` definition reaches
this call.

Two initialized `0x98`-byte sheets occupy context `+0x54C..+0x67C` and draw four
quads each through the second initialized `POLY_GT4` at `+0x1478`. The first
sheet stays at the origin; the second interpolates along the path by the
progress at `+0x1658`. Odd frames add one eighth of the signed size. From phase
three onward, the moving sheet scales its inner and outer RGB components by
`fade / 512` while fade is below 512. Depth is scaled by nine tenths, then
nonnegative depth and flag gate 16-bit narrowed submission.

The sheet phases follow the target exactly. That includes the asymmetric late
growth, which updates only while size is below 16384 but clamps its computed
value at 24576.

## Acceptance evidence

All four complete 20KiB images match using `gcc_2_8_1_g0_split`, the
authoritative GCC 2.8.1/MASPSX 2.81 pipeline. The natural 256-byte frame needs
no unused locals, forced registers or inline assembly. The six-row ledger
records the exact source and second-slot compilations and all four terminal
full-image results.

Regressions cover 37 layouts, repeated header inclusion, all physical loads
and descriptors, original-context reaching definitions, every selected
relocation, all input/final function and data owners, and 35 resident bindings
with loader/context ownership. The private state is only the observed prefix
through `+0x1680`; its stored pointer uses `*G32`.

Terminal dependency fingerprints hash the body, private header, shared
model-variant header, and `gpu_packets.h`, in that order.
