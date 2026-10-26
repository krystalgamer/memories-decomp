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
set the two rotation bytes: face-down selects `rotation_y = 0x80`,
defense selects `rotation_z = 0xC0`, with the unselected component zero.
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

## View-mode marker refresh

`Duel_UpdateCardDisplayMarker` (`0x80017DB4`, size `0x88`) updates the
existing `deck_number_marker` marker from the active side's signed view mode.
The packed read at record +0x14 requires both occupied and display-marker
flags. Mode zero leaves the old marker untouched. A nonzero mode reads
data +0x04 and stores its value plus one as a byte; a negative mode then
overrides that byte with `0xFF`.

The frame renderer prints marker values 1-40 and selects alternate frame
content for a nonzero marker, including the nonnumeric `0xFF` form. The
source byte is pre_shuffle_index, the original slot permuted with its ID.
The packed load, both mode expressions, pre-override data read and byte conversion remain unchanged.

## Packed Euler rotation bytes

DisplayObject.field_20.b names the unchanged u8 bytes at +0x20/+0x21/
+0x22 rotation_x/rotation_y/rotation_z. DisplayObject_ProjectRotationFacing
and Duel_RenderFieldCard3D independently pass those components, multiplied
by 16, to RotMatrixZYX_gte as vx/vy/vz. The packed-rotation projector
unpacks the same three bytes into the same axes. The sprite renderers
read the +0x22 alias and multiply it by 5760 for GsSPRITE.rotate:
both representations therefore give one complete turn per 256 byte units.
The existing field_20.h byte alias also names rotation_z. The aggregate
word, the halfword containing X/Y, the fourth byte and all overlapping
access widths retain their existing declarations and identities.

DuelCardDisplayObject names +0x21/+0x22 rotation_y/rotation_z, including
the result outro's departing sprite. CardObject's former flag byte at
+0x22 is also rotation_z. Duel_GetCardEffectVariant still sets bit 7 for
ANY nonzero value, not just a face-down constant or a normalized Boolean.
The state refresh maps face-down to Y and defense to Z conditionally;
ApplyCardObjectFlags maps face-down to Z and defense to Y, with its
existing unconditional clears. Neither is replaced by the other.

These are member-only identities, not new angle conversions, clamps,
flag meanings or callback contracts. Alpha-named helpers/macros retain
their existing names and operations; the byte at +0x21 is the geometric
Y component, not evidence of a transparency channel. Signed casts,
byte narrowing/wrap, stale values, callback timing, clip gates, mirror/
backface decisions, source groups and every numeric control remain.
Effect 23 deliberately retains its existing vx addition to ALL THREE
rotation bytes; effects 17/22 retain their distinct vx/vy/vz additions.
No apparent axis inconsistency is repaired.

The existing state fixture migrates four member words on two mutation
lines only. Its independent +0x21/+0x22 expected-byte writes, 131072-case
count, guarded backing, flags, stale-byte oracle and all negative controls
remain byte-for-byte. No fixture is executed locally.
