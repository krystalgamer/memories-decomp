# AI Subsystem Structures

`src/game/ai.h` contains shared layouts corroborated across multiple matching
functions. The field names are mechanical project names, not recovered
original symbols.

Scalar AI constants live in the type-free `src/game/ai_constants.h`, re-exported
by `ai.h`. Legacy local state/card views can use those constants without
changing their external declarations.

## `AiActiveCard`

Size: `0x0C`

| Offset | Field | Evidence |
|---|---|---|
| `0x00` | `card_id` | Fusion, equip, deck-size, face-state, and set-query handlers treat zero as an empty slot and nonzero values as card IDs. |
| `0x06` | `flags` | Equip selection, face-state classification, and move-card handlers test/set `0x1000` and `0x4000`. |
| `0x08` | `card_type` | Monster/type searches compare the signed byte against card-type values. |

The structure replaces private 12-byte definitions in the merged fusion,
set-query, card-info, and state-operation units and in existing card-state
handlers.

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
| `0x9C`-`0xD3` | fusion search state | `Ai_CompleteFusion` tracks count, depth, best score/path, and visited entries. |

Some translation units still declare `gAiScript_State` as a byte or halfword
array. This is intentional where GCC must retain a base-symbol relocation plus
an instruction offset instead of folding the field offset into the symbol.
The shared structure remains the authoritative layout while the local extern
controls code generation.

The selected combo buffer at `AI_SCRIPT_COMBO_BYTE_OFFSET` (`0x38`) is not
the in-progress path at `AI_SCRIPT_FUSION_PATH_BYTE_OFFSET` (`0xA4`).
Current depth (`0xA2`) and best depth (`0xA3`) likewise have distinct byte
offsets tied to the existing structure. The fusion copier retains its
inclusive best-depth bound and following zero terminator; combo readers keep
their existing bounds and filtering. Naming the offsets does not replace
raw views, clamp malformed depths, or turn the five-entry combo-start scan
into a six-entry scan.

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
