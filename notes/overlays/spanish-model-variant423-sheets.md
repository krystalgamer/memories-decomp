# Spanish MODEL423 two-sheet renderer

Independently recover `+0x2564..+0x2A50`, 1,260 bytes in each of the two
[MODEL423 images](spanish-model-variant423.md). Both scratch compilations
and both complete production images match with `gcc_2_8_1_g0_split`,
GCC 2.8.1 / MASPSX 2.81. No regional source port, forced registers,
allocation controls, padding or new flags are involved.

This adds two C instances and 2,520 instruction bytes. The accepted mesh
C/header/wrapper, all symbol bindings, physical image manifest, assembly
and raw bytes are preserved. Four C instances now cover 4,856 bytes;
ten function instances remain assembly.

## Context and storage

Entry `+0xC58` calls the helper with original context in `s2`, passed in
delay slot `+0xC5C`. The only reaching definition is original `a0` at
entry `+0xC`; both the mesh and sheet call sites are covered by the
existing context regression.

| Field | Context offset / extent |
|---|---|
| Two 152-byte sheet records | `+0xE1C..+0xF4C` |
| Reused GT4 | `+0x1CA4..+0x1CD8` |
| Signed word origin | `+0x1D4C` |
| Signed word direction | `+0x1D60` |
| Frame / unsigned time / step | `+0x1D8C / +0x1D90 / +0x1D98` |
| G32 descriptor / signed progress | `+0x1DA0 / +0x1DD4` |
| Shared phase | `+0x1E0C` |

The `0x1E10` partial state view does not claim allocation capacity.
Resident storage-owner checks cover the actual contexts and bank extents.
Entry `+0x34` forms `context + 0x1C70` in `s7`; its only intervening
write adds 52 at `+0x2A4`. `SetPolyGT4` at `+0x2A8`, with argument in
delay slot `+0x2AC`, therefore initializes the exact selected packet.
All twelve helper color stores stay within it.

Geometry initialization proves two records, each containing four arrays
of four SVECTORs. Record size, all four arrays, color fields, packet
coordinates, descriptor fields and state offsets are checked by 32
target-compiled layout constants.

## Rendering and progression

The first sheet uses the unchanged origin. The second adds signed
`direction * progress / 1024` to each origin component. Odd frames add
signed `size / 8` to uniform scale. Four quads per sheet use inner RGB
for the first three corners and outer RGB for the fourth. Signed depth
is multiplied by eight and divided by ten; nonnegative depth and flag
permit `GsSortPoly` with the original sixteen-bit priority narrowing.

At phase zero the first sheet grows through unsigned descriptor timing
to 4,096, then sets phase one. At phase two the second sheet is set to
4,096; phase three grows it by `step * 512` to 8,192, then sets phase
four. Both shrink through unsigned timing and clamp signed nonpositive
results to zero. The original division traps remain intact.

The 44-byte descriptor view has growth fields at `+0x18/+0x1C` and
shrink fields at `+0x24/+0x28`. The retail row at image `+0x38D0`
contains timing words 20, 100, 160, 360, 420: growth denominator 80
and shrink denominator 60.

## Exactness and ownership

Three sheet regressions cover initialized storage and timing, target
layouts and terminal fingerprints, and selected input/final C owners
with all relocations. The mesh regressions additionally verify both
C owners, every retained assembly/raw owner, both original-context
call sites, exhaustive physical identity and resident bindings.

Each sheet object has ten external calls to nine distinct addresses,
including two `RotMatrix` calls. Its local jumps are function-relative
`+0xA4 -> +0x134`, `+0x3DC -> +0x49C` and `+0x3F8 -> +0x448`.
The [four-row attempt ledger](spanish-model-variant423-sheets-attempts.csv)
records both independent scratch matches and both complete-image
terminals, including source and transitive-header fingerprints.
