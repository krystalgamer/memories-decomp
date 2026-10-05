# French MODEL76 entry

The instance ledger identifies eight distinct 20,480-byte images with headers
76/206. Models 57, 447 and 562 select stages 7/8; model 489 selects stages
9/10. Each slot has one closed 2,964-byte entry, with 741 instructions and
36 direct calls to 21 resident destinations. There are no local or indirect
calls and no source-owned literals. The slot wrapper renames only the entry
and its raw descriptor view.

All eight complete images were independently linked from the actual sized
C entries and two disjoint raw owners per image. All 21 actual resident
callee bodies also agree with the clean byte-exact French resident ELF.
Existing SDK declarations are reused. French `SetPolyF3` at `0x80082E08`
and `RotAverageNclip3` at `0x80087AB8` were independently calibrated against
the corresponding complete North American SDK bodies.

## Local views and behavior

The 24-byte descriptor contains RGB at `0..2`, byte count, lifetime, fade
threshold, launch interval and model part at `3..7`, eight unidentified
bytes, then signed halfwords for size, Y/X angle ranges and initial delay
at `0x10..0x16`. Commands are `6000 + argument`; the entry selects the full
initial argument. The three observed arguments `0`, `1` and `2` and their
descriptor bytes were checked in all eight images.

The state view has a guest-width descriptor pointer followed by three
256-element SVECTOR arrays: positions at `4`, velocities at `0x804` and
rotations at `0x1004`. Four shape vertices start at `0x1804`, 256 signed
life counters at `0x1824`, elapsed time at `0x1A24`, and a completion byte
at `0x1A28`. Four twelve-byte records at `0x1A2C` each contain three
guest-width vertex pointers. The `0x1A5C` access extent is not an allocator
ownership claim. Twenty-six target-compiled constants verify these views,
SDK types and packet sizes; the native frame is 248 bytes.

Initialization clears positions and velocities, chooses three random
rotation components per particle, constructs a four-vertex shape and its
four triangle faces, and clears life counters twice as in the native code.
Position and rotation cursors advance before velocity clearing. The frame
step is deliberately narrowed to a byte.

During the initial delay, the first particle follows the selected model
part. Its initial velocity uses the native trigonometric calls and signed
division order. Active particles are staggered by `interval * index`;
their launch velocity retains division before multiplication by three
and division by two. Each elapsed step updates position and Z rotation.
Remaining life controls fading and semitransparency. Four faces use
`RotAverageNclip3`, per-face RGB scaling and signed clip/depth/flag gates.
The faded path retains the distinct packet-sort callback and its argument.

The result stays zero before `delay + lifetime`, then four until
`delay + interval * count + lifetime`. Inside that positive final threshold,
an existing completion returns two; otherwise the entry stores literal one
and returns one. Keeping this decision inside the final threshold is
essential to the native branch layout.

## Matching evidence and remaining coverage

The 38-row ledger preserves 16 original paired experiments, an unchanged
control on newer accepted master, the exact structural refinement and two
canonical matches. No attempts were blocked. Earlier pointer/face layouts,
return chains and completion-local rewrites remained nonexact; their
original sizes, frames and failures remain recorded.

The unchanged control reproduced the five-word terminal residual. New
evidence from the independently reconstructed MODEL129 entry showed that
the completion decision belongs inside the positive final threshold.
Applying that enclosing control structure, rather than another local
completion rewrite, resolved all five words in both MODEL76 slots.

The authoritative profile remains `gcc_2_8_1_g0_split`, using GCC 2.8.1
and MASPSX 2.81. No forced registers, artificial stores, fake dependencies,
inline assembly or unmatched code promotion are used.

Each image retains a raw four-byte header and an explicitly unclassified
17,512-byte suffix at `0xB98..0x5000`. Descriptors are views into that raw
suffix, not C-owned storage. The combined 140,096 suffix bytes remain in
the unresolved coverage scope. This adds eight C instances and 23,712
instruction bytes, not exhaustive French runtime completion.
