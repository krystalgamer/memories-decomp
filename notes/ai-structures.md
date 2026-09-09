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
| `0x06` | `flags` | Matchup, equip, face-state, and move-card handlers use defense-position `0x0800`, face-down `0x1000`, and used-this-turn `0x4000` masks. |
| `0x08` | `card_type` | Monster/type searches compare the signed byte against card-type values. |

The structure replaces private 12-byte definitions in the merged fusion,
set-query, card-info, and state-operation units and in existing card-state
handlers.

`func_80028220` spaces its selected-side and opposite-side snapshot destinations
by `AI_ACTIVE_CARD_SIDE_BYTE_STRIDE` (`0x294`), or 55 records. This is storage
spacing, not a fixed exporter count: `func_80027DF8` emits two field rows,
compacts nonnegative hand-slot entries, appends the remaining deck entries, and writes a
zero card-ID terminator. Its private `LocalEnt` view and the shared
`AiActiveCard` view both have record-size assertions. Raw address calculations
use integer-valued byte constants rather than `sizeof`-derived strides, keeping
their arithmetic types unchanged; no loop bound or clearing behavior changes.

`AiScript_FindKiller` and `AiScript_FindBestAttack` retain raw halfword flag
reads at `+0x06` while reusing the shared `DUEL_CARD_FLAG_*` masks. The latter's
attacker filter excludes used-this-turn entries; its target filters separately
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
| `0x9C` | `fusion_count` | Hand-entry count returned by `Ai_GetHandSize`; bounds the existing clear and scan loops. |
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
`func_800742E8` and the saved-baseline update at `0x800742B4`. The returned
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

## `AiDuelistState`

Size: `0x20`

| Offset | Field | Evidence |
|---|---|---|
| `0x14` | `life_points` | `AiScript_LoadLifePoint` selects one of the two records. |
| `0x19` | `pinned` | `AiScript_TestPinned` tests the opposing record and selected index. |

## Deferred definitions

Several matching handlers still contain GCC inline assembly and private
three-word state views. They remain unchanged so this structural refactor does
not consume or obscure their remaining pure-C refinement work. Their offsets
agree with `AiScriptState`, and they can adopt the header when the inline
assembly is removed.
