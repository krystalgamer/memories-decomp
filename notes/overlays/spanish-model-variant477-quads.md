# Spanish MODEL477 two-group quads

Helper `+0x1958..+0x1DC0` is independently reconstructed matching C:
1,128 bytes, six physical instances, 6,768 instruction bytes, and one unique
routine. An independent second-slot compilation also matches. Screening 6,191
accepted regional C entries found no same-sized body; this is not a French or
other regional C port.

## Physical loads and ownership

An exhaustive scan identifies six loads with headers 477/627 and six distinct
complete-image hashes:

| Model | Record | Stages | Sectors | Command |
|---:|---:|---:|---:|---:|
| 117 | 117 | 9/10 | 32492/32502 | 643001 |
| 262 | 262 | 7/8 | 72492/72502 | 643000 |
| 631 | 581 | 7/8 | 160536/160546 | 643000 |

The entry indexes 56-byte descriptors at `+0x1EBC` by its command argument.
Three closed contiguous functions begin at `+4`, `+0x1484` and `+0x1958`,
all entry-reachable; raw data begins at `+0x1DC0`. Only the selected helper
becomes C, leaving twelve ASM instances.

## Behavior

The entry stores its context argument only once, to its stack home at `+0xC`,
and reloads it from there for the call at `+0x12F4`. That call runs once the
phase at context `+0x20A8` is at least two.

Two 764-byte quad groups start at context `+0x118`. Each holds ten quads as four
corner arrays, an RGB color, and per-quad size, angle offset and completion
words. All quads draw through the single `POLY_FT4` at `+0x1F9C`.

For every quad with nonnegative size, the translation orbits the origin by
`128 * cos/sin` of the angle offset combined with two per-index phases that
advance by 1300 and 1700. Sizes above 3072 fade the color by
`(4096 - size) / 1024`. Nonnegative depth and flag, and size below 4096, gate
submission. While its group is enabled, each size grows by `step * 256` up to
4096 and then marks the quad done.

A completion counter starts at one and accumulates the done words across both
groups. At each group's last quad, phase three becomes phase five only when the
counter equals exactly two. This target behavior is preserved as compiled,
not reinterpreted as an all-done test. Two `ratan2` results are computed and
discarded, also as in the target.

## Stack frame

The target frame is 344 bytes. Every non-stack instruction already matched
with the natural 328-byte frame; the only remaining differences were 86 SP
displacements of 16 bytes, starting directly after the 8-byte rotation.
Following the accepted repository convention for observed stack gaps (see
`variant432_draw.c` and [Exodia helpers](exodia-helpers.md)), an unused 16-byte
`VECTOR` local declared after the rotation retains that gap. No runtime role is
asserted for it.

## Acceptance evidence

All six complete 20KiB images match using `gcc_2_8_1_g0_split`, the
authoritative GCC 2.8.1/MASPSX 2.81 pipeline. No forced registers or inline
assembly are present. The ten-row ledger preserves both rejected experiments,
the source and second-slot compilations, and all six terminal full-image
results.

Regressions cover 29 layouts, repeated header inclusion, all physical loads
and descriptors, the single stack-home context store, every selected
relocation, all input/final function and data owners, and 34 resident
bindings with loader/context ownership. The private state is only the observed
prefix through `+0x20AC`.

Terminal dependency fingerprints hash the body, private header, shared
model-variant header, and `gpu_packets.h`, in that order.
