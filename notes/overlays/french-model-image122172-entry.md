# French MODEL122172 attachment trails and timed bursts

This independently recovers the complete loader in two French MODEL.MRG
images. Their relocated code is shared, but their original header words
are different: 142 at sector 122172 and 272 at sector 122182.

| Sector | Entry | C bytes | Complete image SHA-256 |
| --- | --- | --- | --- |
| 122172 | `0x8013B004` | `0x1528` | `abc7448cb7b5ffca0b8a1017b6527526160d30f4caf85900638230c7cff3b7e8` |
| 122182 | `0x8017B004` | `0x1528` | `51e8dd192de454fdfdf1efa356e2e4ca1ecec55a435b678692103fe04b01916a` |

Both use the authoritative named `gcc_2_8_1_g0_split` profile. The four-byte
headers and all bytes from image offset `0x152C` remain original raw data.
The external scale vector, texture descriptor and configurations at slot-zero
addresses `0x8013C52C`, `0x8013C53C` and `0x8013C558` are not new C data owners.
Slot one relocates those addresses by `0x40000`.

## Measured state and configuration

The 1044-byte state holds its stored configuration pointer at zero,
96 `SVECTOR` attachment-history records at offset 4, 32 random seed vectors
at 772, one initialized packed texture value at 1028, elapsed time at 1036
and the event byte at 1040. The word at 1032 remains unidentified; no second
texture initialization or semantic role is invented.

The 44-byte configuration has trail, sprite and dot RGB triples at offsets
0, 3 and 6; attachment IDs at 9; trail count, history length and event count
at 13, 14 and 15; then signed halfword size, random radius, trail duration,
flash/sprite duration, dot duration and trail delay at 16 through 26.
Eight halfword event timestamps occupy offset 28 onward.

## Native behavior and lifetimes

Setup clears all history XYZ values, gives each fourth halfword the negative
history length, initializes 32 random XYZ seeds, uploads one texture, and
clears elapsed time and the event index. The first history record's fourth
halfword is the shared ring cursor, not ordinary unused vector padding.

Each update captures matrices for consecutive attachment IDs into paired
history endpoints. While the cursor is negative it fills one advancing
sample; afterward it wraps by the configured history length. The trail pass
narrows successive endpoint pairs toward their midpoint, projects the
96-element vertex region, and submits fading `POLY_G4` strips using the
native depth and combined projection-flag guards.

Each event also triggers a fading full-screen `POLY_F4`, an eight-frame
textured sprite, and 32 expanding two-pixel boxes. The sprite and box
passes retain their different matrix-composition sequences. Elapsed time
advances by the resident frame step. Return values remain 0 before an event,
4 for intermediate notifications, 1 for the last notification and 2 after
the native final-event/trail-duration completion test.

The entire 2056-byte frame is accounted for by real SDK primitives, matrices,
vectors and projection buffers: 96 `SVECTOR` vertices, 96 `DVECTOR` screen
positions, and three 96-element `u16` depth/interpolation/flag arrays. The
SDK declarations are the project's canonical local declarations. In
particular, the ordering-table getter uses `func_80058F10` from
`src/psyq/sdk_internal.h`, bound to the French target `0x8005C018`, as in
the accepted French MODEL96 and MODEL40200 providers. The provisional
private `func_8005C018` declaration was removed: the French inventory
classifies that target as SDK-owned, not a new game-owned interface.

Four genuine cached vertex indices preserve the values shared by screen
coordinates, `AverageZ4` and flag aggregation. The history sampler is distinct
from the output cursor reused for the final dot projection. The previous
sample index has two real calculation stages, and the next sample is retained
for the second endpoint's color. Finally, the halfword blue value is reused
for grayscale flash intensity; sharing the red value instead changes native
register allocation and scheduling.

## Recovery evidence

The attempt ledger `french-model-image122172-entry-attempts.csv` records
17 frozen source/header experiments, including the initial SDK-field compile
failure and the exact terminal result. All 15 successfully compiled candidates
from the original sixteen experiments were freshly recompiled and independently
linked in both relocation contexts. The appended canonical-SDK-declaration
production experiment separately compiles both C owners and reproduces both
complete 20480-byte original images, including their distinct headers and
unchanged raw suffixes.

No reference source, reference types or reference compiler claims were used.
No padding operations, fabricated dependencies, fixed registers, inline
assembly, synthetic behavior harnesses or compiler-profile changes were added.
