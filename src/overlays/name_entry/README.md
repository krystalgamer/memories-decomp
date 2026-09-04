# Name Entry Overlay

This directory is reserved for matching source from the name-entry runtime
module.

Verified scope:

| Item | Range |
|---|---|
| WA screen load bank | `0x80168000-0x8017A000` |
| Known module symbols | begin at `0x8016868C` |

The archive package and exact loaded code extent have not yet been recovered
from the resident loader trace. Do not assign a WA sector range or treat the
entire screen bank as name-entry code until its load request and disc payload
are verified.

Module-scoped symbol evidence is kept in
[`notes/research/Unchiga_Symbols/modules/name_entry.txt`](../../../notes/research/Unchiga_Symbols/modules/name_entry.txt).
These symbols are medium-confidence evidence because several modules reuse
the same `0x80168xxx` addresses.

No overlay source or build manifest is accepted yet. Keep extracted payloads,
candidate sources, objects, and diffs under `tmp/` until a function passes an
overlay-specific exact-match process. Do not add this module to the resident
`config/slus_01411/matching_c.json`.
