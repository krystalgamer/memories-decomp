# Spanish MODEL headers 422 and 572

Four independently extracted Spanish secondary images select the unchanged
local MODEL422 entry and eight helper sources. Each complete 20,480-byte image
matches its own retail slice. The suffix remains unclassified raw storage,
not additional recovered C.

## Actual records and ownership

| Model | Compact record | Stage | Slot | Sector | Header |
| --- | --- | --- | --- | --- | --- |
| 1 | 1 | 7 | 0 | 456 | 422 |
| 1 | 1 | 8 | 1 | 466 | 572 |
| 550 | 500 | 7 | 0 | 138180 | 422 |
| 550 | 500 | 8 | 1 | 138190 | 572 |

The [instance ledger](spanish-model-variant422-instances.csv) records all four
distinct image hashes and actual command words. The
[attempt ledger](spanish-model-variant422-attempts.csv) identifies each of the
32 original helper matches and four new entry matches by module, not just by
load slot. Two unchanged entry source/profile experiments, one per slot,
reproduce all four 4,388-byte entry bodies.

| Image-relative span | Owner | Bytes per image |
| --- | --- | --- |
| `0..4` | Sized raw header | 4 |
| `4..1128` | Entry C | 4,388 |
| `1128..1620` | Halo C | 1,272 |
| `1620..1CFC` | Veils C | 1,756 |
| `1CFC..2408` | Bands C | 1,804 |
| `2408..28F0` | Sheets C | 1,256 |
| `28F0..2E44` | Webs C | 1,364 |
| `2E44..3154` | Spokes C | 784 |
| `3154..34D0` | Rings C | 892 |
| `34D0..3834` | Quad C | 868 |
| `3834..5000` | Sized unclassified raw suffix | 6,092 |

All nine roles use the authoritative named
`gcc_2_8_1_g0_split` profile: GCC 2.8.1 and MASPSX 2.81. No C, header,
compiler profile, French production metadata, or README progress change is
needed. Accepted local source structure is not substituted for Spanish byte,
relocation, binding, or ownership evidence.

The four images contain 57,536 C instruction bytes and 24,384 header/suffix
bytes: all 81,920 bytes have actual selected owners. The four new entries add
17,552 C instruction bytes while preserving all 32 previously matched helpers
and their 39,984 bytes. Linker scripts, input objects, defining function
symbols and executable ELF sections establish all 36 selected compiler-C
owners; the eight raw owners have their real nonzero extents. No assembly
fallback remains in the nine-function inventory.

Only the entry, halo and veils are directly entry-reachable. The other six
helpers remain retained code, not evidence of additional active dispatch paths.

## Loader, command and resident evidence

All 36 bindings were independently grounded in accepted Spanish metadata and
then checked against the selected objects of a freshly matching Spanish
resident. Four loader/dispatcher C owners and eight load/context-pointer storage
owners were also checked, rather than trusting absolute aliases alone.

Seven legacy address-based SDK aliases are replaced by independently confirmed
Spanish names: `SetPolyG3`, `SetPolyFT4`, `SetPolyG4`, `SquareRoot0`,
`RotTransPers`, `Square0` and `GsGetLwUnit`. Their addresses are unchanged,
all 36 required resident bindings remain, and complete-image compilation
checks the existing helpers as well as the new entry.

The selected Spanish `Model_LoadMonsterMerge` body at `0x8005967C`, transfer
callback at `0x80059EF4`, slot setup at `0x8004FC2C`, and secondary dispatcher at
`0x80058B4C` establish the actual loading and initialization path. Stage-sector
counts are `(96,48,2,1,16,1,16,10,10,10,10,2,2,1,50,1)`, totaling 276.
Stages 7 and 8 select alternate zero and begin at record-relative sectors
180 and 190. Alternate-one stages 9 and 10 are distinct and are not added here.

Load-pointer storage at `0x80010014/18` contains `0x8013B000/0x8017B000`;
context-pointer storage at `0x80010024/28` contains
`0x80136000/0x80176000`. Actual archive sectors agree with both compact records.
The final record sector's `+0x110` word reaches slot `+0xD08`, the first
`commands` element. Both models contain command **588000**. Initialization
receives `command % 1000`, therefore zero; updates receive minus one.

The actual 48-byte descriptor starts at image `+0x386C`, within retained raw
storage. Words `+0x1C/+0x20/+0x24/+0x28` are **60/120/360/380**.
No descriptor is synthesized from another language release.

## Layout and lifetime evidence

A fresh target compilation checks 257 constants occupying 1,028 read-only bytes.
It establishes primitive/pointer widths, SDK vector/matrix/coordinate/OT and
packet layouts, all helper record fields, and these contiguous extents:

| Context interval | Contents |
| --- | --- |
| `0..820` | Five 416-byte veils |
| `820..DD4` | One 1,460-byte halo |
| `DD4..12B4` | Three 416-byte narrow webs |
| `12B4..147C` | One 456-byte band |
| `147C..15AC` | Two 152-byte sheets |
| `15AC..190C` | Six 144-byte rings |
| `190C..1B4C` | Four 144-byte spokes |
| `1B4C..1BDC` | One 144-byte quad |
| `1BDC..1C10` | Halo GT4 packet |
| `1C84..1CB8` | Sheets GT4 packet |
| `1CB8..1CEC` | Veils GT4 packet |

The entry's actual accesses require a minimum context of `0x1E38` bytes.
These checked extents and separation from loaded model regions do **not**
prove a complete allocation boundary or whole-game lifetime isolation.

The unchanged entry header also fixes the matrix at `1D60`, target at `1D80`,
direction at `1D88`, descriptor pointer at `1DC8` and three four-byte part
pointers at `1DD0`. The 64-byte local stack view reserves 60 unaccessed bytes
before two CLUT halfwords; actual instructions store and reload these at
`sp+4C/4E`. The halo CLUT stays in a register. Projected X halfwords are
zero-extended while projected Y remains signed; these expressions are not
simplified or widened.

Raw regressions check 90 complete register-write sets per image: `s0..s7`,
stack pointer and frame pointer across all nine functions. Frame sizes are
256/328/312/256/272/288/272/264/248 bytes in image order. Entry command storage
at `sp+260` is a legitimate incoming argument home above its 256-byte frame.

The entry initially copies the original context to `s3` and then `s6`, but
reuses `s3` during initialization. A delay-slot-aware control-flow traversal
proves that the original `+0xC` definition reaches both update calls:
`+0xFA4` to veils and `+0xFC0` to halo. Both argument moves are in delay slots.
There are 75 entry call sites and 23 distinct resident callees.

Halo uses stable packet register `s2`. Its original root in `s8` is repurposed
at `+0x1510` as a bottom-row pointer only after the context-dependent setup;
it is not described as immutable throughout the helper. Six five-byte color
arrays start at stack 208/216/224/232/240/248, separately from projection
outputs 256/260. Indexed stores and the five-iteration bound are checked.
Direct packet stores cover twelve RGB bytes and eight UV bytes, not `tpage`.
The five rings each use 17 points and 16 strips, vertical UV 127/96 and
horizontal steps of 16. Heights advance by `step << 5`; unsigned clock
comparison against 380 controls wrap versus clamp at 1024. Halo does not write
the phase field. Both unused-result angle calls remain. Sorting requires
nonnegative signed depth and flag and passes the depth's low 16 bits.

Veils preserve the original root in `s8`, while `s6` advances by 416 bytes.
Projection outputs 248/252 are separate from the dark RGB at 232..234 and
light RGB at 240..242; the unused 24-byte stack interval 208..232 remains.
The first projection uses A/C for texture selection, followed by A/B for
final geometry. The signed screen split is `x0 < 160`; selected pages depend
on the active buffer. Direct packet stores include twelve RGB bytes, eight
UV bytes and the `tpage` halfword. Scale advances by `step * 40`, wrapping
below clock 360 or clamping to 2048 with count one. At a crossing and clock
at least 120, phase zero can become two. On the fifth record, a product of
counts equal to one and phase two changes phase to five.

The retained helpers' direct packet footprints and all frame/register sets
are checked too. Bands directly write twelve RGB bytes and eight XY
halfwords; sheets write twelve RGB bytes; webs/spokes/rings write a line
attribute word and six color bytes; quad writes twelve RGB bytes.
Direct-store footprints alone are not claims about all SDK output writes.
The shared family checks also cover 387 actual literal anchors per image,
descriptor selection, exact boundaries, bindings, wrapper ownership and
entry reachability.

## Initial helper checkpoint (historical)

Integration preserves all 190 previously configured Spanish images and adds
four images, 36 inventoried functions and 32 C instances. Resulting configured
totals are **194 images, 1,122/1,322 C instances and 1,432,012 C instruction
bytes**. These are branch totals until external acceptance.

All **194 configured Spanish images** and a fresh Spanish resident pass.
Final production evidence rechecks all C/assembly/raw owners, 179 freshly
compiled constants, 36 callee owners, three callers, both context owners,
four loader/dispatcher owners and four load/context-pointer storage owners.
The 14 durable Spanish family regressions pass; selected Spanish objects,
ELF, map and linker script were archived before the clean North American run.

The clean North American executable matches its retail hash. All six
repository policies pass: basic types, external attempts, declaration
visibility, notes, note links and metadata. **62 focused / 1,441 full regressions**
pass with zero skips. Expanded seven-release runtime recovery remains open,
including the then-generated entries and unclassified suffixes.

## Entry checkpoint

This follow-up preserves all 250 accepted Spanish module registrations and
all existing helper registrations. Only the four existing entry spans change
from generated assembly to verified C. Configured totals become **250 images,
1,310/1,574 functions in C and 1,742,900 C instruction bytes**; the image and
function denominators do not change. These are branch totals until external
acceptance.

Shared regressions now select all nine C functions, check the complete entry
wrapper and require the independently verified canonical SDK aliases for
both French and Spanish images. Spanish runtime checks retain all register,
CFG, packet, CLUT and descriptor evidence, verify actual stage-sector offsets,
and explicitly test skipping before opening an absent Spanish archive.
French registrations, source bodies, headers, compiler profiles and the
README progress snapshot remain unchanged.

The Spanish resident and all 250 configured images match, as does the clean
North American executable after reconciliation with accepted French-options
changes. Seven repository policy gates, 67 focused regressions and 1,534 full
regressions pass. The full suite reports two inherited optional French-options
ownership skips; both are independently covered with the existing local ELF
reader, a freshly matching French resident and the complete French options
image. No optional dependency is installed to conceal those skips. The 17
Spanish family tests also pass with the archive simulated absent, with 11
intentional skips and no retail-file modifications.
