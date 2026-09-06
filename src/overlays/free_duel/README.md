# Free Duel Overlay

This directory is reserved for matching source from the Free Duel runtime
module.

Verified boundaries:

| Item | Range |
|---|---|
| WA package | sectors `7816-7903` |
| Executable phase | sectors `7898-7903`, `0x2800` bytes |
| Runtime code range | `0x80168000-0x8016A800` |

The package also contains phases loaded elsewhere; the five-sector executable
phase is not a complete inner-file manifest. Module-scoped symbol evidence is
kept in
[`notes/research/Unchiga_Symbols/modules/free_duel.txt`](../../../notes/research/Unchiga_Symbols/modules/free_duel.txt).

The verified executable phase is tracked in
[`config/slus_01411/overlays.json`](../../../config/slus_01411/overlays.json).
`make overlays` extracts it to `tmp/overlays/free_duel/module.bin`, and
`make verify-overlays` checks the archive and payload hashes.

Its accepted build layout is
[`config/slus_01411/overlays/free_duel.yaml`](../../../config/slus_01411/overlays/free_duel.yaml).
The adjacent `_functions.csv` inventory tracks per-function status, and
`free_duel_matching_c.json` maps accepted source/profile pairs.
`make match-overlays` remains the complete module's exact-byte gate.

Keep unaccepted candidate sources, objects, and diffs under `tmp/`. Do not add
this module to the resident `config/slus_01411/matching_c.json`.
