# Card display-state refresh

`Duel_UpdateCardDisplayState` names North American `0x80017E3C`
(size `0xC8`). It reads the record selected by the display object's
constructor-bound `card_index` and updates the existing card-rendering
view. It does not allocate, draw, submit packets or run the engine.

The factory invokes it after installing its callbacks. Field actions
invoke it after marking a card used this turn, and the turn-back
animation invokes it from its shared completion/reset block. Those
callers independently support the state-refresh role.

The complete body clears the marker byte only when
`DUEL_CARD_FLAG_DISPLAY_MARKER` is absent, always clears the display
clip-test bit, and re-enables clipping when the card is face-down or
in defense position. Only inside that orientation branch does it
set the two rotation bytes: face-down selects `field_21 = 0x80`,
defense selects `field_22 = 0xC0`, with the unselected component zero.
When neither flag is present, both old rotation bytes remain untouched.
Colour is always reset to neutral `0x808080` or used-turn `0x404040`.

This is not an alias for `Duel_ApplyCardObjectFlags`. That helper swaps
the two angle-byte roles, clears angles unconditionally, and invokes the
marker helper before applying its marker gate. No shared normalizer is
introduced, and neither implementation is simplified.

The existing `void (DuelCardDisplayObject *)` contract, field widths,
conditional store order, callers and source definition order remain
unchanged. All seven resident bindings follow the name; North American
and European grouped owners/profiles remain, while the isolated Japanese
wrapper receives a descriptive path without changing its bytes or guard.
Historical attempts/audits and generated reports keep the old spelling.

The native witness executes only the complete actual field-store helper
over guarded full canonical/card-view unions and real card records.
It covers all 65,536 card-flag words on two object seeds at O0/O2,
including unknown flag bits, exact bytes and untouched surrounding
storage. Wrong angle mapping, unconditional angle clear, reversed marker
gate, lost dimming, lost object flags, wrong record and guard-write
mutations must fail. It does not execute allocation, GTE/GPU, animation
controllers, callbacks or rendering.
