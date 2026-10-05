# French MODEL198 entry

Model 508, record 458, stages 9/10 load two distinct 20,480-byte images
with headers 198/328 and archive command 129000, passing initial argument 0.
Each entry spans 3,440 instruction bytes at offset `0x4`, followed by a
16-byte source-owned unit-scale vector. The slot wrapper renames the entry
and three local symbols.

All 860 native instructions form one closed entry, with 39 direct calls
to 25 resident destinations and no local or indirect calls. Both complete
images match with actual sized linked C entries, exclusive C literal
contributions and disjoint raw owners. All 25 resident callee bodies agree
with the exact French resident ELF. French `0x8004F7C0` uses the existing
`func_8005F7B0(s32, s32)` declaration and definition in `model_effect_state`.
Its second argument is explicitly narrowed to a signed halfword by this
caller; that narrowing is not a different callee ABI or evidence for a
speculative effect name.

## Measured state and behavior

The descriptor is 40 bytes: ray RGB at `0..2`, ring RGB at `3..5`,
unidentified bytes at `6..7`, then signed halfwords for ray height at `8`,
ray half-width at `0xA`, spread at `0xC`, ring radius at `0xE`, ring width
at `0x10`, ring Y at `0x12`, ray duration at `0x14`, ray delay at `0x16`,
ring duration at `0x18`, flash duration at `0x1A`, ray spacing at `0x1C`,
phase spacing at `0x1E`, phase count at `0x20`, start delay at `0x22`,
and unidentified halfwords at `0x24..0x27`. Selection uses the full entry
argument, not the archive command. The selected tuple is
`(128,128,96,128,128,128,6,0,1000,200,300,2000,500,-70,60,20,40,60,2,160,2,110,390,200)`.

The minimum accessed state view is `0x334` bytes: a guest-width descriptor
pointer at `0`, 32 ray vectors at `4`, opponent at `0x104`, 66 ring vectors
at `0x10C`, origin at `0x31C`, two texture words at `0x324`, elapsed time
at `0x32C`, completion byte at `0x330`, trigger byte at `0x331`, and two
unidentified bytes. This is not a recovered allocator extent.
Initialization constructs 33 outer and 33 inner ring positions and 16
pairs of ray positions, uploads two textures in mode 1, and clears both
phase bytes and elapsed time.

Each ring phase subtracts start delay and phase index times phase spacing
from elapsed time. During the ring duration, a matching trigger byte
causes `func_8005F7B0(50, (s16)(phase_spacing / 3))`, then increments that
byte. Only phases greater than zero render the expanding ring. Bulk
projection of all 66 vectors feeds 32 quads with inner indices `i+33`
and `i+34`, eight-wide UV steps, average-depth sorting and the `0x20`
projection-flag check. All phases independently render the fading
fullscreen flash during their flash duration.

Ray phases begin at one, not zero. Each phase resets the ray pointer and
visits 16 groups of two rays, subtracting both ray spacing and ray delay.
Inactive groups still advance by two vectors. Active rays use opponent
position, growing vertical scale and fading color; their matrix pipeline
does not add the ring phase's `MulMatrix2` call. Signed depth and flag
checks remain before sorting.

After adding the frame step, return 0 before the ray delay and 2 after the
last phase's total interval. Otherwise, the first active phase whose index
equals the completion byte increments that byte and returns 1 if it now
equals the phase count, or 4 otherwise. Return 0 when no phase qualifies.

## Matching evidence and remaining coverage

The initial paired candidate was 3,436 bytes with the correct 1,000-byte
frame but 499 differing words. Direct indexing and phase-local scalars
produced distinct 3,444-byte candidates with 455 differences. Reusing the
ring-local scalar for its scale and inner index recovered 3,440 bytes,
leaving 15 flash-only differences. Byte-width and explicit-remainder
variants did not change those bytes; a packet pointer regressed allocation.
Repeated RGB expressions retained redundant divisions, while a chained
assignment did not improve the flash.

Unchanged-output compiler RTL isolated the flash-local numerator allocation.
The native code uses `t0` for all seven color numerators. Reusing one real
color-product temporary across the ring, flash and ray phases reduced the
remaining differences to ten. Preserving the flash's intermediate
`remaining * 256`, then subtracting `remaining` in that same temporary,
recovered the native allocation and argument scheduling exactly. The active
flash bounds give `1 <= remaining <= 32767`, so both this intermediate and
the equivalent multiplication by 255 fit signed 32-bit arithmetic.
These explicit arithmetic steps must not be simplified without a full
match. There are no forced registers, artificial stores, inline assembly
or masked comparisons. All eleven paired experiments and two canonical
matches are retained in the 24-row ledger under the authoritative
GCC 2.8.1/MASPSX 2.81 named local profile.

Each image retains a raw four-byte header, two 28-byte GsIMAGE views at
`0xD84..0xDBC`, and a 16,964-byte unclassified suffix at `0xDBC..0x5000`.
The descriptor is accessed through a local typed view inside that suffix;
its storage is not promoted to C. The combined 33,928 suffix bytes are not
claimed as data-only or excluded game code. This adds two C instances,
6,880 instruction bytes and 32 literal bytes, not exhaustive French
runtime completion.
