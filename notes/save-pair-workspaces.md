# Paired save and trade workspace views

## The four identities

| Original provider | Canonical view | Established role |
|---|---|---|
| `D_801D1200` | `gSaveData_aPairWorkspace` | Base used for the first loaded save and for two-player duel/trade workspace accesses |
| `D_801D2200` | `gSaveData_aSecondPlayerState` | Independently labeled destination for the second loaded save |
| `D_801D160C` | `gSaveData_aFirstPlayerNameSjis` | First save's SJIS name source at workspace offset `0x40C` |
| `tent_TradeSaveBuffer` (`0x801D1880`) | `gSaveData_aTradeWriteStaging` | First trade write-staging window at workspace offset `0x680` |

All four declarations remain mutable incomplete `u8[]` views of external
storage. The names do not introduce an allocation, a complete array bound,
a `SaveDataState[]` declaration or an ownership/lifetime guarantee.
Each name is bound directly at its native address in every region; the
former `D_` providers and their alias lines are gone. The names keep
separate relocation identities.

## Load and duel consumers

`SaveData_UpdateLoadPair` requests `SAVE_DATA_STATE_SIZE` (`0x680`) bytes
into the first base, then requests the second save through its own
`gSaveData_aSecondPlayerState` label. The same second window is reached elsewhere as
base plus `TWO_PLAYER_SAVE_SLOT_STRIDE` (`0x1000`). Neither spelling is
rewritten to the other.

The existing identity comparison, message states, card-port change and
error returns are unchanged. `SaveData_UpdateDuelLoad` still requires
`DECK_SIZE` nonzero halfwords on each side before converting both names.
`Duel_InitScene` uses these windows for its existing negative-opponent
two-player path, publishes the two save pointers and passes their deck
views to `Duel_ShuffleBothDecks`; other paths and stale-pointer behavior
are not changed.

The name at `+0x40C` is the SJIS **source**, not glyph output.
`Text_SjisToGlyphCodes` receives it as argument two with the existing
six-character count, then receives source plus `0x1000` for the other
player. Its destinations remain separate `gText_aFirstPlayerNameGlyphs`
and `gText_aSecondPlayerNameGlyphs` views, documented in
[player-name-glyph-views.md](player-name-glyph-views.md). No encoding
validation, terminator rule, new extent or bounds check follows from
naming either the source or the outputs.

## Trade staging, not another full-save allocation

`MainMenu_RefreshTradeInventory` reads quantity bytes at
base plus slot times `0x1000` plus `SAVE_DATA_CARD_QUANTITIES_OFFSET`.
`MainMenu_UpdateTradeScreen` uses its existing 16-byte block casts to
stage `0x400` bytes per side in the slot-relative `+0x680` windows,
changes those staged quantities and calls `SaveData_RequestTradeWrite`.
After successful I/O its original copy loops publish the staged regions
back to the corresponding live windows.

`SaveData_RequestTradeWrite` stamps primary/secondary integrity at the
first staging address and staging plus `0x1000`, publishes the secondary
transfer cursor, and requests dialog selector `4` with
`TWO_PLAYER_SAVE_TRANSFER_SIZE` (`0x400`). These producer, seal and
transfer consumers support promoting the former tentative name.
The view is not an independent buffer allocation or a full `0x680`-byte
save serialization request. Copy order, overlap, signed/block views,
integrity calls, failure behavior and transfer length remain unchanged.

## Declaration and native boundaries

The narrow workspace header follows the existing `save_data.h` owner
in four root sources. The interior transfer-view header follows
`unmatched.h` only in the memory-card dialog unit. The old headers,
guards, regional wrappers and compiler profiles stay unchanged.

Actual compiler-expanded declarations and named-profile objects cover
15 source/profile contexts and 204 registrations. There are 25 active
imports: thirteen workspace-base imports and four each for the second
state, SJIS source and trade staging. Canonical declarations retain
thirteen additional unused views. Sixteen other original unused
declarations remain legacy-only, and six absent declaration/reference
cases remain absent; a header's presence does not fabricate an import.

Canonical aliases point to each original resident/Main Menu provider.
Existing numeric providers are retained, and automatic providers remain
symbolic without guessed tracked addresses. All existing test files,
ownership probes and behavioral controls remain unchanged; no new
harness, fixture, probe or testcase is added.

See [memory-card-runtime.md](memory-card-runtime.md) for the dialog state
machines and [fm-online.md](fm-online.md) for the parallel two-player
addresses.
