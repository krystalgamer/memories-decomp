# Library globe wireframe

`Library_DrawWireframeGlobe` occupies `0x80029934..0x80029EB0` (1,404 bytes, 351
instructions). The Library card-view controller installs it in draw slot 2.
It draws the globe's coordinate spans through `Library_DrawGlobeXSpan` and
`Library_DrawGlobeZSpan`,
then projects paired points and submits vertical stripe segments.

The paired helpers vary only the three `vx` or `vz` components respectively;
the caller supplies the other components. The initial coordinates are
`origin + span`, `origin + (span * 3) / 4`, and `origin`, with the existing
signed arithmetic and halfword narrowing. A negative `control[1]` causes
exactly one retry using `reduced = span / 2`; the second point is recomputed
as `origin + (reduced * 3) / 4`, not by reassociating the original division.
Point 2 and other components are not rewritten on retry. The packet is
submitted even if the second flags word is still negative, so these names
do not imply a complete axis sweep or a successful-clipping guarantee.

The `LINE_G3` packet is at scratchpad `0x1F800000`, four `SVECTOR` inputs
begin at `0x1F800038`, and the control words are at `0x1F800060`. The centre
copy reads eight bytes from the view buffer at `0x80181000`. Its literal
`lui`/`ori` address construction and `SVECTOR` struct copy preserve the
retail unaligned load/store pairs. `SVECTOR`'s alignment of 2 lowers to the
same two `lwl`/`lwr` and `swl`/`swr` pairs as the alignment-one `Bytes8` spelling
it replaced. The packet, vectors and control words are written through their
`LINE_G3`, `SVECTOR` and `long` members. Three exceptions remain: the colour
words, written through `*(s32 *)&prim->rN`; the three scratch `vy` clears,
kept on absolute addresses because member stores through the vector pointers
reorder the setup; and the unsigned `(u16)` first read of `y0`, which is
retail's `lhu`. The globe radius is `LibraryMotionState.globe_radius`, the
halfword at `gLibrary_State.motion.globe_radius`, declared by
`src/game/library_state.h`. The card-view controller owns the separate
narrow-then-signed-cap growth sequence documented in `library-state-layout.md`;
the renderer's three signed consumers remain unchanged. The axis helpers' shared header records
their actual packet, ordering-table, vector, and control-pointer signatures.

## Exact source shape

The semantic name follows the actual Library slot-2 registration and
mirrored sphere-axis/projected-stripe geometry. It does not imply an
Earth/world-map renderer or new geometry/state guarantees. The standalone
owner and header are `library_draw_wireframe_globe.c/.h`; the European
adapter keeps its existing geometry-offset-Y override while including the
same source. Naming changes none of the measured code shape below.

The named `gcc_2_8_1_g8_split_psyq_rtps_no_cse_skip_blocks` profile keeps
uniform compiler/assembler `-G8`, GCC 2.8.1 and MASPSX 2.81. It adds only
`-fno-cse-skip-blocks` to the existing RTPS profile. That prevents the first
loop's radius-address calculations from consuming a saved register needed
by a scratchpad vector pointer.

Late scratch pointer initialization, reversed assignment chains, a dedicated
next-angle temporary, and separate block-local packet-marker lifetimes
recover the remaining scheduling and register choices. Both projection
sequences use only official `gte_ldv0`, `gte_rtps`, and `gte_stsxy` macros.
No source-authored assembly, register binding, or new opcode filter is used.

## Stripe-spacing correction

The retained candidate divided the stripe spacing by 10. Retail instead
multiplies by `0x66666667`, takes the signed high product, shifts it by four,
and corrects the sign: division by **40**. The corrected C uses `/ 40`.
With radius zero, centre zero, and the explicit projection stub's initial
screen Y of 96, retail submits a first stripe ending at Y 93; `/ 10`
incorrectly ends it at Y 84. This was a candidate discrepancy, not a shipped
game regression: the unmatched assembly fallback was still active.

The local instruction witness covers 104 radius/centre/mutation settings,
34,876 helper calls and 6,656 projections. It exercises 350 instructions;
the remaining word is the negative rounding correction for grayscale, whose
angle is bounded to `0..0x380`. Separate wrong-divisor, wrong-projection-input,
and wrong-projection-store source controls are rejected. These checks use
explicit trig, projection and helper stubs, not a claim of full SDK or GTE
emulation. Exact linked text and the whole-executable retail hash remain the
acceptance criteria.

An independent native ILP32 witness compiles the tracked function at O0 and
O2 and checks all 104 settings at each optimization level, including ordered
calls, scratchpad snapshots and projected vertices. All three source mutants
are rejected at both levels. Its SDK/GTE boundaries use the same explicit
contracts as the instruction witness, rather than executing the real SDK.

[retired `test_library_wireframe_match.py`](https://github.com/krystalgamer/memories-decomp/blob/4b5667dae10ffb9c72df1a242ee060e654214ae1/tools/project/tests/test_library_wireframe_match.py) recompiles the tracked
source, checks the official-macro policy, and compares the complete text
against the legal target. It requires a completed `make match`; no retail
bytes or generated local witness artifacts are tracked.
