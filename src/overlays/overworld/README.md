# Overworld Overlay

This directory contains matching source from the Egypt overworld
runtime module.

Verified variants:

| Variant | WA package | Executable phase | Loaded image range |
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

The two images share a byte-identical code-and-data prefix: offsets
`0x0004-0x1E54` include code **and** the intervening active table/state span,
not one continuous text section. The first difference is at `0x22CA`, inside
trailing module data. Their
function inventories are therefore identical, and one matching source can
serve both modules through an entry in each module's `_matching_c.json`.

Module-scoped symbol evidence is kept in
[`notes/research/Unchiga_Symbols/modules/overworld.txt`](../../../notes/research/Unchiga_Symbols/modules/overworld.txt).

Both variants now have accepted Splat layouts under
`config/slus_01411/overlays/` and share the matching sources in this directory.
Their `_functions.csv` inventories track per-function status, while their
separate `_matching_c.json` manifests map accepted source/profile pairs.
`make match-overlays` remains the exact-byte gate.

## Camera-state translation unit

`camera_state.c` keeps the location-camera loader next to the per-frame view
publisher, the default-camera reset, and a free-look-shaped D-pad routine. All
four operate on `D_800F2848`; the loader, reset, and D-pad routine rebuild its
derived matrix through `func_8001352C`.

The definitions remain in executable order:
`CampaignMap_SetCameraFromLocation` occupies
`0x801681E8..0x80168258`, followed by `CampaignMap_UpdateView` and
`CampaignMap_ResetCamera`, then `CampaignMap_MoveCameraDpad` through
`0x80168588`. All four use `gcc_2_8_1_g0_split`. One C subsegment at module
offset `0x1E8` covers the complete contiguous `0x3A0`-byte text range in both
verified variants.

`CampaignMap_MoveCameraDpad` has no established live dispatch. The active
`CampaignMap_UpdateLocation` path does not call it, and an aligned-word scan of
the resident executable plus all five verified module images found neither a
direct `jal` nor a stored `0x80168388` pointer. Its input and camera writes are
therefore a static function contract; computed or external entry remains an
open question.

## Location-object translation unit

`location_objects.c` keeps the four-slot cleanup helper next to the rebuild
routine that invokes it before creating the selected location's enabled
objects. Both operate on `D_801695F8`.

The definitions remain in executable order:
`CampaignMap_ClearLocationObjects` occupies
`0x80168004..0x80168050`, followed by
`CampaignMap_RebuildLocationObjects` through `0x8016818C`. Both use
`gcc_2_8_1_g0_split`. One C subsegment at module offset `0x4` covers the
complete contiguous `0x188`-byte text range in both verified variants.

## Active and alternate location families

The resident `Main_RunCampaignMap` uses the active thirteen-function family:
it initializes `CampaignMap_SetLocation` and polls
`CampaignMap_UpdateLocation` (`0x80168FCC`), which uses
`CampaignMap_PickExit` (`0x80168E0C`).

Two later functions are a second, alternate-state copy **within each image**:

| Address | Mechanical name | Existing profile |
|---|---|---|
| `0x801697D0` | `CampaignMap_PickAlternateExit` | `gcc_2_8_1_o0_g0_no_split` |
| `0x80169A9C` | `CampaignMap_UpdateAlternateLocation` | `gcc_2_8_1_o0_g0_split` |

"Alternate" does not mean the after-coup variant. These names describe
complete local selection/control behavior; they do not establish live
callability, a debug mode or provenance from a particular older build.
The functions keep their different profiles and are not grouped into a
single same-profile translation unit.

**Their raw calls must not be rebound to active helpers.** Several addresses
currently enter function interiors, and one enters location-table data:

| Raw target | Current image owner |
|---|---|
| `0x80169230` | Active location-table data at `0x801691A8 + 0x88` |
| `0x801680E4` | `CampaignMap_RebuildLocationObjects + 0x94` |
| `0x801682D0` | `CampaignMap_UpdateView + 0x78` |
| `0x80168A48` | `CampaignMap_StartCameraTween + 0x18C` |
| `0x80168624` | `CampaignMap_CreateLocationMarker + 0x9C` |
| `0x80168040` | `CampaignMap_ClearLocationObjects + 0x3C` |
| `0x8004EB9C` | `func_8004EB00 + 0x9C`, not the current story-flag tester |
| `0x80065B24` | `func_80065928 + 0x1FC`, not an established sound API |
| `0x80066574` | `func_80066564 + 0x10` |
| `0x80021EA4` | `func_800218F0 + 0x5B4` |
| `0x80065BFC` | `func_80065BCC + 0x30` |
| `0x800158C8` | `Fade_InitOut + 0x10` |

Those targets are present in the actual encoded JAL instructions, not merely
unfortunate source names. Similarity to the active family's call sequence is
not evidence that they currently allocate, release, fade, play sound or
move the camera. The local contracts below are conditional on opaque calls
returning as their declarations describe.

### Alternate local contracts

The picker indexes a separate sixteen-record, 66-byte location table and
checks four twelve-byte exits in stored order. Its special high-index route
precedes confirmation, which precedes directional-mask matching. Disabled
destination 16 skips the exit predicate; first matching record wins.
The header-gate branch passes the **first exit's gate**, not the header word,
to the raw predicate. It returns `-1`, a destination byte, or
`currentIndex | 0x8000` to request entry. Movement-parameter writes and gate
high bits are preserved, without assigning verified time units or button
captions to the alternate values.

The controller's visibility-mask work precedes its transition gate.
After a completed transition, its marker-shaped call occurs for indices
**at least 10**; lower indices call the raw cleanup-shaped target and clear
the pointer. A separate control gate can defer pending entry work.
The pending-entry path retains its initialization bit, unchecked pointer
uses and unclamped `u16` decrement. Its mask-`0x800` mode copy does not
return before the subsequent picker call. An encoded entry result sets a
pending byte rather than storing the encoded word as a new location.

The alternate input-like words at `0x800C4E68/0x800C4E74` have no established
producers here. Their resemblance to active pressed/held streams is not a
reason to substitute the accepted input globals or pad constants.

No tracked direct C caller or callback installation of the outer controller
was found. Neither module has a direct J/JAL or aligned literal pointer to
`0x80169A9C`; only the call at `0x80169DA8` reaches the paired picker.
This is bounded static evidence, not whole-program dead-code proof or
permission to delete the functions. No resident entrypoint imports are added.

### Variant and table evidence

The combined alternate code span, offsets `0x17D0-0x1E54`, is identical in
both verified images, with SHA-256
`206aa45ed4723d51336cbd1507c43fa0cbdc87a8233b6dc8bf1851cc9d643ac7`.
The alternate table at `0x1E54-0x2274` is identical to the active table at
`0x11A8-0x15C8`, and to both variant copies, with SHA-256
`0259a2516466fd98262c69a1f4da03d22fec71b9f4e125adea7925fe35220c12`.
The following state bytes through `0x22C9` are initially zero in both.
Variant differences start afterward. Identical local bytes do not prove
identical usable runtime behavior when raw callees and external state remain
unresolved.

Project-owned names live in `overworld_symbols.txt`, included by both layouts
alongside the unchanged imported reference file. The semantic registry has
one `overlay/<variant>/function` record per name in each variant. The
internal `alternate_location.h` shares only the paired declarations; it
does not turn this family into supported active-map entrypoints.

## Inventory status

All fifteen inventoried functions in both variants are `matching_c`, so the
overworld code is fully decompiled and both images rebuild byte-for-byte from
this directory.

One assembly subsegment remains in each layout, `text_11A8`, covering
`0x801691A8-0x801697D0`. It is not code. The inventory runs continuously up to
`0x801691A8` -- `CampaignMap_UpdateLocation` at `0x80168FCC` is `0x1DC` long and
so ends exactly there -- and resumes at `0x801697D0`, so no function has ever
been identified inside the span. It begins at `gCampaignMap_aLocationTable`,
the location table, whose 66-byte stride is legible in any caller as
`sll v0,a0,5; addu v0,v0,a0; sll v0,v0,1`, that is `a0 * 33 * 2`. The span is
`0x628` bytes, which is not a whole number of entries, so the table is followed
by further data that has not been classified.

Reclassifying that span from `asm` to `data` is a layout question rather than a
decompilation one, and it is the only thing standing between these modules and
a layout with no assembly in it.

Keep extracted payloads, unaccepted candidates, objects, and diffs under
`tmp/`. Do not add either variant to the resident
`config/slus_01411/matching_c.json`.
