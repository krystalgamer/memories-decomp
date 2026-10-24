# Field-card presentation transitions

The paired callbacks in `duel_card_turn_animations.c` move existing card
objects between the field-selection presentation and the ordinary 3D field
presentation. They are not gameplay face-up/face-down operations.

| North American entry | Identity | Presentation handoff |
| --- | --- | --- |
| `0x80022674` | `Duel_UpdateCardToSelectionView` | Set record `0x400`; finish with `Duel_UpdateCardDisplayState` |
| `0x800229F4` | `Duel_UpdateCardToFieldView` | Clear record `0x400`; finish with `Duel_ApplyCardObjectFlags` |

## Callers and renderers

`Duel_UpdateDisplayTransition` installs the selection callback after its
bit-1 row-view tween reaches its target, then creates the field cursor when
the transition finishes. Field-action and card-pick initialization request
that transition with flags `3`.

The bit-4 transition instead starts a wide field-view tween and installs the
field callback immediately. The cursor's cancellation paths request flags
`0xC`. Both paths preserve the existing object tag, counter initialization,
occupied-record filter and callback casts.

The renderer independently identifies the handoff:
`Duel_DrawFieldCards` sends record flag `0x400` to
`Duel_DrawProjectedFieldCardFrame`; otherwise it calls
`Duel_RenderFieldCard3D`. The callbacks do not modify the record's
`DUEL_CARD_FLAG_FACE_DOWN` or `DUEL_CARD_FLAG_DEFENSE_POSITION` bits.
Those bits select the animation's existing mode.

The historical turn-object header calls its `0x20` union `tint`, but the
3D renderer reads those same three bytes as Euler components shifted left
four. The union label alone must not be used to infer RGB semantics.
Its existing type, packed/byte stores and signed/unsigned angle views are
unchanged.

## Exact state and timing

Both complete functions retain their original goto-arm order, initialization
and phase bits, side-dependent directions, clip-test stores and handoff timing.
Their shared reset blocks deliberately call different display-state helpers;
those helpers use different face-down/defense mappings to object bytes
`0x21` and `0x22`.

Their mode-zero branches are also different. The selection callback sets its
counter to eight, decrements it and returns only on equality to zero;
otherwise it reaches reset. The field callback waits while the decremented
counter is positive. Neither branch is normalized to the other, and no
minimum duration, clamping or additional cleanup is introduced.

## Existing field-record view

`gDuel_aFieldCardRecords` names the existing incomplete `DuelCardRecord[]`
view formerly called `D_801A7B64`. Its North American address is
`gDuel_aCardRecords + 5 * 0x1C = 0x801A7B64`. The Japanese provider remains
`0x801B7B64`; every image's canonical alias refers to its original provider,
including automatically supplied providers where no tracked address exists.

This is not a new array or a contiguous twenty-record allocation. The
underlying pool has thirty records, fifteen per side. Each side's ten field
slots follow five hand slots: consumers retain the original base and
base-plus-fifteen traversals. Draw, resume, field-object collection and
card-effect paths all use this view without replacing it with an inline
`&gDuel_aCardRecords[5]` expression or pointer variable.

## Preservation boundary

`card_presentation.h` isolates canonical declarations after each consumer's
original `duel_card.h` include. Every pre-existing header and overlay C file
remains byte-identical. The developer-screen change only updates its adjacent
function-name comment; no include or executable expression changes there.
Existing tests, overlay metadata, historical attempts/fingerprints and
runtime traces are unchanged.

All seven complete two-function groups retain the original source paths,
definition order, `0x380/0x3A0` extents and `gcc_2_8_1_g8` profile.
Validation compares complete ordinary compiler/MASPSX output and actual native
translation units with only the three declared identities reversed, then
requires a clean byte-identical resident executable. Regional/overlay image
acceptance remains on the PR checks, not replaced by local source probes.

## Runtime observation

A `duel_display_callbacks` trace (#418; interpreter CPU, setup confirmed by
1,538 control hits) covered the opening turns of a Free Duel against
Villager2. The player summoned Spike Seadra (448) face-up and later Dragon
Zombie (97) face-down. The opponent set card 96 face-down and switched it
to defense. The trace hit its 96-row limit before any battle took place.

- **Pairing:** five completed flags-`3` episodes (starting at frames 1246,
  1392, 1627, 1766 and 2044, in scenes `0x8004` and `0x8005`) were each
  followed by a flags-`0xC` transition. A sixth flags-`3` episode started
  at frame 2210 and returned to `0x0000` at frame 2235, where the capture
  stopped at its 96-row limit. Its flags-`0xC` partner, if any, was not
  observed, so the capture does not show the pairing holds for every
  episode or for a whole duel.
- **Selection view:** `Duel_UpdateCardToSelectionView` ran while bits `3`
  were set. It took eight calls for the face-up card and one for each
  face-down card (97 and 96), ending as the transition word went from
  `0x8003` to `0xC003`. On each occupied field card it turned record flag
  `0x400` on.
- **Field view:** `Duel_UpdateCardToFieldView` ran during bits `0xC` and
  turned `0x400` off again.
- **Other flags:** neither callback touched face-down (`0x1000`) or defense
  (`0x800`).
- **Bit `0x2` panel:** `Duel_CreateFieldCardPanel` was called nine times
  from `Duel_UpdateDisplayTransition` (`0x80023784`) with `(0, 240)`. Each
  call came on the frame a transition containing bit `0x2` started.

The record flags also match the accepted names:

| Card | Flags | Meaning |
| --- | --- | --- |
| 448, face-up | `0x8000` | occupied |
| 97, face-down | `0x9000` | occupied, face-down |
| 96, opponent's turn | `0xFC00` | occupied, used this turn, display marker, face-down, defense, selection view |
| 96, next turn | `0xB800` | used-this-turn bit cleared |

These rows were printed only at the end of the trace, so what was on screen
at each transition is not recorded.
