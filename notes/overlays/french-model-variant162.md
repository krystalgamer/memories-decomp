# French MODEL162 entry

Model 37 stages 7/8 load two distinct 20,480-byte images with headers
162/292 and command 93000, passing initial argument 0. Both entries are
3,496 instruction bytes at offset `0x4`, followed by a 16-byte source-owned
unit-scale vector. The slot wrapper renames the entry and three local symbols.

All 874 native instructions form one closed entry, with 53 direct calls to
27 resident destinations and no local or indirect calls. Both complete images
match with sized linked C entry definitions, exclusive C literal contributions
and disjoint raw owners. All 27 actual resident callee bodies were checked
against the exact French resident ELF. French `0x80089C48` is independently
identified as `RotTransSV`: its complete 48 bytes equal the North American
`RotTransSV` at `0x80089CC0`, whose inventory records the authoritative
Psy-Q signature. Its existing three-pointer header declaration is reused;
the callee remains SDK assembly, not a game-C match.

## Measured state and behavior

The descriptor is 20 bytes: line RGB at `0..2`, quad RGB at `3..5`, active
part at `6`, unidentified byte `7`, and signed halfword radius at `8`, size
parameter at `0xA`, line duration at `0xC`, quad duration at `0xE`, line delay
at `0x10` and quad delay at `0x12`. Selection uses the full entry argument,
not the archive command. The selected tuple is
`(128,128,128,64,64,32,22,0,200,50,40,40,140,160)`.

The minimum context view is `0x820` bytes: a guest-width descriptor pointer
at `0`, 256 SVECTOR points at `4`, anchor at `0x804`, rotation at `0x80C`,
one texture word at `0x814`, completion byte at `0x818`, unidentified bytes
at `0x819..0x81C`, and elapsed time at `0x81C`. This is an accessed view,
not a recovered allocator extent. Initialization creates a growing spiral
using `radius * i / 256` and angle `i * 64`, uploads one texture in mode 1,
and clears elapsed/completion. The active-slot call and frame-step read
occur even during initialization.

The line phase samples 64 positions, adding scaled displacement between
adjacent active-model parts to a time-indexed subset of the spiral.
Division by duration precedes multiplication by sample index and division
by 64. Displacement negation and doubling each store back into signed
halfwords. Bulk projection feeds 63 line segments; sorting retains the
depth and `0x20` flag checks. The line attribute is `0x50000000`.

Before the quad delay, the second phase refreshes the anchor and rotation
from the same adjacent parts. During its duration, the quad uses half the
size parameter horizontally and twice it vertically, fades the second RGB,
and visits every other point up to `(time << 8) / duration`. The native
`RotTransSV` call is retained even though its output position is overwritten
immediately afterward. Position is then doubled in signed halfwords before
adding the anchor. UVs span 32 by 128 texels. Quad projection retains signed
depth/flag checks; a separate fading fullscreen flash is also sorted.

After adding the saved frame step, return values are 0 before quad delay,
4 during its duration, 1 on first completion and 2 thereafter. Completion
increments a byte, and the terminal result retains its byte-width convergence.

## Matching evidence and remaining coverage

The first paired candidate was 3,452 bytes with the correct 1,448-byte frame,
but 667 differing words per slot. Established compound vector multiplication,
an explicit spiral base, and native loop sequencing recovered the correct
size with 13 differences. A byte terminal result reduced this to eight.
Unchanged-output compiler RTL identified those eight as spill offsets:
the frame-step pseudo preceded the three temporary vector-address pseudos.
Beginning the saved-step declaration at its existing call in a C89 block
recovered the native spill order without adding operations or changing calls.
All four paired experiments and two canonical matches are in the ten-row
ledger. GCC 2.8.1/MASPSX 2.81 and the named local profile remain authoritative;
there are no forced registers, artificial stores, inline assembly or masks.

Each image retains a raw four-byte header, one 28-byte GsIMAGE view at
`0xDBC..0xDD8`, and a 16,936-byte unclassified suffix at `0xDD8..0x5000`.
The descriptor is read through a local typed view inside that suffix;
its storage is not promoted to C. The combined 33,872 suffix bytes are
not claimed as data-only or excluded game code. This adds two C instances,
6,992 instruction bytes and 32 literal bytes, not exhaustive French
runtime completion.
