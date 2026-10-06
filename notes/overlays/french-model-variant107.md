# French MODEL107 entries

Six complete 20,480-byte runtime images now use matching C for their closed
4,484-byte entries and source-owned 16-byte unit vectors. Models 402 use
stages 7/8; models 28 and 85 use stages 9/10, with headers 107/237. This adds
26,904 matching-C instruction bytes and 96 literal bytes. The 95,856 suffix
bytes remain unclassified; no suffix completeness is claimed.

The instance ledger records sector offsets, commands, slot bases and full-image
hashes. Commands 38003, 38001 and 38000 select descriptor arguments 3, 1 and 0.
Only those selected descriptors are interpreted.

## Native evidence and bounds

Each entry contains 1,121 instructions, an 888-byte frame, 41 ordered calls and
25 resident destinations, without local helpers or indirect calls. Thirty
target-compiled layout checks establish the 28-byte descriptor and 0x3B0 state.
The state contains three 17-point rings, 25 bounded position records and an
opaque 320-byte gap. Unused descriptor bytes and padding remain unknown.

Projection buffers have 51 elements, not 52: compiler padding accounts for
their stack spacing. The SDK body at `0x80087C48` reads one 8-byte SVECTOR,
writes one 4-byte projected record and one halfword to each of three auxiliary
arrays per iteration. Positive counts 34 and 51 write exactly that many
records. Selected instance counts are 20 or 25; durations are positive and
near-Z values are below 450.

`GetDispEnv` at `0x8008098C` copies 20 bytes from `0x80095B04` and returns the
destination. `SetPolyG4` at `0x80082EC8` writes packet length 8 and command 0x38;
it is not a textured-triangle initializer. Matrix composition uses the
established two-argument `MulMatrix2`. GPU declarations precede the SDK
internal header. There are no texture uploads or GsIMAGE records in this
classified prefix.

## Preserved behavior

Construction generates three concentric 17-point rings. The position-zeroing
loop repeatedly clears the same first position without advancing its pointer;
that native quirk is retained. Frame step is queried on every invocation,
including construction, and again when advancing elapsed time.

Each instance is staggered by its descriptor interval. Negative ages copy
attachment translation. Three motion phases approach the near plane, return
to `(0, -350, direction * 450)`, then extend beyond it. Every coordinate
division precedes multiplication by frame step. Rendering uses a local copy
of the position from before the persistent coordinates are updated.

Burst geometry projects 34 points into 16 gradient lines. Main geometry
projects 51 points into 16 textured quads using the current display origin.
The second UV corner intentionally uses X modulo 256, while the other three
use modulo 64. Depth and flag checks, black-to-fading line colors, primary
quad colors and the otherwise-unused POLY_G4 color stores are preserved.
Completion returns 0 before the final phase, 4 during its remaining staggered
window, then 1 once and 2 subsequently.

## Exact source recovery

The 55-row attempt ledger preserves one layout-compilation failure, 26 paired
experiments and two canonical matches. GCC 2.8.1 with MASPSX 2.81 and the named
`gcc_2_8_1_g0_split` profile produces the exact result.

Each phase evaluates scale before writing coordinates, retaining descriptor
loads across potentially aliasing stores. This restores the native
position-Z cursor, return-age spill and caller-saved projection temporaries.
The inner ring index is computed before GetTPage, although native scheduling
places its assignment after the call. The adjacent middle-ring index remains
local to the quad loop rather than sharing the burst-age scalar; that
distinction resolves the final four instruction-scheduling differences.
Doubled duration is staged before multiplication by near-Z. The positive
completion threshold retains the native terminal block order.

Independent complete-image links use actual sized C entry definitions,
exclusive 16-byte C literal sections, and disjoint raw header/suffix owners.
All six complete images and all 25 actual resident callee bodies match their
retail bytes. Production acceptance additionally requires clean French
resident and overlay builds, linked ownership checks, the family regression
and repository policy gates. No forced registers, artificial stores, copied
reference types, source-local extern declarations or generated source edits
are used.
