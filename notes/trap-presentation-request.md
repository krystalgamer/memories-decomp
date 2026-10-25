# Trap presentation request ownership

`Duel_UpdateTrapPresentation` in `src/game/duel_battle_stats.c` receives
an actual `DuelEffectRequest *` from `DuelEffect_CreateRequest(8)`.
That typed API already returns a request-pool entry; no byte API,
display-object allocation or new pointer owner is introduced.

The first two payload stores use explicit signed value conversions of the
selected display object's unsigned halfwords. The third store deliberately
keeps the old `u16 *q34, *d` path: only the address of the real signed
`field_04` member needs an unsigned halfword view. It is not widened to a
word or replaced with a differently scheduled struct assignment.

All three stores precede `DuelCard_RemoveFromField`, which receives the
card record indexed by the display object's `field_6A`. Sound follows the
removal, and the whole mode halfword becomes `2` afterward. These are
distinct objects: request, selected display object, and card record.

The staging/replay offset, selected card index, outer busy gate, low-nibble
mode dispatch, signed timer tests, gotos and do/while shape, screen-effect
calls and final opposing-side counter are unchanged. The sibling attack
trap selector is unchanged. Both matching entries retain their existing
grouped owner, source order and `gcc_2_8_1_g8_split` profile (`0x294` and
`0x1F8` bodies). The first candidate's complete grouped compiler assembly
is identical to the unchanged matching baseline, and incremental
full-executable matching succeeds.

The old shared `DuelEffectObject` prefix is retained as a historical layout;
no declaration, member width or layout assertion is changed or deleted.
Its main-game uses have now recovered actual request ownership.

## Focused witness

`test_trap_presentation_request.py` executes the **actual extracted
request-copy/removal/sound/mode-update fragment**, using the source's real
header prefix and project types. Allocation, card removal and sound are
stubs. The removal stub checks the target and pre-removal payload, then
clears only a fixture card-ID field as an observable event. It does not
execute real card removal, the staging selection, timer/mode gates, the
whole four-mode sequencer, rendering, overlays or engine execution.

Each `-O0`/`-O2` run executes 1,966,080 fragment calls: every halfword bit
pattern in each of the three payloads at every valid card-record index.
The initial mode varies over all upper-bit patterns with low nibble `1`.
Independent byte oracles cover the complete request, card-record array,
display object, mode and guards; stubs require allocation, complete
payload copy, removal, sound, then mode publication in that order.
Negative controls reject a widened third store, removal before the final
payload copy and mode publication before sound.

The exact source audit separately protects the unchanged sequencer and
selector; final full-executable acceptance remains the integration gate.
