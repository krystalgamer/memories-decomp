# Overworld Overlay

This directory is reserved for matching source from the Egypt overworld
runtime module.

Verified variants:

| Variant | WA package | Executable phase | Runtime code range |
|---|---|---|---|
| Before Heishin's coup | sectors `8153-8311` | sectors `8153-8159`, `0x3000` bytes | `0x80168000-0x8016B000` |
| After Heishin's coup | sectors `8311-8469` | sectors `8311-8317`, `0x3000` bytes | `0x80168000-0x8016B000` |

Both executable phases are registered in `config/slus_01411/overlays.json` and
are extracted and hash-verified by `make overlays` and `make verify-overlays`:

| Variant | Module name | Verified SHA-256 |
|---|---|---|
| Before Heishin's coup | `overworld_before_coup` | `9a72a17c1c1716dd799d94c3ab9233b26b87797bfa8b123d7c9d631f8405553f` |
| After Heishin's coup | `overworld_after_coup` | `d1c4f8299d526c8d45911a255bb8388abd14722cb4141df383f4fdb369875dc5` |

Each image begins with a four-byte module identifier followed by the first
function prologue at `0x80168004`.

The resident loader selects the second package when campaign flag `0x47` is
set. Both variants reuse the same addresses, so candidate payloads and exact
matches must retain their variant identity even when a function appears
unchanged between them.

The two images share byte-identical text: `0x0004-0x1E54` is the same in both,
and the first difference is at `0x22CA`, inside the trailing module data. Their
function inventories are therefore identical, and one matching source can
serve both modules through an entry in each module's `_matching_c.json`.

Module-scoped symbol evidence is kept in
[`notes/research/Unchiga_Symbols/modules/overworld.txt`](../../../notes/research/Unchiga_Symbols/modules/overworld.txt).

No overlay source or Splat build layout is accepted yet. Keep extracted payloads,
candidate sources, objects, and diffs under `tmp/` until a function passes an
overlay-specific exact-match process. Do not add either variant to the
resident `config/slus_01411/matching_c.json`.
