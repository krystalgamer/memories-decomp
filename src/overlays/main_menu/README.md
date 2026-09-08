# Main Menu Overlay

This directory contains matching source from the main-menu runtime
module.

Verified boundaries:

| Item | Range |
|---|---|
| Archive | `game/DATA/SU.MRG` (1,239 sectors) |
| SU load request | sectors `0-115` |
| SU executable phase | sectors `98-114`, `0x8000` bytes |
| Runtime image range | `0x80180000-0x80188000` |
| Leading word | `0x0000000F`; semantic role unconfirmed |
| Phase SHA-256 | `34e9421eb10dc3ff97f8810e4f595045d4847b2b54760e9895eb83266008bc97` |

The request is recovered from the resident loader trace. `Main_RunMenu`
(`0x8002D588`) calls `func_8005B85C`, whose request is:

```c
func_80014E1C(1, gFile_szSuMrgPath, 0, 0x73, func_8005B64C, 0, 0);
```

`gFile_szSuMrgPath` at `0x800117C8` is the development path literal
`M:/mrgSU/SU.mrg`, so this form selects a named archive instead of the WA
screen packages. The third and fourth arguments remain the first sector and
the sector count.

## Image shape

The phase does not begin with code. Its first word is the leading value above,
followed by a six-entry pointer table, with the first instruction at `+0x1C`:

| Offset | Contents |
|---:|---|
| `+0x00` | Leading word `0x0000000F` |
| `+0x04` | `0x8018416C`, `0x80183514`, `0x801836F4`, `0x80183884`, `0x80183A14`, `0x80184254` |
| `+0x1C` | First function |
| `+0x4558` | Module data, after the final `jr $ra` and its delay slot |

The module has a Splat layout and rebuilds byte-for-byte under
`make match-overlays`. `gMain_bMenuID` at `0x80184594` falls inside the
trailing data range and is zero in the image, consistent with a variable
rather than initialised content.

`Main_RunMenu` enters the image at `MainMenu_InitFrontendMenu`,
`MainMenu_UpdateFrontendMenu` and `MainMenu_DestroyFrontendMenu`.
All three now build from matching C. These frontend entries
are distinct from the Trade-screen entries below.

The loaded bytes contain resident call targets throughout `0x80180xxx` and
the module-scoped `gMain_bMenuID` at `0x80184594`. A second SU phase at sectors
`1223-1239` loads the same runtime range with the different leading word
`0x00000010` and SHA-256
`f125a2a6a8b57d222df544a7a02bf8c639c1fdde5cf978f80a56ea3fba2b836a`. It lies
outside the `0-115` request above, so its module identity is unresolved and it
must not be merged into this source scope.

Module-scoped symbol evidence is kept in
[`notes/research/Unchiga_Symbols/modules/main_menu.txt`](../../../notes/research/Unchiga_Symbols/modules/main_menu.txt).

The module has its own tracked overlay layout and matching-C manifest under
`config/slus_01411/overlays/`. It rebuilds independently from the resident
executable; main-menu entries must not be added to the resident
`config/slus_01411/matching_c.json`.

## Display-object visibility bits in the frontend

`MainMenu_InitFrontend` and `MainMenu_UpdateFrontend` toggle menu entries
between hidden and visible through the `u16` flag word at object offset `+8`,
now written with the shared names from `src/game/display_object_layout.h`:

| Site | Was | Now |
|---|---|---|
| Entry creation | `(\| 8) & ~0x40` | `DISPLAY_OBJECT_FLAG_SCREEN_SPACE`, `~DISPLAY_OBJECT_FLAG_RENDERABLE` |
| Selected-entry reveal | `\|= 0x40` | `DISPLAY_OBJECT_FLAG_RENDERABLE` |
| Visibility test | `& 0x40) != 0` | `DISPLAY_OBJECT_FLAG_RENDERABLE` |
| Entry hide | `&= 0xFFBF` | `&= ~DISPLAY_OBJECT_FLAG_RENDERABLE` |

`0xFFBF` is the 16-bit complement of `0x40`, so the hide sites are the exact
inverse of the reveal sites; writing both against one name makes that pairing
visible instead of leaving a bare mask that has to be decoded by hand.

Entries are therefore created screen-space and **not** renderable, and the
frontend reveals them by setting the renderable bit. The resident renderers
require `DISPLAY_OBJECT_RENDERABLE_MASK` (`0xC0`, renderable plus allocated)
to be fully set before drawing, so setting `0x40` alone is a reveal only for
objects the allocator has already marked.

The nearby `|= 0x28` writes are **left as numbers**: they combine `0x8` with a
`0x20` bit that `display_object_layout.h` does not define, and naming half a
composite would imply the remainder is understood. The 32-bit writes at `+4`
are a different field and are untouched.

## Frontend lifecycle

`MainMenu_InitFrontendMenu` (`0x8018001C`) retains its unused first
argument and initializes the selected byte from `menu % 11`. It allocates
three singleton objects and all eleven menu entries, seeds an entrance
transition and installs the background callback. It is not the archive
loader or input pump. Final state is not simply all zero: the transition
starter sets active byte `D_80184599` to 1, and nonzero selection sets
`D_80184597` to `0x80`. Individual allocation checks do not make the final
third-singleton access in that nonzero-selection path null-safe. The
singleton artwork and the meaning of the retained sound argument `0x7000`
remain unassigned.

`MainMenu_StartFrontendEntryTransition` (`0x80180D2C`) is the internal
endpoint initializer, not the interpolator. It skips null entries, but
always publishes the low mode byte and transition-active flag. It resets
current X to the selected starting endpoint and writes a sixteen-update
countdown, without modifying Y, visibility or selectors. Known callers use
0/1; the full-width branch and byte-narrowed mode store are not normalized
for arbitrary other integers. `frontend.h` shares this internal declaration;
no resident import is added for it.

`MainMenu_DestroyFrontendMenu` (`0x80180DD0`) releases and clears the
three singleton handles and eleven entry handles, then clears callback slot
0. The release helper handles null singleton pointers. It does not fade,
unload the archive, reset the selected/scalar menu state or release the
separate value/Trade setup. Clearing the callback has no ownership test, so
valid frontend lifecycle remains a caller precondition. The resident-facing
declarations in `entrypoints.h` are included by definitions and both callers.

The contiguous `gcc_2_8_1_g0_split` transition initializer and teardown share
[`frontend_lifecycle.c`](frontend_lifecycle.c) in executable order. Both own
the `gMain_apMenuEntries` handle set; their shared manifest source and one C
subsegment at module offset `0xD2C` cover the complete `0x140`-byte range
through `0x80180E6C`. The following afterimage lifecycle remains separate.

## Trade-screen ownership

The resident `Main_RunTrade` calls `MainMenu_InitTradeScreen` at
`0x80181F68` once, then polls `MainMenu_UpdateTradeScreen` at `0x801821DC`
after its introduction has finished and the effect state is idle.

The initializer creates the two inventory views and cursors, clears both
players' offers, navigation, sort and readiness state, populates the lists
and installs their drawing callback. It does not itself load the saves or
the archive. The updater handles both controllers, ten offered card IDs per
player, readiness, joint confirmation, staged exchange and memory-card
completion. It exchanges chest quantities, not forty-card decks.

On successful transfer completion, the updater copies the staged data back
to the working save slots and rebuilds the lists. These copies are success
handling, not evidence of cancelled-trade rollback; the code does not by
itself establish atomic persistence across two memory cards.

After the updater finishes, the resident caller performs its fades and calls
`MainMenu_ReleaseTradeDisplayHandles` (`0x80183FE4`). That helper releases
and clears only `D_801845DC` and `D_801845E0`, then clears the Trade drawing
callback. It does not itself clear inventory cursors, offers, counts or
navigation, save data, fade or unload the module. Its release helper handles
null handles.

The shared declarations are in `entrypoints.h`. Module definitions belong to
`config/slus_01411/overlays/main_menu_symbols.txt`; resident callers use
conditional linker imports in `c_symbols.ld` only after the image is loaded.
The semantic registry records these entrypoint names as `overlay/main_menu/function`,
without adding them to resident function inventory or primary symbols.

### Offer rendering and working inventory

The contiguous `gcc_2_8_1_g0_split` leaf helpers
`MainMenu_DrawCardTypeIcon` and `MainMenu_DrawTradeColumnOverlay` share
[`trade_draw_helpers.c`](trade_draw_helpers.c) in executable order. Both are
consumed by the installed Trade drawing callback and submit primitives to
`D_800E9D94`.

`MainMenu_DrawTradeOffersAndHighlights` (`0x80183B2C`) is the installed
Trade drawing callback. It animates or dims the two inventory-cursor
highlights and draws the offered card IDs as three digits plus their type
icons in two five-column offer grids. It does not mutate the offers or
render the complete inventory list.

For a ready side, the draw callback also invokes
`MainMenu_DrawTradeColumnOverlay` (`0x80184454`). It submits a grey
`POLY_F4` over the selected 160-pixel column and full 240-pixel height, using
input RGB `0x40`, priority `0x1F` and packet-helper flags 2. This describes
the packet/geometry, not a guaranteed final opaque-pixel or blend result;
known callers pass columns 0/1 and the helper does not validate the index.

The Trade working inventories use the shared four-byte
[`CardCountEntry`](../../ygo_types.h) record: signed 16-bit card ID followed
by unsigned 16-bit quantity. Five C files previously redefined that exact
layout independently; the 722-entry arrays, four-byte `qsort` stride, count
adjustment, row rebuild, and comparator accesses establish one cross-TU type.

`MainMenu_RefreshTradeInventory` (`0x8018338C`) conditionally rebuilds one
side's 722 working records from its save chest, retaining zero-ID holes.
After a rebuild it reapplies offer deductions to **both** sides, then
optionally sorts the selected row and publishes its view mode. Do not
normalize that cross-side loop: refreshing one side can deduct the other
side's outstanding offers again. This is a static observation, not a
runtime reproduction or a proposed behavior change.

The offer pass is `MainMenu_ApplyTradeOfferInventoryDelta` (`0x80184030`).
It visits each offered ID and adjusts that card's working inventory quantity,
converting the supplied signed amount to the existing unsigned modular API.
The current refresh passes `-1`. It does not remove offers or edit the offer
count; duplicate IDs would apply the amount repeatedly. All first-match,
range-rejection and missing-ID behavior belongs to the count helper below.

`MainMenu_AdjustTradeCardCount` (`0x801840F8`) finds an ID in one side's
working inventory and adds an unsigned amount. It writes only when the
modular sum is below `0xFB`, then returns after the first matching ID.
Observed callers add or subtract one: increments at 250 and decrements at
zero are rejected, not saturated. It neither removes offers nor changes
save bytes; the caller owns offer removal. The unsigned argument and
existing missing-ID/no-slot-validation behavior remain unchanged.

`MainMenu_RebuildTradeInventoryRows` (`0x801844D8`) refreshes seven visible
rows, not just the selected card. The starting address is
`inventoryBase + side * 2888 + currentTop * 4`, where `2888 = 722 * 4`.
`D_80185C8C[side][0]` is the current scrolling top; `[1]` is its target.
The cursor row is a separate offset.

The wrapper derives flags from the display selector minus 4 and passes
`flags & (1 << side)` without normalizing it: side 1 can pass 2. The resident
row helper records seven IDs, chooses row text styles, marks zero-count rows,
and updates formatting/selected-card scratch while synchronously rebuilding
the text. It does not alter the input inventory records.

[`card_list_rows.h`](../../game/card_list_rows.h) is shared by that resident
definition and this caller. Its fourth word explicitly preserves the caller's
existing full-flags setup but is ignored by the body; no additional flag
meaning or argument use is invented.

The internal declarations live in `trade_helpers.h`, separate from the
resident-facing `entrypoints.h`.

### Card comparator relations and selection

The six pointers at module `+0x04` have the following order. Negative results
place the first entry before the second; positive results place it after.
These key sequences apply **only when the original signed card IDs differ**.
The names describe the implemented relations, not displayed menu captions.

| Table index | Address | Function | Keys, in comparison order |
|---:|---|---|---|
| 0 | `0x8018416C` | `MainMenu_CompareCardsByName` | signed name key ascending, signed ID ascending |
| 1 | `0x80183514` | `MainMenu_CompareCardsByMaxStat` | max(base ATK, DEF) descending, min(base ATK, DEF) descending, name ascending |
| 2 | `0x801836F4` | `MainMenu_CompareCardsByAttack` | base ATK descending, base DEF descending, name ascending |
| 3 | `0x80183884` | `MainMenu_CompareCardsByDefense` | base DEF descending, base ATK descending, name ascending |
| 4 | `0x80183A14` | `MainMenu_CompareCardsByType` | numeric packed type ascending, name ascending |
| 5 | `0x80184254` | `MainMenu_CompareCardsByCount` | unsigned 16-bit count at entry `+2` descending, name ascending |

All six first compare the signed IDs at entry `+0`. Equal IDs return
`rand() & 1 ? 1 : -1`, including two zero IDs or an entry compared with
itself. The Count comparator takes this branch **before reading counts**,
even when equal-ID records have different counts. This is a stateful,
non-strict relation, not stable sorting or evidence of uniform shuffling.
The RNG calls and their effects must remain intact.

The name key is the signed `s16` table `gCard_asNameSortKey`
(`D_801D4D8E`), indexed by ID minus one, not a string comparison.
`gDuel_adwCardStats` (`D_801D4244`) supplies packed **base** values:
ATK is `(stats & 0x1FF) * 10`, DEF is `((stats >> 9) & 0x1FF) * 10`,
and type is `(stats >> 26) & 0x1F`; see
[`notes/card-catalog.md`](../../../notes/card-catalog.md).
MaxStat compares the larger stat, then the smaller, not ATK first or their
sum. The three stat comparators replace **both** stat keys for type 20 or
higher with signed `0x80000001` (`-2147483647`), retaining the name key.
Thus even zero-stat monsters precede non-monsters, which are ordered by
name. The Type comparator instead preserves numeric subtype order,
including Magic, Trap, Ritual and Equip at 20-23.

For distinct IDs, zero-ID slots follow valid cards. Name substitutes
`INT_MAX` for both keys; Type substitutes it for type and name. The stat
comparators use the two negative stat sentinels and `INT_MAX` name.
Count ignores a zero-ID slot's stored count, using the negative rank
sentinel and `INT_MAX` name. Valid counts zero-extend from `u16` without a
250 cap. A nonzero-ID record with count zero is **not** an empty slot:
the first five comparators never read its count. Only Name has a final ID
tie-break; the other five return zero for distinct IDs with equal effective
keys. Known inputs are ID zero or 1-722; these functions do not validate
arbitrary negative or out-of-range IDs.

`MainMenu_RefreshTradeInventory` copies the six pointers and, for nonzero
mode, passes `entries[mode - 1]` to `qsort` over 722 four-byte
`{s16 id, u16 count}` records. The inspected Trade initializer and updater
use modes 0-5: zero does not sort, and modes 1-5 select table indices 0-4.
Count is stored at index 5 but would require mode 6 through this path;
no selecting caller was established. That is a reachability caveat, not a
claim that the function is globally unused or that a sixth UI mode exists.
No direct resident callers or new resident linker imports are indicated.

The family is grouped along its two contiguous same-profile source ranges:

| Source | Functions in definition order | Complete text range | Object offsets |
|---|---|---|---|
| [`card_stat_comparators.c`](card_stat_comparators.c) | MaxStat, Attack, Defense, Type | `0x80183514-0x80183B2C` (`0x618` bytes) | `0`, `0x1E0`, `0x370`, `0x500` |
| [`card_name_count_comparators.c`](card_name_count_comparators.c) | Name, Count | `0x8018416C-0x80184344` (`0x1D8` bytes) | `0`, `0xE8` |

Both units retain `gcc_2_8_1_g0_no_sched2_split`, the original definition
order and all six individual sizes. Neither contributes data or rodata.
The intervening Trade helpers remain separate; these six functions are not
one contiguous object. Grouping reuses the common declarations without
changing keys, sentinels, RNG effects or table order. Count's signed locals
are `countKeyA` and `countKeyB`, distinguishing effective comparison keys
from the stored unsigned count. This is a source-organization choice, not
proof of the original author's translation-unit boundaries.

## Value-setup input and write-back

`MainMenu_UpdateValueSetup` (`0x801812B4`) updates the two-value/shared-option
editor, not merely the LP fields. `MainMenu_StartValueSetup`,
`MainMenu_UpdateValueSetup` and `MainMenu_FinishValueSetup` share declarations
with the resident `func_8002DC38` caller.

| Result | Meaning | Resident behavior |
|---:|---|---|
| 0 | Keep editing | Poll again |
| -1 | Cancel | Fade, call `MainMenu_FinishValueSetup`, restore the previous mode |
| 1 | Accept | Fade, call `MainMenu_FinishValueSetup`, enter the duel path |

**Cancellation is not a rollback.** The finish helper writes the target
values and normalized binary option back on either nonzero result, before
the caller distinguishes cancel from accept. The option occupies only the
low byte of the caller's wider `D_8009B230` view. Its address is explicitly
viewed as `u8 *` at the shared setup boundary, preserving the wider resident
declaration and emitted address setup.

The two twelve-byte state records hold target/display halfwords at offsets
`0/2` and `0x0C/0x0E`, with output pointers at `+4/+0x10`.
`D_801845BC[0/1]` are side modes, and `[2]` is **one shared choice**.
Modes 0/1 address its two positions; mode 2 is a side's value-bar position.
Setup initializes the choice to `(*toggle == 0)`, and finish writes
`(choice != 1)`: choice 0 writes 1, choice 1 writes 0. The visible option
caption remains unproved. The
[resident handoff](../../../notes/duel-card-record.md#per-side-view-mode-handoff)
conditionally copies the byte into per-side view state with known
card-object/text consumers; it is not itself a literal image-resource index
or a complete description of hidden-card visibility.

Busy status comes from each widget's active position callback and from any
display/target mismatch. Value rolling uses the exact absolute difference,
except display below 2 forces step 99 and differences at least 101 otherwise
cap the step at 100. An update that reaches equality still counts as busy
for that invocation. This is not a generic monotonic interpolation guarantee
for arbitrary caller-provided values.

Only when both sides are idle are newly pressed Circle/Start examined:
Circle on either pad wins over Start on either pad. These return checks
precede per-side edits and do not require leaving the shared-option mode.
Cross/Square are not confirmations here.

Idle sides then process repeat/new-press input in side-0, side-1 order.
Horizontal Left/Right has priority over mode-change directions, with Left
winning if both are present. In modes below 2 it changes the shared choice;
the other side's mode is propagated only if that mode is also below 2, without
separately checking its busy flag. Both eligible sides can edit in one call,
so side 1's later shared-choice write can win. Down enters value mode.

In value mode, Left subtracts 500 when the result stays positive, otherwise
selects 1. Right selects 500 below 2, otherwise adds 500 capped at 8000.
Up returns to the current shared-option position. Starting from the observed
8000 defaults, values are 1 or multiples of 500 through 8000; arbitrary
initial values are not rounded or validated. Target edits happen after
rolling/busy checks and start rolling on a later invocation. Position tweens
and numeric rolling remain separate, and no real-time duration is inferred.

## What the menu shows

`MainMenu_UpdateFrontendMenu` at `0x80180390` services both entry groups,
the title prompt, entry/exit animation and asynchronous load/save dialogs.
It is not merely the selection handler. `Main_RunMenu` translates completed
nonnegative menu IDs into resident modes; the returned values are not those
mode IDs themselves. `-1` keeps polling. In the separate `func_80043BCC`
caller, `-2` tears down and restarts the outer frontend loop; no particular
attract movie or timeout duration is established by that return code.

Exact matching `MainMenu_InitFrontendMenu` establishes the eleven-entry table, its `5+6`
position split, and the modulo-11 initial cursor. The
`main_menu_entry_slots` trace and player report supply the human-readable
entry labels and confirm the visible motion. The frontend presents **two**
menu groups, and `gMain_apMenuEntries` holds the entries of both:

| slots | menu | entries |
|---|---|---|
| 0-4 | before a game is loaded | New Game, Load, 2P Duel, Trade, Option |
| 5-10 | after a game is loaded | Campaign, Free Duel, Build Deck, Library, Password, Save |

The initializer loops over all eleven slots regardless of which menu is on
screen. It computes the first five y positions as `i * 32 + 50` and the
remaining six as `(i - 5) * 32 + 42`.

The resulting positions are 50, 82, 114, 146 and 178 for slots 0-4, then 42,
74, 106, 138, 170 and 202 for slots 5-10. Both use a 32 pixel pitch, and each
group is centred separately, at 114 and 122.

### Entries alternate by slot index, not by menu position

Parked x is `-160` for even slots and `480` for odd ones, across the whole
table rather than restarting per menu. That has a visible consequence:

- slot 0 is New Game, at `-160`, so the first menu enters from the **left**
- slot 5 is Campaign, at `480`, so the second menu enters from the **right**

Both were reported that way before the positions were read, and the reason
the second menu starts on the opposite side is simply that it continues the
same alternation.

### Object fields at parking time

| offset | value | meaning |
|---|---|---|
| `+0x30` | current x | initialized from `+0x36`, then interpolated toward `+0x38` |
| `+0x32` | entry y | separately initialized for the entry group |
| `+0x36` | start x | parked `-160`/`480` on entrance; centered `160` on exit |
| `+0x38` | end x | centered `160` on entrance; parked `-160`/`480` on exit |
| `+0x60` | `0x10` | sixteen-update animation countdown |
| `+0x08` | `0x0088` | flags |
| `+0x0C` | `0x808080` | colour, mid grey |

Both modes animate x at `+0x30`; `+0x36` and `+0x38` are not separate
axes. `MainMenu_StartFrontendEntryTransition` selects entrance for zero and exit for nonzero by
swapping the parked and centered endpoints. The sample's `D_80184596 = 0`
and `+0x38 = 160` therefore identify the entrance setup, not a
horizontal-versus-vertical switch.

## The cursor indexes the slot table

`gMain_bMenuID` at `0x80184594` sits immediately after the eleven pointers,
which occupy `0x80184568`-`0x80184593`. The two use the same numbering:

| cursor | slots | menu |
|---|---|---|
| 0-4 | 0-4 | New Game, Load, 2P Duel, Trade, Option |
| 5-10 | 5-10 | Campaign, Free Duel, Build Deck, Library, Password, Save |

Matching `MainMenu_InitFrontendMenu` reduces its incoming menu value modulo 11 and uses
`gMain_bMenuID != i` while configuring each corresponding entry. The cursor
range and label mapping were also established independently by reading the
byte live while moving the highlight: see `F1` and `F18` in
[`../../../notes/research/Unchiga_Symbols/findings.md`](../../../notes/research/Unchiga_Symbols/findings.md),
which proved `CAMPAIGN=5` and `SAVE=10` and named the continuation past 0-4.

The slot table was established separately, from the parked positions above.
That the two agree on both the range and the split — and that the table ends
exactly where the cursor byte begins — is what ties them together: the cursor
selects an entry by indexing this array.

That is what justifies the name `gMain_apMenuEntries`, declared in this
module's symbol file beside `gMain_bMenuID` and recorded in
[`../../../notes/semantic-symbol-map.csv`](../../../notes/semantic-symbol-map.csv).
The password module sets the precedent: `gPassword_abDigits` is named the same
way, in its own module file.

## Native background packet layouts

`MainMenu_DrawFrontendBackground` (`0x80180B4C`) builds its background
layers with Psy-Q `POLY_F4`,
`POLY_FT4`, and `POLY_G4` records. Their payload lengths are 5, 9, and 8
words, excluding the tag; `setPolyF4`, `setPolyFT4`, and `setPolyG4` supply
the corresponding packet headers. `MainMenu_DrawTradeColumnOverlay`
(`0x80184454`) uses the same native
`POLY_F4` record for its column-sized quad.

The background's screen-coordinate limits use the documented default
`320 x 240` dimensions. Its `getClut(0, 244)` spelling identifies the same
palette row previously packed as `0x3D00`; see the
[tutorial-backed palette evidence](../../../notes/modding-tutorial-evidence.md).
Texture UV limits and the other geometry values are not reinterpreted as
screen constants. Submission order, colors, flags and local value lifetimes
retain the matching behavior.

The optional grey quad uses `D_80184597` as all three input color bytes.
It is submitted at priority 0, followed by five 64-pixel textured columns
at priority 4095 and a black-top/white-bottom Gouraud quad at 4094.
These are submission facts, not a claim about final opaque pixel order.
Texture U is derived from each column's X modulo 256; this body does not
advance UVs with time, so the older "scrolling background" interpretation
is not established here.

The background tiles, three-digit renderer, card-type icon and value-editor bars
submit through the SDK's `GsSortPoly` at `0x80084320`, previously
`func_80084320`. The complete Psy-Q 4.6 `2D_PRIM.OBJ` signature matches
uniquely; see the [SDK evidence](../../../notes/psyq.md).
These callers share the canonical `libgs.h` interface with `GsOT *` and an
unsigned-halfword priority rather than maintaining local prototypes. This
SDK entry remains assembly and is distinct from the game-owned packet helper
`func_8005B260`. `main_menu_linker_symbols.txt` supplies its resident import;
it is not an additional overlay function.

## Frontend entry afterimages

`MainMenu_UpdateFrontendMenu` calls
`MainMenu_SpawnFrontendEntryAfterimage` (`0x80180E6C`) on odd elapsed
transition updates for visible entries, after writing their current X.
The helper allocates a list/type-2 object and configures it from the source's
current signed XY and resource selector byte `+0x69`. It copies the source
RGB bytes, not the complete object, and uses the signed-byte conversion of
the negated timer low byte for its depth adjustment. Allocation failure
skips the copy. It neither moves the source nor creates another menu entry.

The installed `MainMenu_UpdateFrontendEntryAfterimage` (`0x80180F50`)
subtracts 8 from each RGB byte with independent zero clamping. The release
path runs only when a callback starts with all low 24 color bits already
zero; reaching black during the current update does not release the object
until the next callback. An initially black copy releases immediately.
The fourth packed byte is ignored, and this does not establish a separate
alpha fade or a fixed lifetime for every starting color.

`frontend.h` declares both helpers and their one-argument update type,
included by definitions and callers. The release call explicitly passes the
object instead of relying on an unwritten argument-register assumption.

Producer and update callback share `frontend_entry_afterimages.c`, in that
definition order. Their contiguous `0xE4 + 0x88` bytes cover
`0x80180E6C-0x80180FD8` exactly, with callback object offset `0xE4`.
Both retain `gcc_2_8_1_g0_split` and contribute no data or rodata. This
coalesces one effect lifecycle without claiming the original author's
translation-unit boundary or absorbing the following value setup.

## Value-setup visuals translation unit

`value_setup_visuals.c` keeps the value-editor renderer next to the widget
position tween it drives. Both read the same displayed-value halfword table
`D_801845C0` and the same per-side mode array `D_801845BC`, and both work in
the value-lane geometry described below: the tween drives a side widget
toward the bar endpoint at Y 111/139 that the renderer draws, and it writes
the settled mode back into `D_801845BC` when the tween expires.

The definitions remain in executable order: `MainMenu_DrawValueSetup`
occupies `0x80181728..0x80181CB8` and `MainMenu_UpdateValueWidgetTween` runs
through `0x80181E30`. Both use `gcc_2_8_1_g0_split`, and their shared
manifest source and one C subsegment at module offset `0x1728` cover the
complete contiguous `0x708`-byte text range, ending exactly where
`MainMenu_FinishValueSetup` begins.

This is the **visual** slice of the value editor only. It is not the whole
editor: `MainMenu_StartValueSetup`, `MainMenu_FinishValueSetup` and the input
and write-back routines remain separate units. The two files kept distinct
views of neighbouring globals — the renderer's `ValueWidgetView *D_801845B0[]`
and the tween's `u8 *D_801845B8` — and those are preserved as-is rather than
reconciled into one array access, because no evidence establishes that
`D_801845B8` is an element of `D_801845B0`.

## Value-editor rendering

`MainMenu_DrawValueSetup` (`0x80181728`) is the callback installed by
`MainMenu_StartValueSetup` and removed by `MainMenu_FinishValueSetup`.
The old filename `starchip_bars.c` did not establish a currency role: this
renders the two-value/shared-option editor, with 2P life points as its known
caller context.

It reads the **displayed** `u16` values at `D_801845C0[1]` and `[7]`,
not the targets at `[0]` and `[6]`. It places the shared marker at X 116
for choice zero or 220 otherwise, with Y 74. A side widget remains visible
while its update callback is active or its mode equals 2. During an active
position tween the renderer leaves that widget's XY alone; an idle value-mode
widget follows its displayed bar endpoint at Y 111/139. Otherwise it hides
the side widget and copies the shared marker position.

Regardless of side mode, it submits both `POLY_G4` bars, then both runs of
`POLY_GT4` digits through `GsSortPoly` at ordering-table entry 2, priority
2048. The bars run from X 176 to `176 + display * 128 / 8000`, at
Y 107-115 and 135-143, without clamping. The first has a red input-color
gradient and the second a blue one.

Digit alignment uses `MainMenu_CountDecimalDigits(8000)` once, giving a
four-digit alignment basis and a fixed right edge of X 166. The digit count
is independently calculated from each displayed value: zero draws one digit,
there is no leading-zero padding, and five-digit `u16` values are not
truncated. Digits are emitted least significant/rightmost first, at
Y 106-114 or 134-142. This differs from the fixed three-digit Trade renderer
below.

`value_setup.h` shares the draw and digit-count declarations with their
definitions and installer. The local `ValueWidgetView` preserves only the
accessed prefix; its callback word is tested, never called by this renderer.
The source retains the named per-loop constants and pointer rereads that
control old-GCC allocation. It does not modify the target/display values,
shared choice, modes, input, save data or any currency balance.

## Card-number and type rendering

`MainMenu_DrawThreeDigitNumber` (`0x80183E8C`) draws exactly three
`8 x 8` digit quads. For nonnegative input it shows the lowest three decimal
digits, including leading zeros; current Trade callers pass card IDs. The
least significant digit is submitted first, on the right, then the other
two proceed leftward in eight-pixel steps. The full horizontal span is
`x-4..x+20`, at `y+4..y+12`. It does not clamp values, draw a minus sign,
or validate arbitrary negative input. Signed division/remainder and byte
UV narrowing retain the original behavior.

Both renderers use native Psy-Q `POLY_FT4` records and `setPolyFT4`, whose
nine-word payload excludes the tag and whose GPU code is `0x2C`.
`trade_helpers.h` is shared by their definitions and the offer renderer;
there are no independent local declarations to drift.

`MainMenu_DrawCardTypeIcon` (`0x80184344`) draws the small 16 by 16 marker
for a card. It reads the
card's packed stat word from `D_801D4244` at `id - 1`, takes the type from
bits 26-30, and builds a 40-byte textured quad — length 9, GPU code `0x2C`,
grey `0x80`, texture page `0xB`, `u` `0`-`0x10` and `v` `0xC8`-`0xD8` — which
it submits through `GsSortPoly`.

Only the palette changes with the type, so all four kinds share one texture:

| type | card kind | CLUT column |
|---|---|---|
| 0-19 | monster | `0x260` |
| 20, 23 | Magic, Equip | `0x270` |
| 21 | Trap | `0x280` |
| 22 | Ritual | `0x290` |

The type ids are the ones documented in
[`../../../notes/research/the-game.md`](../../../notes/research/the-game.md):
twenty monster types followed by Magic, Trap, Ritual and Equip. Magic and
Equip sharing a colour is consistent with the game, where equips are drawn as
green magic cards. The implementation also falls back to column `0x260` for
other numeric type values; it does not validate card IDs or invent a separate
graphic for each monster subtype. Only the palette changes, not the texture.
