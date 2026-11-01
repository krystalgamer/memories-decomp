# Duel-effect texture tiles and projected strips

## Scoped tile, contour and ring-strip views

`drawing_semantic_views.h` gives five complete helpers typed C names; the
symbol files bind them in place of their former labels. The complete three-function
`drawing_tail.c` object and the separate one-function `gradient_strip.c` and
`contour_quads.c` objects keep their original definition order, extents,
SDK declarations and `gcc_2_8_1_g0_split` profile. In particular, the contour
function's different NA placement is not flattened into a new grouped object.

| C name | Former label | NA address | JP address | UK / FR / ES address |
|---|---|---|---|---|
| `DuelEffect_DrawQuadContour` | `func_801558F4` | `0x8015616C` | `0x80163984` | `0x801558F4` |
| `DuelEffect_DrawFixedTileQuad` | `func_80155F94` | `0x801481DC` | `0x80164024` | `0x80155F94` |
| `DuelEffect_DrawRandomTileQuad` | `func_80156064` | `0x801482AC` | `0x801640F4` | `0x80156064` |
| `DuelEffect_DrawTexturedRingStrip` | `func_8015616C` | `0x801483B4` | `0x801641FC` | `0x8015616C` |
| `DuelEffect_DrawGradientRingStrip` | `func_80156448` | `0x80148690` | `0x801644D8` | `0x80156448` |

These helpers share the module's texture/ordering-table owners and the
existing projection/offset submission pipeline. Actual bank callers now use
the five C views. Effect 1 calls the fixed tile; the random tile, contour
and strip helpers are shared across multiple effect handlers, including
the gather path. Their names describe observed rendering operations, not
unproven gameplay animation or texture-art identities.

### Fixed and random texture tiles

The fixed tile builds an SDK FT4 using texture halfwords at offsets
`0x08/0x0A`, UVs `(0,64)..(63,127)`, square-generator argument 64 and half
the input RGB. It calls the existing FT4 projector with bias zero and mode
one. It does not take a caller position or identify the depicted artwork.

The random tile uses halfwords at `0x0C/0x0E`, unscaled input RGB and caller
vertices. Two separate, ordered `rand() % 2` calls supply U/V tile choices;
the U range starts at 160 and each tile is 32 by 32. The signed-halfword
depth argument is forwarded unchanged as projector bias, with mode one.
Keep signed remainder, narrowing and both calls, rather than substituting
a mask, one random call, a normalized index or a uniformity claim.

### Textured and untextured ring strips

Both complete strip bodies walk exactly 32 wrapped segments, projecting
first-to-middle and last-to-middle quads with `(i + 1) % 32`. The textured
GT4 uses page/CLUT offsets `0x04/0x06`, UVs `(128,0)..(159,63)`, a black
first edge and input-color second edge. It rejects either negative depth
or negative projection flag. The original separate `adjusted = depth + 1`
assignment precedes mode selection. Mode zero uses the GT4 offset helper
with mode one; every nonzero signed-halfword mode clamps
`adjusted - mode` to zero and supplies that mode to the resident submitter.

The untextured G4 instead uses half-intensity color on its second edge and
rejects only a negative projection flag. It keeps `depth -= bias`, with no
added one. Zero bias delegates to the generic offset helper with flags one.
Nonzero unsigned-halfword bias clamps the adjusted depth to zero and uses
the resident submitter with flags one. Both bodies retain arithmetic shifts
and explicit `u16` priority conversion. Do not unify their rejection,
signedness, adjustment or submission contracts.

### Four-edge contour with independent inner tiles

The contour routine projects four outer-to-inner gradient G4 edges, black
at the outer vertices and half-intensity at the inner vertices. A
nonnegative flag permits `depth++`; zero bias delegates to the generic
offset helper, while nonzero bias separately subtracts bias and submits
without adding a negative-depth clamp. Caller flags remain unchanged.

Outside that edge-acceptance branch, every iteration independently builds
a square with argument 8 for zero bias or 16 otherwise, translates it to
the inner vertex and attempts full-input-color FT4 projection. Those tiles
use texture offsets `0x00/0x02` and UVs `(0,0)..(63,63)`. A rejected edge
does not suppress its tile attempt; the existing tile projector still
applies its own acceptance rules. The `u16` contour bias retains its
original cast to `s16` for that projector call. No clipping fix, shared
rejection rule, new guard or texture reinterpretation is introduced.

### Conservation

Original headers, native prototypes and machine exports remain intact.
The five bank inventories change only 25 name cells, not addresses, sizes,
statuses, notes or profiles. Five scoped semantic records append at the
actual NA addresses. All prior registry bytes remain unchanged. No new
storage, wrapper, native alias, test harness or relaxed guard is added.
Accepted-parent source conservation reverses only the five identifiers and
one added include per affected C file; actual named-profile GCC/MASPSX and
native object comparisons preserve the complete implementation/caller units.
Regional/runtime coverage remains subject to the existing ordinary PR gates.

## Original matching evidence

The contiguous Spanish bank interval `0x80155F94..0x80156448` is owned by
`drawing_tail.c` (now part of `src/overlays/duel_effects/layered_drawing.c`), in definition order, with the
unchanged `gcc_2_8_1_g0_split` profile (GCC 2.8.1 / MASPSX 2.81).

| Function | Bytes | Recovered behavior |
|---|---:|---|
| `func_80155F94` | 208 | Build a size-64 FT4 with half-intensity input color and a fixed texture tile. |
| `func_80156064` | 264 | Select one of four texture tiles using two signed `rand() % 2` results, then submit the supplied vertices and depth. |
| `func_8015616C` | 732 | Project and draw both sides of a 32-segment strip between three vertex arrays. |

The first two routines matched their complete instruction ranges on the first
candidate. For the third, three distinct source experiments were retained in
the local attempt ledger:

1. Incrementing `depth` in place produced the correct 732 bytes but four
   register-choice mismatches at relative offsets `0x188`, `0x1A0`,
   `0x24C` and `0x264`: the increment overwrote `$a2` rather than producing
   the target's temporary `$v0`.
2. Combining `depth + 1 - mode` in the nonzero-mode branch still produced
   732 bytes and four mismatches at those offsets. GCC rewrote the operation
   as `depth - (mode - 1)`, changing both instruction operands.
3. A separate `adjusted = depth + 1` before the mode branch, followed by
   `depth = adjusted - mode`, reproduced all four instructions exactly.

Both depth and projection flag must be nonnegative. Mode zero uses the
existing GT4 packet helper. Nonzero mode clamps the adjusted depth to zero,
shifts it by two, truncates the priority to `u16`, and passes the signed mode
to the resident submitter as flags. The two projection blocks and signed
modulo operations are retained rather than simplified.

The texture-prefix view exposes the observed page/CLUT pairs at offsets
`0x04/0x06`, `0x08/0x0A` and `0x0C/0x0E`. Together with the independently
observed `0x00/0x02` pair, these occupy the first sixteen bytes; the remaining
`0x18` unknown bytes preserve the accepted `0x28..0x2E` fields. This is a view
of generated retail data, not a new C-owned definition or linker alias.
Canonical packet, texture, SDK random and projection declarations are reused.

A private full-bank link first verified all three routines alongside the
accepted 21-function baseline, then again alongside all 37 accepted functions:
40 C functions / 7,564 bytes, preserving every accepted entry. The complete
90,112-byte Spanish bank retains SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
Production acceptance additionally checks all terrain copies, every compiled
object extent, final function address/size/executable ownership, actual local
callees and generated texture/ordering-table data. At that independent cutoff,
the other 45 provisional boundaries remained assembly.

Additive integration of the accepted layered-drawing group preserves all
40 accepted entries and reaches 43 C functions / 9,260 bytes, with 42
provisional assembly boundaries. Configured Spanish overlays reach 167/209
matching instances / 65,112 bytes. All source groups, shared declarations and
texture offsets remain intact; no unreviewed runtime coverage is claimed.

## Later grouping

`layered_drawing.c`, `drawing_tail.c` and `gradient_strip.c` have since been
merged by meaning (#39) into one `layered_drawing.c` unit of six contiguous
primitive-drawing functions, from the layered tile quad through the gradient
ring strip, at the same `gcc_2_8_1_g0_split` profile in every region. The
sections above, and the bank notes, record the units as they were accepted.
