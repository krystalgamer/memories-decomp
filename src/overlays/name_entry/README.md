# Name Entry Overlay

This directory is reserved for matching source from the name-entry runtime
module.

Verified scope:

| Item | Range |
|---|---|
| WA archive package | sectors `7903-7983` (80 sectors) |
| WA executable phase | sectors `7968-7983`, `0x7800` bytes |
| Runtime code range | `0x80168000-0x8016F800` |
| Known module symbols | `0x801680F4-0x8016D4D2` |

The package is recovered from the resident loader trace. `Main_RunNameEntry`
(`0x8002D62C`) calls `func_8003BBF8`, whose request is
`func_80014E1C(0, 0, 0x1EDF, 0x50, func_8003BA14, 0, 0)`. The third and fourth
arguments are the first WA sector and the sector count, which the two
already-verified screens confirm: Free Duel uses `(0x1E88, 0x57)` for
`7816-7903` and the password screen uses `(0x1F2F, 0x56)` for `7983-8069`.

## Shared image with the password screen

The `0x80168000` phase of this package is **not** a distinct module. Its
`0x7800` bytes are byte-identical to the password package's phase at sectors
`8054-8069` except inside the final sector, where the first difference is at
phase offset `0x7326`. Both images carry module identifier `0x00000015`.

| Phase | Sectors | SHA-256 |
|---|---|---|
| Name entry package | `7968-7983` | `b751bfdc256a0941d088f5a47bc0db2638edced777071c5b0b7214b044259521` |
| Password package | `8054-8069` | `6925509db80d4ff8869f41350f0256524f5ff15cb9b3bf2f6dc7351a7caf3d55` |

The resident callers enter the same image at different points, all inside
`0x80168000-0x8016F800`:

| Screen | Resident mode | Overlay entry points |
|---|---|---|
| Name entry | `Main_RunNameEntry` | `func_801683EC`, `func_80169C08` |
| Password | `Main_RunPasswordMenu` | `func_8016A080`, `func_8016A37C` |

Treat name entry as an entry point into the shared front-end screen image
rather than as an independent code module. Matching source for that image
belongs to one module scope, not two, so do not create a second copy of the
same functions here.

Module-scoped symbol evidence is kept in
[`notes/research/Unchiga_Symbols/modules/name_entry.txt`](../../../notes/research/Unchiga_Symbols/modules/name_entry.txt).
These symbols are medium-confidence evidence because several modules reuse
the same `0x80168xxx` addresses.

No overlay source or build manifest is accepted yet. Keep extracted payloads,
candidate sources, objects, and diffs under `tmp/` until a function passes an
overlay-specific exact-match process. Do not add this module to the resident
`config/slus_01411/matching_c.json`.
