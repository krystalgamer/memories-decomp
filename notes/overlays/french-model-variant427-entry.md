# French MODEL427 entry

The loader entries `func_8013B004` / `func_8017B004`, at offset `0x4`
in MODEL379 stages 9/10, now use pure C with the existing
`gcc_2_8_1_g0_split` profile (GCC 2.8.1 and MASPSX 2.81).
Each entry retains all 4,016 instruction bytes and its 248-byte frame.
The five accepted helper bodies and registrations are unchanged; the
unmatched ribbon at `0x1FC4` remains assembly.

## Measured context and initialization

All 1,004 native entry words establish the layout, stores, arithmetic
and call order. Accepted local mesh, sheet, curtain and streamer views
corroborate the shared records. Entry-only ribbon and spiral views remain
local. The minimum accessed extent is `0x37E0`, not allocation capacity.

| Offset | Initialized family |
|---|---|
| `0x0000` | Three `0x4F0` meshes, nine rows of seventeen points each |
| `0x0ED0` | Three `0x3A8` ribbons |
| `0x19C8` | Twelve `0x84` spirals |
| `0x1FF8` | Six `0x98` sheets |
| `0x2388` | Two `0x378` streamers |
| `0x2A78` | Nine `0x118` funnels |

Packets occupy `0x3450..0x3618`, with retained opaque gaps. Three part
matrices begin at `0x3628`; the opposite-slot target is at `0x3688`,
three orbit targets at `0x3690`, primary directions at `0x36C0`, and
orbit-relative directions at `0x36F0`. Screen and camera deltas begin at
`0x3720` and `0x3724`. Unsigned elapsed time is at `0x3740`, configuration
at `0x3750`, three part pointers at `0x3758`, phase at `0x37C4`, ambient
at `0x37D8`, and signed slot/command halves at `0x37DC`/`0x37DE`.

Configuration records have stride `0x2C` at image data plus `0xFC`,
with three part bytes at `0x10..0x12` and unsigned sheet start at `0x18`.
Initialization resolves all three parts and matrices. The initial target
is `(0,-300,-450)` for slot zero or `(0,-300,450)` otherwise.
Both page queries use `(1,1,896,0)` and palette `(640,244)`.
Mode-one texture uploads use data offsets `0xC4`, `0x8C` and `0x1C`.

Mesh latitude uses `(base << 2) / 8`, advancing base by 325 per row.
Radius is `rsin(latitude) * 384 >> 12`; height combines the latitude
cosine with `rsin(276)`, both scaled by 384. Seventeen longitude samples
advance by 256. Every mesh row is `(0,192,0)`.
Spirals initialize their first outer/inner color pair to `(0,255,0)` /
`(128,128,128)` and their second pair to zero.
Ribbon colors are `(128,128,128)` and `(0,192,16)`, scale is 4096,
and initial progress decreases by 1024 per ribbon. Their alternating
bearings and radius-256 positions retain the native branch structure.
Sheets use white outer RGB and `(0,255,64)` inner RGB.

Funnels use unsigned shifts of the trigonometric results: inner X/Z
is `(u32)(trig * 3) >> 5`, outer X/Z is `(u32)trig >> 3`, and
outer Y is -300. The first three scales are zero; subsequent scales
are `-((index - 3) * 8192 / 6)` with signed division by six.
All nine counts are zeroed. Both streamers initialize seventeen gray
colors and the word at record `0x2A8`. Context RGB is fixed
`(128,192,128)` / `(0,192,0)`, not copied from configuration.

## Update and reachability

The camera-facing bearings use reference minus viewpoint. Both explicit
slot-copy branches clear target Y. All three part matrices and primary
directions refresh. Three radius-256 orbit targets use thirds of a full
turn, with an additional half turn in slot zero. Each secondary direction
subtracts matrix `index % 3` translation from its orbit target.
Screen projection uses the first matrix translation and the main target,
preserving unsigned X and signed Y treatment.

Sheets run at unsigned `start <= elapsed` while phase is below 5.
Funnels then meshes run in phases 3-5, ribbons in phases 2-4, and spirals
in phases 1-4. These five helpers are directly entry-reachable.
Streamer initialization does not imply a direct entry call; the retained
streamer helper is unchanged, with no global-dead-code claim.

Animation changes advance unsigned elapsed time and cached step through
two separate frame-step calls. Ambient is submitted to constant slot 2
before updating. Below phase 6 it decreases by 128 and clamps at zero;
otherwise it becomes `fade << 5`. Phases 3/4 return 4; phase 6 returns
1 and enters 7; phase 7 advances fade to 64 and enters 8; phase 8 returns 2.

## Exact recovery

The first complete candidate had the exact size and register-normalized
instruction sequence, with 86 register-only word differences.
Reusing the real part/matrix index for sheet corners recovered the native
allocation; latitude and longitude share their actual angle lifetime.
The signed funnel numerator retains its native induction and division.
No fake dependencies, forced registers, inline assembly, fabricated
accesses or new compiler profiles were introduced.

The [attempt ledger](french-model-variant427-entry-attempts.csv) records
both source/header experiments and both production terminals.
Both entire 20,480-byte images reproduce retail, with all six C
`STT_FUNC` owners retaining their original addresses, sections and sizes:

| Image | Full-image SHA-256 |
|---|---|
| MODEL379 stage 9, slot 0 | `f14ad30b942052d0d5760089ad294f268bd7381fd1be054c3a8c1c77ece241ee` |
| MODEL379 stage 10, slot 1 | `8a246df22ac924e2078183da0cf25e21112d159f07f8b0a5330840308404e1c2` |

This adds two C instances / 8,032 matching instruction bytes.
Spanish registrations and all five prior helper sources remain unchanged.
