# AI Subsystem Structures

`src/game/ai.h` contains shared layouts corroborated across multiple matching
functions. The field names are mechanical project names, not recovered
original symbols.

Scalar AI constants live in the type-free `src/game/ai_constants.h`, re-exported
by `ai.h`. Legacy local state/card views can use those constants without
changing their external declarations.

## `AiActiveCard`

Size: `0x0C` (`AI_ACTIVE_CARD_RECORD_SIZE`)

| Offset | Field | Evidence |
|---|---|---|
| `0x00` | `card_id` | Fusion, equip, deck-size, face-state, and set-query handlers treat zero as an empty slot and nonzero values as card IDs. |
| `0x02` | `attack` | Snapshot field entries use the low half of `Duel_CalcCardStats`; hand/deck entries unpack and scale the attack value from the card table. |
| `0x04` | `defense` | Snapshot field entries use the high half of `Duel_CalcCardStats`; hand/deck entries unpack and scale the defense value from the card table. |
| `0x06` | `flags` | Matchup, equip, face-state, and move-card handlers use defense-position `0x0800`, face-down `0x1000`, and used-this-turn `0x4000` masks. |
| `0x08` | `card_type` | Monster/type searches compare the signed byte against card-type values. |
| `0x09` | `guardian_star` | Matchup searches pass it as the first guardian-star operand. |
| `0x0A` | `guardian_star_2` | `Ai_ExportSideSnapshot` exports the second packed guardian-star value for hand and deck cards. |
| `0x0B` | `deck_index` | `Ai_ExportSideSnapshot` copies the source `DuelDeckCardRecord.deck_index` for remaining deck cards. |

The structure replaces private 12-byte definitions in the merged fusion,
set-query, card-info, and state-operation units and in existing card-state
handlers.

`Ai_RefreshBoardSnapshot` spaces its selected-side and opposite-side snapshot destinations
by `AI_ACTIVE_CARD_SIDE_BYTE_STRIDE` (`0x294`), or 55 records. This is storage
spacing, not a fixed exporter count: `Ai_ExportSideSnapshot` emits two field rows,
compacts nonnegative hand-slot entries, appends the remaining deck entries, and
writes a zero card-ID terminator. It writes `AiActiveCard` directly; the
former private view had the same record size and field offsets. Its field-card
source object is a `DuelCardDisplayObject`, whose `field_68` is populated from
the same packed card-type bits. Raw address calculations use integer-valued
byte constants rather than `sizeof`-derived strides, keeping their arithmetic
types unchanged; no loop bound or clearing behavior changes.

### Snapshot producers and storage alias

[`ai_board_snapshot.h`](../src/game/ai_board_snapshot.h) is the shared home
for the side exporter, the board refresh and their typed interior array
alias. It replaces the address-named exporter header, the refresh-only
`duel_card_state_helpers.h` and the `D_801AB00C_VISIBLE` declaration arm
in `unmatched.h`.

`gAiSnapshot_aOwnCards` is the former `D_801AB00C`, exactly one
`AiActiveCard` above `gDuel_aActiveCards`: `0x801AB00C` versus
`0x801AB000`, or `0x801BB00C` versus `0x801BB000` in Japan. It is the
first own-monster snapshot slot, not a separate allocation or a stored
pointer. The incomplete array declaration and the direct alias relocation
remain; no use is rewritten to a base-plus-one expression and no array
capacity is invented.

[`Ai_ExportSideSnapshot`](../src/game/ai_turn_action.c) takes an
output cursor and a physical duel-side index. It writes four consecutive
segments in their original order:

| Segment | Source and output behavior |
|---|---|
| Monster row | Five records starting at side-local card-record offset five; each output advances even for an empty slot. Occupied entries copy effective stats, flags, object type and the selected guardian star. |
| Magic/trap row | Five records starting at side-local offset ten, using the same occupied/empty behavior. |
| Hand | The five `hand[]` values are read in order. Negative values are skipped; nonnegative values index the existing staged card-ID table. Valid entries are compacted and use packed base stats, zero flags and both guardian stars. |
| Remaining deck | Starts at that side's `deck_draw_cursor`, uses the side-strided deck records, preserves draw order and copies the source `deck_index` byte. No entries are written when the cursor is already at least `DECK_SIZE`. |

This is not a whole-buffer reset. An empty field entry and the final
terminator write **only `card_id = 0`**. Occupied field entries do not
initialize `guardian_star_2` or `deck_index`; hand entries do not initialize
`deck_index`. Other bytes therefore retain their previous contents where
the original producer did not store them. No `memset`, extra guardian-star
store, range clamp, count argument or terminator-record clear is added.
All existing signedness, packed-stat expressions, integer address arithmetic
and cursor/counter increment ordering remain.

`Ai_RefreshBoardSnapshot` calls the exporter for `gDuel_bActiveSide` at the
interior alias, then for `gDuel_bActiveSide ^ 1` at that alias plus 55 records.
It does not clear slot zero, the gap between occupied exports, or the unused
trailing bytes. Both scene handlers call it before `AiScript_Init`, with
the hand or field script respectively. The hand-script search window may
include remaining-deck entries; rebuilding the snapshot does not limit
the VM's existing range semantics.

The exporter source and Japanese wrapper have semantic file names, but
their source grouping and compiler profiles remain fixed. In particular,
the refresh still shares its `gcc_2_8_1_g8` object with
`DuelCard_DecodeTaggedSlotIndex`; the side exporter remains its own
`gcc_2_8_1_g0_split` unit. The decoder is not moved, exposed through the
new header, renamed or changed.

| Binding | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| `Ai_ExportSideSnapshot` | `0x80027DF8` | `0x80027C0C` | `0x80027D3C` | `0x80027E80` |
| `Ai_RefreshBoardSnapshot` | `0x80028220` | `0x80028034` | `0x80028164` | `0x800282A8` |
| `gAiSnapshot_aOwnCards` | `0x801AB00C` | `0x801BB00C` | `0x801AB00C` | `0x801AB00C` |

The Japanese refresh previously kept the native address-based
`func_80028034` spelling through three wrapper macros. The shared semantic
name now binds directly to that original native address; only those
obsolete macros are removed. All inventories retain addresses, sizes,
status, row order, ownership and notes. Source-path changes retain every
profile and grouped-object boundary.

Size and offset contracts in `ai.h` cover all eight `AiActiveCard` fields.
The shared-header stage preserves all eight complete exporter, refresh and
scene-handler compiler/MASPSX artifacts raw-identically. The named stage
compares those complete artifacts after reversing only the two function
names, the interior alias and the exporter's `.file` path. Full executable
matching remains mandatory; regional/cloud validation stays on the PR.

`AiScript_FindKiller` and `AiScript_FindBestAttack` both walk `AiActiveCard`
records and read the `+0x06` halfword as the typed `flags` member, tested
against the shared `DUEL_CARD_FLAG_*` masks. `AiScript_FindKiller` skips empty
and used-this-turn entries among the own monsters. `AiScript_FindBestAttack`'s
attacker filter also excludes used-this-turn entries; its target filters separately
exclude used and defense-position entries, and hide face-down targets for any
nonzero scripted visibility value. That condition differs from the strongest/weakest
searches' `hide_face_down == 1`; the predicates are not consolidated.

### Slot map

`Ai_GetCardRange` is the game's own table of the six slot ranges, and it is
the source for the `AI_SLOT_*` constants in `ai_constants.h`. Ranges are
inclusive and the hand's upper bound is the current hand size:

| Range kind | Slots | Constant |
|---|---|---|
| `0`, `1` | `1..5` | `AI_SLOT_OWN_MONSTER_FIRST` |
| `2`, `3` | `6..10` | `AI_SLOT_OWN_SPELL_FIRST` |
| `4` | `11..` | `AI_SLOT_OWN_HAND_FIRST` |
| `5`, `6` | `56..60` | `AI_SLOT_OPPONENT_MONSTER_FIRST` |
| `7`, `8` | `61..65` | `AI_SLOT_OPPONENT_SPELL_FIRST` |
| `9` | `66..70` | `AI_SLOT_OPPONENT_HAND_FIRST` |

Slot `0` is the zero entry the field searches start from, so an index of `0`
doubles as "nothing found" (`AI_SLOT_NONE`). Each opponent range sits
`AI_ACTIVE_CARD_SIDE_SLOT_STRIDE` (55 records, the
`AI_ACTIVE_CARD_SIDE_BYTE_STRIDE` of `0x294` divided by the record size) past
its player counterpart, so all six bases derive from
`AI_SLOT_OWN_MONSTER_FIRST` and the row width. `AiScript_LoadDeckSize` scans
`AI_SLOT_OWN_HAND_FIRST` up to `AI_SLOT_OPPONENT_MONSTER_FIRST`, that is the
player's hand and deck together, and subtracts `HAND_SIZE`.

`Ai_GetWinningCardRange` uses the same slot bases but **a different kind
encoding** (`0,1` own monsters, `2` own hand, `3,4,5` opponent monsters, `6`
opponent hand). The kind codes themselves are deliberately left as literals
for that reason.

The paired-field search uses `DUEL_SIDE_COUNT` rows of
`DUEL_FIELD_ROW_SIZE` taken marks, matching the two five-card ranges
`1..5` and `56..60`. Primary-field scans reuse that row size; one-based
exclusive cutoffs retain `DUEL_FIELD_ROW_SIZE + 1` (`6`). This slot cutoff
is distinct from the six-byte `AI_SCRIPT_COMBO_CARD_COUNT` storage.
Zero-entry sentinel behavior, strict comparisons, slot bases, hand ranges,
and the existing cursor/loop-control forms remain unchanged.

## `AiScriptState`

Size: `0xD4`, matching the initialization clear in `AiScript_Init`.

Established regions:

| Offset | Field | Evidence |
|---|---|---|
| `0x00` | `enabled` | Set to one when initialization receives a null script pointer. |
| `0x04` | `script_base` | Added to relative jump/call operands. |
| `0x08` | `script_cursor` | Advanced by byte/short readers and assigned by jump/call handlers. |
| `0x0C` | `previous_cursor` | Receives the current cursor before each opcode dispatch. |
| `0x14` | `return_depth` | Bounded to eight by `AiScript_Call`. |
| `0x18` | `return_stack[8]` | Stores script cursors for call/return handling. |
| `0x38` | `combo_cards[6]` | Read by combo-start selection before the card set begins. |
| `0x3E` | `card_set[32]` | Add/clear/card-membership handlers use 32 16-bit entries. |
| `0x7E` | `type_set[25]` | Add/clear/type-membership handlers use 25 byte entries. |
| `0x9C` | `fusion_count` | Configured hand/fusion search count returned by `Ai_GetHandSearchCount`, including lookahead where configured; bounds the existing clear and scan loops. |
| `0x9D` | `fusion_limit` | Operand-derived depth limit used by the fusion search. |
| `0x9E` | `fusion_set` | Set selector passed to `Ai_IsCardInSets` to exclude candidates. |
| `0xA0` | `fusion_best_stat` | Halfword score compared against both attack and defense. |
| `0xA2` | `fusion_depth` | Current index into the in-progress fusion path. |
| `0xA3` | `fusion_best_depth` | Inclusive bound for copying and reading the selected combo. |
| `0xA4` | `fusion_path[6]` | In-progress path, separate from the selected combo. |
| `0xAA` | `fusion_used[0x2A]` | Temporary marks accessed with the existing loop index, not the stored active-card slot ID. |

Some translation units still declare `gAiScript_State` as a byte or halfword
array. This is intentional where GCC must retain a base-symbol relocation plus
an instruction offset instead of folding the field offset into the symbol.
The shared structure remains the authoritative layout while the local extern
controls code generation.

The private `ByteReader` in `ai_script_find_best_attack.c` remains a prefix view. Its pointer
array shares `AI_SCRIPT_RETURN_STACK_COUNT` with `AiScript_Call`, and a size
assertion keeps the prefix ending at `AI_SCRIPT_COMBO_BYTE_OFFSET`. Its
unrelated `unkC[8]` remains byte padding, not eight return-address slots.

The selected combo buffer at `AI_SCRIPT_COMBO_BYTE_OFFSET` (`0x38`) is not
the in-progress path at `AI_SCRIPT_FUSION_PATH_BYTE_OFFSET` (`0xA4`).
Current depth (`0xA2`) and best depth (`0xA3`) likewise have distinct byte
offsets tied to the existing structure. The fusion copier retains its
inclusive best-depth bound and following zero terminator; combo readers keep
their existing bounds and filtering. Naming the offsets does not replace
raw views, clamp malformed depths, or turn the five-entry combo-start scan
into a six-entry scan.

The remaining fusion count, limit, set-selector, best-stat, and used-entry
offsets also have shared byte-offset constants and layout assertions.
Clearing and scanning remain bounded by the stored hand count, not the size
of `fusion_used`. Used-entry marks keep each loop's original index, including
the field-start branch before its recursive hand scan. The signed halfword
initialization views, unsigned score views, and separate `D_800F5C88` accesses
in `AiScript_EvaluateFusion` remain unchanged.

The raw set helpers now name their offset units explicitly.
`AI_SCRIPT_CARD_SET_BYTE_OFFSET` is `0x3E`, while
`AI_SCRIPT_CARD_SET_HALFWORD_OFFSET` is `0x1F` because each card-set entry is
two bytes. The latter is a base offset, not the coincidentally equal
`AI_SCRIPT_CARD_SET_COUNT - 1`. Type-set offsets remain byte-based at `0x7E`.
Compile-time assertions tie these constants to the shared structure without
replacing the raw views or the reverse-order clear loops.

Type-set entries store the requested type plus
`AI_SCRIPT_TYPE_SET_ENCODING_BIAS` (`1`), and membership tests subtract that
bias. `AI_SCRIPT_TYPE_SET_EMPTY` (`0`) and `AI_SCRIPT_CARD_SET_EMPTY` (`0`)
name the separate byte/halfword empty markers; narrowing, duplicate handling,
and invalid-input behavior are unchanged.

Membership does not explicitly skip empty entries. A queried card ID of zero
can match an empty card-set entry, and a signed active-card type of `-1` can
match a zero type-set entry after the bias is subtracted. The names preserve
these cases rather than adding validity checks or treating either routine as
a general validated set implementation.

### Interpreter dispatch and yielding

Matching [`AiScript_Run`](../src/game/ai_script_vm.c) (`0x80070650`) copies
`script_cursor` to `previous_cursor` before reading each opcode. It calls
`gAiScript_apfnCommand[opcode]`, then reloads that table entry to classify
the completed command. The interpreter does not use a handler return value.

| Handler identity after dispatch | Interpreter return |
|---|---:|
| `AiScript_EndHand` | `1` |
| `AiScript_EndField` | `3` |
| `AiScript_PlayFieldCard` | `2` |

Those three tests precede the timing query. After any other completed
handler, `VSync(1) < 0xF0` continues the loop; a result at or above `0xF0`
(240) returns `0`. The retail instructions at `0x800706B8..0x800706EC`
corroborate the terminal precedence, mode-1 call, threshold, and zero return.
There is no entry-time check: at least one valid instruction executes before
the first timing query. The query neither preempts a handler nor imposes a
fixed instruction quota.

The retail SDK `VSync` (`0x80074170`) explains why this is a query rather
than a wait-for-VBlank call. Its mode-1 branch at `0x800741E4` goes to the
return at `0x800742D0`, bypassing both calls to the wait helper
`v_wait` and the saved-baseline update at `0x800742B4`. The returned
value is `(stable_counter - D_80091998) & 0xFFFF`; the executable stores
`0x1F801110` in the counter pointer `D_80091994`. The sampling loop requires
two consecutive counter reads to agree.

The waiting path later refreshes `D_80091998`; repeated mode-1 queries do
not. `AiScript_Run` does not initialize that shared baseline, so the
threshold is not a fresh elapsed-time allowance measured from each
interpreter entry. These source and retail-instruction observations do not
establish a fixed 1/16-second scheduling cadence, a strict handler runtime
limit, or the number of commands executed per pass. No new human trace or
decision-time measurement is implied.

## `AiSelection`

`gAi_Selection` at `0x800EAE88` is one `0x0C`-byte object. Its declaration
now lives unconditionally in `ai.h`, rather than selecting incompatible
`AiSelection` and incomplete `u8[]` declarations through `unmatched.h`.
The shared union exposes the original signed `AiSelectionFields` as
`fields` and the same twelve bytes as unsigned `bytes`. Neither view
allocates storage or introduces a second pending action. The generated
backing object, symbol addresses and compiler profiles remain unchanged.

| Offset | Field view | Byte-view evidence |
|---|---|---|
| `0x00` | `result` | Turn selectors write the first hand/field choice; `AiScript_PlayFaceUp` writes the first combo entry. |
| `0x01` | `field1` | Fusion/equip selectors write or clear the second choice; the hand-scene handler tests the unsigned byte to decide whether to stack a card. |
| `0x02` | `field_02` | The second fusion hit clears it; combo producers also write this byte. |
| `0x03..0x05` | `pad_03` | These are not globally unused padding: the five-entry combo writes `0x03` and `0x04`, then writes its terminator at `0x05`. |
| `0x06` | `value` | Turn/equip/trap selectors write a destination slot; the hand-scene handler reads it unsigned before subtracting the row base. |
| `0x07` | `zero` | The action selectors initialize this byte; `AiScript_LoadGuardianStarChoice` retains its standalone alias store. |
| `0x08` | `random` | Turn selection writes `rand() & 1` or a fixed flag; the hand-scene handler reads the unsigned byte. |
| `0x09` | `field_09` | The field-play opcode and per-turn decision write the acting field slot; field-scene tests retain unsigned reads. |
| `0x0A` | `field_0A` | The field-play opcode writes its category/target value, including the existing `0xFF` to `0x3A` conversion. |
| `0x0B` | `field_0B` | The opcode stores its third value; field-scene dispatch tests this byte with the category/target byte. |

The migrated producers and consumers include the script VM initialization,
action/state opcodes, fusion searches, turn selectors, equip/trap selectors,
`Duel_ApplyAiHandSelection` and both hand/field scene handlers. Local record
pointers now point to `gAi_Selection.fields`; byte cursors and indexed reads
use `gAi_Selection.bytes`. Existing signed field reads, unsigned byte reads,
narrowing stores, write order, reverse clearing and inclusive combo-copy
bounds are preserved. The twelve-byte view does not clamp malformed script
indices or change the five-entry material scan into a twelve-entry scan.

Interior address aliases are deliberate exceptions, not separate state.
`tent_AiSelectionRandom` remains a standalone scalar for the three opcode
accesses whose small-data relocation depends on that symbol.
`D_800EAE8E`, `D_800EAE8F`, `D_800EAE91` and `D_800EAE92` likewise retain
their existing declarations and use sites. In particular, the scene handlers
keep base-symbol-plus-offset byte reads, and the field opcode keeps its
standalone `D_800EAE92 = 0` store. Unifying the backing object's type does
not force those independently matched accesses through a field lvalue.

Compile-time contracts assert the union and field-record sizes and every
live field offset. All eleven affected complete compiler/MASPSX units
reproduce the pre-migration output after removing only the
`gAi_Selection, 12` external-size directive where the formerly incomplete
array emitted none (or placed it later); fourteen of the twenty-two
artifacts are raw-identical. This normalization does not change instructions,
labels, relocations or data. Full executable matching remains the acceptance
gate; regional builds use their existing wrappers and PR validation.

## Script-side duel-state queries

The script handlers read the canonical `0x20`-byte `DuelSideState` in
[`duel_side_state.h`](../src/game/duel_side_state.h), not a separate
AI-only duelist record.

| Offset | Field | Evidence |
|---|---|---|
| `0x14` | `life_points` | `AiScript_LoadLifePoint` selects one of the two records. |
| `0x19` | `swords_turns_remaining` | `AiScript_TestSwordsActive` reads whether the selected side's Swords counter is nonzero. |

The two selectors are intentionally different: life points use `1 - i`,
while Swords uses `(gDuel_bActiveSide[0] ^ 1) ^ i`. Neither query normalizes its
scripted index, changes the counter, pins a card or modifies duel state.

## Deferred definitions

Several matching handlers still contain GCC inline assembly and private
three-word state views. They remain unchanged so this structural refactor does
not consume or obscure their remaining pure-C refinement work. Their offsets
agree with `AiScriptState`, and they can adopt the header when the inline
assembly is removed.
