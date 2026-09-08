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

`Main_RunMenu` enters the image at `func_8018001C`, `MainMenu_UpdateFrontendMenu` and
`func_80180DD0`. All three now build from matching C. These frontend entries
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

The shared declarations are in `entrypoints.h`. Module definitions belong to
`config/slus_01411/overlays/main_menu_symbols.txt`; resident callers use
conditional linker imports in `c_symbols.ld` only after the image is loaded.
The semantic registry records both names as `overlay/main_menu/function`,
without adding them to resident function inventory or primary symbols.

### Offer rendering and working inventory

`MainMenu_DrawTradeOffersAndHighlights` (`0x80183B2C`) is the installed
Trade drawing callback. It animates or dims the two inventory-cursor
highlights and draws the offered card IDs as three digits plus their type
icons in two five-column offer grids. It does not mutate the offers or
render the complete inventory list.

`MainMenu_RefreshTradeInventory` (`0x8018338C`) conditionally rebuilds one
side's 722 working records from its save chest, retaining zero-ID holes.
After a rebuild it reapplies offer deductions to **both** sides, then
optionally sorts the selected row and publishes its view mode. Do not
normalize that cross-side loop: refreshing one side can deduct the other
side's outstanding offers again. This is a static observation, not a
runtime reproduction or a proposed behavior change.

`MainMenu_AdjustTradeCardCount` (`0x801840F8`) finds an ID in one side's
working inventory and adds an unsigned amount. It writes only when the
modular sum is below `0xFB`, then returns after the first matching ID.
Observed callers add or subtract one: increments at 250 and decrements at
zero are rejected, not saturated. It neither removes offers nor changes
save bytes; the caller owns offer removal. The unsigned argument and
existing missing-ID/no-slot-validation behavior remain unchanged.

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

## What the menu shows

`MainMenu_UpdateFrontendMenu` at `0x80180390` services both entry groups,
the title prompt, entry/exit animation and asynchronous load/save dialogs.
It is not merely the selection handler. `Main_RunMenu` translates completed
nonnegative menu IDs into resident modes; the returned values are not those
mode IDs themselves. `-1` keeps polling. In the separate `func_80043BCC`
caller, `-2` tears down and restarts the outer frontend loop; no particular
attract movie or timeout duration is established by that return code.

Exact matching `func_8018001C` establishes the eleven-entry table, its `5+6`
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
axes. `func_80180D2C` selects entrance for zero and exit for nonzero by
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

Matching `func_8018001C` reduces its incoming menu value modulo 11 and uses
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

`func_80180B4C` builds its background layers with Psy-Q `POLY_F4`,
`POLY_FT4`, and `POLY_G4` records. Their payload lengths are 5, 9, and 8
words, excluding the tag; `setPolyF4`, `setPolyFT4`, and `setPolyG4` supply
the corresponding packet headers. `func_80184454` uses the same native
`POLY_F4` record for its column-sized quad.

The background's screen-coordinate limits use the documented default
`320 x 240` dimensions. Its `getClut(0, 244)` spelling identifies the same
palette row previously packed as `0x3D00`; see the
[tutorial-backed palette evidence](../../../notes/modding-tutorial-evidence.md).
Texture UV limits and the other geometry values are not reinterpreted as
screen constants. Submission order, colors, flags and local value lifetimes
retain the matching behavior.

The background tiles, three-digit renderer, card-type icon and starchip bars
submit through the SDK's `GsSortPoly` at `0x80084320`, previously
`func_80084320`. The complete Psy-Q 4.6 `2D_PRIM.OBJ` signature matches
uniquely; see the [SDK evidence](../../../notes/psyq.md).
These callers share the canonical `libgs.h` interface with `GsOT *` and an
unsigned-halfword priority rather than maintaining local prototypes. This
SDK entry remains assembly and is distinct from the game-owned packet helper
`func_8005B260`. `main_menu_linker_symbols.txt` supplies its resident import;
it is not an additional overlay function.

## The card type icon

`func_80184344` draws the small 16 by 16 marker for a card. It reads the
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
green magic cards.
