# High-memory load-address constants

## Scope

The initialized words at `0x80010000..0x80010034` are fourteen contiguous
high-memory pointers used by the resident loader, model runtime, movie
runtime, and SU overlay setup. Earlier research described the first eight
words as one `gMain_apLoadArena` table. Complete consumer coverage and the
`MODEL.MRG` phase callback do not support that abstraction.

The words are physically contiguous and related to the fixed high-memory
layout, but they form four role-specific cohorts. Retail code normally loads
their individually labeled addresses. The third cohort contains data
arguments rather than load destinations, and the final word is an interior
pointer into an SU module. One homogeneous array type or `gMain_` name would
therefore erase distinctions established by the consumers.

## Complete constant block

| Address | Value | Cohort | Established use |
|---:|---:|---|---|
| `0x80010000` | `0x80100000` | general/model base | Slot-0 MODEL payload; also reused by duel, Library, Build Deck, Free Duel, display-effect, and movie paths |
| `0x80010004` | `0x80140000` | general/model base | Slot-1 MODEL payload |
| `0x80010008` | `0x80180000` | general/model base | Non-duel model payload base selected by the short MODEL/SU callback |
| `0x8001000C` | `0x8013A000` | paired MODEL module | Slot-0 primary module destination; called at `+4` |
| `0x80010010` | `0x8017A000` | paired MODEL module | Slot-1 primary module destination; called at `+4` |
| `0x80010014` | `0x8013B000` | paired MODEL module | Slot-0 variant module destination; called at `+4` |
| `0x80010018` | `0x8017B000` | paired MODEL module | Slot-1 variant module destination; called at `+4` |
| `0x8001001C` | `0x80135000` | paired MODEL data | Slot-0 data argument stored at channel offset `+0xDE8` |
| `0x80010020` | `0x80175000` | paired MODEL data | Slot-1 data argument stored at channel offset `+0xDE8` |
| `0x80010024` | `0x80136000` | paired MODEL data | Slot-0 data argument stored at channel offset `+0xDEC` |
| `0x80010028` | `0x80176000` | paired MODEL data | Slot-1 data argument stored at channel offset `+0xDEC` |
| `0x8001002C` | `0x80180000` | SU-specific | Destination selected by `MainMenu_LoadPackageStage` for SU sectors `98-114` |
| `0x80010030` | `0x80180000` | SU-specific | Direct destination for the 16-sector SU load beginning at sector `0x4C7` |
| `0x80010034` | `0x80185CD4` | SU-specific | Interior pointer passed to `SD_SetMusicTrackBuffer` after that direct SU load |

The repeated value `0x80180000` does not make the three words aliases in
source. They occur in different call paths and preserve different relocation
sites: one general model base, one phase-callback destination, and one direct
SU destination.

## PAL boot text tail

The PAL boot text tail is a separate, later cohort outside the fourteen
NA words above. `gFile_pBootTextStage5Destination`,
`gFile_pBootTextStage6Destination` and `gFile_pBootTextStage7Destination`
retain the original stored `u8 *G32` views of `D_800101E0/4/8`, under the
European build guard only. Boot stages 5/6/7 read those individual words
for their 30/32/7-sector transfers. The names identify the measured
transfer roles, not independently verified pointer contents, allocation
sizes or a uniform text-bank format. See the
[PAL text handoff](european-text-bank-handoff.md).

## MODEL record phase map

The
[`Model_LoadMonsterMerge`](../src/game/model_slot_control.c) source requests
one `0x114`-sector record from `MODEL.MRG` and installs `Model_LoadMonsterMergeStage` as its
phase callback. The callback has seventeen stage entries. Sixteen consume
bytes and the last finalizes the staged model metadata.

| Stage | Sectors | Destination or action |
|---:|---:|---|
| 0 | 96 | Direct load to `0x80100000` for slot 0 or `0x80140000` for slot 1 |
| 1 | 48 | Buffered VRAM image upload, positioned from the slot index |
| 2 | 2 | Stage `0x1000` bytes at `0x801DD000` |
| 3 | 1 | Upload the staged image block, then load the next sector at `0x801DE000` |
| 4 | 16 | Position-0 image phase; irrelevant variants are consumed without copying |
| 5 | 1 | Stage one sector at `0x801DD000` |
| 6 | 16 | Position-1 image phase; irrelevant variants are consumed without copying |
| 7 | 10 | Slot-0, position-0 variant to `0x8013B000`, otherwise skipped |
| 8 | 10 | Slot-1, position-0 variant to `0x8017B000`, otherwise skipped |
| 9 | 10 | Slot-0, position-1 variant to `0x8013B000`, otherwise skipped |
| 10 | 10 | Slot-1, position-1 variant to `0x8017B000`, otherwise skipped |
| 11 | 2 | Slot-0 primary module to `0x8013A000`, otherwise skipped |
| 12 | 2 | Slot-1 primary module to `0x8017A000`, otherwise skipped |
| 13 | 1 | Slot-local `0x800`-byte block at `0x801A8000 + slot * 0x800`, or consumed without copying on the flagged slot path |
| 14 | 50 | Buffered model image/data phase, or consumed without copying on the flagged slot path |
| 15 | 1 | Final staged sector at `0x801DD000` |
| 16 | 0 | Copy staged metadata into the active model-channel record and mark it ready |

The consuming stages total:

```text
96 + 48 + 2 + 1 + 16 + 1 + 16
+ 10 + 10 + 10 + 10 + 2 + 2 + 1 + 50 + 1
= 276 sectors = 0x114 sectors
```

This exact total independently confirms the stage boundaries.

Stages 7-10 explain why each slot has one variant-module destination: two
position variants occupy separate disc phases but are mutually exclusive at
runtime, so both variants for a slot reuse `0x8013B000` or `0x8017B000`.
Stages 11-12 similarly choose one primary module destination by slot.

## Module and data pairing

`Model_InitSlot` stores the data addresses into each model-channel record:

- slot 0 receives `0x80135000` at `+0xDE8` and `0x80136000` at `+0xDEC`;
- slot 1 receives `0x80175000` at `+0xDE8` and `0x80176000` at `+0xDEC`.

The model dispatcher and cleanup paths call the primary module at
`0x8013A004` or `0x8017A004` and the selected variant module at `0x8013B004`
or `0x8017B004`. The paired `+0xDE8`/`+0xDEC` values are passed as arguments
to those module entries. They are not themselves called and are not proven
archive destinations.

This distinction is why the four data words cannot safely be folded into an
array named only for load arenas.

## SU-specific tail

The last three words are a separate SU cohort:

- `MainMenu_LoadPackageStage` uses `0x8001002C` to load the main-menu
  executable into `0x80180000`.
- `Model_UpdateCreditsPresentation` uses `0x80010030` as the direct
  destination for SU sectors
  `0x4C7..0x4D7`.
- After the direct load, the same routine calls loaded SU entries and passes
  `0x80010034`, whose value `0x80185CD4` lies inside that module, to
  `SD_SetMusicTrackBuffer`.

The matching `Model_UpdateCreditsPresentation` uses the owning declarations in
`high_memory_addresses.h`. Its three fixed imports are declared by
`model_scene_imports.h` as measured caller views for this SU sector range;
they are not declarations for the main-menu package that reuses the same
virtual addresses, nor recovered implementation signatures for that overlay.

An interior module pointer is not an arena base. Its presence at the end of
the contiguous words is decisive counter-evidence to a uniform
`gMain_apLoadArena` array.

## Naming conclusion

The following are supported:

- the fourteen words form a contiguous high-memory pointer-constant block;
- the four cohorts above have distinct, repeatable roles; and
- the module and data words can be named individually through their existing
  scalar declarations without changing relocation identities or types.

The proposed `gMain_apLoadArena` name is rejected. The block is not owned only
by the main frontend, not every element is a load destination, and retail
consumers do not establish one common element type or indexing contract.

## Individual module and data words

The eight model-only words now have names supported by transfer producers
and module-call consumers. Their existing scalar labels remain distinct:

| Word | Identity | Existing declaration view |
|---|---|---|
| `0x8001000C` | `gModel_pSlot0PrimaryModule` | `u8 *G32`, `.data` |
| `0x80010010` | `gModel_pSlot1PrimaryModule` | `u8 *G32`, `.data` |
| `0x80010014` | `gModel_Slot0VariantModuleAddress` | `s32`, `.data` |
| `0x80010018` | `gModel_Slot1VariantModuleAddress` | `s32`, `.data` |
| `0x8001001C` | `gModel_Slot0PrimaryDataAddress` | `s32`, `.data` |
| `0x80010020` | `gModel_Slot1PrimaryDataAddress` | `s32`, `.data` |
| `0x80010024` | `gModel_Slot0VariantDataAddress` | `s32`, `.data` |
| `0x80010028` | `gModel_Slot1VariantDataAddress` | `s32`, `.data` |

Module dispatch still adds four to the selected module base and passes the
separate data word through the slot's `+0xDE8` or `+0xDEC` value. No new
function-pointer type, data-record type, storage, const qualifier or array
bound follows from these names. The signed address words are not retyped.

The reused primary word's scalar consumers now use `gRuntime_pSharedWorkBase`;
see [shared-transfer-workspaces.md](shared-transfer-workspaces.md). Its
incomplete legacy `D_80010000` prefix view, declaration selectors and
base-relative relocations are unchanged. Naming the independent scalar
words does not replace accesses through that prefix.

## Model payload and credits words

Four further direct-word identities have producer/consumer support:

| Word | Identity | Existing declaration view |
|---|---|---|
| `0x80010004` | `gModel_pSlot1PayloadBase` | `u8 *G32`, `.data` |
| `0x80010008` | `gModel_AuxiliaryPayloadAddress` | `s32`, `.data` |
| `0x80010030` | `gModel_CreditsModuleAddress` | `s32`, `.data` |
| `0x80010034` | `gModel_pCreditsMusicTrackBuffer` | `void *G32`, `.data` |

`Model_LoadMonsterMerge` selects its monster callback for slots below 2.
That callback sends slot 1's first payload to the first word above.
`Model_AdvanceSlotLoad` independently selects the same word for case 1
before passing the payload to `Model_InitSlot`.

The loader's other branch selects `Model_LoadAuxiliaryPackageStage` after
its existing background-record lookup. That callback loads its first
18 sectors through the auxiliary word; the slot stepper uses the same
word as its default payload, replacing it only for explicit cases 0/1.
No index validation or default-branch behavior is changed.

The credits controller loads its 16-sector SU range through the credits
module word, waits, then calls the existing fixed imports. It passes the
last word through its original `u16 *` cast to `SD_SetMusicTrackBuffer`.
The target is interior module data, not a function pointer or a separately
allocated music buffer. Poll order, status predicates, reset calls and
borrowed lifetime remain.

These are four independent scalar labels, not a new homogeneous array.
The auxiliary and credits destination words retain distinct relocations
even where their stored values are equal. Existing accesses through the
`D_80010000` prefix still use that base and its original offsets, rather
than being rewritten to the new direct names.

The isolated `model_load_words.h` declarations preserve the exact
pointer/integer types, `G32` placement and `.data` attributes. The old
`high_memory_addresses.h` and measured `model_scene_imports.h` stay frozen;
imported code names remain address-based because other SU packages reuse
the runtime range.

Three existing behavioral controls receive only namespace/include
migrations, with complete inverse-file equality and unchanged cases,
assertions, mutations, fixture layouts, generators and compiler/setup.
The legacy type/HI-LO ownership controls intentionally retain their old
names and header. No new harness, fixture, probe or testcase is added;
cloud/regional/overlay/witness suites are not executed locally.

Canonical aliases retain each original provider in all seven images.
Twenty bindings have local numeric evidence; the eight English/Japanese
automatic-provider cases remain symbolic without guessed tracked
assignments. Full regional-image PR checks remain required.

The shared four-byte `D_8009B004` record is outside this naming component:
debug uses its height halfword while credits uses its first two bytes, so
a credits-only identity would be unsupported.

## Shared C contracts

The original consolidation put nine named address constants in
[`high_memory_addresses.h`](../src/game/high_memory_addresses.h), replacing 23
private declarations across 14 matching resident translation units, the
shared overworld translation unit, and two integrated resident candidates.
The four direct-word names above extend that work without changing the
legacy declaration owner or inventing storage. The remaining shared
primary word and its indexed prefix view retain the contracts below.

The scalar `D_80010000` contract is `u8 *`: the word contains a payload
address, not the first bytes of a payload. Its consumers reuse that address
for portrait records, effect scratch, model data and a movie VLC buffer, so
no more specific pointed-to record type is valid across those lifetimes.
The old signed-word and `void *` spellings converge on this pointer type;
casts remain at the transfer descriptor's integer fields and other existing
integer ABI boundaries.

| Header arm | Consumers | Addressing retained |
|---|---|---|
| Default pointer | `Campaign_LoadScenePackageStage`, `DuelEffect_AllocateRequest`, `Main_InitFreeDuelMenu`, overworld `set_location.c` | Plain scalar |
| `HIGH_MEMORY_ADDRESSES_BASE_IN_DATA` | `Campaign_LoadScenePackage`, `Duel_LoadPackageStage`, `file_transfer_steps.c`, `func_8003A560`, `FreeDuel_LoadPackageStage`, `CampaignMap_LoadPackageStage`, `Movie_OpenStream`, `Model_AdvanceSlotLoad` | Forced `.data` declaration, not a storage definition |
| `HIGH_MEMORY_ADDRESSES_MODEL_PREFIX` | `Duel_InitModelScene`, `main_run_duel_and_library.c`, `main_run_selection_menus.c` | `D_80010000`-relative array addressing |

The indexed arm takes `HighMemoryModelAddressPrefix` from
[`ygo_types.h`](../src/ygo_types.h). This is only the first five address
words: `payload_bases[3]` at `+0x00` and `primary_modules[2]` at `+0x0C`,
with asserted size `0x14` and module offset `0x0C`. Both cohorts contain
byte pointers, but their roles remain distinct. The module entry is still
reached by adding four bytes; its function signature is not inferred here.
Three resident loads and five candidate loads now use these cohort fields.

Only element zero of the incomplete prefix array is used. It is an
addressing view, not evidence of repeated prefix records or the extent of
the fourteen-word allocation. `Duel_InitModelScene` formerly used a complete
three-word signed array; both that 12-byte declaration and the new
incomplete view avoid small-data classification under its named profile.
The other indexed consumers already used incomplete arrays. Measurement
confirms that the pointer conversion and prefix fields preserve their
instructions and relocations.

The independently named `gModel_AuxiliaryPayloadAddress`, `gModel_Slot0VariantModuleAddress`,
`gModel_Slot1VariantModuleAddress` and `gMainMenu_dwSuModuleBase` transfer destinations
retain signed-word `.data` declarations. The initializer's
`gModel_Slot0PrimaryDataAddress`, `gModel_Slot1PrimaryDataAddress`,
`gModel_Slot0VariantDataAddress` and `gModel_Slot1VariantDataAddress`
arguments are signed-word `.data` declarations too: the
matching `Model_InitSlot` is built under `gcc_2_8_1_g8_split`, and a
small-data view would turn its four absolute loads gp-relative. Reaching
one of these through a field of `D_80010000` would change its relocation
identity, even when the eventual runtime address is the same.

The earlier `Model_AdvanceSlotLoad` candidate retained base-relative relocations
under the indexed view; that consolidation was not a matching promotion.
The complete pin-free reconstruction of `Model_AdvanceSlotLoad` in `model_slot_control.c` now selects
the absolute scalar arm and uses the retail assembly's individually labeled
payload and primary-module loads. Its obsolete candidate and target
snapshot are retired only after exact text/table and full-image proof.
The matching `Model_InitSlot` keeps its four independently labeled data loads.

The original contract consolidation left the remaining constants, Psy-Q
CRT's zero-count callback walks, generated
data, function grouping, compiler profiles and existing register assignments
unchanged. Neither that consolidation nor the dispatcher match maps this
address-word storage to a new C allocation.
