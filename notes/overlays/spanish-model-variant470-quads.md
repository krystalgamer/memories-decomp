# Spanish MODEL470 single-group quads

Helper `+0x19F4..+0x1ED0` is independently reconstructed matching C:
1,244 bytes, two physical instances, 2,488 instruction bytes, and one unique
routine. An independent second-slot compilation also matches. Screening 6,191
accepted regional C entries found no same-sized body; this is not a French or
other regional C port.

## Physical loads and ownership

An exhaustive scan identifies MODEL425, record 375, stages 7/8, sectors
103680/103690, with headers 470/620 and distinct complete-image hashes.
Command 636000 selects the 56-byte descriptor at `+0x1FCC`.

Three closed contiguous functions begin at `+4`, `+0x1510` and `+0x19F4`, all
entry-reachable; raw data begins at `+0x1ED0`. Only the selected helper
becomes C, leaving four ASM instances.

## Behavior

The entry stores its context argument only once, to its stack home at `+0xC`,
and reloads it from there for the call at `+0x1380`. That call runs once the
phase at context `+0x2620` is at least two.

The helper is a single-group relative of the MODEL477 two-group quads; its
body was reconstructed independently from this target. One 764-byte group at
context `+0x988` holds ten quads as four corner arrays, an RGB color, and
per-quad size, angle offset, completion and reset words. All quads draw through
the single `POLY_FT4` at `+0x2510`.

Translations orbit the origin by `256 * cos/sin` of per-quad angle offsets
combined with 1300/1700 per-index phases. Sizes above 3072 fade the color by
`(4096 - size) / 1024`. Nonnegative depth and flag, and an unset completion
word, gate 16-bit narrowed submission.

Each size below 4096 grows by `step * 256`. Reaching 4096 completes the quad
only when the descriptor limit at `+0x20` is not later than the current time and
the phase is at least three. Otherwise the reset word is cleared, the size
wraps by 4096, and the angle offset gains 600. A size that falls to zero or
below completes the quad once the same limit is reached. The source keeps the
limit on the left of both unsigned comparisons, as its load order requires.

The completion counter starts at one and is multiplied by each completion word.
After the tenth quad, phase three becomes phase five only when that product
equals one, which holds when every completion word is one. Two `ratan2` results are computed and discarded, as in the target.

## Stack frame

The target frame is 344 bytes. The 16 bytes directly after the 8-byte rotation
are never accessed. Following the accepted repository convention for observed
stack gaps (see `variant432_draw.c` and [Exodia helpers](exodia-helpers.md)),
an unused 16-byte `VECTOR` local declared after the rotation retains that gap.
No runtime role is asserted for it.

## Acceptance evidence

Both complete 20KiB images match using `gcc_2_8_1_g0_split`, the authoritative
GCC 2.8.1/MASPSX 2.81 pipeline. No forced registers or inline assembly are
present. The five-row ledger preserves the rejected first experiment, the exact
source and second-slot compilations, and both terminal full-image results.

Regressions cover 31 layouts, repeated header inclusion, all physical loads
and descriptors, the single stack-home context store, every selected
relocation, all input/final function and data owners, and 34 resident
bindings with loader/context ownership. The private state is only the observed
prefix through `+0x2624`; its stored pointer uses `*G32`.

Terminal dependency fingerprints hash the body, private header, shared
model-variant header, and `gpu_packets.h`, in that order.
