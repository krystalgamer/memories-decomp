# Swords of Revealing Light runtime contract

Swords of Revealing Light is the only card mapped to duel-effect group 10.
The group's two dispatch entries are `DuelEffect_StartSwords` at `0x80025EE0`
and `DuelEffect_ApplySwords` at `0x80025F3C`. This exclusive table mapping,
together with the complete producer and consumer chain below, supports naming
the handlers and their persistent state without relying on card text alone.

## Application

`DuelEffect_StartSwords` starts the presentation. The apply handler then
creates effect object `0x15` for the opposing side, stores it in
`gDuel_apSwordsEffectObjects[side]`, and marks each occupied card object in the
selected field scan for the shared reveal animation. Once the presentation is
ready, it writes `DUEL_SWORDS_INITIAL_COUNTER` (`4`) to the opposing
`DuelSideState.swords_turns_remaining`.

The internal value is one larger than the displayed duration because draw
entry decrements the current side's counter before that side can act:

| Draw entry | Stored value after decrement | Effect |
| --- | ---: | --- |
| First affected turn | 3 | Attack lock active |
| Second affected turn | 2 | Attack lock active |
| Third affected turn | 1 | Attack lock active |
| Following turn | 0 | Effect object removed |

`func_80023144` copies the counter into the field-card text staging values and
clamps any nonzero value outside `1..3` to `3`. Thus the setup value `4` is an
internal pre-decrement state, while the visible contract is three turns.

## Consumers and lifetime

`AiScript_TestPinned` exposes whether a selected side's counter is nonzero to
the AI script. The normal AI play-selection candidate at `0x800279BC` also
suppresses its attack-selection result while the active side's counter is
nonzero. These independent consumers establish that the byte is an attack
lock rather than only a display timer.

Replay or phase reconstruction recreates object `0x15` for every side whose
counter is still active and republishes it in
`gDuel_apSwordsEffectObjects`. At draw entry, expiration writes the object's
removal state, clears the side's pointer, and leaves the counter at zero.
The array is exactly two pointers and remains small data because all retail
accesses are GP-relative.

This is a static code contract. It establishes dispatch ownership, countdown
ordering, AI gating, display staging, reconstruction, and cleanup; it does not
claim frame timing for the presentation animation.
