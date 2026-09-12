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
| 0 | `func_80022618` |
| 1 | `func_80018608` |
| 2 | `func_8001898C` |
| 3 | `func_80018DB4` |
| 4 | `DuelScene_UpdateHandActions` |
| 5 | `DuelScene_UpdateFieldActions` |
| 6 | `func_80019608` |
| 7 | `func_80019D18` |
| 8 | `func_8001B170` |
| 9 | `func_8001F55C` |
| 10 | `func_800208D4` |
| 11 | `func_8001825C` |
| 12 | `func_80020F4C` |
| 13 | `func_800218F0` |
| 14 | `func_80018FEC` |

The mask permits index 15, but the initialized table has only fifteen entries.
The following initialized word belongs to the terrain-boost table. No recovered
producer establishes that index 15 is reachable, so the declaration remains
unsized rather than asserting a sixteenth callback.

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
