# Library state and card records

`gLibrary_State` is the resident Library screen's existing `LibraryState`
view at North American address `0x800EA1E8`.
`src/game/library_state.h` combines its existing `LibraryMotionState` prefix
with the display-object pointers and directly indexed card table. Both the
runtime and cursor headers now import this single declaration, replacing the
incompatible byte-array and motion-only declarations.

The semantic name changes only the shared logical binding. Japan keeps
`0x800EA0C8`, Europe `sles_03947` keeps `0x800EB150`, and the other four
European images keep `0x800EB570`. These are the existing absolute C/linker
or resident-symbol bindings, not new storage definitions or allocations.
The unrelated French nominal-address alias `D_800EA1E8_French` stays at
`0x800EA1E8`; it is not the French Library view at `0x800EB570`. The existing
`0xBA0` declaration and every member/neighbor boundary are unchanged, and
the name does not assert exclusive physical-RAM ownership in other modes.

`Library_UpdateScreen` (`0x8002BAB4`) is the active-screen updater called
from `Main_RunLibraryMenu`'s ongoing-update branch. That caller is installed
in the resident main-mode table and retains entry, exit and fade services.
The Library updater dispatches the owned state's low nibble to the
initial/grid/card-view/no-op handlers; its card-view branch also updates
the model/view and eligible animation countdown, then publishes the orbit
buffer and view metrics. Drawing remains separate callbacks. The updater
keeps its existing `void (void)` interface and is not a replacement for the
main runner's complete lifetime handling.

`Library_InitializeScreen` (`0x8002BFCC`) is called from that main runner's
initial branch before its fade wait. It requests the regional Library
package with the existing phase callback, waits for transfers, and prepares
the screen state, card table, row/selector/cursor objects, count text and
music request. This names setup, not a blanket reset: the partial writes,
including the repeated entry-zero CLUT store, and untouched state bytes
remain exactly as before. The main runner still owns entry/exit fades and
ongoing dispatch through `Library_UpdateScreen`.

The motion prefix also exposes its dispatch byte at `+0` and selected-card
ID at `+6`. `Library_UpdateGridCursor` writes the ID before selecting state
2, and `Library_UpdateCardView` repeatedly reads that same halfword for the card
view's resource and stat lookup. The old separate `LibrarySelectionState`
prefix is no longer needed. The four adjacent card-view phase bytes are
also recovered from the card-view controller:

| Offset | Motion member | Local evidence |
|---|---|---|
| `0x01` | `card_view_phase` | The controller switches on its low five bits; phase 4 uses bit `0x80` as its one-time initialization flag. |
| `0x02` | `card_view_subphase` | Phase 4's nested sequence advances through 0, 1 and 2 while entering, waiting in and leaving the selected-card action view. |
| `0x03` | `card_view_slide_phase` | Low-nibble values 1 and 2 slide `card_view` in opposite directions; bit `0x80` guards each slide's initial setup. |
| `0x04` | `model_load_phase` | Value 1 requests the selected monster model, 2 polls its load, and 0 is the completed/inactive load path. |

All four are whole `u8` members, not bitfields or enums; the original masks,
flags and scalar narrowing remain unchanged. Byte `0x05` has no recovered
use and remains padding. No other Library owner accesses these phase bytes.

The six selected-card reads in the card-view handler use the canonical
`motion.selected_card_id` member with an explicit `u16` view. Its shared
storage and the published `gDuel_wSelectedCardID` remain `s16`; replacing
the old unsigned halfword read with a bare signed member would change the
forwarded word, direct indices and flag-byte offsets for high-bit values.
Unsigned 16-bit values still promote to the target's signed 32-bit `int`:
zero minus one remains `-1`, and `0xFFFF` minus one remains `65534`.
The separate signed-global reload/index is unchanged (`0xFFFF` publishes
`-1`, whose index is `-2`). This preserves every bit pattern without adding
card-ID validation, clamping or an out-of-range table-safety guarantee.

The card-view position calculations also use the canonical motion members:
opening reads unsigned `rest_x`/`rest_y`, while closing reads signed `x`/`y`.
Those four views retain their original integer promotions. Opening still
subtracts signed viewport offsets and unsigned object origins (including
the already-captured Y origin); closing still subtracts signed object
origins and viewport offsets in its original order. The signed halfword
position stores and full-width cosine-helper arguments are unchanged.
No coordinate is clamped or reinterpreted through the opposite signedness,
and the SDK object views are not rewritten.

The card-view action sequence now reaches its existing signed `globe_radius`
member directly. Initialization writes zero; advancement keeps the original
unsigned `u16` read, adds `0x2A`, narrows into the halfword, and only then
tests the stored signed value against `0x400` before applying that cap.
This is not an unsigned saturation or a general nonnegative clamp:
`0x7FD6 + 0x2A` stores `0x8000` and skips the signed cap, while
`0xFFD6 + 0x2A` wraps to zero. The renderer's three signed radius consumers,
all other state bytes and the following transition/callback gates remain
unchanged. The explicit unsigned RHS is retained for the measured load
and promotion shape, not replaced by a bare signed increment.

| Offset | Member | Local evidence |
|---|---|---|
| `0x00..0x47` | `motion` | `func_80029590` fills eight grid-row sprite pointers; `Library_UpdateCursorMotion` and `Library_StartCursorMotion` update the cursor's 16.16 motion; `func_80029934` reads the globe radius. |
| `0x48` | `selector_sprite` | `Library_InitializeScreen` retains the display object configured at `(0x10, LIBRARY_SELECTOR_SPRITE_Y)`. No more specific selector role is asserted. |
| `0x4C` | `card_view` | `Library_UpdateCardView` creates, slides and releases this display object while opening and closing the selected-card view. |
| `0x50` | `loading_icon` | `Library_UpdateCardView` retains the loading sprite, changes its resource variant during model playback and releases it on exit. |
| `0x54` | `cards[723]` | `Library_InitializeScreen` clears entry zero, then initializes IDs `1..722` with a four-byte stride. |

The card-view handler uses the canonical `card_view` and `loading_icon`
stored-pointer members for all ten direct accesses to those slots. Two
acquired objects are published as `DisplayObject *` values rather than
round-tripping through `s32`; the existing byte-walking locals and the
narrow configuration-view call boundaries remain intact. No storage
declaration or pointer width changes: both slots already carry `G32`.

Pointer lifecycle order is unchanged. The loading icon is nulled on entry,
published after setup, read for its variant changes and passed to the
release helper on cancellation without a new clear. The card view is read
separately by both slide arms and released before its slot is cleared and
the text box is destroyed. The captured loading-icon pointer remains read
before the existing volatile close-path write. No caching, extra null
check, early clear or object-ownership redesign is introduced. The selector
pointer and the separate nine-object row-sprite/render walks are unchanged.

Each `LibraryCardRecord` is four bytes. Its halfword at `+0` is `clut_x`:
the initializer chooses `LIBRARY_CARD_SELECTOR_*` values from the card type,
and `Library_DrawCardGrid` copies that halfword into `GsSPRITE.cx`. The
unsigned byte at `+2` is `flags`; `+3` has no established use and remains
padding. The recorded signed halfword storage and unsigned renderer reads
are preserved.

The card-view controller's two flag-byte updates use
`cards[(u16)motion.selected_card_id].flags` directly. Entry clears `0x80`;
the close sequence sets it after releasing the paired resource objects.
Both operations preserve the other seven bits, including the separate
unowned bit `0x01`, and touch neither the CLUT halfword nor the padding
byte. The selected ID remains an unsigned view and is read again after the
release call, not cached across it. The matched typed lvalues retain the
retail base-first address calculation without a new cursor, helper call,
mask rewrite or card-ID bounds guarantee.

`LIBRARY_CARD_FLAG_KNOWN` (`0x80`) is set when
`Campaign_TestStoryFlag(card_id + CAMPAIGN_FLAG_LIBRARY_CARD_BASE)` succeeds.
It permits drawing and opening the card. `LIBRARY_CARD_FLAG_UNOWNED` (`0x01`)
is added only when `Library_CheckCardOwned` returns negative, meaning the
card is in neither the trunk nor the player's deck. The renderer then uses
the dim tint and the selected-card text refresh selects its alternate mode.
These flags describe the Library's historical visibility and current
ownership separately; `KNOWN` is not a current-owned flag.

The table includes the empty-selection record for ID zero, not a
zero-based array of 722 cards. Its last record occupies
`0x800EAD84..0x800EAD87`. The resulting `0xBA0`-byte state ends at
`0x800EAD88`, the separately mapped request pool; this boundary corroborates
the initializer's count rather than being the only evidence for its extent.
Header assertions cover the record size, flag offset, all three pointer
offsets, table offset and total state size.

## Matching constraints

`Library_StartCursorMotion` (`0x8002A4A8`) receives the next column, row and
frame count from `Library_UpdateGridCursor`. It caches the changed target
cell, computes the two-bank grid coordinates, and initializes the target,
half-pixel fractions and 16.16 velocities in the shared motion prefix.
A same-cell request returns without changing an active tween. The signed
full-width divisor and byte-stored frame count are retained; the name does
not imply clamping, a zero-frame guard or a successful restart on every call.

`Library_UpdateCursorMotion` (`0x8002A3CC`) advances that motion and returns
its active byte as the existing `s32` result. The grid handler gates new
input on a zero return. Inactive calls do not publish render coordinates.
An active call integrates x/y, decrements the frame byte, and, on expiry,
clears active, snaps to the target, plays sound 53 and refreshes the selected
card's text before publishing the render position. A zero countdown wraps
rather than completing immediately. This is cursor motion, not scrolling;
`Library_UpdateGridScroll` remains a separate viewport operation.

The signed word at motion `+0x20` is `animation_countdown`. The card-view
controller seeds it to `0x12C` when requesting a monster model and when
starting its action-view sequence. The dispatcher decrements it only in
screen state 2, after the slot-status query returns 1 and the slot-animation
index is also 1. A resulting value at or below zero resets it to `0x12C`,
then requests animation 2 or 7 according to the existing random low bit.

This is not an unconditional frame count, a load-completion timeout or a
cursor-motion duration. Its original signed subtraction/comparison, update
eligibility, reset-before-random-call order and all five word accesses are
preserved. The canonical member replaces exactly four padding bytes;
the eight row sprites, separate render pointer and full state retain their
offsets. The handler uses its typed owner, while the dispatcher keeps its
existing byte cursor and projects the already-established motion prefix.

The initializer keeps its existing byte/integer base cursors, but writes the
card records and selector pointer through typed members. The card-view controller accesses the five established state/phase bytes
through the canonical motion members. Its remaining halfword/word helpers
and four byte-offset boundaries keep their measured access shape; this
does not guess new meanings for other padding or merge the phases.

The renderer must also keep cursors anchored at the unshifted state base,
advancing by `sizeof(LibraryCardRecord)`, with a typed view at each CLUT
read. A measured `LibraryCardRecord *` candidate anchored directly at
`&state.cards[index]` changes the address setup and fails the whole-image
size gate (`0x1D07FC` rather than `0x1D0800`). Keeping the original cursor
anchor preserves the retail `lhu` displacement while removing the literal
field offsets. The motion coordinates use the existing typed prefix with
explicit unsigned reads, preserving `lhu`.

The unified global, typed table initializer, selected-card flag read,
renderer views and `const LibraryState *` flag-reader contract each passed
the North American whole-executable exact-match gate under the existing
named compiler profiles. No profile, linker address, object order, overlay
or regional configuration is changed.

## Typed helper interfaces

The grid cursor and selected-card text helpers now carry the verified
`LibraryState *` context through their interfaces, rather than converting
it back to bytes and recasting it at every member use. The card-ID query
and text refresh take a read-only state; the grid updater retains mutable
access for the selection and mode bytes.

The card-ID query's context parameter remains present even though its
calculation reads the global row/column instead. Removing the unused
argument is not part of this type change or its ABI.

`Library_UpdateGridScroll` (`0x8002A660`) takes only
`const LibraryMotionState *`. Its name follows the grid/initializer call
chain and the 2D viewport effects, not a model-camera interpretation.
It fixes horizontal scroll at zero and follows cursor y outside the
`0x40..0xB0` dead zone. The existing European `0xC0` lower-bound override
is retained. Source/header names, manifest owners and equivalent regional
symbols use this name without changing their addresses or profiles.

Its signed first y read and unsigned follow-up reads remain in their
original order around the viewport store. The typed grid updater passes
`&state->motion`; the initializer's byte cursor uses the same known prefix.

The motion updater's selected-text call converts the address of the first
member back to its enclosing `LibraryState`. That updater is anchored
directly at `gLibrary_State.motion`, so the call still receives the complete
state and card table, not an unrelated standalone motion record.

The phase-dependent runtime keeps its byte cursors at the untyped dispatch
boundary. No new meanings are assigned to unknown phase bytes, and held
input remains volatile. Supported-GCC owner checks accept all four typed
contracts and reject their former byte-pointer variants.
