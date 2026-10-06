# French MODEL variant141 entries

Models428/445 at stages7/8 and model20 at stages9/10 supply six runtime
images with headers141/271. Each contributes a closed3,932-byte C entry
and16-byte source-owned unit vector using the existing
`gcc_2_8_1_g0_split` profile. GCC2.8.1/MASPSX2.81 and shared SDK declarations
are unchanged. This is family141, not model-index141 in family178.

## Evidence and ownership

All983 instructions in both slot entries match native bytes, including52
ordered direct calls to23 resident destinations. Independent complete-image
linking verifies12 actual C entry/literal definitions and18 disjoint raw
owners across all six20,480-byte images. The resident executable and all23
imported callee bodies agree with retail and the actual resident ELF.

The owned spans are raw header0..4, C entry4..0xF60, C unit vector0xF60..0xF70,
two raw image records0xF70..0xFA8, and unclassified suffix0xFA8..0x5000.
The contribution is23,592 C instruction bytes and96 literal bytes.
The98,832 suffix bytes remain unclassified. Complete-image identity does not
prove exhaustive runtime coverage or allocator ownership.

Actual loader commands72000,72001,72003 select36-byte descriptors directly,
without modulo in the C entry. All six selected records are decoded and
checked. Observed position counts12,28,20 justify28 declared positions and
an unknown0x120-byte gap through offset0x204, not64 positions inferred from
address spacing. Two32-vector arrays occupy0x204 and0x304; signed packed
textures start at0x404, elapsed at0x40C and completion at0x410. The observed
context extent is0x414. Twenty-nine target-compiled size/offset assertions
protect the view. All selected random spreads and timing divisors are positive.

## Native behavior preserved

The constructor's clear loop repeatedly writes positions[0] without
advancing its cursor. This apparent defect is intentional preservation.
Random offsets retain centered X/Y and signed direction-before-remainder Z.
The independent point velocities use centered random components on all axes.

The first pass uses a nine-vertex flat grid and four explicit projected
faces. Pending particles capture the active part translation. The local
render position is copied before Y/Z updates; interpolation divides by the
remaining duration without multiplying by frame step. X does not move.

The second pass draws a fading animated sprite for each of32 offsets.
Texture coordinates are assigned after projection. Its matrix chain omits
MulMatrix2. A second matrix chain, including MulMatrix2, runs unconditionally
for the point cloud, even outside either visible window. Each cloud uses32
velocities, copied into vertices[4] within the existing nine-vector array.
Signed16-bit products narrow without division.

All three box color components separately use descriptor byte6; bytes7/8
remain unread even when their selected values differ. Width is written before
height. Matrix calls, visibility tests, signed texture-page loads, primitive
initialization and fresh terminal frame-step query retain native ordering.
The terminal uses the shorter sprite duration, not the point-cloud duration.

## Refinement and acceptance

The22-row ledger records ten paired experiments and two canonical matches.
An initial layout-only failure used the broad local stdlib header; switching
to the established narrow rand header fixed its unavailable include paths.
No entry was compiled in that failed preparation.

Assigning the descriptor through state restores a native register copy.
Explicit width/height stores preserve ordering. Most importantly, the entire
completion-latch block belongs inside the elapsed sprite-duration window,
with return4 outside it. Accepted local variants121/129/76 provide this
structural prior art. Merely permuting latch tests left a one-instruction
near-match or changed branch layout; recovering window ownership matches both
entries without padding, forced registers, volatile accesses or special flags.

Final integration requires a fresh clean French resident and every configured
overlay, actual linked ownership, focused regressions and repository policies.
