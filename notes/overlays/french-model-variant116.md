# French MODEL116 entry

The instance ledger identifies 22 distinct 20,480-byte images with headers
116/246. Models 27, 422 and 556 select stages 7/8; models 22, 73, 127, 454,
484, 504, 581 and 701 select stages 9/10. Each slot has one 4,440-byte closed
entry and a 16-byte source-owned unit-scale vector. Its 1,110 instructions
contain 56 direct calls to 22 resident destinations, without local or
indirect calls. The slot wrapper renames the entry, literal and raw views.

All 22 complete images were independently linked from the actual C objects
and three disjoint raw owners. Their 44 C contributions and 66 raw owners
reproduce every byte without masking. All 22 actual resident callee bodies
also agree with the exact French resident ELF. No resident implementation
or speculative SDK declaration is added.

## Local views and native behavior

The descriptor is 38 bytes: ring, dot and sprite RGB triples at `0..8`, an
unidentified byte at `9`, then signed halfwords for ring height, width and
radius at `0xA..0xE`, dot radius at `0x10`, sprite size/radius at `0x12/0x14`,
three counts at `0x16..0x1A`, three durations at `0x1C..0x20`, ring stagger at
`0x22`, and initial delay at `0x24`. Archive commands are `47000 + argument`;
the descriptor selector uses the full initial argument, not `% 100`.
All nine selected descriptors were checked in all 22 images.

The state access view has a guest-width descriptor pointer at `0`, 16 ring
vectors at `4`, 128 dots at `0x84`, 64 sprites at `0x484`, three signed packed
texture handles at `0x684`, a halfword frame toggle at `0x690`, and elapsed
time at `0x694`. Its `0x698` extent is not an allocator ownership claim.
The native frame is 1,576 bytes; 30 target-compiled constants verify the
local and shared layouts. Packed handles preserve signed high-half loads.

Initialization generates 16 ring points using sine and negative cosine.
Dots and sprites use three random calls each: azimuth, elevation and radius.
Nested divisions and repeated trigonometric calls are retained. French
`ccos` is `0x800868A8`; `csin` is `0x80086B38`. Three textures are uploaded;
elapsed time starts at negative initial delay and the toggle starts at zero.
The native stores clearing local `vertices[0]` before return are preserved.

Each active ring draws 16 quads, with stagger-relative time controlling
position, scale and fading RGB. The lower edge is slanted in Y/Z. All three
RGB scalars precede packet stores. Dots use absolute elapsed time and a
one-pixel `GsBOXF` with attribute `0x50000000`; their RGB fields are assigned
separately, preserving descriptor reloads. Projection depths are shifted
right by two and projection flag bit `0x20` gates sorting.

Ring and dot matrices include `MulMatrix2(base, matrix)`. Sprites deliberately
omit that call, retaining their billboard path; they use absolute elapsed
time, square vertices and a separate RGB triplet. Native signed depth/flag
tests are retained. Frame time advances through fresh frame-step calls and
active updates XOR the frame toggle with one.

The terminal duration is the maximum of
`ring_stagger * ring_count + ring_duration`, dot duration and sprite
duration. Negative elapsed time returns zero; elapsed time above the maximum
returns two. Otherwise the result is `elapsed <= ring_stagger`, not a
one-time completion flag.

## Matching evidence and remaining coverage

The 35-row ledger preserves 16 paired experiments, one blocked ordered-call
check and two canonical matches. The blocked attempt passed layout checks
and slot-0 compile/link, but used swapped trigonometric meanings; it claims
neither a full comparison nor a slot-1 result.

Shared spherical-azimuth/scale/maximum work and signed-word conditional
maxima recovered the native size. Phase-local RGB lifetimes and dot cursors
initialized after projection recovered register allocation. With those
cursor lifetimes corrected, sharing the elapsed-time work scalar recovered
the ring register permutation. Forming the first maximum directly from its
compound operand recovered terminal result copies. Direct `i * 256`
operands in both initial trigonometric calls recovered the final angle
copies through common-subexpression handling. The rejected shared-angle
experiment's 1,584-byte frame remains recorded rather than normalized.

The accepted source uses the authoritative `gcc_2_8_1_g0_split`
GCC 2.8.1/MASPSX 2.81 profile. No forced registers, fake dependencies,
artificial stores, inline assembly or guessed allocator ownership are used.

Each image retains a raw four-byte header, three 28-byte GsIMAGE records at
`0x116C..0x11C0`, and an explicitly unclassified 15,936-byte suffix at
`0x11C0..0x5000`. Descriptors are views into that suffix, not C-owned storage.
The combined 350,592 suffix bytes remain in the unclassified coverage scope.
This adds 22 C instances, 97,680 instruction bytes and 352 literal bytes,
not exhaustive French runtime completion.
