# French MODEL177 entry

The instance ledger identifies ten distinct 20,480-byte images with headers
177/307: models 164, 191, 494, 521 and 599 at stages 9/10. Both slots have
one closed 4,504-byte entry, a 656-byte frame and a 16-byte source-owned
unit-scale vector. The 1,126 instructions contain 41 direct calls to 24
resident destinations, without local or indirect calls.

All ten complete images were independently linked from the actual C objects.
Twenty C contributions and forty disjoint raw owners reproduce every byte,
without masking. All 24 resident callee bodies agree with the exact French
resident ELF. No resident implementation or shared SDK declaration changes.

## Local views and native behavior

The 30-byte descriptor contains ribbon, grid and flash RGB triples at
`0..8`, a model-part byte at `9`, a count byte at `0xA`, and an unknown
byte at `0xB`. Signed halfwords at `0xC..0x12` describe ribbon half-width,
height, depth and grid half-size. Travel, fade and flash durations, spacing,
and delay occupy `0x14..0x1C`. Commands are `108000 + argument`; arguments
`0, 1, 2, 3` retain the native `% 100` selection. Each descriptor was
checked in its own image; selected particle counts are 4, 12, 36 and 64.

The `0x5B0` state access view contains the descriptor pointer at `0`,
34 ribbon SVECTORs at `4`, 64 particle positions at `0x114`, one observed
saved origin at `0x314`, an explicitly unknown 504-byte interval at `0x31C`,
nine grid vertices at `0x514`, sixteen guest-width vertex pointers at
`0x55C`, two texture handles at `0x59C`, a completion byte at `0x5A4`,
elapsed time at `0x5A8`, and an animation counter at `0x5AC`. The origin
pointer does not advance with the particle cursor. These are measured
access views, not allocator ownership or claims about the unknown interval.
Twenty-nine target-compiled constants verify the local and SDK layouts.

Initialization builds two contiguous 17-point ribbon edges. Each sample
uses two separate sine calls; the second edge copies the first edge's XYZ
then replaces Y. Neither edge's unused padding is initialized. A 3x3 grid
and four pointer-defined quads follow. Two images are uploaded in mode one,
then elapsed time, animation and completion are cleared.

The grid pass uses texture one and four RotAverage4 calls through the
pointer table. Its signed animation remainder selects one of two 64-pixel
columns and a scale of 4096 or 4352. Rotation Z is time multiplied by 32.
Grid RGB fades during the final fade-duration interval. The scale's
animation read occurs after translation is copied but before rotation
stores; moving that read across the escaped local stores changes the
original compiler's instruction schedule.

The ribbon pass staggers particles by descriptor spacing. Before launch,
each particle is reset from the selected model part and its XYZ copied to
the same saved origin. During travel, RGB rises and the position approaches
`(0, 0, direction * 450)`; during fade, RGB falls and movement continues
using the saved origin and the fixed travel-duration denominator. Division
precedes multiplication by frame step. Position padding is cleared only
in the pre-launch reset.

Each active particle projects the two 17-point edges in one RotTransPersN
call. Two screen cursors and paired depth/flag cursors produce sixteen
quads. UV intervals advance by sixteen pixels, with V coordinates 0 and 31.
The four flags are ORed and masked with `0x20`, but the native gate still
tests signed `flag >= 0`, not `flag == 0`; the source preserves that behavior.
The ribbon matrix chain includes MulMatrix2; the grid chain does not.

A separate 320x256 POLY_F4 flash fades the third RGB triple and uses
`func_8005B260(..., 1, 1)`. Elapsed time advances by frame step, while the
animation counter advances by one. Completion returns zero before the
travel threshold, four before the final stagger threshold, two strictly
after fade duration, and one once by incrementing the completion byte.
Subsequent calls inside that final interval return zero.

## Matching evidence and remaining coverage

The ledger preserves sixteen paired experiments, one blocked slot-zero
compile, and two canonical records. The blocked attempt passed all target
layouts but used the existing void-pointer light-matrix getter without
its established MATRIX conversion. No entry comparison occurred there.

Explicit projection cursors recovered the native frame. Declaration and
cursor-update order recovered the stack slots and loop schedules. A
positive terminal phase removed duplicated return paths. A shifted sample
index retained native multiplication order. The final twelve differences
were resolved by capturing animation before rotation stores. A final
conservative origin-view refinement remained byte-identical.

The source uses authoritative `gcc_2_8_1_g0_split`, GCC 2.8.1/MASPSX 2.81.
There are no forced registers, fake dependencies, artificial stores,
inline assembly, source-local externs or imported reference-header types.

Each image retains four raw owners: a four-byte header, two 28-byte image
records at `0x11AC..0x11E4`, an unclassified 28-byte gap at
`0x11E4..0x1200`, and an unclassified 15,872-byte suffix at
`0x1200..0x5000`. Descriptor declarations are views into the suffix, not
C-owned storage. The combined 280 gap bytes and 158,720 suffix bytes remain
unclassified. This adds ten C instances, 45,040 instruction bytes and
160 literal bytes, not exhaustive French runtime completion.
