# Spanish MODEL425 two-sheet renderer

Independently recover `+0x26DC..+0x2BC8`, 1,260 bytes in each of the two
[MODEL425 images](spanish-model-variant425.md). Both scratch compilations
and both complete production images match with `gcc_2_8_1_g0_split`,
GCC 2.8.1 / MASPSX 2.81. No regional source port, forced registers,
allocation controls, padding or new flags are involved.

This adds two C instances and 2,520 instruction bytes. The accepted mesh
C/header/wrapper, all symbol bindings, physical image manifest, assembly
and raw bytes are preserved. Four C instances now cover 4,848 bytes;
ten function instances remain assembly.

## Context and storage

Entry `+0xCBC` calls the helper with original context in `s2`, passed in
delay slot `+0xCC0`. The only reaching definition is original `a0` at
entry `+0xC`; both the mesh and sheet call sites are covered by the
existing context regression.

| Field | Context offset / extent |
|---|---|
| Two 152-byte sheet records | `+0xE1C..+0xF4C` |
| Reused GT4 | `+0x16A0..+0x16D4` |
| Signed word origin | `+0x1748` |
| Signed word direction | `+0x175C` |
| Frame / unsigned time / step | `+0x1788 / +0x178C / +0x1794` |
| G32 descriptor / signed progress | `+0x179C / +0x17D0` |
| Shared phase | `+0x180C` |

The `0x1810` partial state view does not claim allocation capacity.
Resident storage-owner checks cover the actual contexts and bank extents.
Entry `+0x34` forms `context + 0x166C` in `s7`; its only intervening
write adds 52 at `+0x29C`. `SetPolyGT4` at `+0x2A0`, with argument in
delay slot `+0x2A4`, initializes the exact selected packet.
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
4,096; phase three grows it by `step * 2048` to 16,384, then sets phase
four. Both shrink through unsigned timing and clamp signed nonpositive
results to zero. The original division traps remain intact.

The 48-byte descriptor view has growth fields at `+0x18/+0x1C` and
shrink fields at `+0x28/+0x2C`. The retail row at image `+0x3A04`
contains timing words 120, 220, 240, 360, 540, 580: growth denominator
100 and shrink denominator 40. The larger cap, growth step and descriptor
offsets are independently established rather than borrowed from MODEL423.

## Exactness and ownership

Three sheet regressions cover initialized storage and timing, target
layouts and terminal fingerprints, and selected input/final C owners
with all relocations. The mesh regressions additionally verify both
C owners, every retained assembly/raw owner, both original-context
call sites, exhaustive physical identity and resident bindings.

Each sheet object has ten external calls to nine distinct addresses,
including two `RotMatrix` calls. Its local jumps are function-relative
`+0xA4 -> +0x134`, `+0x3DC -> +0x49C` and `+0x3F8 -> +0x448`.
The [four-row attempt ledger](spanish-model-variant425-sheets-attempts.csv)
records both independent scratch matches and both complete-image
terminals, including source and transitive-header fingerprints.
