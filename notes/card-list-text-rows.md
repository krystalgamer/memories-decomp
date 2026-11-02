# Card-list row text and shared UI views

This component names the row-set text builder, the card-list-only typed
row-pool view and the packed sort-icon table. It does not change UI
behavior, scratch ownership, record layout, glyph bytes or text staging.

## Row-set text builder

`CardList_BuildRowSetTextBoxes`, formerly NA `func_80060E70`, keeps
`void(u16 *, s32, s32, s32)` and its unused fourth word. It selects one
row set, stores the original flag in `enabled` and object `field_68`,
then builds each card-ID/count pair's text box.

The flag is not normalized: style selection tests nonzero, while both
stores keep the original value. Card ID zero retains style zero and
does not replace the prior card/count staging. Nonzero IDs use the
monster/non-monster distinction, preserve the direct/cached staging stores
and publish the already named selected-card ID.

Zero count sets the existing color and entry bit `0x8000`. Following rows
retain their regional text flag, vertical offsets still advance by `0x16`,
and each build still blocks. No allocation check, side/index validation
or style correction is introduced.

NA/Japan keep seven entries, `0x18`-byte row sets and following bit `0x40`.
Europe keeps eight, `0x1C` bytes and bit `0x80`, plus its color-slot-four
reset/set and incremented staging cursor. The NA nested do-while-zero
stores remain load-bearing; no loop or register-lifetime cleanup is made.

## Typed row view of shared scratch

`gCardList_aRowSets` names only the existing incomplete
`CardListRowSet[]` view formerly spelled `D_801A8000` in three typed
consumers: the row constructor, renderer and builder.

The constructor initializes two logical records and their display objects,
sets each object's side selector and renderer callback, and the other
two consumers select those same records. All pointer `G32`, offsets,
entry-count macros and incomplete bounds remain.

This is not a private two-record allocation. Script bytecode, model/DMA
payloads, raw words and `MainMenuState` use the same staging RAM at different
times. Their declarations, source, imports and controls remain unchanged.
Japan's card-list view is `0x801B8000`; NA/PAL use `0x801A8000`.
No global replacement or consolidation of that numeric family is made.

## Packed sort-icon table

`gCardList_SortIconTable`, formerly `D_80090DD8`, retains the mutable
byte-array owner, incomplete byte-array consumers and all 32 initialized
NA bytes. It is two 16-byte banks of two-byte glyph/sort descriptors, not
a new struct, constant-qualified table or six-icon-only allocation.

Sort-mode code reads the second byte's low nibble at
`kind * 16 + choice * 2 + 1`. Rendering uses packed UV/page bits and the
selected key for brightness. The seventh entries differ by bank; padding
stays. No new meaning is assigned to mode values eight/nine or to
uninspected regional values. Every existing CLUT/UV macro, pointer/integer
cast and byte interpretation remains.

| Image | Builder | Row view | Sort-icon table |
| --- | --- | --- | --- |
| NA | `0x80060E70` | `0x801A8000` | `0x80090DD8` |
| Japan | `0x8004F9A4` | `0x801B8000` | `0x80090C88` |
| SLES-03947 | `0x80050B6C` | `0x801A8000` | `0x80091E00` |
| Other PAL images | `0x8006AED4` | `0x801A8000` | `0x80092214` |

The French literal `D_80090DD8_French` at the NA address is not the
logical sort table at `0x80092214` and remains untouched.
European builds call the builder by its semantic name; frozen Trade
overlays keep their independently bound
`func_80060E70` import. Unrelated literal function identities are not rebound.

## Scope boundaries

The generic `D_801D5608` message-input union and separate result-card word
remain untouched. They serve broader card, deck, mem-card, rank, credits
and overlay contexts, not one card-list allocation.

Only registered active glyph sources are changed; the standalone
`card_list_update_sort_mode.c` is active in some native registrations and
is not removed as an orphan. The sort-table data owner retains its
registered `gcc_2_8_1_g0` profile, and all builder variants retain
`gcc_2_8_1_g8_split_no_strength_reduce`. Source filenames and grouping stay.

Two minimal canonical headers reuse existing types and declarations.
Every old header, overlay C/import, shared-buffer test, manifest/profile,
function status/extent/ownership/note, attempt, report, trace, validator
and exception remains unchanged. No test, C fixture, probe or source-string
harness is added or migrated.

Whole compiler/filtered/MASPSX and complete fallback comparisons reverse
only these three identities and original per-context aliases; no
directive, instruction, register or filename filtering. Clean resident
matching, data ownership/policy checks and fresh applicable PR gates remain
required. Regional inputs are not local, and no cloud/full regional/
overlay/witness suite is run locally.
