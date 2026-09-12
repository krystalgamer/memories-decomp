# What the password overlay does to the save block

Derived from matched overlay code, cross-checked against the save map in
[`../research/the-game.md`](../research/the-game.md) §4.5 and §12. Every
address below is read from a function that reproduces the retail bytes
exactly; the interpretation is flagged where it goes beyond that.

## `NameEntry_Main` clears and seeds the new-save workspace

The function does five things in order:

1. `Util_FillMemory(D_801D0000, 0, 0x3000)` (`0x80035748`) — clears
   `0x801D0000`–`0x801D3000`.
2. `printf("SaveLoadBuf add = 0x%x size = 0x%x\n", D_801D0000, 0x3000)` —
   logs the cleared buffer's address and size. The 36-byte format string is
   emitted at overlay address `0x80168090`; it is not source data for the
   buffer.
3. Runs `NameEntry_Init`, then advances the normal frame update and `rand`
   until `NameEntry_PollCompletion` reports that name entry is finished.
4. Calls `NameEntry_BuildStarterDeck`.
5. Stamps a non-zero word at `0x801D0534`, exactly `0x334` bytes after
   `gDuel_awPlayerDeck` (`0x801D0200`), retrying until it is non-zero.

The cleared range is much wider than the live persistent state. No template
is copied by the diagnostic call; the subsequent name-entry and starter-deck
steps populate the new save after the workspace has been zeroed.
`SaveData_RequestWrite` later copies exactly `0x680` bytes beginning at
`gDuel_awPlayerDeck`, so the persisted state occupies
`0x801D0200-0x801D087F`. The surrounding `0x801D0000-0x801D2FFF` overlay
workspace must not be interpreted as the save-file size; the separate header
and duplicate-state staging layout is documented in
[`../memory-card-runtime.md`](../memory-card-runtime.md#save-payload-staging).

## Shared live-workspace contract

[`save_data.h`](../../src/game/save_data.h) owns `D_801D0000` and the
independently labeled chest, player-name and Free Duel record interiors.
The live `SaveDataWorkspace.state` starts at `+0x200`, while `SaveDataState`
remains a prefix of the persisted block, not a replacement allocation:

| State offset | Workspace offset | Observed view and evidence |
|---|---|---|
| `0x000` | `0x200` | Forty `u16` deck IDs; starter-deck generation writes them and `func_8002C4DC` scans them. |
| `0x050` | `0x250` | 722 `u8` card quantities; `library_runtime.c` walks `gLibrary_abCardChest`, and `func_8002C518` reads the same bytes as `card_quantities[id - 1]`. Its Library caller supplies IDs 1 through 722. |
| `0x334` | `0x534` | Signed 32-bit duelist code; the name-entry writer now uses the same field as save validation. |
| `0x40C` | `0x60C` | Twelve SJIS name bytes; the name-entry checksum now uses the field, while `name_entry_runtime.c` retains the `gSaveData_aPlayerNameSjis` label. |
| `0x418` | `0x618` | 256 packed flag bytes; `Campaign_TestStoryFlag` indexes `campaign_flags` using the established encoded-ID masks. |
| `0x51C` | `0x71C` | Forty four-byte Free Duel records, including slot zero; `FreeDuel_Init` updates the independently labeled `gFreeDuel_aDuelistRecords`, and `FreeDuel_PlaceCursor` reads through the workspace base. |
| `0x5DC` | `0x7DC` | Scene byte plus the following byte, written together by the save prompt; runtime restore reads only the scene byte. |
| `0x5E0` | `0x7E0` | Unsigned starchip balance; the password shop's decrement now uses `state.starchips` instead of word index 504. |

The four unknown bytes at `+0x518` and the gap after the Free Duel records
remain unnamed padding. Compile-time assertions fix the newly exposed
offsets, record stride, workspace state offset and unchanged `0x5E4` state
prefix size. Neither this prefix nor the `0x7E4` workspace view claims the
full `0x680` persisted-state or `0x3000` cleared-workspace extent.

`SaveDataDuelistRecord` has named unsigned win/loss fields and a two-element
halfword array for the outcome cursor. The renderer explicitly casts each
counter to `s16`, preserving its signed loads even though the updater loads
and stores unsigned halfwords and saturates through a signed comparison.
The array view lets that updater advance from wins to losses without
stepping outside a scalar member. No counter label is redirected to an
offset from another symbol.

All eleven former private declarations in six resident and four overlay
translation units now use the shared header. The eight workspace-base users
are the story-flag tester/updater, deck lookup, effect-object pool helper,
save prompt, Free Duel screen, password shop and name-entry entry point.
Library and name-entry runtime supply the other two units. No integrated
candidate directly declares these symbols; all nineteen candidate object
fingerprints and their canonical dependency contracts remain unchanged.

### Measured addressing boundaries

The default workspace declaration is an incomplete byte array. The save
prompt in `func_8002EE94` selects `SAVE_DATA_WORKSPACE_AS_HALFWORDS` to keep
its incomplete `s16` array. Its two stores use the shared header/state
offsets but intentionally still write both bytes at `+0x7DC`. Replacing
them with a casted scalar field store made GCC fold away the base-address
`addiu`, changed register allocation and shortened the resident image by
four bytes; the guarded array retains the original instructions.

`func_8002C570` retains its byte-offset API and two-stage base calculation.
Its displacement is now derived from the card-quantity layout, but no caller
establishes that its argument is a bounded one-based card ID. Moving the
one-based bias into the base pointer changed the `D_801D0000` relocation
addend from zero to minus one and the byte-load displacement from `0x24F` to
`0x250`; that spelling was rejected. `Library_UpdateCardUsedFlag` likewise
retains its existing byte cursor, encoded requests and register assignments.

The resident image and all five complete overlays remain byte-identical.
These are declaration and access-view changes only: the storage remains
generated, integer interfaces remain integers, and no semantic symbol rename,
new register pin, compiler-profile change or C data mapping is implied.

## The stamped word and its source

| address | how it is formed |
|---|---|
| `0x801D0534` | `(D_8009B09C << 8) | checksum`, rewritten until non-zero |
| `gSaveData_aPlayerNameSjis` (`0x801D060C`–`0x801D0617`) | the twelve bytes XORed together to make `checksum` |

Two things line up with the documented map and are worth recording:

- The twelve XORed bytes are the player-name field at state offset `+0x40C`.
  They end at `0x801D0618`, exactly where the flag array begins, so the
  checksum covers the complete 12-byte name storage immediately preceding the
  flags.
- The stamped word is state offset `+0x334`. Matching
  `SaveData_HasSameDuelistCode` compares that 32-bit field between two loaded
  saves, establishing it as the duelist code used to reject a save competing
  or trading with a copy of itself.

The retry therefore guarantees that every newly initialized save receives a
nonzero duelist code. Its value combines timing/RNG state with all 12 bytes of
the name field; it is not a pointer to, or copy of, the overlay format string.

## Password use is a bitfield, not a counter

The earlier §4.5 wording called `0x801D0534 + 0x164` a password-use
"counter". That expression resolves to `0x801D0698`, the beginning of the
used-password part of the shared flag bank in §1. Individual cards use flag
`0x400 + card`; this records whether each password was used, not a numeric
purchase tally. The primary description now calls it a bitfield. The
GameShark addresses and their original code listings are unchanged.

## Shared flag-bank encoding

The resident flag accessors mask an encoded ID with `CAMPAIGN_FLAG_ID_MASK`
(`0x7FF`), select its byte using `CAMPAIGN_FLAG_BYTE_SHIFT` (`3`), and select
the bit with `CAMPAIGN_FLAG_BYTE_HIGH_BIT` (`0x80`) shifted right by the low
three ID bits. The bank therefore contains 2048 MSB-first flags in 256 bytes.
Its `CAMPAIGN_FLAG_BANK_OFFSET` (`0x618`) is relative to `D_801D0000`, placing
it at `0x801D0618`; relative to the persisted state at `0x801D0200`, the same
bank starts at `+0x418`. The existing two-stage address calculation and
set/clear polarity are preserved.

The update request is an encoded integer, not a pointer to flag storage.
`campaign_flags.h` declares `Library_UpdateCardUsedFlag(s32)` once and is
included by its definition and all current callers. The older pointer-shaped
call in `func_80019CC8` retains its byte-address arithmetic but explicitly
converts the resulting value at this integer interface. Other callers keep
their existing IDs and modifier masks.

The established function name is retained; it does not restrict the helper
to library flags. Password-use, event-script and text-control callers all
use the same bank and request encoding.

## Request modifiers and return values

The matching
[`Campaign_TestStoryFlag`](../../src/game/campaign_test_story_flag.c) and the
candidate
[`Library_UpdateCardUsedFlag`](../../src/candidates/func_8002CCE4.c)
access that same bank. Let `m` be the selected MSB-first mask:

| Request | Test result | Update action |
|---|---|---|
| `id` | `0` when clear, otherwise `m` | Set the bit |
| `0x8000 \| id` | `1` when clear, otherwise `0` | Clear the bit |

The tester never writes. Its ordinary result is not normalized: a set flag
with low ID bits zero returns `0x80`, whereas one with low bits seven returns
`1`. `CAMPAIGN_FLAG_CLEAR_MODIFIER` selects an inverted Boolean predicate for
the tester and a clear operation for the updater; it is not a stored flag bit.

The event-script handler
[`Script_OpStoryFlag`](../../src/game/func_8002E918.c) and text handler
[`Text_HandleCampaignFlagCommand`](../../src/game/text_control_commands.c)
interprets
`CAMPAIGN_FLAG_COMMAND_WRITE` (`0x4000`) before calling either helper.
Write commands pass `command & 0xBFFF`, retaining the clear modifier:
`0x4000 | id` sets and `0xC000 | id` clears. A test command instead evaluates
the requested predicate and may use a following jump target. The command
selector does not make a direct call to the tester mutate the bank.

Matching [`func_8002CD48`](../../src/game/func_8002CD48.c) saves the test
result, invokes the updater only when that result is zero, and returns the
saved result. Nonzero therefore means the requested state was already
satisfied; zero is returned after applying a needed change, not as an update
failure or the new bit value.

## The duelist-code address is used as a structure base twice

Two separate fields in the map are expressed as offsets from `0x801D0534`,
each with a different offset:

| field | as written | resolves to |
|---|---|---|
| password use, §4.5 | `0x801D0534 + 0x164` | `0x801D0698` |
| duelist win/loss records, §12 | `0x801D0534 + 0x1EC` | `0x801D0720` |

Both are given alongside a plain absolute address elsewhere, so neither
needed to be written that way. One such expression would be incidental
phrasing; two independent offsets from the same base is a much better
argument that whoever wrote the map was reading a structure rooted there.

Matching `SaveData_HasSameDuelistCode` now establishes the word at
`0x801D0534` as the duelist code. The two relative expressions still indicate
that the external map was reading a larger structure rooted at that field,
rather than two unrelated pieces of address arithmetic.

Those relative expressions describe the external map's notation. The
resident flag helpers themselves use `D_801D0000 + 0x618`, not the
duelist-code field as their access base.
