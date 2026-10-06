# Spanish MODEL444 fading sheets

Helper `+0x1DC4..+0x240C` is independently reconstructed matching C on its
first source attempt: 1,608 bytes, two physical instances, 3,216 instruction
bytes, and one unique routine. An independent second-slot compilation also
matches. Screening 6,185 accepted regional C entries found no same-sized body;
this is not a French or other regional C port.

## Physical loads and ownership

An exhaustive scan identifies MODEL175, record 175, stages 9/10, sectors
48500/48510, with headers 444/594 and distinct complete-image hashes.
Command 610000 selects the 36-byte descriptor at `+0x2A18`.

Five closed contiguous functions begin at `+4`, `+0xDB4`, `+0x14E8`,
`+0x1DC4`, and `+0x240C`; raw data begins at `+0x291C`.
The function at `+0x14E8` is not entry-reachable. Only the selected helper
becomes C, leaving eight ASM instances, including both orphan instances.

## Geometry, fading, and timing

Entry `+0xC30` passes the original context captured in `s2` at `+0xC`,
under the strict unsigned comparison of start time against current time.
Two initialized `0x98`-byte sheets occupy context `+0x54C..+0x67C`.
Their four corner arrays, two colors, size, and stride independently agree
with the shared local `ModelVariantSheet` declaration.

The first sheet stays at the origin. The second interpolates along the path.
Odd frames add one eighth of the signed size to the scale. Both draw four
quads through the second initialized `POLY_GT4` at `+0x1478..+0x14AC`.
UV, CLUT, texture-page, and packet setup remain owned by the ASM constructor.

From phase three onward, the moving sheet scales its inner and outer RGB
components by signed `fade / 512` while fade is below 512. Six meaningful
scalar temporaries retain the original evaluation order. Three vertices use
the inner color, the fourth the outer color. Depth is scaled by eight tenths,
then nonnegative depth and flag gate submission with 16-bit depth narrowing.

The stationary sheet grows to 4096 in phase zero and later shrinks using
the unsigned fade-time ratio in phase three. These are independent checks,
not an `else` chain. The moving sheet receives size 4096 in phase one,
grows to 8192 in phase two, and changes the shared phase to three.

The late moving-sheet growth deliberately retains an asymmetric condition:
it updates only while size is below **16384**, but computes
`8192 + elapsed * 16384 / duration` and clamps at **24576**. It can therefore
stop updating below its clamp. This is original behavior, not a threshold
to repair or a generic growth helper to substitute.

## Acceptance evidence

Both complete 20KiB images match using `gcc_2_8_1_g0_split`, the authoritative
GCC 2.8.1/MASPSX 2.81 pipeline. The natural frame is 256 bytes; no forced
registers, inline assembly, artificial padding, or unused allocation controls
are present. The four-row ledger records both source/slot compilations and
both terminal full-image results.

Regressions cover 37 layouts, repeated header inclusion, all physical loads
and descriptors, original-context reaching definitions, sheet initialization,
packet lifetime, the unusual growth guard and clamp, every selected relocation,
all input/final function and data owners, and 35 resident bindings with
loader/context ownership. The private state is only the observed prefix through
`+0x1680`; its stored pointer uses `*G32`.

Terminal dependency fingerprints hash the body, private header, shared
model-variant header, and `gpu_packets.h`, in that order.
