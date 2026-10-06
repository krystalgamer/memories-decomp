# Spanish MODEL436 pulsed sheet

Helper `+0x153C..+0x1AEC` is independently reconstructed matching C:
1,456 bytes, two physical instances, 2,912 instruction bytes, and one unique
routine. An independent second-slot compilation also matches. Screening 6,191
accepted regional C entries found no equivalent body; this is not a French or
other regional C port.

## Physical loads and ownership

An exhaustive scan identifies MODEL707, record 607, stages 9/10, sectors
167732/167742, with headers 436/586 and distinct complete-image hashes.
Command 602000 selects the 48-byte descriptor at `+0x30B8`.

Five closed contiguous functions begin at `+4`, `+0xD98`, `+0x153C`,
`+0x1AEC`, and `+0x26C8`; raw data begins at `+0x2FBC`.
The function at `+0xD98` is not entry-reachable. Only the selected helper
becomes C, leaving eight ASM instances, including both orphan instances.

## Geometry and phases

Entry `+0xC40` passes the original context captured in `s2` at `+0xC`. The
entry later reuses `s2` at `+0x2A4`, `+0x5EC` and `+0x8A0`, but only the
`+0xC` definition reaches this call.

One 136-byte sheet at context `+0x800` holds four corner arrays and outer and
inner colors. It is drawn as four quads through the second initialized
`POLY_GT4` at `+0x1424`; the constructor owns the packet setup. Three vertices
use the inner color and the fourth the outer color.

Two `ratan2` results are computed and discarded, as in the target. Translation
is origin plus path times the signed progress `/ 1024`. Scale is size plus a
pulse of one eighth of the signed size on odd frames. Depth is scaled by eight
tenths, then nonnegative depth and flag gate 16-bit narrowed submission.

The phase machine uses signed descriptor times. Phase zero grows size to 4096
over `+0x10..+0x14`, then sets phase one, which this helper does not advance.
Phases two, three and four each run a separate 0..1024 progress value over
`+0x14..+0x18`, `+0x18..+0x1C` and `+0x1C..+0x20`, then advance one phase.
Phase five adds `step * 2048` up to 16384. Phase six subtracts `step * 128`
down to zero, then sets phase seven.

## Stack frame

The target frame is 280 bytes. Every non-stack instruction already matched
with the natural 264-byte frame; the only remaining differences were 27 SP
displacements of 16 bytes, starting directly after the 80-byte coordinate.
Following the accepted repository convention for observed stack gaps (see
[Exodia helpers](exodia-helpers.md) and `variant432_draw.c`), an unused 16-byte
`VECTOR` local declared after the coordinate retains that gap. No runtime role
is asserted for it.

## Acceptance evidence

Both complete 20KiB images match using `gcc_2_8_1_g0_split`, the authoritative
GCC 2.8.1/MASPSX 2.81 pipeline. No forced registers or inline assembly are
present. The nine-row ledger preserves all five rejected source/profile
experiments, the source and second-slot compilations, and both terminal
full-image results.

Regressions cover 41 layouts, repeated header inclusion, all physical loads
and descriptors, original-context reaching definitions, packet placement,
every selected relocation, all input/final function and data owners, and 34
resident bindings with loader/context ownership. The private state is only the
observed prefix through `+0x1640`; its stored pointer uses `*G32`.

Terminal dependency fingerprints hash the body, private header, shared
model-variant header, and `gpu_packets.h`, in that order.
