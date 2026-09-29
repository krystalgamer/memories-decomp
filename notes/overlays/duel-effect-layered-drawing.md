# Duel-effect layered drawing

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
