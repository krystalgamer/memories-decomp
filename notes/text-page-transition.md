# Page waiting, entry control and completion handoff

The page transition uses three matching APIs plus one existing completion
callback. The entry-control helper is deliberately generic: other callers,
including the shared Spanish intro, use values unrelated to page waiting.

| API | NA identity | Role |
|---|---|---|
| `DuelEffect_SetEntryControlBytes` | `0x800373C8` | Store two raw control bytes across an active entry prefix |
| `Text_BeginPageClear` | `0x800374A8` | State-table slot 3, request control 3/render 0 then hand off as `0x82` |
| `Text_WaitForPageAdvance` | `0x800375A4` | State-table slot 4, initialize/poll the page cursor and advance input |
| `Text_CompletePageAdvance` | `0x8003741C` | Existing state-table slot 2, complete clearing and reset page fields |

The table stays unsized and retains all eighteen initialized entries in
their original order. No new state enum, table bound or callback cast is
introduced.

## Raw entry control is not a fade-only API

The helper retains two `u8` parameters and the original biased cursor
pointing at the second control byte. It starts at the configured range,
stops at the first entry without active bit `0x80`, and otherwise stores
the two bytes up to the original count. It does not skip gaps, clear the
active flag, reserve entries or process an entry callback itself.

NA/Japan store at entry offsets `0x13/0x15` with strides `0x1C/0x18`.
Europe stores at `0x11/0x13` with stride `0x16`, retaining its register
formal, explicit address/active locals and selection macros. Japan also
keeps its byte range count at channel `+0x5D`, rather than the shared
halfword count at `+0x5E`.

Resident callers use control/render pairs `0/0`, `2/0` and `3/0`.
The shared Spanish intro additionally passes controls 5, 6, 8 and 9.
Keep those raw values and the existing narrow ABI; no universal transition
or fade interpretation is assigned to all values.

## Clear starter and existing completion

On an uninitialized slot-3 call, `Text_BeginPageClear` first ORs the state
latch into the channel byte, calls the helper with `3/0`, then writes
literal state `0x82`. Both state writes remain, including the constant's
replacement of other state bits. An already-initialized call does nothing.
It neither opens a cursor nor polls input.

The literal handoff means the existing slot-2 completion callback skips
its own first-call `2/0` request. When slot 2 is entered uninitialized
instead, that callback sets its latch, requests `2/0` and returns before
checking completion.

On subsequent completion calls, the original check reads only the first
configured range entry's active flag. It does not add a full-range scan.
Once that flag clears, it resets line count, pen X/Y and text state;
only NA additionally clears `field_62`. All regional field/reset behavior
is retained.

## Page input and shared signed-byte countdown

`Text_StartPageWait` selects slot 4. Its uninitialized callback sets the
state latch, stores ten in `gText_PageAdvanceHoldCountdown` and calls
`Dialog_OpenChoice`, storing the returned cursor at channel `+0x30`.
That branch does not poll input or sound in the same call. Allocation
remains unchecked, with no extra release, retry or fallback.

On initialized calls, Square hold decrements the existing signed byte and
then clamps a negative result to zero; releasing Square stores ten.
After this update, a nonzero countdown requires a regional pressed-mask
hit to proceed. Zero bypasses that press requirement. The signed byte,
store/read order and low-byte behavior are not widened or normalized.

Advancing plays sound eleven, writes state two, releases the cursor and
nulls the pointer in that order. The next callback follows the ordinary
uninitialized slot-2 path. The countdown stays shared across channels;
it is neither new per-object storage nor a guaranteed independent
ten-frame timer for each concurrent text box.

NA/Europe retain the default confirmation mask; Japan retains its
Circle/Square mask. Square-held counting itself is unchanged in all
regions. No first-call press handling, timing adjustment or new input
condition is introduced.

## Native identities and collision caveat

| Identity | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Entry-control setter | `0x800373C8` | `0x80036B84` | `0x80037154` | `0x80037354` |
| Clear starter | `0x800374A8` | `0x80036C60` | `0x80037240` | `0x80037440` |
| Page-wait callback | `0x800375A4` | `0x80036D5C` | `0x8003733C` | `0x8003753C` |
| Shared countdown | `0x8009B32C` | `0x8009B21C` | `0x8009C2BE` | `0x8009C6DE` |

The Japanese countdown binding comes from the already-matched page-wait
wrapper's native `D_8009B21C` alias. The unrelated duel pointer
`gDuel_pLifePointsPanel` lives at `0x8009B10C` and remains untouched; the
numeric stem alone is not a reason to rename it. The NA-address cell
`0x8009B32C` also has an unrelated Japanese owner.

Wire definitions, declarations, all callers, callback table positions and
the existing French/Spanish model-intro resident imports. Remove only the
four obsolete Japanese/European callback/countdown macros. Other wrapper
aliases, include/source paths, selection guards, compiler profiles and
inventory fields stay fixed.

Only the countdown extern moves from `unmatched.h` to the callback owner,
retaining `s8`. That ownership stage leaves all complete compiler/MASPSX
outputs raw-identical. The naming stage compares all whole outputs after
reversing just four identifiers, without filename, instruction, directive
or relocation filtering. Full clean retail matching remains mandatory;
tests, controls and runtime captures do not change. Regional/cloud
validation remains PR-side.

The separate halfword delay and optional viewport-shake states are covered
in [text-delay-shake.md](text-delay-shake.md); their initial-call decrement
and shared state must not be conflated with this page hold counter.

## Range names preserve different stopping predicates

entry_range_start/entry_range_count replace the original range_start_5C
and regional range_count_5E/range_count_5D names. Shared storage remains
u16 +0x5C/+0x5E; Japanese channel and narrow entry-range view remain
u8 +0x5C/+0x5D. No padding, stride, narrowing or macro selection changes.

HasActiveEntry still returns zero for an empty count or the first
inactive flag, and one for the first nonzero handler byte in that active
prefix. SetEntryControlBytes still stops on the first inactive entry or
the original count and stores handler-before-render bytes through its biased
regional view; it is not a gap-skipping or whole-allocation operation.

CompletePageAdvance still checks ONLY the first configured entry after
its original initialization/return path, even when a new count guard might
look desirable. It does not scan to entry_write_cursor or add an empty-
range special case. State/pen/newline resets and NA field_62 behavior stay.
The names add no bounds, capacity policy or handler/release timing.
