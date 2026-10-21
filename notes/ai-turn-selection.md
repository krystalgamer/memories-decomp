# AI turn-selection API and battle comparison

This is one static contract for the six-function run in
[`ai_turn_action.c`](../src/game/ai_turn_action.c), its
[`shared API`](../src/game/ai_turn_action.h), and the battle-strength helper
in [`duel_battle_stats.c`](../src/game/duel_battle_stats.c). Names describe
the matching code, not recovered original symbols or a claim that every
entry is called during normal play.

## Selection producers

| NA address | API | Existing behavior |
|---|---|---|
| `0x8002712C` | `Ai_SelectMagicPlay` | Collects active-side type-`CARD_TYPE_MAGIC` entries from records `0..4`, chooses the first, and writes a hand choice and destination starting at six. Returns one if the collection is empty, otherwise zero. |
| `0x80027228` | `Ai_SelectFusionPlay` | After its original random gate, tries hand monsters against the active field row with `Duel_CheckFusion`; with a free monster slot, also tries distinct hand entries against one another. Returns zero after writing a selection, one otherwise. |
| `0x80027508` | `Ai_SelectHandPlay` | Tries equip, trap, magic and fusion paths, then picks a random entry from the side's first five records and selects a monster-row or magic-row destination. Always returns zero. |
| `0x800279BC` | `Ai_SelectFieldPlay` | Writes pending bytes `9..11`: optional equip/type-`0x14` action, then a scan of grid slots `10..14` using the two favorable-target helpers and the existing random/give-up paths. |

The first five records are the hand entries: the hand-record lookup in
`duel_grid.h` maps them to `0..4` or `15..19`, while the collectors apply
the same `DUEL_CARD_SIDE_RECORD_COUNT` stride. Their generic historical
`Duel_CollectFieldCards*` names do not make every selected record a field
card. The grid-based final decision is a separate path.

These are not validated or optimal-play APIs. A full magic destination row
does not make `Ai_SelectMagicPlay` fail: it retains destination six and
returns zero. The fusion path keeps its unused interval draw, random early
rejection, first compatible pair and distinct-entry check. The general hand
path keeps its fallback to a random slot when the selected row is full, and
does not introduce card-ID, occupancy or capacity guards.

The field decision clears byte nine **before** testing bit `0x1000` of
`gDuel_wOutcomeFlags`. It preserves the Swords check, packed-word flag predicates,
record/grid arithmetic, action encodings, random draw order and original
give-up path. Renaming does not reinterpret `field_09`, `field_0A` or
`field_0B` globally or change their stores.

## Target selectors and canonical record boundary

`Ai_FindFavorableFieldTarget` (`0x8002778C`) and
`Ai_FindFavorableFaceUpFieldTarget` (`0x800278A0`) both take a
`DuelCardRecord *source`. Their caller already has exactly that record.
Its first word is the `object` pointer, and the returned object's byte
`+0x6A` is `DisplayObject.field_6A`, the same card-record index read by
the comparison and battle-resolution paths.

This replaces the selector-only `DuelSelectionSource` and
`DuelSelectionObject` prefixes and removes their caller casts. The source
record's object-offset and `0x1C`-stride contracts already live in
`duel_card.h`; the object index remains an unsigned byte. The argument is
not an object pointer itself, and no new record or display-object storage is
allocated. The old `duel_card_selection.h` is replaced by `ai_turn_action.h`
with prototypes for the complete family.

Both helpers scan grid positions five through nine in their original order.
They return the first candidate whose comparison is **strictly positive**,
not the strongest candidate. The face-up helper increments its occupied
count before excluding face-down cards. Consequently:

- With no occupied cards, both return the middle grid entry at side base
  plus seven.
- With occupied cards but no accepted positive comparison, both return `-1`.
  In particular, an all-face-down row is not an empty row.
- A successful comparison returns the candidate object's `field_6A`, not
  the loop's grid position. The empty-row fallback instead returns the grid
  entry directly. This mixed-domain behavior is preserved.

The field decision tries the face-up helper first. Its randomly enabled
fallback may use the other helper, then an additional packed-flag scan.
Nothing consolidates their occupied/visibility predicates or changes the
random conditions.

## Battle-strength result, not a generic comparator

`Duel_CompareBattleStrength` (`0x8001EFD4`) takes two `DisplayObject *`
arguments and uses their `field_6A` bytes to select canonical card records.
The left card's defense-position flag chooses `Duel_CalcBattleDefense` or
`Duel_CalcBattleAttack`, including the existing guardian-star bonus and
clamp. The right card's defense-position flag selects the high or low half
of `Duel_CalcCardStats`.

Unequal values return their signed difference. Equal values return zero if
either card defends, otherwise `-1`. A null right argument returns the left
card's effective attack without taking that subtraction path. Thus the
result is neither a symmetric ordering nor a normalized `-1/0/1` value;
the tie rule must not be repaired into equality.

Both AI target scans require a positive result. The three calls from
[`DuelScene_UpdateBattle`](../src/game/duel_scene_battle.c) also preserve
their separate consumers: resolution uses the sign and magnitude for side
markers and life-point deltas, model presentation distinguishes `-1` from
other results, and direct-attack presentation passes a null right operand.
Those are evidence for the comparison's shared role, not new battle logic.

## Regional bindings and validation

All seven inventories keep their original row order, address, size, status,
ownership and notes; only the seven function-name fields change. Symbols
use each image's native address, including the Japanese bindings below.
Grouped source ownership and every compiler profile are unchanged.

| API | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| `Duel_CompareBattleStrength` | `0x8001EFD4` | `0x8001EE1C` | `0x8001EF30` | `0x8001EF34` |
| `Ai_SelectMagicPlay` | `0x8002712C` | `0x80026F40` | `0x80027070` | `0x800271B4` |
| `Ai_SelectFusionPlay` | `0x80027228` | `0x8002703C` | `0x8002716C` | `0x800272B0` |
| `Ai_SelectHandPlay` | `0x80027508` | `0x8002731C` | `0x8002744C` | `0x80027590` |
| `Ai_FindFavorableFieldTarget` | `0x8002778C` | `0x800275A0` | `0x800276D0` | `0x80027814` |
| `Ai_FindFavorableFaceUpFieldTarget` | `0x800278A0` | `0x800276B4` | `0x800277E4` | `0x80027928` |
| `Ai_SelectFieldPlay` | `0x800279BC` | `0x800277D0` | `0x80027900` | `0x80027A44` |

The type-only stage reproduces the complete compiler and MASPSX units
raw-identically. The named stage compares the complete selection, battle-stat
and battle-scene units after reversing only these seven identifier spellings.
Full executable matching remains mandatory. Regional/cloud builds remain
PR validation, not local work.

Runtime liveness of the six selection entries remains unconfirmed. The
`ai_turn_action_liveness` trace included known-live controls, but its one
capture missed both controls and was discarded as invalid; the script itself
was retired unrun when #418 capped pending traces at five, and stays
recoverable from git history. It supplies no runtime evidence that the six
functions are dead, or that the proposed names are dynamically confirmed.
No new trace, fixture, harness or test scenario is introduced here.
