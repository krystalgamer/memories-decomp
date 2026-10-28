# French MODEL443 entry

The loader entries `func_8013B004` / `func_8017B004`, at image offset
`0x4`, own 6,872 bytes each in MODEL62 stages 9/10. Both now use pure C
with the existing `gcc_2_8_1_g0_split` profile (GCC 2.8.1 and MASPSX
2.81). The six accepted helper bodies and their ownership are unchanged.
This is French family 443, not French module 443 or the unrelated
North American family-443 entry that wraps French family 460.

## Context and initialization

All 1,718 native instructions supplied the entry's layout, arithmetic,
call order and store order. Accepted local French and Spanish helpers
corroborate the shared geometry records and existing SDK/game declarations.
The recovered context extends through `0x36B0`; this is an accessed view,
not an allocation-capacity claim. Uncertain entry-only views remain local.

| Offset | Initialized family |
|---|---|
| `0x0000` | Three `0x370` ribbons |
| `0x0A50` | Five `0x70` framebuffer rings |
| `0x0C80` | One `0x54C` mesh |
| `0x11CC` | Three `0x274` streamers |
| `0x1928` | Three `0x1A0` two-layer spheres |
| `0x1E08` | Twelve `0x7C` spirals |
| `0x2738` | One `0x1C8` long band |
| `0x2900` | Three `0x78` short bands |
| `0x2A68` | Five `0x98` sheets |
| `0x2D60` | Six `0x90` rings |
| `0x30C0` | Four `0x90` rings |
| `0x3300` | One `0x90` retained shape |

Packet templates begin at `0x3390`; transformed vertices, three derived
positions and their mean occupy `0x3550..0x35B0`. The opposite-slot target
is at `0x35B0`, cyclic path vectors at `0x35C8`, projected deltas at
`0x35F8`, camera vector at `0x3608`, unsigned elapsed time at `0x3624`,
configuration pointer at `0x3634`, phase at `0x3698`, ambient at `0x36A8`,
and signed slot/command halfwords at `0x36AC`/`0x36AE`.

Four `Model_TransformPartVertex` calls establish the positions: the first
two vertices are averaged, and the remaining two supply the other positions.
Their mean is the origin; cyclic next-minus-current paths wrap at position
three. Configuration records have stride `0x58` at image data plus `0x650`,
with planar RGB bytes, four part bytes, four vertex halfwords and unsigned
start at `0x38`. Initialization returns 0 and sets ambient to 2048.

The first page query uses `(1,1,896,0)` and palette `(640,244)`.
The unused second query pair remains. Five uploads use data offsets
`0x618`, `0x5E0`, `0x570`, `0x5A8`, `0x634`; the first and last results
supply the two flat-packet families. The mesh's sixth indexed color
(`k == 6`) is `(128,0,128)`, not another blue endpoint: its red assignment
executes in a native branch delay slot. Previously stored ribbon inputs
at `0x3544..0x354C` are read, never given invented initializer writes.

## Update and reachability

Camera direction is view reference minus viewpoint. The opposite-slot
target, vertex means, cyclic paths and projected screen deltas update each
frame. At unsigned `start <= elapsed`, sheets then streamers run; bands
additionally require phase above 0 and ribbons phase at least 2. Mesh and
framebuffer rings independently run at phases at least 4 and 5.
All six helpers are therefore directly entry-reachable. Other initialized
families are retained without claiming they are globally dead.

Animation changes advance elapsed time and cached step with separate
frame-step calls. Ambient is submitted to constant slot 2 before updating:
below phase 8 it decreases by 128 and clamps at zero; otherwise it is
`fade << 5`. Phases 5/6 return 4; phase 7 returns 1 and enters 8; phase 8
advances signed fade, clamps at 64 and enters 9; phase 9 returns 2.

## Exact recovery

The 280-byte native frame includes two unused matrix locals.
Actual retained-row and sheet-corner counter lifetimes, shared
ring-base/latitude values, conditional endpoint indices, native loop
increments and independent origin-delta locals recover the complete entry.
No dummy operations, fabricated dependencies, inline assembly or new
compiler profiles were introduced.

The [attempt ledger](french-model-variant443-entry-attempts.csv) records
all 22 measured source experiments and both production terminals.
Both full 20,480-byte images match retail, with original-sized C
`STT_FUNC` owners and all six helper owners preserved:

| Image | SHA-256 |
|---|---|
| MODEL62 stage 9 slot 0 | `e7d2de54fd08e30386a2b866301c00cdd2e53baa6d2b5ccbf9c7a66d71cd911d` |
| MODEL62 stage 10 slot 1 | `48c2f37643adfcb265ef40f97efe3697761fc977f81af9c5a8c208208598e94e` |

This adds 13,744 matching C instruction bytes across both images and
completes all seven functions in each. Spanish registrations are unchanged.
