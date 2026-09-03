# Overworld Overlay

This directory is reserved for matching source from the Egypt overworld
runtime module.

Verified variants:

| Variant | WA package | Executable phase | Runtime code range |
|---|---|---|---|
| Before Heishin's coup | sectors `8153-8311` | sectors `8153-8159`, `0x3000` bytes | `0x80168000-0x8016B000` |
| After Heishin's coup | sectors `8311-8469` | sectors `8311-8317`, `0x3000` bytes | `0x80168000-0x8016B000` |

The resident loader selects the second package when campaign flag `0x47` is
set. Both variants reuse the same addresses, so candidate payloads and exact
matches must retain their variant identity even when a function appears
unchanged between them.

Module-scoped symbol evidence is kept in
[`notes/research/Unchiga_Symbols/modules/overworld.txt`](../../../notes/research/Unchiga_Symbols/modules/overworld.txt).

No overlay source or build manifest is accepted yet. Keep extracted payloads,
candidate sources, objects, and diffs under `tmp/` until a function passes an
overlay-specific exact-match process. Do not add either variant to the
resident `config/slus_01411/matching_c.json`.
