# Library card-view state interface

The resident card-view handler `func_8002ACA4` takes `LibraryState *`.
Its sole caller is the Library dispatcher, whose state cursor is based on
the existing typed `D_800EA1E8` object. The handler uses the motion/flag
prefix and the selector/card-view/loading-icon slots through `0x50`.
This is not an arbitrary byte payload or a new allocation.

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
Byte `0x05` stays padding. The four byte-offset arithmetic boundaries and
halfword/word helpers remain unchanged; no new alias local is needed.

## Preserved ownership and behavior

`LibraryState`, its `0x48` motion prefix, three stored pointers and 723
card records remain exactly as declared. The raw display-object and
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
