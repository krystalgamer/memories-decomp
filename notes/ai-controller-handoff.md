# AI controller and pending-choice handoff

These identities connect duel setup, AI script/heuristic producers and the
hand/field presentation controllers. They do not define a new controller
structure or replace the existing twelve-byte `AiSelection` union.

| Former identity | Canonical name | Preserved view |
| --- | --- | --- |
| `D_8009B360` | `gDuel_bSide0ControllerSelector` | signed scalar / `.data` / two-side array |
| `D_800EAE8F` | `gAi_bGuardianStarChoice` | unsigned scalar / incomplete array, selection +7 |
| `tent_AiSelectionRandom` | `gAi_bHandFaceDownRequested` | unsigned scalar, selection +8 |
| `D_800EAE91` | `gAi_bFieldSourceCode` | unsigned scalar / incomplete array, selection +9 |
| `D_800EAE92` | `gAi_bFieldTargetCode` | unsigned scalar / incomplete array, selection +10 |

## Signed controller selection

`Main_StartDuel` stores its first argument into the selector, and
`Text_StartCampaignDuel` writes -1. The hand, field and card-move
controllers take their AI paths when the selected signed byte is
nonnegative; negative values take human paths. Side initialization,
created-card knowledge, ritual handling and result handling independently
use the same distinction.

The array view includes the separately named `gDuel_bOpponentID` at element
1. That byte also carries an opponent ID, but this does not establish that
the magnitude of the first selector is a duelist or deck-template ID.
`Duel_ShuffleDeck` with a null deck samples the single opponent deck pool,
not a table indexed by this first byte. Neither selector becomes a Boolean
or a newly range-checked enumeration.

The isolated header mirrors all three original declaration arms under
their existing guards. The opponent's scalar, sized-array and
incomplete-array views remain independent. There is no new allocation or
layout assertion.

## Guardian choice versus face-down request

`AiScript_LoadGuardianStarChoice` copies the raw low byte of a selected VM
register. Under the existing AI/card-type guard,
`DuelScene_UpdateCardMove` clears card flag `0x200`, then sets it when
selection byte 7 has bit 0 set. `duel_card_layout.h` independently identifies
that flag as `DUEL_CARD_FLAG_USE_GUARDIAN_STAR_2`. Other bits of the stored
choice remain unmodified; the name does not normalize the byte.

Selection byte 8 has a different consumer and predicate.
`DuelScene_UpdateHandActions` can bypass a monster preview when it is zero.
A nonzero value requests the face-turn leg; that leg fixes the display
face byte to `0x80` and sets card flag `0x1000`
(`DUEL_CARD_FLAG_FACE_DOWN`). Heuristic writers sometimes choose the byte
with `rand() & 1`, but randomness describes a producer, not the byte's
meaning. The request is neither an immediate mutation of every card nor a
guarantee of its final later face state.

The three coupled API corrections retain `void(void)`:

| Former API | Canonical API | NA address | Size |
| --- | --- | --- | --- |
| `AiScript_SetSelectionRandom` | `AiScript_SetHandFaceDownRequest` | `80073448` | `0x10` |
| `AiScript_ClearSelectionRandom` | `AiScript_ClearHandFaceDownRequest` | `80073458` | `0x0C` |
| `AiScript_LoadSelectionRandom` | `AiScript_LoadHandFaceDownRequest` | `80071510` | `0x3C` |

They still store 1, store 0, or load the whole unsigned byte into a
script-selected VM register. The dispatch table references the canonical
functions. All 21 native function records retain their addresses, extents,
grouping and `gcc_2_8_1_g0_split` profile; old callable names remain linker
aliases. The unrelated set/clear pair for `AiScriptState.field_97` is not
renamed.

## Field source and target codes

`Ai_SelectFieldPlay` initializes the source to zero and writes own-monster
codes 1 through 5 or own-back-row codes 6 through 10 when selecting a play.
The field controller's case 1 decodes the source with the original
`(a - 1) % 5` and `a >= 6` expressions into own rows 2 or 3.
`AiScript_PlayFieldCard` can instead write an arbitrary truncated VM byte.
The name does not impose a valid range or a new no-action guard.

Heuristic target producers write an own equip target, zero for non-target
plays, or an opposing target encoded with `+0x38`. The VM producer retains
its original category-operand branches: copy the source under its special
condition, zero, copy the operand, or default to `0x3A`.

The field controller's case 5 decodes values below `0x38` into own rows
2/3 with column `a - 1` or `a - 6`; other values select row 1 with column
`0x3C - a`. The initialized table in `duel_field_layout.c` independently
maps row 1 to opposing monster records and rows 2/3 to own monster/back-row
records. This is a mixed target/navigation code, not a card type, plain
card-record index or hand index. The old local parameter `category` is
not renamed, and selection byte 11 remains unclassified here.

Signed struct stores, unsigned standalone reads, arbitrary VM bytes,
zero/default branches, raw comparisons and out-of-range behavior remain.
The old signed members `zero`, `random`, `field_09` and `field_0A` stay in
the frozen legacy header. Standalone and `gAi_Selection.bytes[...]`
expressions are not folded into those members: their addressing forms
and instruction ordering are load-bearing.

## Native identities and compatibility

| Image | Controller 0 | Guardian | Face-down | Source | Target |
| --- | --- | --- | --- | --- | --- |
| NA | `8009B360` | `800EAE8F` | `800EAE90` | `800EAE91` | `800EAE92` |
| Japan | `8009B250` | `800EAD6F` | `800EAD70` | `800EAD71` | `800EAD72` |
| SLES_039.47 | `8009C2D2` | `800EBDF7` | `800EBDF8` | `800EBDF9` | `800EBDFA` |
| Other PAL | `8009C6F2` | `800EC217` | `800EC218` | `800EC219` | `800EC21A` |

The Japanese `symbols.txt` binds the controller provider `D_8009B360` at
`8009B250`, and Japan builds `Main_StartDuel` from the shared source. The
movie code's `D_8009B4A0`-`D_8009B4A2` triplet sits at `8009B360`-`8009B362`
there and is unrelated. No RGB or tint meaning is inferred.
French literal aliases also remain separate from logical PAL bindings.
The NA source/target providers are generated automatic symbols, not new
tracked storage; generated files are not manually edited.

Existing headers, overlays/imports, profiles, source grouping, function
metadata notes and all unrelated identities remain. Four existing controls
receive only necessary literal namespace substitutions, checked by
whole-file inverse reconstruction. Their cases, assertions, mutations,
generators and compiler/setup logic are unchanged. No new harness,
fixture, probe or testcase is added. Cloud/regional/overlay/witness
execution remains PR-side validation; native-profile artifact conservation
does not replace full regional image matching.
