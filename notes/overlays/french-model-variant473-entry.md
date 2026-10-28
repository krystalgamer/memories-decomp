# French MODEL473 entry

The loader entries `func_8013B004` / `func_8017B004`, at offset `0x4`
in MODEL385 stages 9/10, now use pure C with the existing
`gcc_2_8_1_g0_split` profile (GCC 2.8.1 and MASPSX 2.81).
Each entry preserves all 4,168 instruction bytes and its 248-byte frame.
The five accepted helper bodies and registrations are unchanged.
The unmatched ribbon at `0x1FB0` remains assembly.

## Measured context and initialization

All 1,042 native entry words establish the layout, stores, arithmetic
and call order. Accepted local grid, sheet, funnel and streamer
declarations corroborate the shared records. Entry-only ribbon and
spiral views stay local. The accessed extent is `0x3C78`, not
allocation capacity.

| Offset | Initialized family |
|---|---|
| `0x0000` | Five `0x4F8` meshes, nine rows of seventeen points each |
| `0x18D8` | Five `0x22C` ribbons |
| `0x23B4` | Twelve `0x7C` spirals |
| `0x2984` | Six `0x98` sheets |
| `0x2D14` | Two `0x334` streamers |
| `0x337C` | Five `0x118` funnels |

Packet templates span `0x38F4..0x3AD8`, with retained opaque gaps.
The single part matrix is at `0x3AE8`, opposite-slot target at `0x3B08`,
five derived positions at `0x3B10`, primary direction at `0x3B60`,
five position-relative directions at `0x3B70`, screen delta at `0x3BC0`,
and camera delta at `0x3BC4`. Unsigned elapsed time is at `0x3BE0`,
configuration at `0x3BF0`, part pointer at `0x3BF8`, phase at `0x3C5C`,
ambient at `0x3C70`, and signed slot/command halves at `0x3C74`/`0x3C76`.

Configuration records have stride `0x2C` at image data plus `0xFC`,
with outer/inner RGB at `0x0`/`0x4`, part byte at `0x10`, and unsigned
sheet start at `0x18`. The initial target is `(0,-300,-450)` in slot
zero and `(0,-300,450)` otherwise. Both page queries use `(1,1,896,0)`
and palette `(640,244)`; the second query results remain unused.
Four mode-one uploads use data offsets `0xC4`, `0x8C`, `0x1C`, `0xE0`.
The last result supplies an additional two-FT4 family with UV rectangles
`(0,224)..(31,255)` and `(32,224)..(63,255)`.

Mesh latitude uses `(base << 2) / 8`, with base increasing by 325.
Radius and height use sine/cosine scaled by 256. Seventeen longitude
samples advance by 256. Row RGB is `(row*20,row*20,192)`.
Each mesh's size is zero, word `0x4F0` is 1024, and done word is zero.
Spirals initialize their first outer/inner pair to `(255,0,255)` /
`(128,128,128)` and their second pair to zero.

Each ribbon initializes nine inner colors `(j*24,j*24,j*24)` and outer
colors `(0,0,j*16)`, scale 4096, and progress `-(index*512)`.
Its alternating bearings retain signed division of the actual
1024-multiple by six. Positions have radius 256 in X/Y and zero Z.
Sheets use white outer RGB and `(0,64,255)` inner RGB.

Funnels use unsigned trigonometric shifts: inner X/Z is `(u32)trig >> 4`,
outer X/Z is `(u32)(trig*5) >> 6`, and outer Y is -64.
The native `index == 5` zero-scale branch remains even though this
initialization loop covers indices zero through four. Otherwise scale
is `-(index*4096/5)` with signed division; word `0x114` is zero.
Both streamers initialize seventeen gray colors and word `0x2A8`.
Context inner and outer RGB come from the corresponding configuration
bytes, without assuming meaning for their fourth bytes.

## Update and reachability

Camera-facing bearings use reference minus viewpoint. The height reset
belongs to each explicit opposite-slot copy branch. The single part
matrix and primary target-minus-translation direction refresh.

The first four derived positions are radius-512 orbits around the target,
with bearing `index*2560/3 - 256` and an additional half turn in slot
zero. The fifth position copies the target directly. Every secondary
direction subtracts the same part-matrix translation. Projection uses
that translation and the main target, preserving unsigned X and signed
Y through real delta locals.

Funnels then meshes run in phases 3-5, ribbons in phases 2-4, and spirals
in phases 1-4. Sheets run afterward at unsigned `start <= elapsed`
while phase is below 5. All five are directly entry-reachable.
Streamer initialization does not imply a direct entry call; the retained
streamer helper is unchanged, with no global-dead-code claim.

Animation changes advance unsigned elapsed time and cached step through
two separate frame-step calls. Ambient is submitted to constant slot 2
before updating. Below phase 7 it decreases by 128 and clamps at zero;
otherwise it becomes `fade << 5`. Phases 3/4 return 4; phase 6 returns
1 and enters 7; phase 7 advances fade to 64 and enters 8; phase 8 returns 2.

## Exact recovery

The first complete candidate had the correct size and only eleven
instruction differences. Real negative-shift grouping, width/progress
reset order and signed projection locals left six scheduling words.
A conditional slot argument folded away the native branch and was
rejected. Keeping target Y reset in each actual slot-copy branch restored
the native call merge and matrix-boundary scheduling.
No fake dependencies, forced registers, inline assembly, fabricated
accesses or new profiles were introduced.

The [attempt ledger](french-model-variant473-entry-attempts.csv) records
all four immutable source/header experiments and both production terminals.
Both entire 20,480-byte images reproduce retail, with all six C
`STT_FUNC` owners retaining their original addresses, sections and sizes:

| Image | Full-image SHA-256 |
|---|---|
| MODEL385 stage 9, slot 0 | `7f74d3cad4e561f830dc115f95bb6ced48247cf0de1339a86e80d85578f71d9b` |
| MODEL385 stage 10, slot 1 | `737fb9e4e627eeb7015f3481e0f8c19f5014c8a0f93d6513357a1945dd3a0911` |

This adds two C instances / 8,336 matching instruction bytes.
Spanish registrations and all five prior helper sources remain unchanged.
