# Spanish MODEL478 petal rings

Helper `+0x1DA0..+0x2220` is independently reconstructed matching C:
1,152 bytes, four physical instances, 4,608 instruction bytes, and one unique
routine. An independent second-slot compilation also matches. Screening 6,191
accepted regional C entries found eight same-sized bodies, none with the same
normalized shape; this is not a French or other regional C port.

## Physical loads and ownership

An exhaustive scan identifies four loads with headers 478/628 and four distinct
complete-image hashes:

| Model | Record | Stages | Sectors | Command |
|---:|---:|---:|---:|---:|
| 272 | 272 | 9/10 | 75272/75282 | 644000 |
| 636 | 586 | 9/10 | 161936/161946 | 644000 |

The entry indexes 48-byte descriptors at `+0x231C`. Four closed contiguous
functions begin at `+4`, `+0x134C`, `+0x1898` and `+0x1DA0`, all
entry-reachable; raw data begins at `+0x2220`. Only the selected helper
becomes C, leaving twelve ASM instances.

## Behavior

Entry `+0x1178` passes the original context captured in `s2` at `+0xC`. The
entry redefines `s2` eleven other times, but only the `+0xC` definition
reaches this time-gated call.

Three 612-byte rings start at context `+0x2FC`. Each holds eight quads as four
corner arrays, an RGB color, and per-quad size and hidden words. All quads
draw through the single `POLY_FT4` at `+0x2948`.

Each ring has a tilt of `i * 1024 / 3` and a phase that advances by 1400; each
quad adds `j * 512` to the phase. For nonnegative sizes, the size is capped at
4096 to place the quad on a tilted circle with radius
`192 * cos(tilt) * size / 4096` and height `128 * sin(tilt) * size / 4096`,
lifted by 32. The scale adds a wave term of `256 * sin(wave + angle + phase)`.
Nonnegative depth and flag, and a zero hidden word, gate submission.

Before the descriptor's unsigned growth end at `+0x28` each size grows by
`step * 48`; after the fade start at `+0x2C` it shrinks by the same amount and
clamps at zero. The shared wave advances by `step * 32`. Two `ratan2` results
are computed and discarded, as in the target.

## Stack frame

The target frame is 320 bytes. With scalar RGB lifetimes, every non-stack
instruction already matched with the natural 304-byte frame; the only remaining
differences were 83 SP displacements of 16 bytes, starting directly after the
8-byte rotation. Following the accepted repository convention for observed
stack gaps (see `variant432_draw.c` and [Exodia helpers](exodia-helpers.md)),
an unused 16-byte `VECTOR` local declared after the rotation retains that gap.
No runtime role is asserted for it.

## Acceptance evidence

All four complete 20KiB images match using `gcc_2_8_1_g0_split`, the
authoritative GCC 2.8.1/MASPSX 2.81 pipeline. No forced registers or inline
assembly are present. The nine-row ledger preserves all three rejected
experiments, the source and second-slot compilations, and all four terminal
full-image results.

Regressions cover 30 layouts, repeated header inclusion, all physical loads
and descriptors, original-context reaching definitions, every selected
relocation, all input/final function and data owners, and 34 resident bindings
with loader/context ownership. The private state is only the observed prefix
through `+0x2A20`; its stored pointer uses `*G32`.

Terminal dependency fingerprints hash the body, private header, shared
model-variant header, and `gpu_packets.h`, in that order.
