# French MODEL173 entry

The instance ledger identifies 12 distinct 20,480-byte images with headers
173/303. Model 392 selects stages 7/8; models 8, 113, 350, 386 and 448 select
stages 9/10. Each slot has one closed 3,756-byte entry, a 312-byte frame and
a 16-byte source-owned unit-scale vector. The 939 instructions contain 45
direct calls to 23 resident destinations, without local or indirect calls.
The slot wrapper renames only the entry, literal and two raw storage views.

All 12 complete images were independently linked from the actual C objects
and three disjoint raw owners. Their 24 C contributions and 36 raw owners
reproduce every byte without masking. All 23 actual resident callee bodies
agree with the exact French resident ELF. No speculative SDK declaration,
resident implementation or shared type is introduced.

## Local views and native behavior

The 26-byte descriptor starts with byte RGB and model-part fields, followed
by signed halfwords for half-size, spread, an unidentified value, gather,
travel and burst durations, group size, particle count, group spacing,
initial delay and another unidentified value. Loader commands are
`104000 + argument`; observed arguments are `0, 1, 2, 103, 104, 105`.
The entry selects descriptor `argument % 100` and mode `argument / 100`.
Every selected descriptor was checked against its own image.

The state access view has a guest-width descriptor pointer at `0`, 256
positions at `4`, 256 offsets at `0x804`, the center at `0x1004`, ten texture
handles at `0x100C`, completion byte at `0x1034`, elapsed time at `0x1238`,
mode byte at `0x123C` and frame counter at `0x1240`. Its `0x1244` extent is
not an allocator ownership claim. Twenty-six target-compiled constants
verify descriptor, state, vector, matrix, image and packet layouts.

Initialization generates spherical offsets using three random remainders
and native nested signed divisions. It copies the opposing slot center,
overwrites center Y with -350, uploads all ten image descriptors with mode
two or one, and clears time, frames and completion. Rotation and position
are cleared and direction/frame step are obtained before either command
branch, as in the native entry.

Runtime processes `count / group_size` groups using integer floor division:
the observed 256-particle, six-particle-group descriptors yield 42 groups.
Before the staggered start, each position comes from the selected model
part through `GsGetLwUnit`, not a guessed `GsGetLw` declaration. Gather and
travel use texture one, random Z rotation and native scale arithmetic.
Gather fades configured RGB only in mode zero and approaches quarter-offset
center coordinates with Z targeting `direction * 450`. Travel approaches
full offsets plus center. Burst uses texture two, zero rotation, unit scale
and eight animation frames in a four-column, 64-pixel UV grid.

Point and offset walkers advance in every phase, including completed
groups. Depth and flag must both be nonnegative before sorting. The
otherwise unused POLY_F4 initialization remains. Elapsed time advances by
the captured step and frames by one; completion yields zero before gather
completion, four until all group/travel/burst timing expires, then one once
and two thereafter. No new bounds or division guards change native behavior.

## Matching evidence and remaining coverage

The 22-row ledger preserves ten paired experiments and two canonical
matches, with no blocked attempts. Shared initialization arithmetic and
direction declaration order recovered the native frame and spill order.
Common byte RGB carriers recovered the exact instruction and literal sizes.
Keeping the particle-loop index independent of angle/travel scratch work
resolved 77 register differences. Separate growth and base-scale expressions
resolved the final 2048-bias operand difference.

The accepted source uses authoritative `gcc_2_8_1_g0_split`,
GCC 2.8.1/MASPSX 2.81. The unsplit-address profile is only a recorded failed
experiment. No forced registers, fake dependencies, artificial stores,
inline assembly or reference-header types are used.

Each image retains a raw four-byte header, ten 28-byte GsIMAGE records at
`0xEC0..0xFD8`, and an explicitly unclassified 16,424-byte suffix at
`0xFD8..0x5000`. Descriptor declarations are views into that suffix, not
C-owned storage. The combined 197,088 suffix bytes remain unclassified.
This adds 12 C instances, 45,072 instruction bytes and 192 literal bytes,
not exhaustive French runtime completion.
