# French MODEL headers 415 and 565

Ten distinct images at stages 9/10 for models 102, 282, 288, 642 and 645
reuse unchanged accepted `variant398_sheets.c` and `variant398_webs.c`
through four canonical renaming wrappers. The named
`gcc_2_8_1_g0_split` profile uses GCC 2.8.1/MASPSX 2.81; US compiler
provenance is not a French compiler-selection rule.

## Loader and complete ownership

The matched MODEL loader selects ten 2,048-byte sectors from 276-sector
compact records, at record offsets 200/210. Loads are `0x8013B000` and
`0x8017B000`, with entry at `+4`. The controller supplies context and
the initial nonnegative command or update `-1`.
The [instance ledger](french-model-variant415-instances.csv) records
actual models, compact indices, commands, slices and complete hashes.

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `0x4..0x12B4` | 4784 | generated assembly | yes |
| `0x12B4..0x18A4` | 1520 | generated assembly | yes |
| `0x18A4..0x2024` | 1920 | bands C | yes |
| `0x2024..0x2508` | 1252 | sheets C | yes |
| `0x2508..0x2A34` | 1324 | webs C | no |
| `0x2A34..0x2FB8` | 1412 | generated assembly | yes |

Strict control-flow walks cover every instruction and delay slot in all
six spans, each with one terminal return and no unresolved indirect
transfer. Webs remains retained game code: no direct entry-call path is
claimed. The four-byte header and 8,264-byte suffix at
`0x2FB8..0x5000` have real storage owners. The suffix remains unclassified,
not proven non-code.

## Independently recovered views

Entry captures `a0 -> s3 -> s8`. It initializes two 152-byte sheets at
context `0x6A8..0x7D8`, saving the pointer at stack `0x84`. Four corner
arrays begin at record offsets `0/0x20/0x40/0x60`, outer/inner colors
at `0x80/0x84`, and size at `0x88`. Pointer reloads, the four-corner
bound, two-record bound and 152-byte advances establish the accessed view.

Three 416-byte narrow webs at context `0..0x4E0` have two 4-by-6
`SVECTOR` grids at `0/0xC0`, color at `0x180`, scale at `0x194` and
done at `0x198`. The grid walker is `s1`, unlike Family439's `s2`.
Entry clears done separately from scale; the helper updates this field
during its final phase. Eight-byte element, 48-byte row and 416-byte
record advances agree with the six/four/three bounds.

Real commands `581000`, `581001` and `581004` select 48-byte descriptors
at module `0x30B4 + (command % 1000) * 48`. Entry's shift/add sequence
independently establishes the stride; the resulting descriptor pointer
is stored at context `0x1A00`. The direct context-access minimum is
`0x1A44`. Loader/controller pointer evidence separates the accessed
context from the model, auxiliary and overlay loads. This minimum is
not whole-allocation capacity or proof of global isolation.

Forty retail instruction anchors and the batch's 50 target-compiled
local C/SDK constants support these views. All 36 resident callee
addresses and the loader/controller owners are checked against actual
resident ELF bytes; `ratan2` retains its established French address
`0x80089928`.

## Exactness and scope

The [attempt ledger](french-model-variant415-attempts.csv) records four
terminal canonical-wrapper matches. Both C helpers are linked together
in each complete unmasked image, not accepted via masked comparisons.
Production ownership comprises 20 C owners / 25,760 C bytes,
40 assembly owners / 96,360 assembly bytes, and 20 raw owners.

This family and [Family439](french-model-variant439.md) form an independent
batch from accepted master `e4f38060e`, preserving all 198 existing
French registrations. Together they add 24 images, 144 inventoried
function instances and 48 C instances / 57,960 C bytes. All 222 configured
images and the clean resident must match; configured totals are
890/1,521 C instances and 850,164 C instruction bytes.

Shared bodies, headers and profiles are unchanged. Initial acceptance
used `15558fb58`; the branch was refreshed after the independent
Family465 work was maintainer-merged, not stacked while pending.
Seven family regressions
cover legal slices, commands, hashes, sources, terminal fingerprints,
complete spans, reachability, binding addresses, raw extents and accessed
views. Untranslated functions, suffixes and other runtime areas remain;
these counts do not establish exhaustive coverage. Report snapshots are
separate.

## Entry-called bands follow-up

Two canonical wrappers reuse accepted `variant398_bands.c` with measured
`VERSION_FRENCH` guards for four next-column `sc/sb` loads. The original
US expressions remain unchanged. Both French functions match 1,920 bytes
with a 296-byte frame; both complete-slot scratch links reproduce all ten
images. The original failures differ only at function-relative
`0x528/0x534/0x558/0x564`. The four original terminal ledger rows remain
byte-identical, followed by two original failures, two indexed experiments
and two canonical matches.

Independent evidence covers 35 target-compiled constants, 144 instruction
anchors, twelve helper callees, 36 resident bindings and three caller
owners. One 456-byte band occupies context `0x4E0..0x6A8`, with three
nine-point `SVECTOR` rows at `0/0x48/0x90`, packed screen arrays at
`0xD8/0xFC/0x120`, colors at `0x144/0x168` and depths at `0x1A4`.
Flags are stack `PSXLONG[1][9]` at `+0xC8`, not band storage.
The packet at context `0x1854` is reused for two quads per each of eight
adjacent pairs; negative depths clamp to zero before low-sixteen-bit
sorting.

The radius halfword at `0x1A18` is divided by 64 or multiplied by 70/4096
according to the low bit at `0x19EC`. Entry calls the band at `+0x114C`,
passing the original context. The 48-byte descriptors at `+0x30B4`
retain timing pairs at `+0x20/0x24` and `+0x28/0x2C`: commands
`581000/581001/581004` select `(50,60,290,304)`, `(80,88,280,356)` and
`(68,76,280,340)`, respectively. Both denominator differences are positive.
The direct context minimum `0x1A44` is not an allocation-capacity claim;
the `0x2FB8..0x5000` suffix remains unclassified.

This independent batch starts at accepted
`9da5d222f77fa21ec63ad4ecb773dea9e20dbef9`, excluding pending curtains.
It adds ten C instances / 19,200 bytes while retaining all 252 registrations
and 1,022 accepted C instances. Configured totals become 1,032/1,581 C
instances and 1,082,020 bytes. Expected family ownership is thirty C
owners / 44,960 bytes, thirty assembly owners / 77,160 bytes and twenty
real header/suffix owners / 82,680 bytes. Production acceptance passed
all 263 complete US images, the clean US resident, all 252 complete French
images and the clean French resident. Actual linked ELF/object evidence
preserves the twenty prior sheets/webs C owners / 25,760 bytes and ten
US398 band owners / 19,200 bytes. The 202 French, 112 Spanish, sixteen
progress and five US-toolchain regressions pass without skips, alongside
G32 and repository policy. No new header, types, binding or compiler
profile is needed. The authored scope is 38 paths.
