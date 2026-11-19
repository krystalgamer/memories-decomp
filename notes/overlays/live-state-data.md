# Live overworld state ownership

The live map state is zero-initialized common storage. `set_location.c`,
its reader, declares all 20 objects as tentative definitions and builds
with `gcc_2_8_1_g0_split_comm`; each overworld layout places them with a
`set_location_commons` subsegment (the `bss_commons` type, see
[`build.md`](../build.md#common-declarations-for-small-globals-a-unit-reads))
from its symbol file. North America and Japan hold them at
`0x801695C8-0x8016961A` (identical addresses and bytes); the PAL images hold
the same names in another order at `0x801695C8-0x8016961C`, as PAL does for
every resident `.sbss`/`.bss` block. The existing consumers keep their
scalar widths and pointer views; `live_state.h` declares the first 18 and
`campaign_map.h` the last two.

## Proven fields and boundary

| Address | Symbol | Type / observed role |
|---|---|---|
| `0x801695C8` | `gCampaignMap_pLocationMarker` | `DisplayObject *`, location marker |
| `0x801695CC` | `gCampaignMap_dwTweenRefX` | `s32`, tween accumulator for `view.vrx` |
| `0x801695D0` | `gCampaignMap_dwTweenRefZ` | `s32`, tween accumulator for `view.vrz` |
| `0x801695D4` | `gCampaignMap_nTweenFramesLeft` | `s32`, remaining transition steps |
| `0x801695D8` | `gCampaignMap_pSpotlight` | `DisplayObject *`, spotlight mask |
| `0x801695DC` | `gCampaignMap_dwTweenRefXStep` | `s32`, per-step `view.vrx` delta |
| `0x801695E0` | `gCampaignMap_dwTweenRefZStep` | `s32`, per-step `view.vrz` delta |
| `0x801695E4` | `gCampaignMap_dwTweenYaw` | `s32`, heading accumulator |
| `0x801695E8` | `gCampaignMap_dwTweenPitch` | `s32`, `field_04` accumulator |
| `0x801695EC` | `gCampaignMap_bTransitionFlags` | `u8`, transition flags |
| `0x801695F0` | `gCampaignMap_dwTweenYawStep` | `s32`, heading delta |
| `0x801695F4` | `gCampaignMap_dwTweenPitchStep` | `s32`, `field_04` delta |
| `0x801695F8` | `gCampaignMap_apExitObjects` | Four object pointers, 16 bytes |
| `0x80169608` | `gCampaignMap_MoveState` | `s32`, chosen transition step count |
| `0x8016960C` | `gCampaignMap_Location` | `u8`, current location |
| `0x8016960D` | `gCampaignMap_bEnterSceneState` | `u8`, scene-entry request: 1 after confirming the current location, 0x80 latches the fade-out setup |
| `0x80169610` | `gCampaignMap_dwTweenDistance` | `s32`, `field_00` accumulator |
| `0x80169614` | `gCampaignMap_dwTweenDistanceStep` | `s32`, `field_00` delta |

`CampaignMap_StartCameraTween` writes the accumulators and deltas;
`CampaignMap_UpdateLocationTransition` consumes them. Initialization,
location updates and the spawn/release helpers establish the pointer and
flag roles. No new semantic symbol names are asserted.

In North America and Japan the first 80 bytes hold the table's 18 objects
(75 bytes and five alignment bytes at `0x801695ED-0x801695EF` and
`0x8016960E-0x8016960F`), then `gCampaignMap_LocationPrev` and `D_80169619`
at `0x80169618` and `0x80169619`. All are zero. The block ends there, on a
halfword: the word at `0x80169618` reads `00 00 43 00`, and the `0x43` is the
first stale byte. The `bss_commons` type lists blocks that end mid-word, so
the live-state segment aligns to 2.

The rest of the window, from `0x8016961A` (North America and Japan) or
`0x8016961C` (PAL) to `0x801697D0`, is its own `live_state_tail` segment
(a `bin` that starts on a halfword, or `data`), not code. Past the boundary
it holds stale `-O0` instructions: the frame-pointer-relative body of some
earlier function from its middle (no prologue) through its epilogue.
Nothing branches to, calls or loads from it; its only jump targets itself.
The US and Japanese images carry the same bytes; the PAL images carry a
variant.

## One pointer-slot declaration

The source previously used `D_801695F8_objects` and `D_801695F8_words` as
two C spellings of the same assembler symbol. Creation and release pass the
stored values as object pointers. The integer spelling only writes zero.
The single bounded pointer array now serves both: the initializer's zero
stores and the spawn/release accesses reproduce the same instructions.
Neither an assembler alias nor an integer interpretation is needed for a
zero store.

This changes three functions' source spellings but not their behavior,
profiles, loop order or emitted code. The other state declarations move
from the source/subsystem header to the owning header without changing type.

## Why a hash alone was insufficient

The first build matched every overlay, but an ELF audit found
`gCampaignMap_MoveState` and `gCampaignMap_Location` were still absolute
symbols. They appeared in both the imported reference map and the new
build map, and the old undefined-symbol entries still produced absolute
assignments despite the C definitions.

The overworld layouts now consume one project-owned `overworld_symbols.txt`.
Every existing reference-map name and address is retained, as are the
project's alternate-function names; the reference file itself is unchanged.
All 18 state symbols are marked `defined:True` in that one build map.
Their linked symbols now belong to the commons subsegment's section rather
than absolute assignments.

The build also checks this distinction automatically for all data-only
manifest units. After linking and before extracting the binary, it inspects
global data exports from each C object and requires a section-defined symbol
in the ELF. Missing, absolute, undefined and unallocated-common results are
errors, as are duplicate data definitions across data-only units. Interior
aliases that the C object does not define remain allowed, including the
documented alternate call into the live table. Units built with a `_comm`
profile, `set_location.c` among them, get the same check for every common
they declare.

A real negative control demonstrates the need for this check: adding an
absolute `gCampaignMap_MoveState = 0x80169608` assignment still produces a
byte-identical module, but the new ownership guard rejects that ELF.
Regression tests cover that case, duplicate common definitions, section
variants, legal interior aliases and symbol-tool failures.

Because the data are all zero, validation also checked **every one** of the
18 object-relative and linked symbol addresses, not just the 80 output
bytes. Both images retain the nonzero boundary word and match completely.
