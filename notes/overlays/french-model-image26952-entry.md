# French MODEL26952 entry

## Exact image family

This #6460 recovery covers the entry shared by the two raw French MODEL.MRG
images below. The actual archive header numbers are 818 and 915; sector offsets
are not header numbers.

| Sector | Load address | Entry | Entry size | Complete image SHA-256 |
| --- | --- | --- | --- | --- |
| 26952 | `0x8013B000` | `func_8013B004` | 5,440 bytes | `3b5b12a238cca8ce82782efa58eb1b05f4c44a9338f00efdd2cd88244a783388` |
| 26962 | `0x8017B000` | `func_8017B004` | 5,440 bytes | `26e3aa0520acbe4e32570a2e1996f1b2e1d2c0b87d503734ebd19cbb54a06c0c` |

Both complete 20,480-byte production images equal the retail bytes, including
their original headers and unclassified suffixes. Each entry has one
original-sized C owner. The named profile is `gcc_2_8_1_g0_split`, using the
authoritative GCC 2.8.1/MASPSX 2.81 pipeline. There are no retained matching
owners in these two inventories and no existing C donor was substituted.

Native preparation and production verification used
`7e5d80e6594108d77c80ae207e726023bd5b2c5e`. Later publication composition must
preserve unrelated current-master metadata and every frozen build input; this
record does not claim a later-master combined build.

## Recovered behavior and layout

The entry launches configured objects from a circle toward randomized points
on the opposite slot, renders textured heads and impact sparks, and joins
successive history positions with fading Gouraud ribbon segments. Camera,
ambient, elapsed-time, visibility, launch, hit and completion transitions
remain in native order.

- Configuration records are 24 bytes and use the command directly as an index.
  Dimensions, travel divisor, object count, radius, spark spread/count/size,
  fade decrement and the two delays come from measured native accesses.
- State is 3,028 bytes: eight positions, velocities, rotations and targets,
  eight 32-point history rows, eight-by-eight spark offsets and frame counters,
  then the measured stage, hit, visibility, launch, length, head, ambient,
  signed texture words and eight color bytes.
- The native frame is 392 bytes. Its genuine objects are two matrices, two
  textured quads, one Gouraud quad, rotation, eight-byte dimensions/position
  storage, scale, four projection vertices and SDK outputs. No padding object,
  fabricated access, volatile or register constraint is used.
- The dimensions/position union uses the canonical eight-byte
  `ModelEffectAdjustment` and `SVECTOR` declarations. Canonical GTE types precede
  the GPU/GS headers; all primitive aliases come from `src/types.h`.
- Preserve constructor z values `-sign * 449` and `sign * 450`, fresh active-slot
  texture-upload calls, signed texture high halves, and the frame-step getter
  before the override setter.
- Head and spark submission ignore the projection flag and test signed depth.
  Spark transforms omit the head's light-matrix multiplication. Trail colors
  retain twelve separately evaluated divisions.

## Source recovery and attempts

The companion CSV records the genuine initial compiler failure and all 29
linked experiments: the first three linked trials belong to tick 291, the next
18 to tick 292, and the final eight to tick 293. Each frozen linked object was
independently relinked and its source/header/profile, bytes, frame and exact
instruction differences checked before promotion.

The decisive structures were canonical vector comma-expressions, direct indexed
history copies, separate current/previous history indices, and render-scoped
matrix/flag call views with a call-only head-packet view. The live history test
captures `head - 1` before subtracting the current length on every test; it does
not freeze a bound across SDK calls or add a new loop exit. Constructor angular
spacing reuses the scratch counter before the independently initialized
spark/history loops, and sine/cosine share the inline phase expression.

Native-sized near-matches, equivalent source forms and the alternate named
conditional-CSE profile were not accepted as identity. The final source first
matched every entry byte, then independently compiled the relocated second-slot
wrapper, then passed both complete production-image byte/hash and C-owner checks.
No reference declarations, headers or compiler claims were imported.
