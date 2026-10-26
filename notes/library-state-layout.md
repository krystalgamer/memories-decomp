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

The updater's captured `state` cursor is now the full `LibraryState *`
already supplied by `gLibrary_State`. Capture still precedes `one`, the
original global byte read and low-nibble dispatch. States 0, 1 and 2 pass
that owner directly to their typed handlers; state 3 retains an explicit
byte-pointer boundary to the unchanged empty handler. The three countdown
accesses use the same signed `motion.animation_countdown` member directly,
without changing decrement/store/test/reset order. The model/view logic,
volatile reload, random choice, two 16-byte copies and callback timing
are unchanged. The initializer's separate integer card-table carrier remains
intact.

`Library_EnterGridState` (`0x8002BAA0`) is that dispatcher's sole state-0
helper. It receives the captured Library owner and writes its complete
`motion.state` byte to 1, whose dispatcher arm runs the grid cursor.
This is state selection, not grid initialization, drawing or a blanket
reset: the high flag bits are cleared by the original whole-byte write,
and every other owner byte is untouched. The canonical `LibraryState *`
parameter follows the only caller and first-byte layout. All seven
`0xC` exports retain their profile; Japan keeps its separate definition.
The neighboring empty state-3 handler remains address-based and unchanged.

`Library_InitializeScreen` (`0x8002BFCC`) is called from that main runner's
initial branch before its fade wait. It requests the regional Library
package with the existing phase callback, waits for transfers, and prepares
the screen state, card table, row/selector/cursor objects, count text and
music request. This names setup, not a blanket reset: the partial writes,
including the repeated entry-zero CLUT store, and untouched state bytes
remain exactly as before. The main runner still owns entry/exit fades and
ongoing dispatch through `Library_UpdateScreen`.

The initializer's slot-3 count text uses the actual `DuelEffectChannel *`
returned through `TextBox_Create` or Europe's flagged factory. The two
`0x10` byte overrides use `field_5A/field_5B` only in the original NA arm;
Japan and Europe still omit both. The build helper receives that same
channel directly. Its `field_28` is the display-object pointer that the
shared layout helper acquires from the pool and publishes, including the
Japanese shared-helper variant. The ordering-table call and subsequent
screen-space flag update retain separate pointer reads, with no snapshot
across the call or new allocation/null-safety guarantee. The local never
walks channel records or touches differing tails; the generic factory APIs,
regional arguments, signed integer card carrier and loop shape are unchanged.

`Library_CreateGridSectionSprites` (`0x80029590`) creates the eight
retained section objects during initialization. Their anchors are
`x = 8/168`, `y = (i >> 1) * 178 + 8`, matching the two-column/four-bank
grid section geometry six pixels before each section's first-card origin.
The sprite resource index follows `i`; no particular glyph is inferred.
The helper keeps its original resource/color/texture values, 8bpp flag,
viewport-relative flag clearing, ordering table 3 and depth zero. It does
not clear requests or tear down objects, and does not create the distinct
cursor/render object that initialization configures separately.

`Library_LoadPackageStage` (`0x8002BD0C`) is the initializer's registered
descriptor/mode callback, following the existing `*_LoadPackageStage`
family. It programs the NA/Japanese six-stage payload sequence and final
sound-bank mode; Europe adds its language-image stage and now exports the
same `Library_LoadPackageStage` name at its own address. The
callback is not the whole-package request/wait operation or a callable
overlay. Its exact typed prototype is preserved without tightening the
separate, historically unprototyped `FileTransferCallback` typedef.

`Library_RefreshSelectedCardText` (`0x8002A2F4`) is called during initial
setup and when cursor motion completes. It stages the cursor ID through
`s16` before creating slot-1 text with selector 0 or 5. An unknown nonzero
card clears only the published `gDuel_wSelectedCardID`, not the staged ID,
local index or selector 5. The unowned check still includes record zero;
the existing field-`0x54` byte value and override to 4 are retained.
The final channel helper still runs
its build loop until done. This is a refresh/build operation, not a pure
ID query or an added card-ID validation boundary.

The selected-card text helper now holds its text-box result as
`DuelEffectChannel *`, not a `DisplayObject *` used only to reach a
coincident offset. This follows `TextBox_Create`'s actual return from
`DuelEffect_InitEntry`: a selected channel whose initializer explicitly
writes the unsigned byte `field_54`. The direct byte read preserves the
old first-byte view exactly. The Japanese initializer returns its compact
`0x60` channel through that same public channel pointer; its `field_54`
is also one byte at `+0x54`. The local helper performs no channel stride
arithmetic or tail access. No generic `void *` API, shared channel layout
or separate `D_800EB15C` build alias is changed, and the field is not
assumed to remain zero merely because initialization writes it.

The card-view handler's separate slot-0 `TextBox_Create` result now uses
that same actual `DuelEffectChannel *` view. Its three writes reach the
existing byte members `field_54`, `field_53` and `field_59` directly, in the
original order. The European build still omits the `field_54` write; the
other builds retain their existing override value, followed by 1 and 4.
Both channel declarations place all three one-byte fields at those same
offsets, including Japan's compact channel. This local neither walks an
array nor touches the differing tails. The generic factory API, channel
layouts, regional adapters and the handler's other byte-walking locals
remain unchanged.

The card-view light local uses the existing model-owned
`ModelSlotLightEntry`, rather than inventing a Library record or replacing
it with an SDK view. `Model_GetFlatLight` returns a clamped element of the
slot's three `flat_lights` records; both model render paths pass those same
records to `GsSetFlatLight`. Two explicit casts mark the unchanged legacy
byte-pointer accessor boundary. Nine signed direction-word writes now use
the canonical members, including the two adjacent records in the setup
path. The three packed color writes remain full `s32` accesses at `+0x0C`,
`+0x1C` and `+0x2C`: each also writes that record's padding byte at `+0x0F`.
They must not be replaced with three byte stores that leave padding intact.
Acquisition, calculation and store order, the accessor API, shared layouts
and regional adapters are unchanged.

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

The zoom/return transition arms repeatedly call
`Library_CreateFadingCardOutline` (`0x8002ABB4`) with depth deltas -1 and 1.
Each call acquires one new display object, snapshots selected source
transform/dimension words, seeds its halfword countdown to `0x100` and
colour to zero, then installs `Library_DrawFadingCardOutline`
(`0x8002A9C0`) in the two-argument `field_4C` draw slot. It is not a full
object/texture/colour clone or a whole-trail operation.

The callback subtracts `0x10` from an unsigned-loaded halfword and releases
on the narrowed signed nonpositive result. Otherwise it writes the blue
byte from the remaining low byte, projects the four corners, emits a
three-edge flat polyline and closes the first-to-last edge with a separate
flat line. From the constructor's seed, fifteen invocations draw and the
sixteenth releases; this is a callback count, not a video-frame guarantee.
The source/destination ABI, signed depth narrowing, typed scratch records,
projection output permutation and both packet submissions are unchanged.
The two functions retain separate source objects and their distinct
`gcc_2_8_1_g0_split` / `gcc_2_8_1_g0` profiles.

The display-object schema's source-filename citation intentionally retains
its historical spelling. The recorded French Options grid attempt hashes
that entire header as raw bytes, including comments; the naming change
therefore leaves the header byte-for-byte intact rather than rewriting
historical attempt evidence or weakening its fingerprint check.

The card-view handler also reads its fixed model-copy buffer at `0x80181000`
through a local SDK `SVECTOR *`. `Model_CopySlotPosition` still writes all
four halfwords through its unchanged `u16 *` call, and the globe renderer
already copies those same eight bytes as an `SVECTOR`. The handler consumes
only the first three signed halfwords as `view.vrx/vry/vrz`, preserving the
previous `s16` views for every bit pattern. This is a local coordinate view,
not a global ownership/type claim or an interpretation of the fourth word.
The literal address and capture point remain unchanged; the handler neither
reads nor writes `pad`. Guarded actual-source checks cover every halfword
pattern per coordinate and reject unsigned-read, axis-swap and padding-write
mutants.

The independently labeled Y halves now have the scoped names
`gLibrary_sCapturedReferenceY` and `gLibrary_sSampledReferenceY`.
Their capture/sample provenance and distinct movement branches are
documented in [library-reference-y.md](library-reference-y.md).
Only those signed scalar identities change; the literal buffers,
generic four-halfword helper, local `SVECTOR` view, globe copy and
existing controls above remain unchanged.

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
| `0x00..0x47` | `motion` | `Library_CreateGridSectionSprites` fills eight retained grid-section sprite pointers; `Library_UpdateCursorMotion` and `Library_StartCursorMotion` update the cursor's 16.16 motion; `Library_DrawWireframeGlobe` reads the globe radius. |
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
pointer and the nine-object traversal order are unchanged.

The card-view object-walking local now uses the actual `DisplayObject *`:
the pool accessor returns that type's storage, the paired resource
constructor returns its allocated primary object, and the stored card-view
and resource-primary pointers refer to those same display objects. The
constructor's legacy byte-pointer API is preserved with one explicit local
cast; the two card-view captures use their already-typed owner directly.
Nine flag reads/updates reach the existing `u16 flags` member at `+8`,
retaining every mask, branch and the separate `bits` snapshot/write.
All other mixed-width byte views and configuration-boundary casts remain
unchanged, including the shared locals' reuse and object lifetime order.

The initializer's separate `o` local likewise keeps the actual
`DisplayObject *` returned by both pool acquisitions. Its ordering-table
argument and two owner publications use that type directly; the screen-space
flag expression keeps the same unsigned-halfword member and read/or/store
shape. Both former byte writes at `+0x5F` remain explicit `u8` views of byte
1 of `field_5E`, not halfword stores or guessed alpha/scale members. The
target boundary is checked without changing the shared record. Acquisition,
configuration, primary/integer capture, publication and palette-loop order
remain unchanged, with no allocation/null-safety guarantee added.

The motion prefix's `display_objects` is one `0x24`-byte
`LibraryDisplayObjects` union at `+0x24`. Its `by_role.slots[8]` view
retains the eight row objects, and `by_role.render` retains the distinct
object at `+0x44`; `all[9]` exposes the complete pointer window used by the
two flag loops. Every stored pointer keeps `G32`. This adds no ninth row,
second pointer table or new storage, and never indexes the eight-row
array at 8. The supported compiler accepts the standard named union;
no anonymous-struct extension or new profile flag is needed.

Row setup and cursor rendering use the role view. The two flag walks
read `all[i]` and update each object's canonical unsigned halfword `flags`,
keeping the exact `0xFFBF`/`0x40` masks and iteration order through 8.
No pointer snapshot, callback, null check or ownership behavior is added.
Header/layout controls retain the `0x48` motion prefix, `0x44` render
offset, complete `0xBA0` state and every later field.

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
offsets. Both the handler and dispatcher use their captured typed owners
and reach the same signed countdown through the canonical motion member.

The initializer's primary cursor `r` is `LibraryState *`, captured directly
from `gLibrary_State` at its original point; six formerly recast accesses
now use that owner directly. Its separate `rb` address carrier deliberately
remains `s32`. The remaining seven motion-prefix member views now use
`r->motion` directly, and the scroll helper receives `&r->motion` at the
same point. The now-unused private prefix macro is removed. Rest coordinates,
signed current coordinates, sprite arguments and the stored render pointer
retain their original types and store/capture order. All five card-table
cast-backs, palette-loop induction and repeated entry-zero CLUT store are
unchanged. Supported-compiler experiments with `LibraryState *rb`
(both direct and with the old integer conversion at capture) and a true
`LibraryCardRecord *` card base all produced `0x1D07F8`, eight bytes short
of the required `0x1D0800` image.

The compiler comparison identifies a real shape change: the typed carrier
lets the palette loop use pointer-end termination instead of retaining its
integer `n` counter, removes one saved register and changes the frame from
64 to 56 bytes. The original repeated entry-zero CLUT store is not removed;
the regression is induction/register shape, not a newly discovered layout.
Typing only `r` and removing its redundant owner casts retains exact bytes
while leaving that measured `rb` boundary intact. This is not a complete
pointer-width portability fix or an excuse to change profiles/volatile
state; further `rb` recovery needs a new exact compiler discriminator.

The initializer writes card records and retained pointers through typed
members at unchanged capture/store points. The card-view
controller reaches the recovered state, phase, coordinate, radius, timer,
card flags and display-pointer window through canonical members. Local
SDK-object, light and scratch views retain their measured scalar access
shape; no new meanings are guessed for other padding and phases are not
merged.

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
