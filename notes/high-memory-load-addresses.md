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

Matching
[`Model_LoadMonsterMerge`](../src/game/model_load_monster_merge.c) requests
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
