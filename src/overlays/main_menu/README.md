# Main Menu Overlay

This directory is reserved for matching source from the main-menu runtime
module.

Verified boundaries:

| Item | Range |
|---|---|
| SU executable phase | sectors `98-114`, `0x8000` bytes |
| Runtime code range | `0x80180000-0x80188000` |

The loaded bytes contain resident call targets throughout `0x80180xxx` and
the module-scoped `gMain_bMenuID` at `0x80184594`. A second SU phase at sectors
`1223-1239` loads the same runtime range with a different leading identifier;
its module identity is unresolved and it must not be merged into this source
scope.

Module-scoped symbol evidence is kept in
[`notes/research/Unchiga_Symbols/modules/main_menu.txt`](../../../notes/research/Unchiga_Symbols/modules/main_menu.txt).

No overlay source or build manifest is accepted yet. Keep extracted payloads,
candidate sources, objects, and diffs under `tmp/` until a function passes an
overlay-specific exact-match process. Do not add this module to the resident
`config/slus_01411/matching_c.json`.
