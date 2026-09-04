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
func_80014E1C(1, &D_800117C8, 0, 0x73, func_8005B64C, 0, 0);
```

`D_800117C8` is the development path literal `M:/mrgSU/SU.mrg`, so this form
selects a named archive instead of the WA screen packages. The third and
fourth arguments remain the first sector and the sector count.

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
`func_80180DD0`; the latter two begin with `addiu $sp, $sp, -0x28` and
`addiu $sp, $sp, -0x20` at their exact loaded offsets.

The loaded bytes contain resident call targets throughout `0x80180xxx` and
the module-scoped `gMain_bMenuID` at `0x80184594`. A second SU phase at sectors
`1223-1239` loads the same runtime range with the different leading identifier
`0x00000010` and SHA-256
`f125a2a6a8b57d222df544a7a02bf8c639c1fdde5cf978f80a56ea3fba2b836a`. It lies
outside the `0-115` request above, so its module identity is unresolved and it
must not be merged into this source scope.

Module-scoped symbol evidence is kept in
[`notes/research/Unchiga_Symbols/modules/main_menu.txt`](../../../notes/research/Unchiga_Symbols/modules/main_menu.txt).

No overlay source or build manifest is accepted yet. Keep extracted payloads,
candidate sources, objects, and diffs under `tmp/` until a function passes an
overlay-specific exact-match process. Do not add this module to the resident
`config/slus_01411/matching_c.json`.

## What the menu shows

Established by the `main_menu_entry_slots` trace together with the player
report that produced it. The module drives **two** menus, not one, and
`D_80184568` holds the entries of both:

| slots | menu | entries |
|---|---|---|
| 0-4 | before a game is loaded | New Game, Load, 2P Duel, Trade, Option |
| 5-10 | after a game is loaded | Campaign, Free Duel, Build Deck, Library, Password, Save |

Five plus six is eleven, which is why every function that touches the table
iterates all eleven regardless of which menu is on screen.

Measured y positions confirm the split: slots 0-4 sit at 50, 82, 114, 146 and
178, and slots 5-10 at 42, 74, 106, 138, 170 and 202. Both use a 32 pixel
pitch, and each group is centred separately, at 114 and 122.

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

## Module header

The first bytes at `0x80180000` are the module identifier `0x0000000F`
followed by pointers:

```
0f 00 00 00  6c 41 18 80  14 35 18 80  f4 36 18 80
             0x8018416C   0x80183514   0x801836F4
```

`0x8018416C` is a function in this module's inventory, so the header carries
an entry-point table rather than data.
