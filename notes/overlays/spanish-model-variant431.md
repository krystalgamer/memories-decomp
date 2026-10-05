# Spanish MODEL variant 431

Two independently recovered Spanish MODEL401 images contain headers 431
and 581. Fourteen unchanged accepted local C instances reproduce 24,200
instruction bytes. Both complete 20,480-byte images match their Spanish
archive slices, including the five-band renderer and unclassified
suffixes. French configuration was a structural lead, not Spanish loader
or compiler ownership evidence.

## Loader and command ownership

Model401 is compact record351: the loader subtracts the first 50-model gap.
The matched Spanish `Model_LoadMonsterMerge` at `0x8005967C` requests
276 sectors with the actual `func_80056D7C` transfer callback at
`0x80059EF4`. Its European wrapper includes the accepted common transfer
body without changing stages7/8. The first seven phase sizes are
96,48,2,1,16,1,16 sectors, totaling180. Stage7 then occupies ten sectors
and stage8 another ten.

| Model | Record | Stage | Slot | Sector | Load address | Header |
| --- | --- | --- | --- | --- | --- | --- |
| 401 | 351 | 7 | 0 | 97056 | `0x8013B000` | 431 |
| 401 | 351 | 8 | 1 | 97066 | `0x8017B000` | 581 |

Stage7 loads only for alternate0/slot0; stage8 only for alternate0/slot1.
The other path skips the same ten sectors. Stages9/10 belong to the
alternate images and are not counted as MODEL431.

The final record sector,275, stages the metadata copied into the slot by
phase16. Its word at `+0x110` becomes slot `+0xD08`, the first command.
The actual word is597000. The matched secondary dispatcher
`func_800559D4` at `0x80058B4C` selects the current alternate's command,
passes its remainder modulo1000 for initialization, and passes-1 for
updates. Thus this entry initializes with command0, not597000.

Actual resident storage at `0x80010014/18` contains the two load addresses.
Storage at `0x80010024/28` contains contexts `0x80136000/0x80176000`.
The matching setup owner `func_8004CB0C` at `0x8004FC2C` and dispatcher
preserve those secondary contexts. Four loader/caller compiler owners and
all four pointer-storage owners were checked against the fresh Spanish
resident ELF, its selected objects, linker map/script and retail bytes.

The descriptor is `0x3044 + command * 104` within each loaded image.
Descriptor zero has parts22,17,9,2,14, first threshold0, and timing words
10,40,70,100,130,160,190,220,250,280,310 followed by four zeros.
Its SHA-256 is
`7bb7ea728ab8ac81745e398ab1d777efd65011aa6f78fb25cfe878b9518aa2eb`.
The trailing zeros are not additional demonstrated animation stages.

## Complete image inventory

| Offset | Bytes per image | Owner |
| --- | --- | --- |
| `0x0000` | 4 | Raw loader header |
| `0x0004` | 3864 | Entry C |
| `0x0F1C` | 1052 | Webs C |
| `0x1338` | 1168 | Fan C |
| `0x17C8` | 3196 | Five-band C |
| `0x2444` | 1152 | Sheets C |
| `0x28C4` | 776 | Spokes C |
| `0x2BCC` | 892 | Rings C |
| `0x2F48` | 8376 | Unclassified suffix |

The entry directly reaches fan, sheets, companion and webs, in that update
order. Spokes and rings remain executable functions without a demonstrated
direct-entry call path. They are neither discarded nor mislabeled data.
Strict CFG census covers all fourteen function instances, with206 literal
anchors per image. The final band integration replaces each 799-instruction
companion fallback with the unchanged accepted French C body and its slot
wrapper. All selected compiler definitions, linked extents, external call
relocations and raw owners are checked against Spanish retail bytes.

Fourteen compiler owners/24,200 bytes and
four storage owners/16,760 bytes cover all40,960 bytes. Sources remain
under `src/overlays/french_model_variant/variant431_*`; the existing
slot-one wrappers relocate entry calls and resource bases. The band calls the
canonical `RotTransPers3` resident at `0x80087898`; the historical opaque alias
is removed from both image symbol files and the linker bindings. Sheets alone
uses `gcc_2_8_1_g0_split_no_cse_follow_jumps`; all other roles use
`gcc_2_8_1_g0_split`. Both are unchanged named GCC2.8.1/MASPSX2.81
profiles. No source, header or compiler-profile changes are required.

The twelve terminal records in the adjacent attempt ledger describe fresh
Spanish compilations, not inherited French successes. A sheets preflight
rejected an assumed default profile before compilation; the actual accepted
local registration supplied the correct named profile. Its private
diagnostic is retained, without inventing a compiler experiment.

## Layout, contexts and lifetimes

Fresh target compilation proves140 constants/560 read-only bytes, including
primitive widths, the104-byte descriptor, all packet views and the minimum
`0x18F4` entry state. Five288-byte companion records occupy
`0x4E0..0xA80`; six152-byte sheets end at`0xE10`; six144-byte rings end
at`0x1170`; four144-byte spokes end at`0x13B0`; five116-byte fans end
at`0x15F4`. Fan completion is at record`+112`, not the shorter related
fan layout.

Five matrices occupy`0x1780..0x1820`, five directions
`0x1820..0x1870`, and the two five-halfword screen-delta arrays start
at`0x1870/0x187A`. Configuration is at`0x18B0`, the five guest-width
part pointers at`0x18B8`, timing index at`0x18D8`, phase at`0x18E0`,
tint at`0x18EC`, slot at`0x18F0`, and command at`0x18F2`.
The accessed extent is not proof of whole-context allocation or lifetime
isolation across every game mode.

The entry has a240-byte frame. Original context is stored once in its
incoming home at`sp+240`; command is at`sp+244`. Register`s6` retains
the original context between its prologue definition and epilogue restore.
A delay-slot-aware CFG walk proves all four local calls load that original
context home. All69 entry call sites resolve to25 distinct resident owners
or the four local helpers. Fresh resident proof covers all36 family
bindings, three callers and both context-pointer owners.

Complete write sets for`s0..s7`, `sp` and`fp` are fixed independently
for all six C functions:60 sets per image, including initialization cursor
advancement and register reuse. Entry pointer homes at128..160 and both
projection calls are checked separately. First projected x/y and second
projected x are unsigned halfword loads; second projected y is signed.
Two unused matrix locals retain their observed64-byte stack interval,
without claiming recovered runtime matrix contents.

Helper frames are296 bytes for webs,264 for fan,272 for sheets,
272 for spokes and264 for rings. Projection outputs use stack208/212.
Stable packet bases are context`+0x1730` for webs/spokes/rings,
`+0x1610` for fan and`+0x1668` for sheets. Direct packet-write
footprints and complete packet-register lifetimes are separately checked;
those footprints do not replace the projection callees' output writes.

## Sixth sheet and timing behavior

The sheet loop processes six records but there are only five companions.
Its companion cursor starts at`0x4E0` and advances288 bytes each time.
On iteration5, the pre-branch32-bit read at cursor`+0xF4` lands at
`0xB74`, inside sheet storage: the address of `sheets[1].v2[3].vz`.
The load is still32-bit, not merely that16-bit component. Its resulting
amount is ignored on the sixth-sheet path. Do not create a sixth companion
or remove this retail read.

The first five sheets interpolate their matrices/directions using the
companion amount. The sixth uses target halfwords at`0x1764/66/68`.
Projection depth is scaled by8/10; negative depth or flag rejects drawing.
The first five also require companion progress at`+0x104` below1024;
the sixth bypasses that gate. During phase2 its size grows by
`step<<9`, capped4096; phase3 subtracts`step<<5`, and reaching zero
sets phase5.

Fan, sheets and companion dispatch each retain their separate unsigned
descriptor/frame comparison, even when a selected threshold is zero.
Webs requires phase at least2. The companion bounds the descriptor timing
index at10 and resets all five fan completion words when advancing.
Rendering precedes frame-step accounting and the final tint/state update.

## Scope and acceptance

The independent research base was`0129fc7d7a0bce70cdf9ed4452ae62ac6153693a`.
Before registration, a guarded fast-forward to accepted
`feff912818880bbfdd04343babb2e3cc6f2ca2b7` preserved49 immutable source,
header, binding and profile dependencies, plus all188 earlier image records.
Private runtime checks passed12 tests; the reconciled private/French suite
passed27 without skips before metadata promotion.

Configured Spanish inventory becomes190 images,1,090/1,286 C instances
and1,392,028 C instruction bytes. These are configured inventory counts,
not an exhaustive Spanish runtime census or seven-release completion.
The fixed-cutoff README report is intentionally unchanged.

Final production acceptance reproduces all190 Spanish images and a fresh
Spanish resident. Selected compiler, generated-assembly and storage owners
again cover both complete images. All140 constants were freshly compiled,
the36 callee/three caller/two context owners were rechecked, and four
loader/caller plus four load/context-pointer owners were archived from the
final resident. The clean North American executable, all six repository
policy gates,84 focused checks and1,427 full regressions pass without skips.
External exact-head CI and maintainer acceptance remain required.
