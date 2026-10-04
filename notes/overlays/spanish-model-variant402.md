# Spanish MODEL headers 402/552

Models 6 and 551 load this family in stages 7/8 from compact records 6 and
501. Four independently verified ten-sector images load at `0x8013B000`
and `0x8017B000`. The [instance ledger](spanish-model-variant402-instances.csv)
records their sectors, headers, hashes and normal commands.

The Spanish manifests reuse the accepted French entry, strip, ribbons, rings and bands sources,
their local headers and slot-one wrappers without regional forks. Each helper and
slot compiles independently under the named GCC 2.8.1 / MASPSX 2.81
`gcc_2_8_1_g0_split` profile; all four complete Spanish images match.

| Offset range | Bytes | Owner | Direct entry-call path |
|---|---:|---|---|
| `0x4..0xA5C` | 2648 | Entry C | Loader entry |
| `0xA5C..0x1048` | 1516 | Strip C | No |
| `0x1048..0x169C` | 1620 | Ribbons C | No |
| `0x169C..0x1B38` | 1180 | Rings C | No |
| `0x1B38..0x2060` | 1320 | Bands C | Yes |

Entry calls only `0x1B38`; the other retained functions were checked from
their own boundaries, not described as entry-call reachable. Every instruction
in each span is covered by direct control flow with one terminal return and
no unresolved indirect jump. This does not prove all possible runtime entries.

Twenty C instances contribute **33,136 instruction bytes**, including four
new entry instances / 10,592 bytes and all sixteen earlier helper instances /
22,544 bytes. No inventoried function remains assembly in these four images.
Each four-byte header and **12,192-byte unclassified suffix** at
`0x2060..0x5000` retains real storage. No unknown bytes are classified away.

## Independent layout and ownership evidence

Ninety-four target-compiled constants and 165 instruction anchors per Spanish
image verify the local and SDK views. The constants occupy a real 376-byte
`.rodata` section, with arrays at offsets zero and 128. The 88-byte strip
starts at context zero. Rings start at context `0x58`, use
two **144-byte records**, and have scale at record offset 128. They are not
the 152-byte rings used by family 337. The two bands at `0x438` use
**280-byte records**, with two rows of seventeen canonical `SVECTOR`s,
scale at 272 and cycle count at 276.

Eight distinct 88-byte ribbon records fill `0x178..0x438`. Their local view
keeps endpoint vectors, projected coordinates, angles, widths, depths and
texture offsets separate from the strip's different 88-byte layout. The
28-byte G3 packet at `0x668` and 52-byte GT4 packet at `0x6A8` do not overlap.
The retained helpers are independently compiled and checked even though
normal entry calls only bands.

Normal metadata requests 568000 pass initialization argument zero. The
20-byte configuration view at image `0x215C` has part count one and unsigned
duration 92. Each view remains inside its own suffix owner; this is not a
generic descriptor-array capacity claim.

The rings use nested positive-depth and depth-below-2048 checks, rather than
a combined range expression; exact code generation depends on that control
flow. Their projection flag is not tested. The bands preserve their observed
curve, color, scale and cycle expressions. Existing bodies and declarations
are shared rather than forked.

All 35 resident bindings have real selected input objects, sized final
function symbols and retail-identical bytes in a clean Spanish resident.
The local linker and four symbol files expose the existing SDK aliases
`RotTransPers = 0x80087868` (44 bytes) and `RotTransPers3 = 0x80087898`
(84 bytes), as required by the strip/ribbon C relocations. Their Spanish
resident inventory remains `sdk_asm`; no additional game C is claimed.
Three matching initializer/controller/loader owners and the actual context
pointer words at `0x80010024/28` are also verified. These point to
`0x80136000/0x80176000`. Stable context-register captures and restores prove
the direct accessed extents independently: entry `0x8C4`, strip/rings `0x8B0`,
and ribbons/bands `0x8C2`. The larger entry minimum does not overlap the
selected model, primary or secondary loads.
Neither these partial views nor load separation establishes allocated capacity
or whole-game lifetime isolation.

The [terminal attempt ledger](spanish-model-variant402-attempts.csv) records
the source hash for each independently compiled helper/slot. Earlier source
recovery remains in the [French ledger](french-model-variant402-attempts.csv).
Full-image hashes, actual compiler/assembly/data ownership and these partial
inventories remain distinct from exhaustive Spanish or seven-release coverage.
The entry extension preserves all 188 accepted configured images and raises
their selected C total to 1,072 of 1,272 function instances /
1,353,692 instruction bytes. Accepted MODEL460 entries and all prior helpers
remain selected. The independent branch fast-forwards only accepted master;
no pending report or source branch is stacked.

## Shared retained-layout support

The ribbon body now also serves French MODEL412 through 20 measured
compile-time header selectors. Their default branch preserves every
Spanish instruction byte. Fresh clean Spanish resident/all-273-overlay
gates and all four affected complete-image links verify 28 actual sized
image owners and 35 resident input owners.

The original ten ledger rows remain unchanged historical evidence.
Two appended ribbon terminals bind the current shared source and slot-one
wrapper; regression coverage distinguishes those current fingerprints
from the original slot-zero body hash rather than rewriting old records.

## Independently recovered entry

Two freshly compiled slot objects match all four 2,648-byte entry spans.
All four retained helpers were recompiled independently. Twenty genuine
compiler owners cover 33,136 bytes; eight sized, non-code header/suffix owners
cover the other 48,784 bytes of the four complete 20,480-byte images. A fully
C inventoried function list does not prove the unclassified suffixes contain
no additional code.

Ninety-seven freshly target-compiled constants occupy 388 read-only bytes.
They verify the entry's local `0x8C4` view, canonical SDK layouts, a 20-byte
descriptor, 88-byte record, 144-byte rings, 280-byte bands and the 16-byte
projection spill. The entry treats `0x178..0x438` as opaque rather than
importing the ribbons helper's interpretation.

Each actual Spanish image supplies 157 literal instruction anchors, four
relocated anchors, ten complete register-write sets and 57 owned calls.
The 200-byte frame saves the incoming command at caller-owned `sp+204..208`.
Projection output occupies `sp+112..128`; direct stack stores do not overlap
it. The band cursor at `sp+132` advances during initialization and is not
misidentified as a stable home. The record and packet-pointer homes are stable.

Original `a0` is captured in `s2` at entry offset `0xC`; `s8` captures the
typed state root at `0x14`. Initialization reuses `s2`, but its jump at
`0x6A8` bypasses update. Delay-slot-aware control-flow analysis proves the
original capture alone reaches band dispatch at `0x8E0`, whose delay slot
passes `s2`. No entry call reaches strip, ribbons or rings.

Five directly initialized packet footprints cover paired GT4s, paired FT4s
and the band GT4. The last packet uses both `a0` and `t9` aliases loaded
from the stable `sp+144` home: its texture-page store occurs through `a0`,
and UV/CLUT stores through `t9`, including the `0x3F8` delay slot.
The extra GT4 uses a separate stable SDK argument; no direct UV writes are
claimed for it.

All four actual command-568000 descriptors select parts `8/0/0`, count one
and start 92. Initialization resolves all three part bytes, two sixteen-point
rings and two seventeen-point bands. Inner and outer coordinates retain
unsigned logical shifts; band scales are zero and -2048, with the scale
decrement occurring after the current store.

Update loads unsigned projected X before resetting the part index, then
computes signed Y. A zero-count guard precedes the bottom-tested **signed**
part-index comparison. The observed count is one, not a generic capacity
claim. Band dispatch starts when unsigned frame is at least 92. Two distinct
frame-step calls remain distinct. Phase below three reduces tint; phase one
returns four and becomes two, phase two returns one, and phase three advances
fade only from below 64 before clamping and becoming eight. Phase eight
returns two. This differs from the phase tails of families 337, 338 and 460.

Eleven SDK aliases are independently grounded in accepted Spanish bindings
and fresh resident ownership, then renamed at unchanged addresses in the
local linker and four symbol files. No C source, header or compiler profile
changes. Partial context extents remain distinct from allocation capacity
and whole-game isolation.

Final entry integration passes all 188 configured Spanish image matches,
a fresh Spanish resident, production input/final ownership and archived
resident/caller/context ownership, followed by the clean North American
exact-match gate. All six repository policies, 53 focused tests and 1,397
full regressions pass with zero skips. Four durable entry tests preserve
the original-pointer dataflow, complete register lifetimes, stack homes,
packet aliases, actual descriptors, part-loop scheduling and phase tail.
These gates establish the selected images and views, not completion of
Spanish or all seven releases' expanded runtime inventory.
