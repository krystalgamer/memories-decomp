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
