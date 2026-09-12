# Duel scene action runtime

The resident duel loop dispatches one of fifteen scene-state callbacks through
`gDuel_apfnSceneStateHandler`. Candidate `func_80024200` reads the low nibble
of `gDuel_wSceneStateFlags` and calls that table entry after card-effect and
display-effect work are idle.

`gDuel_wSceneStateFlags` combines the callback index with state-local progress
bits. `DUEL_SCENE_PHASE_MASK` names the low-nibble selector.
`DUEL_SCENE_FLAG_INITIALIZED` is the only high bit with a uniform meaning:
scene handlers test and set it around their first-frame setup. Other high bits
are interpreted by individual handlers and remain unnamed globally.

The table order is:

| Index | Callback |
| ---: | --- |
| 0 | `DuelScene_UpdateEffectPreview` |
| 1 | `DuelScene_UpdateStartup` |
| 2 | `DuelScene_UpdateDrawPhase` |
| 3 | `DuelScene_UpdateDrawResolution` |
| 4 | `DuelScene_UpdateHandActions` |
| 5 | `DuelScene_UpdateFieldActions` |
| 6 | `DuelScene_UpdateCardUse` |
| 7 | `DuelScene_UpdateCardPlacement` |
| 8 | `func_8001B170` |
| 9 | `DuelScene_UpdateBattle` |
| 10 | `DuelScene_UpdateTurnSwitch` |
| 11 | `DuelScene_UpdateResume` |
| 12 | `DuelScene_UpdateResultOutro` |
| 13 | `DuelScene_UpdateResultRewards` |
| 14 | `DuelScene_UpdateExodiaResult` |

The mask permits index 15, but the initialized table has only fifteen entries.
The following initialized word belongs to the terrain-boost table. No recovered
producer establishes that index 15 is reachable, so the declaration remains
unsized rather than asserting a sixteenth callback.

## Scene lifecycle

The opening sequence is represented by three matching handlers.
`DuelScene_UpdateStartup` loads the duel resources, starts the music, builds
the two rank baselines, clears the hand slots and advances to draw resolution.
`DuelScene_UpdateDrawPhase` reconstructs the active hand and stages its drawn
card. `DuelScene_UpdateDrawResolution` completes that draw, handles deck
exhaustion and Exodia detection, and chooses the next scene.

The action controllers hand presentation work to four larger generated
assembly states. `DuelScene_UpdateCardUse` owns the selected-card zoom and both
card-effect phases. `DuelScene_UpdateCardPlacement` owns ordinary field
placement. `DuelScene_UpdateBattle` sequences the attack, damage and aftermath.
`DuelScene_UpdateTurnSwitch` then animates the side change, flips the active
side, repoints its state records and returns to draw entry.

`DuelScene_UpdateResume` restores every occupied card object and side indicator
after an interrupted presentation, then replays field effects or starts the
music before fading back in. Index 0 is the separate developer
`DuelScene_UpdateEffectPreview` state rather than part of the normal duel
opening.

The result path is explicit. `DuelScene_UpdateResultOutro` streams the outro,
spawns the seven winning-side sprites and advances to index 13.
`DuelScene_UpdateResultRewards` constructs the three result pages, derives the
POW/TEC rank, and awards the starchips and selected card drop. Exodia instead
uses index 14, `DuelScene_UpdateExodiaResult`, for its five-stage piece and
sparkle presentation before the result handoff.

Index 8 remains `func_8001B170`. Its body is clearly a six-step selected-card
UI/action state, but the available evidence does not distinguish a sufficiently
specific role from neighboring placement and card-use states. It remains
address-named rather than turning that uncertainty into a durable API.

## Human action controllers

`DuelScene_UpdateHandActions` is the 5,304-byte state-4 controller. Its
resident assembly owns hand navigation, card inspection, ordinary card play,
and the routes into placement, magic/trap/equip presentation, and cancellation.
Its card-inspection branches publish `gDuel_wViewerCardID`, set the shared
viewer Y offset to 20, and request `DUEL_EFFECT_STATE_CARD_VIEWER`.

`DuelScene_UpdateFieldActions` is the 5,632-byte state-5 controller. It owns
field navigation and the human monster-action menu: card inspection, attack
selection, position changes, sacrifices, and the transition to battle or other
scene states. Its attack filter is the consumer that requires occupied,
attack-position monsters with `DUEL_CARD_FLAG_USED_THIS_TURN` clear, as
documented in [`duel-turn-action-eligibility.md`](duel-turn-action-eligibility.md).

Both functions remain generated assembly, but their scene-table positions,
input branches, state transitions, and viewer payload writes establish the
names independently of a C reconstruction.

Candidate `func_80018FEC.c` retains the address-based `D_8009B23A` declaration
as a lexical contract. Its linked target and the rest of the runtime use
`gDuel_wSceneStateFlags`.
