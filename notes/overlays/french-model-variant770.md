# French MODEL770 entry

The instance ledger identifies 18 distinct 20,480-byte images with headers
770/867. Models 23, 85, 103, 239, 480, 499 and 555 select stages 7/8;
models 187 and 596 select stages 9/10. Each slot has one closed 5,860-byte
entry and a 16-byte source-owned unit-scale vector. Its 1,465 instructions
contain 73 ordered direct calls to 26 resident destinations, without local
or indirect calls. The slot wrapper renames the entry, literal and raw views.

All 18 complete images were independently linked from the actual C objects
and three disjoint raw owners. Their 36 C contributions and 54 raw owners
reproduce every byte without masking. All 26 actual resident callee bodies
also agree with the exact French resident ELF. No resident implementation
or speculative SDK declaration is added.

## Local views and native behavior

The 30-byte descriptor contains trail and sprite RGB triples at `0..5`,
two model-part selectors at `6/7`, then eleven signed halfwords: trail
layers, sprite count, dot count, trail-layer step, sprite spread, dot
radius, sprite size, initial delay, trail duration, burst delay and sprite
delay. Archive commands are `302000 + argument`. All seven selected
descriptors were checked in all 18 images; each selects 32 sprites and
64 dots. The source retains the configured reads rather than substituting
those observed counts.

The state access view has a guest-width descriptor pointer at `0`, two
32-element trails at `4/0x104`, a quad at `0x204`, sprite offsets/origins
at `0x224/0x324`, an opaque region at `0x424`, dot positions/velocities at
`0x524/0x724`, two 16-element rings at `0x924/0x9A4`, and projected points
at `0xA24`. Counts, scale, timers, depths, texture data and packed colors
occupy `0xB24..0xC08`. The `0xC08` extent is an access view, not an allocator
ownership claim. Fifty target-compiled constants verify local and shared
layouts. The native stack frame is 592 bytes.

Initialization preserves the identical Y/Z dot-velocity expressions.
Frame step is captured before setting the override to one. The initial
delay returns before the matrix push. Trail capture retains its native
count checks without an invented empty-trail guard; new sprite origins
read the newest captured sample.

Sprite offsets advance for every active index, including expired sprites.
Sprites and quarter quads gate submission on signed depth only; trail and
ring paths also test the projection flag. Scale has the native unsigned
comparison against `0x4000`. Packed color operations preserve the fourth
code byte. Particle activity is set for Y below 350 independently of depth,
and gravity uses velocity Y itself, not particle position Y.

The terminal condition is deliberately unusual: elapsed time below trail
duration while phase is zero sets phase one and returns one. Return two
requires phase at least two and both packed RGB masks cleared.

## Matching evidence and remaining coverage

The 152-row ledger preserves 74 paired experiments, two blocked compiles
and two canonical matches. The malformed UV-macro experiment passed all
50 layout checks but produced no slot object. The RTPS-profile calibration
failed during layout filtering because its required placeholder was absent;
neither layout checks nor slot compilation completed. The isolated
no-CSE-skip-blocks profile was rejected and removed from active metadata.

SDK vector expressions, capture-local views and newest-sample addressing
recovered native address evaluation. A phase-local velocity cursor restored
the velocity update. Division by four, rather than a right shift, recovered
both particle-priority argument copies. An indexed shared second-vector
base recovered the complete 720-byte sprite block, including its initial
spill, unbiased cursor and matrix/frame-count register allocation.

Compiler RTL separated three distinct causes of the quarter-loop mismatch:
direct word negation never creates a result copy; adjacent temporary
copies disappear during first CSE; and local register allocation rewrites
a same-mode result use after a surviving copy. The accepted source keeps
a halfword persistent orientation, a word negation result and the actual
first-coordinate value. The second corner consumes the persistent signed
view. This preserves the native copy before normalization without forced
registers, fake dependencies, artificial stores or inline assembly.
The authoritative profile remains `gcc_2_8_1_g0_split`, GCC 2.8.1/MASPSX 2.81.

Each image retains a raw four-byte header, one 28-byte GsIMAGE at
`0x16F8..0x1714`, and an explicitly unclassified 14,572-byte suffix at
`0x1714..0x5000`. Descriptors are views into that suffix, not C-owned storage.
The combined 262,296 suffix bytes remain in the unclassified coverage scope.
This adds 18 C instances, 105,480 instruction bytes and 288 literal bytes,
not exhaustive French runtime completion.
