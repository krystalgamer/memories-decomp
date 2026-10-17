# Library card-view state interface

The resident card-view handler `Library_UpdateCardView` (`0x8002ACA4`)
takes `LibraryState *`.
Its sole caller is `Library_UpdateScreen`, whose state cursor is based on
the existing typed `gLibrary_State` object. The handler uses the motion/flag
prefix and the selector/card-view/loading-icon slots through `0x50`.
This is not an arbitrary byte payload or a new allocation.

The name follows the complete screen-state chain: the grid handler selects
a card and enters state 2; the dispatcher calls this updater; the updater
opens and advances the presentation, handles its nested action view, and
eventually returns the owner to grid state 1. It is not a grid renderer or
a monster-only model updater: non-monster cards still use its card/text
path. The existing typed `void` interface and every phase remain unchanged.

The owner was already `library_runtime.h`, which includes the canonical
Library state header. The dispatcher keeps its existing byte cursor and
uses an explicit owner view at this call, just as it does for the grid
handler. No other state-handler contract is changed.

## Measured byte boundaries

The initial owner-interface checkpoint kept the original `state` identity,
made byte indices and byte-offset arithmetic cast explicitly to `u8 *`,
and retained the halfword/word helpers' existing boundary views. It changed
no branch, call, aliasing, timing or memory-access expression and added no
local cursor.

A first candidate with a separate byte-alias local was rejected: the
whole-executable comparison changed the prologue at function `+4`
(`0x1B4A8`, expected `0x2C`, actual `0x24`) despite equal image size.
The candidate/diff/log remain under local `tmp/`. Explicit access
boundaries preserve the matched code while exposing the actual owner type.

A subsequent exact-match checkpoint replaces all 45 direct byte-index
views with the canonical motion state's existing `state` byte and four
recovered phase members, documented in `library-state-layout.md`. Their
whole-byte types retain all masks and flags, including initialization bits.
Byte `0x05` stays padding. At that checkpoint the four byte-offset
arithmetic boundaries and halfword/word helpers were retained without a
new alias local.

The stored-pointer follow-up uses `LibraryState.card_view` and
`LibraryState.loading_icon` for their ten direct reads/writes. Their
canonical `DisplayObject *G32` declarations were already established.
Publication no longer converts an object through a signed integer, while
local byte/configuration views and every release/clear ordering boundary
were retained at that checkpoint; it did not guess a larger row array.

The later pointer-window recovery exposes the measured nine stored
`DisplayObject *G32` values through `LibraryDisplayObjects`: eight
`by_role.slots` plus the separate `by_role.render`, overlapping a real
`all[9]` array. The two flag loops now use that bounded array and canonical
object flags, with identical bytes/order and no index 8 into `slots[8]`.
The role paths change only their spelling; every offset and allocation
relationship remains intact. Local SDK/light/scratch scalar views remain
separate and unchanged.

## Preserved ownership and behavior

`LibraryState`, its `0x48` motion prefix, three outer stored pointers and 723
card records retain their measured storage. The raw display-object and
scratch views, scalar-width accesses, state transitions, callbacks and
regional conditional behavior are not redesigned.

All source paths, seven-function grouping, profiles, sections, addresses
and stored layouts remain intact. Regional include adapters receive the
same shared declaration; no overlay implementation or generated manifest
is edited. Genuine byte/no-op handlers remain outside this bounded change.

Supported-GCC controls accept `void (LibraryState *)` and reject the old
byte-pointer function contract while retaining the earlier Library helper
and layout controls. Complete body normalization and whole-executable
matching establish byte neutrality; equivalent behavior alone is not an
acceptance criterion.
