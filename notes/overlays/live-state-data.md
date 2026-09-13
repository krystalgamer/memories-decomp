# Live overworld state ownership

`live_state.c` defines the aligned live-state prefix at
`0x801695C8-0x80169618` in both overworld images. The existing consumers keep
their scalar widths and pointer views, and `live_state.h` is the sole
declaration point for all 18 C-owned symbols.

## Proven fields and boundary

| Address | Symbol | Type / observed role |
|---|---|---|
| `0x801695C8` | `D_801695C8` | `MapObject *`, location marker |
| `0x801695CC` | `D_801695CC` | `s32`, tween accumulator for `view.vrx` |
| `0x801695D0` | `D_801695D0` | `s32`, tween accumulator for `view.vrz` |
| `0x801695D4` | `D_801695D4` | `s32`, remaining transition steps |
| `0x801695D8` | `D_801695D8` | `MapObject *`, persistent map object |
| `0x801695DC` | `D_801695DC` | `s32`, per-step `view.vrx` delta |
| `0x801695E0` | `D_801695E0` | `s32`, per-step `view.vrz` delta |
| `0x801695E4` | `D_801695E4` | `s32`, heading accumulator |
| `0x801695E8` | `D_801695E8` | `s32`, `field_04` accumulator |
| `0x801695EC` | `D_801695EC` | `u8`, transition flags |
| `0x801695F0` | `D_801695F0` | `s32`, heading delta |
| `0x801695F4` | `D_801695F4` | `s32`, `field_04` delta |
| `0x801695F8` | `D_801695F8` | Four object pointers, 16 bytes |
| `0x80169608` | `gCampaignMap_MoveState` | `s32`, chosen transition step count |
| `0x8016960C` | `gCampaignMap_Location` | `u8`, current location |
| `0x8016960D` | `D_8016960D` | `u8`, departure/fade flags |
| `0x80169610` | `D_80169610` | `s32`, `field_00` accumulator |
| `0x80169614` | `D_80169614` | `s32`, `field_00` delta |

`CampaignMap_StartCameraTween` writes the accumulators and deltas;
`CampaignMap_UpdateLocationTransition` consumes them. Initialization,
location updates and the spawn/release helpers establish the pointer and
flag roles. No new semantic symbol names are asserted.

The prefix is 80 bytes: 75 bytes of named objects and five alignment bytes
at `0x801695ED-0x801695EF` and `0x8016960E-0x8016960F`. The original bytes
are all zero. Ordinary compiler alignment reproduces the gaps; no invented
padding objects or packed state struct is needed. Each definition explicitly
selects `.data`, since these zeros are present in the module image and must
not migrate into zero-fill-only `.bss`.

The two bytes at `0x80169618` and `0x80169619` are **not** included.
They share the raw word `00 00 43 00`; the remaining halfword `0x0043` cannot
be absorbed as zero-fill padding. `gCampaignMap_LocationPrev` and
`D_80169619` stay declared in `campaign_map.h`, and the whole word and
following opaque tail stay in the generated assembly beginning at `0x1618`.

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
Their linked symbols now belong to the C data section rather than absolute
assignments.

The build also checks this distinction automatically for all data-only
manifest units. After linking and before extracting the binary, it inspects
global data exports from each C object and requires a section-defined symbol
in the ELF. Missing, absolute, undefined and unallocated-common results are
errors, as are duplicate data definitions across data-only units. Interior
aliases that the C object does not define remain allowed, including the
documented alternate call into the live table.

A real negative control demonstrates the need for this check: adding an
absolute `gCampaignMap_MoveState = 0x80169608` assignment still produces a
byte-identical module, but the new ownership guard rejects that ELF.
Regression tests cover that case, duplicate common definitions, section
variants, legal interior aliases and symbol-tool failures.

Because the data are all zero, validation also checked **every one** of the
18 object-relative and linked symbol addresses, not just the 80 output
bytes. Both images retain the nonzero boundary word and match completely.
