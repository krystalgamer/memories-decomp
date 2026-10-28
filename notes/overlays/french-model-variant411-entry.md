# French MODEL411 entry

The loader entries `func_8013B004` / `func_8017B004`, at image offset
`0x4`, own 3,676 bytes each in MODEL364 and MODEL452 stages 9/10.
All four French images now have five matching C functions, using
`gcc_2_8_1_g0_split` (GCC 2.8.1 and MASPSX 2.81). The four existing
helper bodies and their ownership are unchanged.

## Context and initialization

All 919 native instruction words establish the entry's layout, arithmetic,
calls and stores. Existing local tube, sheet, funnel and beam declarations
corroborate the recovered records. The entry's accessed extent is `0x1690`,
not an allocation-capacity claim. Entry-only views remain local.

| Offset | Records |
|---|---|
| `0x0000` | One `0x54C` tube |
| `0x054C` | Two `0x98` sheets |
| `0x067C` | Two `0x118` seventeen-point funnels |
| `0x08AC` | Four `0xB0` eight-point ring records |
| `0x0B6C` | Two `0x12C` records with colors and scale initializers |
| `0x0DC4` | Sixteen `0x64` beams |

Packet templates begin at `0x1404`; the matrix is at `0x15E4`, target at
`0x1604`, direction at `0x160C`, projected deltas at `0x161C`, and camera
vector at `0x1620`. Frame count, elapsed time, animation frame and step
occupy `0x1638..0x1644`; signed fade is at `0x1648`, configuration pointer
at `0x164C`, part pointer at `0x1654`, progress at `0x1658`, phase at
`0x167C`, ambient at `0x1688`, and signed slot/command at `0x168C/0x168E`.

Configuration records have stride `0x30` at data plus `0xFC`: part byte
at `4`, unsigned sheet start at `0xC`, expansion start/end at `0x10/0x14`,
fade start/end at `0x18/0x1C`, and signed XYZ offsets at `0x24..0x2C`.
The initial target is `(0,-300,-450)` in slot 0 and `(0,-300,450)` otherwise.

The entry retains both page/palette query pairs and five uploads at data
offsets `0xC4`, `0x8C`, `0x1C`, `0x38`, `0x54`. The first upload supplies
the flat pair; the third supplies the ring packet. The final real return
overwrites the packed local, ending the earlier snapshots. The secondary
GT4 receives non-semitransparent, unshaded flags without invented UV/page
stores. Its neighboring extra GT4 is semitransparent and unshaded.

Funnels use XY radii 256/320 and outer z=-64. Both sizes initialize to zero:
the second uses the explicit zero branch, while the other branch retains
the count-derived `k * -4096` expression. Its otherwise redundant negative
induction is present in retail and must not be simplified away.
Tube rows alternate gray `(96,96,96)` and blue `(0,0,128)`.
Beam age order repeats `0,2,3,1,4,7,5,6`; outer colors add 127 to their
inner RGB channels. Sheets use extent 256, white outer colors and
`(0,64,255)` inner colors.

Ring bearings advance by 256 between records and 512 between points.
Their palette counter continues across all four rings, with sizes
`1024 - row*256`, inner color `(64,128,255)` and black outer color.
The two `0x12C` records receive scale `(4096,4096,4096)`, white inner
colors and `(0,16,128)` outer colors. Vector padding and color code bytes
remain untouched; the entry does not initialize every geometry point.

## Update and reachability

Camera direction is view reference minus viewpoint. The opposite-slot
position supplies the target. After refreshing the part matrix, slot 0
adds the configuration's XYZ offsets; other slots subtract X/Z and add Y.
Projected x is unsigned and projected y is signed.

There are three direct helper calls: sheets run at `sheet_start < elapsed`;
the tube and then funnels run at `expansion_start < elapsed`. These are
strict unsigned comparisons. The accepted beam helper is initialized but
has no direct entry call; this does not prove global unreachability.

Animation changes advance elapsed time and cached step with two separate
frame-step calls. Ambient is always submitted to constant slot 2; below
phase 5 it decreases by 128 and clamps to zero, otherwise it becomes
`fade << 5`. Phases 2/3 return 4; phase 4 returns 1 and enters 5; phase 5
unconditionally advances signed fade, clamps at 64 and enters 6; phase 6
returns 2. There is no additional fade-below-64 entry guard.

## Exact recovery

The [attempt ledger](french-model-variant411-entry-attempts.csv) records
all five measured source experiments and four production terminals.
Observed field-cursor declaration order, an independent sheet counter,
palette store order and bearing-update lifetime recover all but the funnel
induction. Expressing that progress through its actual outer count recovers
the remaining spill order and all 3,676 bytes. No inline assembly, fixed
registers, fabricated accesses or arithmetic dependencies, or new profiles
are used.

Each complete 20,480-byte image matches retail, with an original-sized C
`STT_FUNC` entry and all four existing helper owners preserved:

| Image | SHA-256 |
|---|---|
| MODEL364 stage 9 slot 0 | `9e7469410ebcf1f9043b5e65d0b44f62186cd69f3ffc73aedded9a10b6f4a71e` |
| MODEL364 stage 10 slot 1 | `1b376cd5148754401d08efa464a08b506ca14d65f5f4a5a8aa30bf234088673b` |
| MODEL452 stage 9 slot 0 | `d2308e43e7d8fab71a28f95469f668be87c84f34117b16493c9da574e5c83444` |
| MODEL452 stage 10 slot 1 | `920ed95a4727117d93789deabbf7c70321ad0abe01892db31a01fe9e1b97f985` |

This adds 14,704 matching C instruction bytes across the four French images.
Spanish registrations, raw header/tail ownership and retained helpers are
unchanged.
