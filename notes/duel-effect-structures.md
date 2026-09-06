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
symbol, `gInput_abRawPadBuffers`, begins 16 bytes later, so the intervening
bytes are not claimed by this type.

Verified entry fields are:

| Offset | Shared field | Evidence |
|---:|---|---|
| `0x11` | `flags_11` | bit `0x80` gates exact scans; target `func_8003741C` and `DuelEffect_ProcessEntries` agree |
| `0x12` | `field_12` | compared with the requested marker plus one |
| `0x13` | `field_13` | active-value test/write in the bounded scan pair |
| `0x15` | `field_15` | second byte written by `func_800373C8` |
| `0x18` | `field_18` | range write in `func_8003B6AC`; `DuelEffect_UpdateObjectLayout` reads it |

Exact C for `DuelEffect_ProcessEntries` (`0x80039D64`) copies complete
seven-word (`0x1C`) records and advances both pointers by `0x1C`,
independently confirming that the named bytes belong to one record rather
than parallel arrays. Unchiga's same-address sketches and GMS pseudocode
corroborate these accesses but do not determine the shared types.

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

## Runtime state and channel lifecycle

Matching `DuelEffect_UpdateState` defines the global dispatcher lifecycle.
State zero is inactive. On the first update of a nonzero state, the function
copies its value to the callback index, sets bit `0x80` as the initialized
latch, clears the companion state byte, and reports active. A later bit
`0x40` cancels and clears the state; otherwise the latched index dispatches
through `D_80090B3C`.

`DuelEffect_CreateChannel` resets the current dialog choice to `-1`, creates a
`DuelEffectChannel` through `TextBox_Create` using the low 15 bits of its
request value, and stamps `field_59` from the shared sequence byte minus one.
A nonzero setup argument adds flags `0x1008`; otherwise a high request bit
selects the alternate `func_80039A14` setup path before the channel is
returned.

`DuelEffect_MarkObjectIfActive` scans the separate three-entry table at
`D_800EB010` from the last entry backward. It sets object flag `0x2` when any
entry has a nonnegative signed marker, and leaves the object unchanged when
all three markers are negative. `DuelEffect_InitEntryDefaultFlags` is the
zero-flags wrapper around `DuelEffect_InitEntry`, making the default entry
construction path explicit without broadening either shared structure.

## Deferred and exact-code exceptions

Matching-C functions implemented with inline assembly remain unchanged:

- `Main_RunCredits` references `D_800EB0F8`;
- `func_80030998` references both globals through explicit relocations.

`DuelEffect_UpdateObjectLayout` is also matching C, but it retains a minimal
local `EffectEntry` view and is not counted among the six shared-header users.
`DuelEffect_ProcessEntries` likewise remains on its local complete-record view
so its accepted source does not broaden the shared declaration.
`func_8003DA40` is now matching C as well, but retains a local `Rec64` channel
view and byte-offset accesses, so it is not counted among the seven
`DuelEffectChannel` users.
`func_8003D74C` is likewise matching C but keeps a raw byte view with the
verified 100-byte channel stride while coordinating the duel-intro card
reveal.

The changing assembly-user lists for `D_800EB0F8` and `D_800EB288` are not
duplicated here. [`global-usage.csv`](global-usage.csv) is the generated
authority: filter by `global_address` and a `function_status` other than
`matching_c`. Regenerate it with `make global-usage` whenever source or symbol
changes affect these globals.

`DuelEffect_HasActiveEntry` and `func_800373C8` in
`duel_effect_entry_control.c` are the sole raw-view exceptions among migrated
pure-C users. Both include the shared header and use its typed global
declaration, but retain byte-pointer field expressions. Replacing those
expressions with direct `DuelEffectEntry` member access shortened the two
exact functions by eight bytes in total and failed the resident text-size
check. The retained expressions preserve GCC 2.8.1's address-adjustment and
register schedule; they are not a competing layout declaration. The third
function in that grouped source, `func_8003741C`, uses the shared
`DuelEffectEntry` fields directly.

The shared field syntax was exact-tested successfully in the other migrated
users, including direct indexing in `DuelEffect_InitEntry` and both marker
functions in `duel_effect_entry_occupancy.c`.

## Validation

The completed pass retained the full executable SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88` with
`MAKEFLAGS=-j2 make match`. `make basic-types`, `make global-usage`, and
`make check-global-usage` also passed after the final source changes.
