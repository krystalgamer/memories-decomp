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
| `0x80010034` | `0x80185CD4` | SU-specific | Interior pointer passed to `func_80049394` after that direct SU load |

The repeated value `0x80180000` does not make the three words aliases in
source. They occur in different call paths and preserve different relocation
sites: one general model base, one phase-callback destination, and one direct
SU destination.

## MODEL record phase map

The
[`Model_LoadMonsterMerge`](../src/candidates/func_80056504.c) source requests
one `0x114`-sector record from `MODEL.MRG` and installs `func_80056D7C` as its
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

`func_8004CB0C` stores the data addresses into each model-channel record:

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
- `func_800507D0` uses `0x80010030` as the direct destination for SU sectors
  `0x4C7..0x4D7`.
- After the direct load, the same routine calls loaded SU entries and passes
  `0x80010034`, whose value `0x80185CD4` lies inside that module, to
  `func_80049394`.

An interior module pointer is not an arena base. Its presence at the end of
the contiguous words is decisive counter-evidence to a uniform
`gMain_apLoadArena` array.

## Naming conclusion

The following are supported:

- the fourteen words form a contiguous high-memory pointer-constant block;
- the four cohorts above have distinct, repeatable roles; and
- individual address-based symbols should remain until each cohort has a
  source representation that preserves the required relocation behavior.

The proposed `gMain_apLoadArena` name is rejected. The block is not owned only
by the main frontend, not every element is a load destination, and retail
consumers do not establish one common element type or indexing contract.

## Shared C contracts

[`high_memory_addresses.h`](../src/game/high_memory_addresses.h) now owns
all nine address constants named by current C consumers. It replaces 23
private declarations across 14 matching resident translation units, the
shared overworld translation unit, and two integrated resident candidates.
The five other labels in the fourteen-word block have no individually named
C consumers; no speculative declarations or storage definitions are added
for them. Three of those words are read through the prefix view below.

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
| `HIGH_MEMORY_ADDRESSES_BASE_IN_DATA` | `Campaign_LoadScenePackage`, `Duel_LoadPackageStage`, `file_transfer_steps.c`, `func_8003A560`, `func_8003B808`, `func_8003BF00`, `func_8005B8A0` | Forced `.data` declaration, not a storage definition |
| `HIGH_MEMORY_ADDRESSES_MODEL_PREFIX` | `func_8001755C`, `main_run_duel_and_library.c`, `main_run_selection_menus.c`, candidate `func_80056828` | `D_80010000`-relative array addressing |

The indexed arm takes `HighMemoryModelAddressPrefix` from
[`ygo_types.h`](../src/ygo_types.h). This is only the first five address
words: `payload_bases[3]` at `+0x00` and `primary_modules[2]` at `+0x0C`,
with asserted size `0x14` and module offset `0x0C`. Both cohorts contain
byte pointers, but their roles remain distinct. The module entry is still
reached by adding four bytes; its function signature is not inferred here.
Three resident loads and five candidate loads now use these cohort fields.

Only element zero of the incomplete prefix array is used. It is an
addressing view, not evidence of repeated prefix records or the extent of
the fourteen-word allocation. `func_8001755C` formerly used a complete
three-word signed array; both that 12-byte declaration and the new
incomplete view avoid small-data classification under its named profile.
The other indexed consumers already used incomplete arrays. Measurement
confirms that the pointer conversion and prefix fields preserve their
instructions and relocations.

The independently named `D_80010008`, `D_80010014`, `D_80010018` and
`D_8001002C` transfer destinations retain signed-word `.data` declarations.
The initializer candidate's `D_8001001C`, `D_80010020`, `D_80010024` and
`D_80010028` data arguments retain plain signed-word declarations. Reaching
one of these through a field of `D_80010000` would change its relocation
identity, even when the eventual runtime address is the same.

Candidate `func_80056828` keeps its existing base-relative relocations;
they are not rewritten to the retail assembly's individual labels.
Candidate `func_8004CB0C` keeps its four independently labeled data loads.
Both consume the shared header, so their reviewed schema-2 dependency
metadata removes only the five superseded private-extern entries. Their
object fingerprints remain respectively
`9c4d76a2fa8a88514edc63839f9cacf472f6c03f1043c3006ae29f0d9923425b`
and `8a7cb6cbdeb962db3fc43fd2d667c10a2c1da30a83671a995122c4b6f750d625`.
Neither candidate is promoted or claimed to match retail.

The remaining constants, Psy-Q CRT's zero-count callback walks, generated
data, function grouping, compiler profiles and existing register assignments
are unchanged. This contract consolidation does not map the storage to C.
