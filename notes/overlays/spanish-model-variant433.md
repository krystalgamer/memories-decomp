# Spanish MODEL headers 433 and 583

Four distinct Spanish images for models180/440 reuse the unchanged accepted
`variant416_{webs,spokes,rings,quad}.c` bodies through the existing French433
wrappers, plus the accepted local standalone `variant433_{strips,sheet}.c`
and their slot-one wrappers. Twenty-four compiler-owned C instances cover
28,080 instruction bytes
using the named `gcc_2_8_1_g0_split` profile, GCC 2.8.1 and MASPSX 2.81.
Shared C, local declarations, SDK types and compiler profiles are unchanged.

## Loader and boundaries

Models180/440 map to compact records180/390. Stages7/8 load ten-sector
record slices180/190 at `0x8013B000`/`0x8017B000`. The matched resident
controller calls offset4 with the context and decoded initial command,
then update command-1. The [instance ledger](spanish-model-variant433-instances.csv)
records each actual slice, complete hash and request599001/599000.

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `4..1050` | 4172 | generated assembly | yes |
| `1050..1810` | 1984 | strips C | yes |
| `1810..1C64` | 1108 | sheet C | yes |
| `1C64..21CC` | 1384 | phased webs C | yes |
| `21CC..26C8` | 1276 | generated assembly | yes |
| `26C8..29D8` | 784 | spokes C | no |
| `29D8..2D54` | 892 | rings C | no |
| `2D54..30B8` | 868 | quad C | no |

Strict walks cover every instruction and terminal return in all eight
spans. Strips, sheets and webs have direct entry-call paths; spokes, rings and quad remain
retained module-local code without that demonstrated path.
Eight assembly instances, 21,792
bytes, remain untranslated. Real input and final storage owners preserve
every four-byte header and 8,008-byte suffix. Those suffixes remain
unclassified, not established non-code.

## Layouts and real owners

Entry preserves the context through `a0 -> s3 -> s6`. The 356 checked
Spanish instruction anchors include three 152-byte sheets at `+0x10B0`,
six 144-byte rings at `+0x1278`, four 144-byte spokes at `+0x15D8` and
one 144-byte quad at `+0x1818`, including pointer reloads, advances,
counters and bounds. The spokes timing input is the first sheet's
size at `0x10B0 + 0x88 = 0x1138`. Ring color is offset128; spoke color
is offset132.

One hundred fifty-four independently recompiled constants verify those
canonical local record types and accessed fields, SDK vectors/matrices,
stored pointers in `GsCOORDINATE2`, `GsOT`, `GsGLINE`, `POLY_G4`,
`POLY_GT4` and four-byte target pointer/integer widths. Actual storage
is a complete 616-byte `.rodata` section with four size-zero NOTYPE array
labels at offsets0/292/364/452; verification uses the real extent and all values.

Three 416-byte phased-web records start at context `+0xA80`, with near/far
4-by-6 vector grids at record offsets `0/0xC0`, color at `0x180`, scale
at `0x194` and done at `0x198`. Element, row and record strides are
8/48/416 bytes. The shared line packet is at `+0x19E4`. Phase at `+0x1A84`
selects word translations `+0x1A0C/10/14` or halfwords `+0x1A18/1A/1C`.
Phase-zero scaling reads descriptor `+0x1C/+0x20`, phase one sets staggered
negative scales, and later phases advance by context `+0x1A58 << 8`.
The threshold, color/endpoint, signed depth/flag and final phase transitions
are covered by the raw Spanish instruction anchors.

Included descriptor anchors prove base`+0x31B4`, stride48
and the saved descriptor pointer at context`+0x1A60`. Actual requests
select commands1/0, and both 48-byte windows fit the suffix owners.
The minimum directly accessed entry context extends to `+0x1A98`.

The entry calls the sheet helper at `+0xEB0`, passing the context in its
delay slot. Only the first two sheet iterations read companion word `+104`
from the two 168-byte records at `+0xF60`; the third bypasses that read.
The independent strip proof below recovers their accessed fields without
assigning meaning to the remaining opaque ranges.

The fixed 52-byte GT4 at `+0x191C` is reused without advancing its pointer.
The 264-byte frame separates coordinate storage `[128,208)`, projection
outputs `[208,212)` and `[212,216)`, and the ordering-table pointer
`[216,220)` from saved registers starting at 224. Stable packet/context
registers, companion advancement and stack offsets have regional regressions.
The sheet's direct context minimum is `+0x1A88`, below the separately
proved entry minimum.

Fresh exact Spanish resident proofs identify 36 real callee input and
final owners, three matching initializer/controller/loader owners, and
both context-pointer storage owners at `0x80010024/28`. Those pointers
are input `.data` in `spanish_raw_80010000.o`, even though their linked
output `.main` also contains executable code. The selected contexts
`0x80136000/0x80176000` and minimum views do not overlap the selected
96-sector MODEL, two-sector primary or ten-sector secondary loads.
This does not prove allocation capacity or whole-game lifetime isolation.

## Entry-called strips

The strip helper at `+0x1050` independently reproduces 1,984 bytes in both
slots and all four actual Spanish images. Its two 168-byte records occupy
`+0xF60..+0x10B0`: vector rows at `0/16/32`, projected words at `48/56/64`,
progress at104, two completion words at112, colors at120/128 and depths
at160. Ranges `[72,104)` and `[136,160)` remain opaque. Forty-one new
target-compiled constants extend the 113 retained canonical/SDK constants.

Actual Spanish instructions prove signed-radius truncation toward zero,
using the logical shift and signed16 narrowing retained by the local C.
All 65,536 input halfwords were checked. The reset loop intentionally
reuses the signed-short column index and leaves it equal to two: the
following completion-product read lands at record+120, the first color
word. Full-record word-column views preserve this access without inventing
a third completion field or using an out-of-bounds `done[2]` expression.

The call at `+0xECC` follows the sheet call; the shared unsigned descriptor
gate and additional phase `< 3` test remain unchanged. The four-byte guest
pointer at `+0x1A60` selects descriptor+40 for completion versus reset.
Actual commands599001/599000 select descriptors `+0x31E4/+0x31B4` and
measured timing words `0/76/90/216` or `0/26/70/250`.

A fixed 52-byte GT4 occupies `+0x18E8..+0x191C` without packet advancement.
The 296-byte frame separates coordinate storage `[120,200)`, four flags
`[200,216)`, projection output `[216,220)` and ordering-table pointer
`[220,224)` from saved registers starting at256. Stable context/packet
registers, two-record advancement, projection argument addresses and
low16 depth sorting have regional regressions. The helper minimum is
`+0x1A88`; it is not an allocation or lifetime claim.

## Exactness and scope

The [attempt ledger](spanish-model-variant433-attempts.csv) preserves twelve
terminal canonical-wrapper matches covering all four images. Complete
unmasked links reproduce every image with sized compiler functions,
explicit assembly fallbacks and real header/suffix storage. Dependency
fingerprints and actual resident owners are checked separately from hashes.
The `ratan2`, `rcos` and `RotTransPers3` aliases name their existing verified
addresses `0x80089928`, `0x800866F8` and `0x80087898`; the 36-callee address
set and SDK classifications are unchanged. The strip's twelve callees,
including both newly named SDK bindings, have fresh real input/final owners.
All five helpers were freshly recompiled after the accepted shared web
body gained its French-only tail-offset selection. The Spanish/default
body still matches exactly. Historical objects were not relabeled with
new source hashes. The strip objects were compiled independently afterward,
with all retained source dependencies rechecked unchanged.

Regional regressions reuse the French source and boundary fixture, adding
Spanish fallback-binding, actual descriptor and minimum-context checks.
Previously accepted module records remain unchanged; progress snapshots
stay separate. This strip extension adds four C instances / 7,936 bytes and
keeps all 154 configured images, including accepted MODEL415 curtains and
MODEL402 layers and MODEL433 sheets, now 800/1,082 C instances and
808,628 C bytes.
Spanish helper expectations explicitly select the independently verified
strips and sheet, rather than implicitly inheriting French ray ownership.
The shared strip regressions run against each release's actual archive.
Pending MODEL440 sheets are not stacked.
Unknown game code, further Spanish runtime discovery and
the expanded seven-release campaign remain open.
