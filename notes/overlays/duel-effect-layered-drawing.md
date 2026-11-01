# Duel-effect layered drawing

## Scoped two-pass C views

The complete two-function `layered_drawing.c` object now exposes
`DuelEffect_DrawLayeredTileQuad` and
`DuelEffect_DrawLayeredGradientTrapezoid` through
`layered_semantic_views.h`. The symbol files bind both names in place of
their former labels. The original `layered_drawing.h`, SDK types,
72-byte textured record, 88-byte gradient record and its four-byte unknown
alignment gap remain unchanged. No record is flattened or newly shared.

| C name | Former label | NA address | JP address | UK / FR / ES address |
|---|---|---|---|---|
| `DuelEffect_DrawLayeredTileQuad` | `func_80155BC0` | `0x80147E08` | `0x80163C50` | `0x80155BC0` |
| `DuelEffect_DrawLayeredGradientTrapezoid` | `func_80155D90` | `0x80147FD8` | `0x80163E20` | `0x80155D90` |

Nineteen actual effect/gather caller units use the two C views. The names
describe the two rendering passes, not the artwork, gameplay effect,
world-space dimensions or runtime coverage. The complete object retains
its 464-byte then 516-byte definition order and unchanged
`gcc_2_8_1_g0_split` profile. The separate contour object is not absorbed.

### Layered tile quad

The first FT4 pass uses texture halfwords at offsets `0x00/0x02`, UVs
`(0,0)..(63,63)`, the input RGB and the original square-generator size.
Each vertex is translated by the supplied offset before projection. The
second pass rereads half-intensity input RGB, regenerates the square with
`size * 2` converted through the original `u16` generator parameter, then
translates and projects all four vertices again. Both calls retain the
signed-halfword depth argument and mode one. The second attempt is not
conditioned on whether the first projector accepted its packet.

Keep the distinct `packet->vertices` and `drawing.vertices` expressions
in the two translation loops. Do not cache RGB or offsets, replace the
second pass with a scaled packet copy, widen the size domain or normalize
the depth argument. The existing SDK `addVector` macro performs unguarded
component accesses. Effect 17 supplies literal null offsets at two call
sites; this observation is not a null-safe or optional-offset contract,
and this naming pass adds no guard or substitute zero vector.

### Layered gradient trapezoid

The GT4 uses texture halfwords at `0x04/0x06` and UVs
`(128,0)..(159,63)`. Its first pair of vertices uses X coordinates
`-widths[0]` and `widths[0]` at Y `-height`; the second pair uses
`-widths[1]` and `widths[1]` at Y zero. Initial Z values are zero. The
first edge is black and the second edge rereads half-intensity input RGB.

After the first projection attempt, the second edge rereads quarter-
intensity RGB. The SDK `applyVector(..., 2, 2, 0, *=)` expression updates
every vertex before the second projection, retaining halfword narrowing
and the original multiplication/store sequence. Both projector calls keep
the signed depth and mode one. The four-byte record gap is not initialized
or assigned new meaning. No packet/vertex copy, padding write, width guard
or equivalent-looking vector rewrite is introduced.

### Conservation

Twenty complete owner/caller sources retain their accepted-parent bodies
after reversing only these two identifiers and removing one added include
per file. Actual named-profile GCC/MASPSX output, native symbol tables and
disassembly/relocations remain unchanged for all twenty complete objects.
The five configured bank inventories change only ten name cells; their
other fields, order, extents and profiles remain intact. Two semantic
records append at actual NA addresses, conserving all earlier registry
bytes. Original headers, guards, selectors and native/data owners remain
unchanged; no wrapper, storage, linker alias or test harness is added.
Full-image and regional acceptance uses the ordinary project/PR gates.

## Original matching and layout evidence

The complete contiguous group `0x801558F4..0x80155F94` contains three
functions / 1,696 bytes: 716, 464 and 516 bytes. All use the unchanged
`gcc_2_8_1_g0_split` profile and existing SDK polygon/vector declarations.

The first routine now lives in `contour_quads.c` and the other two in
`layered_drawing.c`, both declared in `layered_drawing.h`. The North American
bank does not keep them together: `func_80155BC0` and `func_80155D90` sit at
`0x80147E08..0x801481DC`, directly before the `drawing_tail.c` group as in the
PAL and Japanese banks, while `func_801558F4` sits at `0x8015616C`, before
`func_80157794`. Two objects reproduce both layouts; one object cannot.

The first joins corresponding inner/outer four-vertex contours with
semitransparent gradient quads and draws a translated textured quad at each
inner vertex. It preserves signed `(i + 1) % 4`, projection-flag rejection,
the zero-bias common-layout packet path, and the nonzero-bias depth adjustment.
The increment belongs inside the accepted-projection branch, and the bias
subtraction is a separate assignment before preparing submission arguments.
The texture pass remains outside that rejection branch.

The second draws a textured quad twice: caller color and size first, then
half-intensity color and doubled halfword size. Both passes translate every
vertex by the supplied vector. Direct aggregate accesses in the second loop
preserve a different base-address instruction than the first loop's pointer
accesses.

The third constructs a textured gradient trapezoid with a black top edge
and half-intensity bottom edge, then draws a second pass at quarter intensity
with doubled X/Y coordinates and cleared Z. The existing SDK
`applyVector(vector, 2, 2, 0, *=)` comma expression reproduces the recovered
compound updates; separate assignments and `setVector` did not.

## Layout and ownership evidence

The first two routines read texture-page/CLUT halfwords at
`D_8015B748 + 0/2`; the third reads the pair at `+4/6`. The existing owning
texture-prefix declaration is refined only for these observed fields, keeping
the older quad pair's `+0x28..+0x2E` offsets unchanged. All texture storage
remains generated data.

The second routine reuses the existing 72-byte FT4/vertex aggregate.
The third's stack addresses show a 52-byte SDK `POLY_GT4` at `sp + 0x10`,
an unaccessed four-byte gap, and four `SVECTOR` records at `sp + 0x48`.
Its private 88-byte stack-record view preserves exactly that observed gap;
it assigns no semantic field or global-storage ownership to those bytes.
The first routine uses ordinary separate stack locals, not a padded
aggregate introduced solely to force allocation.

The common XY-layout helper also receives a `POLY_G4` from the first
routine, supporting a generic primitive parameter rather than an FT4-only
interface. Both projected callees and that helper remain actual local code,
never absolute aliases replacing their contents.

## Experiments and acceptance

Snapshots, profiles and exact instruction diffs remain under
`tmp/layered-probe/`. Initial ordinary locals produced a spilled array base
and extra saved register in the gradient routine; the 464-byte middle
function differed by only its second loop's base-address instruction.
An aggregate trial for the first routine was rejected. Restoring separate
locals and direct stack-field accesses fixed its allocation, while moving
the increment inside projection acceptance and subtracting bias before
submission resolved its remaining five scheduling differences.

For the gradient routine, the observed stack-record view reduced the
mismatch to the vector loop. `setVector` left one different load base;
the SDK compound-update macro matched every instruction. No new compiler
flags, register annotations, scheduling barriers or inline assembly were used.

A private full-bank link against the independent 21-function baseline
reproduces all 90,112 bytes with 24 C functions / 5,780 bytes. Production
acceptance also checks every terrain copy and all seven configured Spanish
module images, preserving all 85 boundaries and 61 assembly functions, with
exact C object and linked-ELF addresses, sizes, types and sections.
