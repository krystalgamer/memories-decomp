# DuelEffect Shared Structures

This pass is limited to the shared records rooted at `D_800EB0F8` and
`D_800EB288`. The types in `src/game/duel_effect.h` describe verified storage
widths and offsets, not recovered original Konami names. Offset-based field
names remain where semantics or signedness are uncertain.

## `D_800EB0F8`: four `0x64`-byte channels

`DuelEffectChannel` has size `0x64`, and `D_800EB0F8` is declared as four
entries. The following local evidence establishes the stride and extent:

- `DuelEffect_InitEntry`, `TextBox_SetRect`, `func_8003D614`, and
  `func_8003F388` independently index or advance entries by `0x64`.
- `func_8003F388` scans exactly four entries.
- `D_800EB288 - D_800EB0F8` is `0x190`, exactly `4 * 0x64`; linker aliases
  such as `D_800EB15C` also land on `0x64` entry boundaries.
- Target assembly, GMS pseudocode, and Unchiga's same-address sources agree on
  the `25 * u32`/100-byte indexing. GMS and reference types are treated only
  as guesses; the local matching code and target instructions are decisive.

Verified channel fields used by matching C include:

| Offset | Shared field | Evidence |
|---:|---|---|
| `0x28`, `0x2C` | `field_28`, `field_2C` | cleared across four entries by `func_80035A64` |
| `0x30` | `field_30` | pointer-sized assignment in `func_80028310`; cleared by `func_80035A64` |
| `0x34` | `flags_34` | `0x10`, `0x2000`, and `0x8000` tests; reset by `func_80035A64` |
| `0x36`-`0x3A` | `field_36`, `field_38`, `field_3A` | halfword initialization in `DuelEffect_InitEntry` |
| `0x3C`-`0x42` | `field_3C`-`field_42` | four halfword writes in `TextBox_SetRect` |
| `0x53`-`0x5B` | byte fields and `index_57` | initialization sequence in `DuelEffect_InitEntry` |
| `0x5C`, `0x5E` | `range_start_5C`, `range_count_5E` | adjacent `D_80090E58` bounds |
| `0x61` | `field_61` | byte clear in `DuelEffect_InitEntry` |

Seven pure-C users now include the shared header and use
`DuelEffectChannel`: `func_80028310`, `func_80035A64`, `TextBox_SetRect`,
`DuelEffect_InitEntry`, `DuelEffect_CreateChannel`, `func_8003D614`, and
`func_8003F388`.

## `D_800EB288`: 620 `0x1C`-byte entries

`DuelEffectEntry` has size `0x1C`, and `D_800EB288` is declared as 620
entries. Exact local C repeatedly uses 28-byte pointer increments, while the
target assembly computes the same stride as `(index * 8 - index) * 4`.
`DuelEffect_ClearMatchingMarker` and `DuelEffect_ResetEntryMarkers` each scan
620 records. The resulting modeled extent is `0x43D0` bytes; the next linker
symbol, `D_800EF668`, begins 16 bytes later, so the intervening bytes are not
claimed by this type.

Verified entry fields are:

| Offset | Shared field | Evidence |
|---:|---|---|
| `0x11` | `flags_11` | bit `0x80` gates exact scans; target `func_8003741C` and `func_80039D64` agree |
| `0x12` | `field_12` | compared with the requested marker plus one |
| `0x13` | `field_13` | active-value test/write in the bounded scan pair |
| `0x15` | `field_15` | second byte written by `func_800373C8` |
| `0x18` | `field_18` | range write in `func_8003B6AC`; target `func_80036DBC` reads it |

Target `func_80039D64` copies complete seven-word (`0x1C`) records and advances
both pointers by `0x1C`, independently confirming that the named bytes belong
to one record rather than parallel arrays. Unchiga's same-address sketches
and GMS pseudocode corroborate these accesses but do not determine the shared
types.

`DuelEffect_UpdateObjectLayout` now provides an additional exact-C read of
`DuelEffectEntry.field_18`. It selects the display-object coordinate layout
from `field_18 % 10` after resolving the entry through the channel's
`D_80090E58` range index.

`DuelEffect_PlaySoundCommand` is now exact C in the effect-handler dispatch
family. It consumes one 16-bit script value, uses the high bit to select the
flagged sound path, and arms effect state `0x11` with a follow-up value.

Six pure-C functions now use the shared declaration:
`DuelEffect_ClearMatchingMarker`, `DuelEffect_ResetEntryMarkers`,
`DuelEffect_HasActiveEntry`, `func_800373C8`, `func_8003741C`, and
`func_8003B6AC`.

## Deferred and exact-code exceptions

Matching-C functions implemented with inline assembly remain unchanged:

- `Main_RunCredits` references `D_800EB0F8`;
- `func_80030998` references both globals through explicit relocations.

`DuelEffect_UpdateObjectLayout` is also matching C, but it retains a minimal
local `EffectEntry` view and is not counted among the six shared-header users.

The remaining assembly users are also unchanged. For `D_800EB0F8` they are
`func_80019D18`, `func_8001B170`, `func_800235C0`, `func_800262D4`,
`func_800283F4`, `func_8002ACA4`, `Main_RunTrade`, `func_8002DC38`,
`func_8002E5AC`, `func_8002EE94`, `func_80031084`, `func_800339D0`,
`func_80035E20`, `func_80039794`, `func_8003D74C`, `func_8003DA40`,
`func_8003DC1C`, `func_8003F454`, `func_8003F8D4`, and `func_80043BCC`.
For `D_800EB288` they are `func_800393B0` and `func_80039D64`.

`duel_effect_entry_scan.c` is the sole raw-view exception among migrated
pure-C users. Both functions include the shared header and use its typed
global declaration, but retain byte-pointer field expressions. Replacing
those expressions with direct `DuelEffectEntry` member access shortened the
two exact functions by eight bytes in total and failed the resident text-size
check. The retained expressions preserve GCC 2.8.1's address-adjustment and
register schedule; they are not a competing layout declaration.

The shared field syntax was exact-tested successfully in the other migrated
users, including direct indexing in `DuelEffect_InitEntry` and both marker
functions in `duel_effect_entry_occupancy.c`.

## Validation

The completed pass retained the full executable SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88` with
`MAKEFLAGS=-j2 make match`. `make basic-types`, `make global-usage`, and
`make check-global-usage` also passed after the final source changes.
