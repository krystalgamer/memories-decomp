# Placement marker turn and battle slide cleanup

Two existing display-object callbacks receive names from their producers,
updates and renderer consumers. Existing layouts, callback casts, source
groups and old declaration headers remain unchanged.

## Placement display-marker removal

`Duel_UpdateCardMarkerRemovalTurn`, formerly `func_80019BD0`, is installed
by `DuelScene_UpdateCardPlacement` on present work-slot objects whose card
record has `DUEL_CARD_FLAG_DISPLAY_MARKER` (`0x2000`). Placement assigns
active tag 1, then waits for `DisplayObject_FindAllocatedByTag(1)` before
advancing.

The callback initializes its threshold to `0x40`, or `0xC0` when byte
`+0x21` is already nonzero, and sets the clip-test flag. Each update adds
8 to that byte. Reaching the threshold sets phase bit `0x40`, jumps the
byte to `0xC0`, and clears display-marker member `field_67`. The return
leg ends at the existing signed-byte test: byte `+0x21` becomes zero,
the indexed card's `0x2000` marker flag is cleared, and the clip-test flag
is cleared only if bytes `+0x20` and `+0x22` are both zero. Finally, the
active tag and update callback are cleared.

`Duel_RenderFieldCard3D` and the display projection checks consume bytes
`+0x20/+0x21/+0x22` as the three Euler components, shifted by four.
`Duel_UpdateCardDisplayMarker` populates `field_67`, and
`Duel_UpdateCardDisplayState` clears it when the record marker is absent.
Together with the placement producer, these establish a marker-removal
turn rather than a name based only on its final mask.

“Removal” refers to the display marker, not destruction/removal of a card
or object. No gameplay face-up/face-down transition is asserted. The
separate `0x400` selection/field-view turn routines are not this callback.
The initial threshold, promoted `current` value, explicit signed-char
test, byte wrap, two phase legs, midpoint clear and cleanup order are
preserved, not simplified.

## Cosine slide with roll reset

`DisplayObject_UpdateCosineSlideWithRollReset`, formerly `func_8001ED20`,
is installed by `DuelScene_UpdateBattle` on terrain and life panels with
duration 16, on card work slots with duration 24, and on the returning
panels with duration 16. It is not a card-only update.

On first update it snapshots the start position and resets the cosine
phase. A nonzero byte `+0x22` advances by `0x40 / duration`. Position
interpolation uses the current phase, then the phase advances by
`TRIG_ANGLE_HALF_TURN / duration`. On completion the callback copies the
entire target position word into the output position, resets byte `+0x22`,
clears the clip-test flag only if byte `+0x21` is zero, and clears the tag
and update callback.

The sprite consumers in `display_object_runtime.c` and
`display_object_core.c` multiply byte `+0x22` by 5760 for the sprite's
rotation; the 3D/projection consumers use it as Euler Z at sixteen angle
units per byte. “RollReset” describes the demonstrated completion action.
It does not promise smooth convergence from every initial angle or a
globally unrotated object: other angular bytes can remain nonzero.

Keep the signed duration casts, integer divisions, conditional byte
advance, narrowing, interpolation-before-increment order and final
whole-word snap. No duration guard, clamping or rounding repair is added.
The adjacent `DisplayObject_UpdateCosineSlide` remains distinct: it does
not acquire this callback's final snap or roll cleanup.

## Native registrations and compatibility

| Image | Marker-removal turn | Slide with roll reset |
| --- | --- | --- |
| North America | `0x80019BD0` | `0x8001ED20` |
| Japan | `0x80019A4C` | `0x8001EB68` |
| SLES_039.47 | `0x80019B5C` | `0x8001EC7C` |
| Other PAL images | `0x80019B60` | `0x8001EC80` |

All fourteen records preserve game ownership, matching-C status, notes and
extents: `0xF8` with `gcc_2_8_1_g8_split` for the marker turn, and `0x124`
with `gcc_2_8_1_g8` for the slide. The slide stays after its existing
cosine-slide sibling in the grouped translation unit. Source filenames,
definition order and registrations do not change. Japan's marker source
keeps its existing card-record alias.

The isolated header declares the same `void(DisplayObject *)` definition
views. Existing casts to the byte-pointer `DisplayObjectCallback` remain
unchanged, as does the old `DISPLAY_OBJECT_MOTION_BYTE_CALLBACK` selector
for the sibling API. Numeric function aliases remain available. All
pre-existing headers, overlays and test modules stay byte-identical; no
harness, C fixture, probe or case is added. Full regional and cloud/witness
execution remains PR-side validation.
