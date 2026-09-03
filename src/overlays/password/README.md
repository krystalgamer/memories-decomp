# Password Overlay

This directory is reserved for matching source from the password-screen
runtime module.

Verified boundaries:

| Item | Range |
|---|---|
| WA package | sectors `7983-8069` |
| Executable phase | sectors `8054-8069`, `0x7800` bytes |
| Runtime code range | `0x80168000-0x8016F800` |

The main-menu package contains a closely related `0x7800` phase at sectors
`7968-7983`. The two chunks carry the same password-shop code but differ in
their final data sector, so they must not be treated as one interchangeable
module image.

Module-scoped symbol evidence is kept in
[`notes/research/Unchiga_Symbols/modules/password.txt`](../../../notes/research/Unchiga_Symbols/modules/password.txt).

No overlay source or build manifest is accepted yet. Keep extracted payloads,
candidate sources, objects, and diffs under `tmp/` until a function passes an
overlay-specific exact-match process. Do not add this module to the resident
`config/slus_01411/matching_c.json`.
