# Library state and card records

The resident Library screen owns the state at `0x800EA1E8`.
`src/game/library_state.h` combines its existing `LibraryMotionState` prefix
with the display-object pointers and directly indexed card table. Both the
runtime and cursor headers now import this single declaration, replacing the
incompatible byte-array and motion-only declarations.

The motion prefix also exposes its dispatch byte at `+0` and selected-card
ID at `+6`. `Library_UpdateGridCursor` writes the ID before selecting state
2, and `func_8002ACA4` repeatedly reads that same halfword for the card
view's resource and stat lookup. The old separate `LibrarySelectionState`
prefix is no longer needed. Other phase bytes remain unnamed.

| Offset | Member | Local evidence |
|---|---|---|
| `0x00..0x47` | `motion` | `func_80029590` fills eight grid-row sprite pointers; `func_8002A3CC` and `func_8002A4A8` update the cursor's 16.16 motion; `func_80029934` reads the globe radius. |
| `0x48` | `selector_sprite` | `func_8002BFCC` retains the display object configured at `(0x10, LIBRARY_SELECTOR_SPRITE_Y)`. No more specific selector role is asserted. |
| `0x4C` | `card_view` | `func_8002ACA4` creates, slides and releases this display object while opening and closing the selected-card view. |
| `0x50` | `loading_icon` | `func_8002ACA4` retains the loading sprite, changes its resource variant during model playback and releases it on exit. |
| `0x54` | `cards[723]` | `func_8002BFCC` clears entry zero, then initializes IDs `1..722` with a four-byte stride. |

Each `LibraryCardRecord` is four bytes. Its halfword at `+0` is `clut_x`:
the initializer chooses `LIBRARY_CARD_SELECTOR_*` values from the card type,
and `Library_DrawCardGrid` copies that halfword into `GsSPRITE.cx`. The
unsigned byte at `+2` is `flags`; `+3` has no established use and remains
padding. The recorded signed halfword storage and unsigned renderer reads
are preserved.

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

The initializer keeps its existing byte/integer base cursors, but writes the
card records and selector pointer through typed members. The card-view
controller still walks its phase-dependent prefix as bytes; this change
does not guess new meanings for the prefix's padding or merge its phases.

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

The viewport-follow helper takes only `const LibraryMotionState *`.
Its signed first y read and unsigned follow-up reads remain in their
original order around the viewport store. The typed grid updater passes
`&state->motion`; the initializer's byte cursor uses the same known prefix.

The motion updater's selected-text call converts the address of the first
member back to its enclosing `LibraryState`. That updater is anchored
directly at `D_800EA1E8.motion`, so the call still receives the complete
state and card table, not an unrelated standalone motion record.

The phase-dependent runtime keeps its byte cursors at the untyped dispatch
boundary. No new meanings are assigned to unknown phase bytes, and held
input remains volatile. Supported-GCC owner checks accept all four typed
contracts and reject their former byte-pointer variants.
