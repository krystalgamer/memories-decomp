# Main Menu Overlay

This directory is reserved for matching source from the main-menu runtime
module.

Verified boundaries:

| Item | Range |
|---|---|
| Archive | `game/DATA/SU.MRG` (1,239 sectors) |
| SU load request | sectors `0-115` |
| SU executable phase | sectors `98-114`, `0x8000` bytes |
| Runtime code range | `0x80180000-0x80188000` |
| Module identifier | `0x0000000F` |
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

The phase does not begin with code. Its first word is the module identifier,
followed by a six-entry pointer table, with the first instruction at `+0x1C`:

| Offset | Contents |
|---:|---|
| `+0x00` | Module identifier `0x0000000F` |
| `+0x04` | `0x8018416C`, `0x80183514`, `0x801836F4`, `0x80183884`, `0x80183A14`, `0x80184254` |
| `+0x1C` | First function |
| `+0x4558` | Module data, after the final `jr $ra` and its delay slot |

The module has a Splat layout and rebuilds byte-for-byte under
`make match-overlays`. `gMain_bMenuID` at `0x80184594` falls inside the
trailing data range and is zero in the image, consistent with a variable
rather than initialised content.

`Main_RunMenu` enters the image at `func_8018001C`, `func_80180390` and
`func_80180DD0`. The initializer and teardown now build from matching C;
`func_80180390`, the selection/update path, remains mapped assembly.

The loaded bytes contain resident call targets throughout `0x80180xxx` and
the module-scoped `gMain_bMenuID` at `0x80184594`. A second SU phase at sectors
`1223-1239` loads the same runtime range with the different leading identifier
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

## What the menu shows

Exact matching `func_8018001C` establishes the eleven-entry table, its `5+6`
position split, and the modulo-11 initial cursor. The
`main_menu_entry_slots` trace and player report supply the human-readable
entry labels and confirm the visible motion. The module drives **two** menus,
not one, and `gMain_apMenuEntries` holds the entries of both:

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
| `+0x30`, `+0x32` | parked x, y | position, copied from `+0x36` by `func_80180D2C` |
| `+0x36`, `+0x38` | `-160`/`480`, `160` | the moving axis and the pinned one |
| `+0x60` | `0x10` | the value `func_80180D2C` writes |
| `+0x08` | `0x0088` | flags |
| `+0x0C` | `0x808080` | colour, mid grey |

`D_80184596` read `0` in the sample and `+0x38` held the pinned `0xA0`, so
argument `0` selects **horizontal** movement.

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

## The card type icon

`func_80184344` draws the small 16 by 16 marker for a card. It reads the
card's packed stat word from `D_801D4244` at `id - 1`, takes the type from
bits 26-30, and builds a 40-byte textured quad — length 9, GPU code `0x2C`,
grey `0x80`, texture page `0xB`, `u` `0`-`0x10` and `v` `0xC8`-`0xD8` — which
it submits through `func_80084320`.

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
