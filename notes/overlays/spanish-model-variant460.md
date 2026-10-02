# Spanish MODEL headers 460 and 610

Twenty-eight independently checked Spanish secondary-handler images use the
unchanged accepted French460 entry and `variant443_{ribbons,sheets,strand}.c`
bodies through the six local French460 helper wrappers. The named
`gcc_2_8_1_g0_split` profile uses
GCC 2.8.1 and MASPSX 2.81. No source, declaration, header or compiler-profile
change is required. French and North American matches supplied a starting
hypothesis, not Spanish byte identity or ownership.

## Loader and complete boundaries

The [instance ledger](spanish-model-variant460-instances.csv) records each
actual compact record, command, sector, slot and complete Spanish hash.

| Stages | Models |
|---|---|
| 7/8 | 70, 125, 168, 460, 469, 704 |
| 9/10 | 44, 98, 161, 370, 400, 458, 462, 558 |

The matching Spanish `Model_LoadMonsterMerge` at `0x8005967C` rejects
300..349, 650..699 and 720 and compacts subsequent IDs. Model704 is record604,
not654. Each record occupies276 sectors. Stages7/8 use sectors180/190 and
stages9/10 use200/210 within it. Ten-sector images load at `0x8013B000`
or `0x8017B000`; all28 full hashes are distinct.

Actual nonnegative commands come from
`(record * 276 + 275) * 2048 + 0x110 + ((stage - 7) // 2) * 4`.
Their remainder modulo1000 selects a64-byte view at `load + 0x29C8`.
Every selected view fits inside the actual image suffix. The resident
controller calls the secondary entry at `load + 4`, with the original
context and initialization command or update argument-1.

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `0x4..0xCFC` | 3,320 | entry C | yes |
| `0xCFC..0x17EC` | 2,800 | ribbons C | yes |
| `0x17EC..0x1D74` | 1,416 | sheet-set C | yes |
| `0x1D74..0x20B4` | 832 | strand C | no |
| `0x20B4..0x28CC` | 2,072 | generated assembly | no |

All five complete control-flow spans have one terminal return and no
unresolved indirect transfer. Strand and the final helper are real local
code, but no direct-entry execution path is established. The four-byte
header and10,036-byte suffix each have real sized storage owners.
The suffix stays unclassified; representing its bytes as data does not
prove the absence of additional code.

## Context, records and packet lifetimes

Entry has a208-byte frame, saves original `a0` in the caller argument-home
slot at `sp + 208`, and retains the context in`s6`. The negative-command
branch at`+0x70` reaches`+0x848` without replacing it. Calls at`+0xB54`
and`+0xB94` reload that original argument; neither delay slot replaces it.
The observed entry minimum is`0x2ED8`, not an allocation size.

Eight744-byte ribbon records fill`0..0x1740`; sixteen156-byte sheets
fill`0x1740..0x2100`. Six124-byte strands with thirteen points fill
`0x2100..0x23E8`, without establishing their execution path.
Actual descriptors have mode1 and ribbon counts4,5,6 or8. Depending on
kind0/2/1, sheet `(count, first)` is `(n+1,1)`, `(n+2,2)` or`(2*n,n)`.
Thus every following-sheet count equals`n`, fits eight preceding ribbon
records, and keeps the complete sheet count at most16.

Ribbon fields are independently target-compiled: seventeen-point grids
at`0/0x110`, projected words at`0x88/0x198`, angles at`0xCC`,
widths at`0x1DC`, RGB at`0x220`, signed count at`0x240`,
state at`0x248`, length at`0x24C`, depth at`0x260`, and signed
screen offsets at`0x2A4/0x2C6`. Opaque intervals stay uninterpreted.
Its856-byte frame holds the544-byte flags array at`208..752`,
projection outputs at`752..760`, original context at`760..764`,
ordering-table pointer at`764..768`, and saved registers at`816..856`.
The original-context spill is written once; record cursors`s6/s7`
advance744 bytes per outer iteration.

Two40-byte FT4 packets occupy`0x2BB4..0x2C04`. Frame parity determines
whether segment-parity alternation happens before or after drawing.
Both orderings remain within those two packets. Direct RGB and coordinate
writes end at byte35. Signed nonnegative depth and per-point flags gate
sorting; its depth is narrowed to sixteen bits. The indexed flags fit the
actual descriptor count and seventeen points. Terminal projection, the
unused bend expression and unused-result second angle call remain intact.

The sheet helper has a272-byte frame and retains original context in`s4`.
Crucially, `sp + 216` is an advancing preceding-ribbon cursor, not an
immutable context spill: `+0x1D1C/+0x1D20` advances and stores it by744
only for sheets beyond`first`. Its156-byte sheet cursor advances separately.
The selected GT4 packet is`0x2B4C..0x2B80`; four projected coordinate
words occupy packet offsets8/20/32/44, and RGB writes end at42.
Actual growth intervals`+0x2C..+0x30` and fade intervals`+0x38..+0x3C`
have positive denominators. Unsigned division/traps, phase gates and clamps
are preserved.

The uncalled strand helper has a264-byte frame, original context in`s4`,
and a stable line pointer in`s5`. Its16-byte `GsLINE` at`0x2C04`
has attribute0, coordinates4/6/8/10 and RGB12/13/14, not GPU-polygon
field order. Actual projection arguments and byte stores independently
confirm that layout. Depth is signed nonnegative, below`0x800`, then
narrowed to sixteen bits for sorting.

## Initial helper evidence and retained scope

The [attempt ledger](spanish-model-variant460-attempts.csv) records six
canonical terminal experiments. All84 spans match without changed words.
Complete links prove84 C owners/141,344 bytes,56 remaining assembly
owners/150,976 bytes, and56 raw header/suffix owners/281,120 bytes.
The28 complete images contain573,440 bytes.

Fresh target compilation proves103 layout constants in412 bytes.
Per-image checks cover199 literal anchors, both relocated entry calls,
17 complete register-write lifetimes, the two immutable original-context
spills and the separately advancing sheet cursor. Initial scratch guards
caught an incorrect GsLINE field-order expectation and an incorrect
immutable-sheet-spill assumption; those probe failures were retained and
resolved against actual instructions without changing C or compiler input.

All34 binding addresses independently occur in accepted Spanish metadata.
Fresh Spanish resident output verifies their actual defining objects,
sized functions, linked sections and bytes, plus three matching
initializer/controller/loader owners and both actual context-pointer
storage owners. The observed context views do not overlap the selected
model, primary or secondary loads; this is not whole-game isolation proof.

Independent recovery began at accepted`38935022` and advanced only to
accepted MODEL337 merge`4a43e9f2`, with every relevant source and binding
fingerprint unchanged. All154 earlier Spanish registrations are preserved.
Configured scope becomes182 images and984/1,222 C instances, with
1,151,052 C instruction bytes. This adds140 inventoried functions, not
just84 successes. Unknown code, unclassified suffixes and the expanded
seven-release runtime campaign remain open; general progress reports are
separate.

Production validation reproduces all 182 configured Spanish images and
the fresh Spanish and clean North American residents. Final selected-input
checks reconfirm all 28 images, 103 target constants and 34 actual resident
callees, with the caller and context-pointer evidence archived before the
North American build replaces resident outputs. All 49 focused and 1,322
full-suite tests pass without skips, alongside types, attempt-ledger,
declaration-visibility, notes, note-link and metadata gates. Maintainer
acceptance remains separate from these local results.

## Independently recovered entries

The unchanged accepted local entry matches all28 Spanish instances, adding
92,960 C instruction bytes without changing a source, declaration or profile.
The two slot objects are newly compiled, as are the three retained helpers.
All28 complete20,480-byte images reproduce573,440 bytes:112 genuine C
owners/234,304 bytes,28 generated-assembly owners/58,016 bytes, and56
header/suffix storage owners/281,120 bytes. All518 fallback instruction
annotations per image are checked against the actual freshly assembled
instructions; executable retail bytes are never substituted through `incbin`.
The84 earlier C instances remain selected. Secondary assembly and every
10,036-byte suffix retain their prior classifications.

Fresh target compilation proves113 constants in452 bytes of read-only
storage. Each image has180 literal and five relocated anchors, ten complete
register-write sets, and55 actual entry calls. A delay-slot-aware control-flow
walk proves `s6` retains the capture at`+0x10` at both helper calls. Each
argument reload at`+0xB50/+0xB90` comes from the original pointer stored
at`+0xC`; outgoing calls invalidate the caller-saved argument definition
before subsequent uses are considered. The208-byte frame has **two**
legitimate incoming argument-home words: original `a0` at208 and command
`a1` at212. Neither home is overwritten. Initialization jumps from`+0x840`
to the common tint tail, not into helper dispatch.

Initialization resolves all eight part bytes, including repeated and zero
indices. Eight744-byte records, sixteen156-byte sheets, two888-byte
streamers, eight32-byte matrices and two arrays of eight16-byte vectors
have separate bounds. The entry view leaves`0x2100..0x23E8` opaque.
Paired GT4 and FT4 packet field writes fit52 and40 bytes respectively;
the extra GT4 at`0x2B80` is SDK-initialized through its stable`sp+0x90`
spill. This does not claim direct initialization of its UV fields.
Projection outputs at`sp+0x70..0x80` do not overlap the subsequent pointer
spills. Registers reused as update angles are not treated as lifelong
packet pointers.

Actual archive commands select all thirteen indices1..13 in the64-byte
table at image`0x29C8`; models44 and558 both select index1. Every observed
descriptor has mode1, count4/5/6/8 and kind0/1/2. The non-first targets
use separate800/1300-unit angle accumulators and `(trig << 7) >> 12`
offsets; the first target and non-mode-one code path copy the common target.
Eight matrix calls and direction calculations follow the same strides.

Unsigned strict `phase3 < frame` and `phase4 < frame` tests advance phases
two and three. Ribbons dispatch when `ribbon_start <= frame`; sheets also
require `sheet_start <= frame` and phase below six. Only those two helpers
have entry-call paths. The distinct frame-step calls,128-unit tint clamp,
phase2..4 return4, phase5 return1/advance6, fade64 clamp/advance7, and
phase7 return2 are preserved. An already-at-least64 phase-six fade does
not take the below-threshold update branch.

Fresh Spanish resident evidence independently verifies34 actual callees,
three matching callers and two context-pointer storage owners. Eleven
overlay SDK aliases are renamed using accepted Spanish bindings at unchanged
addresses, without resident/global renaming. Source, declaration, profile
and binding fingerprints remain unchanged across the clean fast-forward
from research base`3745bf50b` to accepted`a3615f75d`.

At that fixed accepted cutoff, the entry registrations preserve all188
Spanish images and the1,272-function inventory, yielding1,068 matching C
instances/1,343,100 bytes. These are configured totals, not exhaustive
runtime completion, allocation-capacity proof or maintainer acceptance.

Production gates reproduce all188 configured Spanish images, the fresh
Spanish resident and the clean North American executable. Selected-input
ownership and113 layout constants pass again;16 resident build artifacts
are independently archived before the North American build replaces them.
All54 focused and1,393 full-suite tests pass without skips, alongside the
six existing types, attempts, declaration, notes, links and metadata gates.
