# French MODEL199 entry

Models 243 and 622, records 243 and 572, stages 9/10 load four distinct
20,480-byte images with headers 199/329. Archive command 130000 passes
initial argument 0. There are two unique 5,520-byte entry bodies, one per
slot, followed by a 16-byte source-owned unit-scale vector. The slot wrapper
renames the entry and three local symbols.

All 1,380 instructions form one closed entry, with 66 direct calls to 23
resident destinations and no local or indirect calls. All four complete
images match with sized linked C entries, exclusive C literal contributions
and disjoint raw owners. The 23 resident callee bodies agree with the exact
French resident ELF. French `0x8005AFA4` uses the existing
`func_80057E20(s32, ModelEffectAdjustment *)` declaration and eight-byte
adjustment type; `rand` uses its existing SDK declaration and French binding.

## Measured state and behavior

The 42-byte descriptor contains ray RGB at `0..2`, grid RGB at `3..5`,
sphere RGB at `6..8`, an unidentified byte at `9`, then signed halfwords
for ray size at `0xA`, ray radius at `0xC`, ray height at `0xE`, grid radius
at `0x10`, sphere size at `0x12`, sphere radius at `0x14`, sphere fall at
`0x16`, ray fade-in at `0x18`, flash duration at `0x1A`, grid duration at
`0x1C`, ray duration at `0x1E`, sphere duration at `0x20`, ray spacing at
`0x22`, ray group size at `0x24`, ray delay at `0x26`, and burst delay at
`0x28`. Selection uses the full entry argument, not the archive command.
All four selected descriptors have tuple
`(64,64,64,160,128,128,48,48,48,6,200,150,300,150,200,400,300,60,40,40,60,60,24,16,10,300)`.

The local state view is `0x5B0` bytes: a guest-width descriptor pointer at
`0`, 128 ray vectors at `4`, anchor at `0x404`, nine grid vectors at `0x40C`,
32 sphere vectors at `0x454`, 16 guest-width quad pointers at `0x554`, five
texture words at `0x594`, elapsed time at `0x5A8`, completion byte at `0x5AC`,
and three unidentified bytes. This is not a recovered allocator extent.
The native frame is 304 bytes; 34 target-compiled layout checks cover the
local and shared views.

Initialization queries the active slot and saves the frame step before
the command branch. It gets the opponent adjustment and constructs 128
descending-index random rays. Preserve negation before the height
remainder, the radius scaling order, and repeated trigonometric calls.
It constructs a three-by-three grid, the 16-pointer quad table, and 32
random sphere vectors. Five mode-1 texture uploads are followed by another
upload of index 4 in mode 2: six calls, not five. Elapsed time and completion
are then cleared.

Before the burst, groups of `128 / ray_group_size` rays use elapsed time
minus ray delay and group spacing. Active groups ramp RGB during fade-in;
inactive groups still advance the ray pointer. Each active ray has random
horizontal jitter and opponent-relative position. Projection precedes
eight-frame UV selection, and signed depth/flag checks precede sorting.
After the burst, all 128 rays fade during the ray duration and expand
relative to the anchor. Coordinate division by `time + grid_duration`
precedes multiplication by the saved step; Y first subtracts ray height.

The fullscreen flash fades during its own duration. Before the burst the
anchor is refreshed from the opponent; afterward four grid quads rotate,
scale and fade during the grid duration. The final phase draws 32 sphere
quads and subtracts `sphere_fall / sphere_duration * saved_step` from each
point's Y after rendering. Preserve division before multiplication and
halfword coordinate stores.

After `PopMatrix`, a second frame-step query advances elapsed time. The
maximum of flash, grid, ray and sphere durations controls the terminal
interval. Return 0 before ray delay, 4 before burst delay, 1 on the first
active completion and 0 on subsequent active ticks, then 2 after the
maximum duration.

## Matching evidence and remaining coverage

The initial candidate was 5,540 bytes with a 296-byte frame. Shared real
initialization scalars recovered the 304-byte frame and both ray-phase
quad-pointer spills. Native quad-table stores use a separate table base.
Recovering that pointer reduced shifted differences to 63. Signed-word
conditional maximum expressions recovered the native length without
halfword conversions; the nonzero completion guard recovered the terminal
layout. Local UV-frame scalars and a separate flash scalar narrowed the
remaining mismatch to twelve flash numerator/divisor register words.

Unchanged-output RTL and observation of the pinned GCC 2.8.1 compiler
showed the coalesced local numerator/result quantity had priority 15,000,
above the divisor's 10,000. Equivalent local expressions did not change
those bytes. Broader scalar sharing, the supported alternate CSE profile,
and reordered packet setup regressed other instructions.

The native preburst blue channel crosses the fade/full-color branch in
`v1`, also used by flash grayscale. Sharing that real scalar between those
two phases recovered the flash. Keeping the later ray, grid and sphere
blue channels local preserved their distinct allocation. Do not flatten
these scopes or simplify the signed-word maximum without full matching.
There are no forced registers, artificial stores, inline assembly or masked
comparisons. All 24 paired experiments and two canonical matches are
retained in the 50-row ledger under the authoritative named local profiles;
the accepted source uses GCC 2.8.1/MASPSX 2.81 with `gcc_2_8_1_g0_split`.

Each image retains a raw four-byte header, five 28-byte GsIMAGE records at
`0x15A4..0x1630`, and a 14,800-byte unclassified suffix at
`0x1630..0x5000`. The descriptor is only a local typed view into that
suffix; its storage is not promoted to C. The combined 59,200 suffix bytes
are not claimed as data-only or excluded game code. This adds four C
instances, 22,080 instruction bytes and 64 literal bytes, not exhaustive
French runtime completion.
