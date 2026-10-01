# French MODEL headers 435 and 585

These 26 secondary-handler archive instances reuse the accepted
`src/overlays/model_variant/variant418_{spiral,sheet,webs,ribbons,bands,spokes,rings,quad}.c` bodies.
Sixteen wrappers rename the functions to their measured French addresses.
The ribbon and spiral wrappers define `VERSION_FRENCH`, selecting their
measured loop-indexed forms; other shared expressions remain unchanged.
All use `gcc_2_8_1_g0_split`, GCC 2.8.1 and MASPSX 2.81; the original US
registration's GCC 2.7.2 profile is not used or modified. Header numbers are
not cross-region identities: the matching French family is 435/585, not
418/568.

## Loader-backed images

The matched `Model_LoadMonsterMerge` and texture-transfer phase callback read
276 sectors per compact model record. Stages 7/8 occupy record sectors
180/190; stages 9/10 occupy 200/210. Each image occupies ten 2,048-byte sectors.
Slots load at `0x8013B000` and `0x8017B000`. The accepted resident
`model_control.c` and `model_intro_controller.c` dispatch secondary handlers
through `D_80010014/18 + 4`, using the stored context and initial command or
the update argument `-1`.

| Stages | Models |
|---|---|
| 7/8 | 34, 71, 124, 182, 279, 361, 491, 580, 640 |
| 9/10 | 166, 275, 469, 590 |

The [instance ledger](french-model-variant435-instances.csv) records compact
indices, stages, sectors, actual positive metadata command words and complete
image hashes. All 26 slices are independently verified against the French
archive; they contain 23 distinct complete images. Shared suffixes or code
prefixes alone were not used to declare whole-image identity.

## Function and storage ownership

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `0x4..0x1084` | 4224 | generated assembly | yes |
| `0x1084..0x1AD4` | 2640 | spiral C | yes |
| `0x1AD4..0x1E7C` | 936 | sheet C | yes |
| `0x1E7C..0x2258` | 988 | webs C | yes |
| `0x2258..0x28A4` | 1612 | ribbons C | no |
| `0x28A4..0x2FB0` | 1804 | bands C | no |
| `0x2FB0..0x32B8` | 776 | spokes C | no |
| `0x32B8..0x3634` | 892 | rings C | no |
| `0x3634..0x3998` | 868 | quad C | no |

All nine spans have complete direct control flow, one terminal return, and no
unresolved indirect transfer. The last five are not reachable from the
entry's direct call graph. No direct J/JAL, absolute pointer word or matching
low-half address immediate for the original spokes/rings/quad helpers was found anywhere in
these images. This does not exclude computed or resident-mediated dispatch.

There is positive module-local ownership evidence beyond recognizing bytes.
The active entry captures its context through `a0 -> s2 -> s6`; at offsets
`0x28`, `0x30` and `0x38` it forms the exact ring, spoke and quad record bases
`context + 0x1374`, `+ 0x16D4` and `+ 0x1914`. Its initialization loops advance
these records by 144 bytes; the ring loop initializes six records. It also
clears the quad rotation/state word at `context + 0x1B94`. Ten entry anchors
were checked in every distinct image. Combined with the contiguous complete
functions, shared record layout and exact game-specific bodies, this supports
retained module-local game code rather than unrelated residual payload.

**Retained code is not proof of execution.** The five retained helpers count matching C
owners in the configured inventory, not additional demonstrated runtime call
paths. No exhaustive overlay coverage or unreachable-code exclusion is claimed.

Each image retains real generated storage for its four-byte header and its
5,736-byte suffix at `0x3998..0x5000`. The suffix remains unclassified;
representing its preserved bytes with a data segment does not establish that
it is entirely non-code. No linker-only alias substitutes for either owner.
Record layout checks likewise do not establish a context allocation bound.

## Matching evidence and experiments

The [attempt ledger](french-model-variant435-attempts.csv) records the initial six
terminal canonical-wrapper matches, two subsequent sheet matches, two
subsequent band matches and two subsequent webs matches.
No function-body or compiler-flag
variation was needed. Initial calibration compiled the three accepted local
US bodies with the authoritative French profile, scanning 2,362 distinct
secondary payloads. Rebased internal J26 values were used only to locate
candidates; actual subsequent links and raw complete-image comparisons, not
masked comparisons, established byte identity.

Two probe assumptions were corrected before integration: internal `.text`
J26 relocations are legitimate, and entry reachability does not cover all nine
functions. A layout probe also initially expected `move s6,a0`; the measured
two-step context capture above replaced that assertion. These were probe
setup/ownership failures, not invented nonmatching C experiments. Their
original artifacts remain under ignored `tmp/coverage-probes/`.

Fifty-two target-compiled constants verify the local quad/ring/spoke records
and canonical `SVECTOR`, `VECTOR`, `MATRIX`, `GsCOORDINATE2`, `GsGLINE`,
`POLY_G4`, `long` and `s32` layouts. The shared header's original compiler
comment describes the US registration; French profiles are explicit in every
matching manifest.

The initial production gate reproduced all 62 then-configured French images and
the full French resident executable. For these 26 images it checked 78 selected C
owners, 156 assembly owners, 52 raw header/suffix owners and all 37 actual
resident callee owners. Those initial helpers added 65,936 matching instruction
bytes; at that point 317,304 bytes remained assembly and 149,136 suffix bytes
remained unclassified in this family. Existing shared sources, compiler profiles, other-region
registrations and all previously accepted French modules are preserved.

## Entry-reachable one-sheet follow-up

The accepted `variant418_sheet.c` body compiles unchanged to 936 bytes under
the French `gcc_2_8_1_g0_split` profile. Two canonical wrappers rename
`func_8013CAA4` to `func_8013CAD4` and `func_8017CAD4`, respectively.
All 26 complete images match after actual linking with those C objects in
place of the original assembly owner at offset `0x1AD4`. The US function's
different instruction count and compiler profile are not adopted.

This helper is the third function in the existing entry call graph, not
another retained-only helper. Its one `ModelVariantSheet` record begins at
`context + 0x12DC`. The entry stores that base at `sp + 0x84`, then reloads,
advances and stores the same pointer by 152 bytes at offsets
`0x78C/0x794/0x798`. The loop counter starts at zero, increments once, and
repeats only while nonpositive, independently confirming a single initialized
sheet. Ten context/base/pointer/counter instruction anchors were checked in
every registered image. This is initialized-record ownership, not proof of
the entire context's allocation bound.

Seventy-one target-compiled constants extend the earlier layout checks to
include `ModelVariantSheet` and `POLY_GT4`. Fresh production validation
reproduces all 98 configured French images and the complete French resident,
checking this family's 104 C owners, 130 assembly owners, 52 raw header/suffix
owners and 37 fresh-resident callees. The sheet adds 26 C instances and
24,336 instruction bytes without changing any boundary, module hash, archive
selection, existing helper body or resident binding.

After the sheet follow-up the family had 90,272 matching C bytes, 292,968
assembly bytes and 149,136 unclassified suffix bytes. Configured French totals became
98 images, 500/769 C instances and 405,156 C instruction bytes. The three
retained C helpers remain distinct from proven entry paths, and every suffix
remains unclassified. General report snapshots are separate.

## Retained padded-band follow-up

The accepted `variant418_bands.c` body compiles unchanged to 1,804 bytes
under the same French profile. Two wrappers rename `func_8013D86C` to
`func_8013D8A4` and `func_8017D8A4`. Actual links of all 26 complete images
replace only the assembly owner at `0x28A4..0x2FB0`, adding 46,904 C bytes.
The band helper remains outside the entry's direct call graph.

The entry initializes one `ModelVariantBandPadded` at `context + 0x10F0`,
saving that pointer at `sp + 0x80`. It reloads, advances and stores the
pointer by 492 bytes at offsets `0x4B0/0x4B8/0x4C0`. The counter `a1`
starts at zero (`0x3F8`), increments once (`0x4A0`), and repeats only while
nonpositive (`0x4BC`), proving one initialized record. Nine four-byte color
elements begin at record offsets `0x144` and `0x168`. Fourteen instruction
anchors independently verify these details in every registered image.

Fifty target-compiled constants verify the 492-byte padded band, its three
nine-point arrays, three packed screen-coordinate arrays, two color arrays
and nine depths, together with the accessed SDK layouts and primitive widths.
The screen-coordinate elements are target `long` values; reads of their
upper and lower halves are not separate guessed arrays. Neither the record
size nor initialization establishes the entire context allocation bound.

The existing resident bindings at `0x80087898`, `0x800866F8` and
`0x80089928` acquire their independently established names `RotTransPers3`,
`rcos` and `ratan2`. The same named addresses were already verified for
French Exodia/model-54 code. The 37-address set is unchanged, and each
callee has a sized resident ELF function owner with bytes matching retail.
Initial scratch probes stopped on missing named bindings and an incorrect
manually listed cosine address; they now read the authoritative bindings
directly. No source/compiler variant or unmatched source was promoted.

Fresh production validation reproduces all 116 configured French images
and the complete French resident. For this family it verifies 130 selected
C owners, 104 assembly owners, 52 real header/suffix owners, all 37
fresh-resident callees and 50 recompiled layouts. Old band assembly objects
are absent from selected link inputs. All image hashes, archive slices,
nine function boundaries and tails remain unchanged.

After the band follow-up the family had 137,176 matching C bytes, 246,064
assembly bytes and 149,136 unclassified suffix bytes. Configured French totals became
116 images, 580/889 matching C instances and 504,812 C instruction bytes.
The existing regression fixture covers all five helpers, 30 distinct entry
anchors and the three named bindings without changing other family fixtures.
Sheet was then the only entry-reachable C helper in this family. These
inventory figures are not exhaustive runtime coverage.

## Entry-reachable webs follow-up

The accepted `variant418_webs.c` body compiles unchanged to 988 bytes with
the French named profile. Two wrappers rename `func_8013CE50` to
`func_8013CE7C` and `func_8017CE7C`. Canonical preflight reproduces all 26
complete images (23 distinct hashes), adding 26 C instances / 25,688
instruction bytes without adding images or changing any function boundary,
archive slice, symbol address, resident binding or suffix owner.
Final production validation reproduces all 192 configured French images
and the clean French resident. Selected link inputs no longer contain the
webs assembly object. All 156 C owners, 78 remaining assembly owners and
52 raw owners in this family retain their exact section-defined extents.
All 37 family resident callees and the three caller/loader owners were
checked again, and all 25 new layout constants were freshly recompiled.
All 95 French MODEL variant and 47 progress/global-usage regressions pass,
along with metadata, basic-type and G32/PSXLONG policy checks.

Entry captures `a0 -> s2 -> s6`, saves the web pointer at `sp + 0x94`,
and initializes three 608-byte records from the context base through
`+0x720`, where the next record area begins. Each record contains two
six-by-six `SVECTOR` grids at offsets 0 and `0x120`; their row stride is
48 bytes. Color begins at `+0x240` and scale is at `+0x254`.
Entry's nested counts, pointer increments and initialization stores prove
these views independently of the reused header. Entry calls this helper
at module offset `0xF14` with `a0 = s2`; webs is entry-call reachable.

The helper captures the same context and reads the line packet at
`+0x1AE0`, transform translation at `+0x1B08/+0x1B0C/+0x1B10`, step at
`+0x1B64` and state at `+0x1B9C`. Twenty-five newly target-compiled
constants verify the web and SDK views, including packed `GsGLINE`
colors at 12 and 15. Twenty-four instruction anchors and the selected
table address were checked in every image. Existing fixture anchors
remain; overlapping capture checks are not counted twice.

The selected descriptor is a 68-byte record at module
`+0x3A94 + (command % 1000) * 68`, inside the single preserved suffix.
All legal archive slices and commands were checked independently.
Eight helper callees and the three resident initializer/controller/loader
owners match the fresh exact French resident. Their existing family
bindings already supply every required name; the 37-address set is unchanged.

Direct entry accesses establish a minimum context view through `+0x1BB4`
(7,092 bytes). The fixed `0x80136000/0x80176000` contexts passed through
slot `field_DEC` do not overlap the selected model/primary/secondary
loads. This is not a whole-context capacity or global lifetime claim;
three initialized web records likewise do not define the entire allocation.

After this follow-up the family has 162,864 matching C bytes, 220,376
assembly bytes and 149,136 unclassified suffix bytes. Overall configured
French totals become 192 images, 788/1,343 matching C instances and
720,812 C instruction bytes. Both sheet and webs are entry-reachable;
the four retained C helpers remain separate from proven entry paths.
These figures do not establish exhaustive runtime coverage.

## Retained ribbons: first-segment indexing recovery

The 1,612-byte ribbon helper needs the original loop-dependent screen
coordinate form even inside `k == 0`: `sa[k + 1] - sa[k]`, with separate
signed low-half and arithmetic upper-half expressions. Constant `sa[1]`
and `sa[0]` are semantically equivalent there but lose a loop-derived
pointer spill. The accepted GCC 2.8.1 profile then emits 1,600 bytes and a
288-byte frame instead of the retail 296-byte frame. Indexed coordinates
recover every instruction, including register allocation and scheduling.
The `VERSION_FRENCH` guard preserves the original US expression branch.

The [attempt ledger](french-model-variant435-attempts.csv) retains all six
earlier rejected source/profile experiments, the exact indexed-text trial,
and two terminal canonical-wrapper records. Both slots and all 26 complete
canonical images match, adding 41,912 C bytes without adding registrations.
The same root fix also matches [Family442 ribbons](french-model-variant442.md).

The helper constructs eight 116-byte records at context `0xD50..0x10F0`,
ending exactly at the separately established band base. Each has two
SVECTOR points at 0, packed screens at 16, angles at 24, offset points at
32, their screens at 48, widths at 56, depths at 100 and halfword offsets
at 108/112. The region at 64..100 stays opaque. Both two-point passes and
all three eight-record loops are independently checked. The third pass
draws one `POLY_G3` per record at context `0x19A4`, accepting nonnegative
depth; projection flags are not a separate drawing gate.

Scale comes from the 152-byte sheet at `0x12DC`, field `+0x88`. The final
angle update compares the signed halfword at `0x1B88`, plus one, with the
unsigned halfword at selected descriptor `+0x20`; it then adds
`step << 5` from `0x1B64` to the angle at `0x1B98`. Every selected legal
descriptor has comparison value one. This is a measured comparison field,
not an inferred duration or whole-animation completion claim.

The two-family proof freshly compiles 45 local/SDK layout constants and
checks 59 focused anchors per family, all physical commands/descriptors,
11 ribbon callees per family and three resident caller/loader owners.
Family435 retains all 37 binding addresses; only the existing
`0x80087868` alias becomes `RotTransPers`. The direct context minimum remains
`0x1BB4`, not allocation capacity. Ribbons remains retained, not entry-called.

The independent combined batch is based on accepted `47eb84ae7`, excluding
the pending petal, Family445 webs, Family402 strip and Family433 webs batches.
Family435 has 182 C owners / 204,776 bytes, 52 assembly owners / 178,464
bytes and 52 raw owners / 149,240 bytes. The combined addition is 30 C
instances / 48,360 bytes; configured French totals become 222 images,
920/1,521 C instances and 898,524 C bytes. All 222 French and 263 US complete
images and both clean residents passed exact matching. Fresh production
verification confirms all 206 combined French C owners, 68 assembly owners,
60 raw owners, all 30 preserved US ribbon C owners, 45 newly compiled
constants and the resident bindings/callers. The 130 French, 57 Spanish,
16 progress and five US toolchain regressions pass without skips, together
with metadata/basic-types/external-attempts/G32 gates. Remaining assembly
and unclassified suffixes stay untouched.

### Accepted ownership reconciliation

Fixed accepted cutoff `e8c82d3d5` is merged normally to resolve the
progress conflict. All 252 accepted registrations and 936 accepted C
instances remain, including thirty petal, twelve Family445 webs and four
Family433 webs owners. The original combined ribbon addition remains
30 C instances / 48,360 bytes: 966/1,581 C instances and 965,620 C bytes.
The original 135 paths retain 131 unchanged authored files; aggregate
progress, the two family notes and a regional ledger guard differ.
The guard applies French ribbon experiments only to French ledgers.
One additional Spanish435 fixture explicitly preserves its six accepted
helpers instead of inheriting
the new French ribbon selection. No Spanish source, inventory or binding
is promoted or changed. The resulting scope is 136 paths, with no pending
French branch stacked.

Accepted header additions require fresh evidence, not the old header
fingerprint. All 30 canonical images and 45 compiled layout constants
pass again, with 59 focused anchors and eleven callees per family.
The scratch relink preserves the baseline assembly's previous
`func_french_80087868` alias at the same verified `RotTransPers` address;
production bindings retain only their existing renamed entries.
All 263 US and 252 French complete images and both clean residents pass
again. Production verification preserves the thirty petal, twelve Family445
webs and four Family433 webs C owners, all thirty US ribbon owners, and
the ribbon families' 206 C, 68 assembly and 60 raw owners. The 141 French,
72 Spanish, 16 progress and five US toolchain regressions pass without skips,
alongside metadata/basic-types/external-attempts/G32 checks.

The subsequently accepted Family402 strips at `ca50339b5` are merged
normally before publication, preserving four more C owners / 6,064 bytes.
The fixed accepted baseline now has 940 C instances; this branch still adds
only the same thirty ribbons, yielding 970/1,581 C instances and 971,684
C bytes across 252 images. The 136-path scope and 131 unchanged
original files remain. No pending Family402 ribbon branch is stacked.
This strip-only accepted delta leaves US/shared/Spanish build inputs
unchanged. All 252 French images and the clean resident pass again,
alongside 142 French, 72 Spanish, 16 progress and five US toolchain
regressions and the policy gates. Production verification establishes
the four accepted strip C owners as well as all previously checked
petal, webs, ribbon, assembly and raw owners; 45 layouts are recompiled.

The subsequent requested cutoff `53922e730` preserves the accepted
Spanish442 fixture's standalone ribbon body and per-helper source directory.
Its six-helper selection is explicit, preventing the new French ribbon
from being inherited a second time. French-only wrapper macros and
experiment-ledger expectations remain region-gated; Spanish source,
inventory and bindings are unchanged relative to that accepted cutoff.
The scope is now 137 paths, with 130 of the original 135 files unchanged.
French totals remain 970/1,581 C instances and 971,684 bytes; no pending
Family402 ribbon branch is stacked. All 263 US and 252 French complete
images and both clean residents pass again. Actual-owner verification
retains the accepted strips, petals and webs, all thirty US ribbon owners
and all original French ribbon owners. The 45 compiled layouts,
142 French, 81 Spanish, 16 progress and five US toolchain regressions
and repository policy gates pass.

The newer requested cutoff `625cf9497` is merged next. Its exact
70-path delta adds only accepted US header-404 bands/header-423 sheets
sources and metadata, their US toolchain fixture and US notes. French and
Spanish inputs, existing shared sources, headers and compiler profiles
are unchanged. The just-verified French image and resident evidence is
retained with hashed resident artifacts. All 263 US images and the clean
US resident pass again, with actual ownership verified for twelve accepted
header-404 bands, four header-423 sheets and all thirty US ribbons.
The unchanged French owners and 45 recompiled layouts are verified again;
142 French, 81 Spanish, 16 progress and five US toolchain regressions
and repository policy gates pass.

Newly maintainer-accepted Family402 ribbons at `3b77bdf0c` are then merged
normally, preserving four additional C owners / 6,480 bytes. The accepted
baseline has 944 C instances; this branch still adds only thirty ribbons,
giving 974/1,581 C instances and 978,164 C bytes across 252 images.
The exact 24-path accepted French-only delta leaves US inputs unchanged;
fresh US resident artifacts are preserved with hashes. All 252 French images
and the clean French resident pass again. Actual production verification
preserves both the four accepted strips and four accepted Family402 ribbons,
all previously verified French/US owners, and 45 recompiled layouts.
The broader regional regression selection passes 199 French and 103 Spanish
tests, including all 143 French and 81 Spanish MODEL-family tests; 16 progress,
five US toolchain regressions and repository policy gates also pass.
The authored scope remains 137 paths, with 130 original files byte-identical.

## Entry-called spiral: last-point indexing recovery

The accepted US418 structure initially emits 2,608 bytes and a 312-byte
frame under GCC 2.8.1, rather than the French 2,640 bytes and 320-byte
frame. Both slots differ at 477 aligned words in their common extent.
The difference is concentrated in the projection loop's pointer lifetimes
and spills, with consequent register allocation and scheduling changes.
Inside `k == 1`, twenty literal `[1]` accesses must retain the loop index
`[k]`. That form reproduces every target instruction. A single
`VERSION_FRENCH` block selects it while preserving all original US
expressions; no type, header, binding or compiler-profile change is needed.

All 26 complete canonical scratch images match without masking, with
sized defining C objects and linked symbols. The 21 accepted ledger rows
remain byte-identical, followed by two original mismatches, two exact
indexed trials and two canonical matches. The addition is 26 C instances /
68,640 instruction bytes, not new registrations.

Thirty-eight freshly compiled layout constants and 244 retail anchors
verify twelve 132-byte `Variant418SpiralArm` records at context
`0x720..0xD50`, between the established webs and ribbons. Each has two
eight-byte spine points at `0x10`, two offset points at `0x30`, projected
screens at `0x20/0x40`, angles at `0x28`, widths at `0x48`, color rows at
`0x50/0x58`, projection flags at `0x64`, depths at `0x6C`, and signed
halfword offsets at `0x74/0x78`. Unaccessed bytes remain opaque.
Entry independently initializes two color entries per arm and advances
twelve times by `0x84`.

The helper uses the `POLY_GT4` packet at context `0x19E4`. It constructs
two-point arms, projects their spines and offset copies, and draws two
quad halves per arm. The depth and flag stores to zero before each
nonnegative-depth test remain, including the redundant reload/branch and
low-sixteen-bit sort argument. Projection `p/flag` stack outputs are at
`0xD0/0xD4`; the per-arm projection flags have separate storage.

Direction words are at `0x1B2C/0x1B30/0x1B34`; transform translation
uses words at `0x1B08/0x1B0C/0x1B10`. Mode `0x1B58` controls arm length
and scale adjustment. Angle halfword `0x1B6C` advances by `0x10`; phase
`0x1B9C` grows scale halfword `0x1B70` to `0x1000`, then tapers halfword
`0x1B72` to `0x400`. Both timing divisions are unsigned.

All ten selected descriptor commands are read independently from their
actual stage-specific metadata words. The 68-byte descriptor base remains
`+0x3A94`, through context pointer `0x1B74`. Growth uses words `+0x2C/+0x30`;
taper uses `+0x34/+0x38`. Every selected denominator is strictly positive.

| Command | Growth start/end | Taper start/end |
|---|---|---|
| 601000 | 120/136 | 280/320 |
| 601001 | 110/122 | 320/380 |
| 601002 | 72/84 | 150/180 |
| 601003 | 100/112 | 280/320 |
| 601005 | 40/52 | 220/250 |
| 601006 | 80/92 | 240/260 |
| 601007 | 60/72 | 100/160 |
| 601009 | 40/48 | 60/160 |
| 601010 | 140/148 | 360/400 |
| 601011 | 100/108 | 240/260 |

Eleven helper callees, all 37 resident bindings and three caller/loader
owners are independently checked. Entry calls the spiral at `+0xF1C`,
passing its original context at `+0xF20`, on the existing phase-gated path.
The direct context extent stays `0x1BB4`, not allocation capacity.
The suffix remains unclassified. Spanish435 explicitly retains its six
accepted helpers and previous reachable-helper selection.

Calibration began independently at accepted
`5f3a5517033eab0cadb00a08cfda54a811c62a48`; before canonical integration,
the clean branch fast-forwarded only to accepted French439
`28bd694898b41b037bdf0d10cf3ed458b76b0f49`. Pending French422 webs were
not stacked.

Spiral production acceptance passed all 263 complete US overlays, all
252 complete French overlays and both clean resident images. Actual
defining objects and linked ELFs prove 208 Family435 C owners / 273,416
bytes, preserving all 182 prior owners / 204,776 bytes. The 26 remaining
assembly owners occupy 109,824 bytes; 52 raw header/suffix owners occupy
149,240 bytes. All 26 original US spiral owners / 67,288 bytes and all
56 accepted Family439 C owners / 77,224 bytes remain actual C definitions.
The fresh French resident also verifies every recorded binding and caller.

All 205 French, 112 Spanish and 21 progress/toolchain regressions pass
without skips. The independent authored scope is 86 paths; the fixed
cutoff is 1,104/1,581 C instances / 1,218,028 instruction bytes. This
checkpoint excludes French422 webs and makes no exhaustive coverage claim.
