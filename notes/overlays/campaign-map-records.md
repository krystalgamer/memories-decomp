# Live campaign-map record contracts

`gCampaignMap_aLocationTable` at overlay address `0x801691A8` is now declared
as `MapLocation[]`, not a byte array. Every live C consumer is in
`src/overlays/overworld/set_location.c`, compiled independently into both
overworld variants with `gcc_2_8_1_g0_split`. `campaign_map.h` includes the
owning `location_table.h`; `MapLocation`, its nested `CampaignMapExit`, and the
existing `MapObject` view are defined only in `src/ygo_types.h`.

This is a union of observed field accesses, not a copy of the alternate
map's types and not a claim about the original source declarations.
The inventory's 66-byte location stride and four 12-byte exits at `+0x12`
exactly fit: `18 + 4 * 12 = 66`. Size and offset assertions pin the layout.
The initial typing change did not change the table bound, initializer,
symbol address or compiler profile. Its subsequent C-owned storage is
described below.

## One location

| Offset | Member | Type | Live evidence |
|---:|---|---|---|
| `0x00` | `confirm_gate` | `u16` | `CampaignMap_PickExit` tests it before querying the first exit's story flag. Confirm is ready when this gate is zero or that query succeeds. |
| `0x02` | `camera_field_04` | `s16` | Immediate setup stores it in `ViewState.field_04`; the tween subtracts the live signed value from it. |
| `0x04` | `camera_angle` | `s16` | Immediate setup writes `ViewState.angle`; the tween forms the wrapped heading difference from the same value. |
| `0x06` | `camera_field_00` | `s16` | Immediate setup writes `ViewState.field_00`; the tween computes its signed delta. No additional physical meaning is assigned to that channel. |
| `0x08` | `view_x` | `s16` | Both camera paths use it for `view.vrx`. |
| `0x0A` | `view_z` | `s16` | Both camera paths use it for `view.vrz`. |
| `0x0C` | `f12` | `s16` | Marker creation passes X to `func_800428A8`; the transition passes it to `DisplayObject_InterpolatePositionCosine` and eventually copies it into the marker's X halfword. |
| `0x0E` | `f14` | `s16` | The same paths use it as marker Y. Existing member spellings are retained. |
| `0x10` | `confirm_destination` | `u8` | The ready confirm path returns this nonzero destination, otherwise returns the current location with bit `0x8000` set. |
| `0x11` | `pad_11` | `u8` | No live access identified; preserved, not interpreted. |
| `0x12` | `exits` | `CampaignMapExit[4]` | Shared by location-object spawning and exit selection. |

The signed types are constrained by arithmetic and position-argument uses,
not just zero stores. Immediate camera setup and several marker copies
formerly loaded the same halfwords through `u16 *`. Their explicit `(u16)`
conversions remain at the copy sites, preserving those accesses without
creating a second struct view. The two tween channels that read
`D_800F2848` through `s32 *cameraLong` are unrelated to the source-table
contract and are deliberately unchanged.

## One exit

| Offset | Member | Type | Live evidence |
|---:|---|---|---|
| `0x00` | `story_flag` | `u16` | Both consumers treat zero as unconditional and otherwise call `Campaign_TestStoryFlag`. |
| `0x02` | `x` | `s16` | The spawning path passes it through `func_800428A8` to `func_800404CC`'s X argument. |
| `0x04` | `y` | `s16` | The same chain uses it as Y. |
| `0x06` | `input_mask` | `u16` | The selector ANDs it with the held-input word. |
| `0x08` | `field_08` | `u8` | Spawning passes it as `func_800428A8`'s `arg5`, then `func_800404CC`'s `field_69`. No stronger sprite/resource meaning is asserted. |
| `0x09` | `destination` | `u8` | Both paths skip the sentinel `16`; the selector returns the value for an eligible exit. |
| `0x0A` | `move_steps` | `u8` | Selection copies it to `gCampaignMap_MoveState`; the transition uses that state as its camera-tween step count and marker interpolation divisor. |
| `0x0B` | `pad_0B` | `u8` | No live access identified; preserved, not interpreted. |

The spawning code now names `record->exits[i]`, while selection walks a
`CampaignMapExit *`. All location and exit reads use members rather than
raw address/halfword casts. The original gate order, input choice, sound
calls, sentinel handling and transition state updates are unchanged.

## What the matching experiment establishes

The first probe used the complete typed table and members while retaining
the spawning loop's explicit `i * 12 + 18` address calculation. All five
overlays matched. The second probe replaced that calculation with
`&record->exits[i]`; all five still matched. Ordinary `exits->destination`
and `exits->input_mask` accesses also reproduce the selector's original
instructions, without its old byte-expression trick `(exits + 6)[3]`.

The previous claims that the table must remain `u8[]` or that these exact
byte expressions were required describe older source shapes, not an
invariant of the current grouped translation unit. The measured profile is
`gcc_2_8_1_g0_split`, not the `-G8` mentioned by the former header comment.
No compiler profile, assembly barrier, register binding or guarded alternate
declaration was added.

The separate `table` and `record` locals in
`CampaignMap_CreateLocationMarker` and `CampaignMap_UpdateLocation` remain.
The latter has a recorded scheduling constraint on materializing the table
base; typing the locals did not require collapsing their expressions.
Camera locals, branch order and the `cameraLong` view also remain.

`MapObject` is the same existing partial display-object view, now centralized,
not a new claim that every display object has this complete type. The
alternate family continues to use its distinct table at `D_80169E54` and
its separate contracts in `alternate_location.h`. Layout similarity alone
does not establish shared ownership or interchangeable live entrypoints.

## C-owned table storage

`location_table.c` defines the complete live table, and `location_table.h`
is its sole extern declaration point. Each overlay's data manifest compiles
the same source independently with `gcc_2_8_1_g0`. The bound is now
`CAMPAIGN_MAP_LOCATION_COUNT` (16): `16 * sizeof(MapLocation) = 1056 = 0x420`,
covering `0x801691A8-0x801695C8` exactly. The next named object is the marker
pointer `D_801695C8`, not another table element.

Both verified module images contain identical bytes in this range, with
SHA-256 `0259a2516466fd98262c69a1f4da03d22fec71b9f4e125adea7925fe35220c12`.
The initializer is decoded field-by-field using the proven signedness and
widths above, not emitted as an opaque byte array. Every field is explicit,
including the unobserved `pad_11`: it contains `0x80` in records 10-15 and
must not be defaulted to zero merely because the live code does not read it.
High-bit story-flag values and all unused-exit sentinel records are likewise
preserved rather than normalized.

The layout has three pieces after the header:

| File range | Runtime range | Ownership |
|---|---|---|
| `0x0004-0x11A8` | `0x80168004-0x801691A8` | Existing live-map text unit |
| `0x11A8-0x15C8` | `0x801691A8-0x801695C8` | `location_table.c` `.data` |
| `0x15C8-0x3000` | `0x801695C8-0x8016B000` | Existing raw tail, alternate text and bulk data |

The table needs its own Splat segment: a `.data` subsegment inside the old
single code segment would be ordered after that segment's remaining text,
not between its live and alternate functions. `module_tail` retains the
rest of the old ordering, starting with the unchanged raw `0x15C8-0x17D0`
range. Its state words are not silently claimed by the C table.

The old `text_11A8` assembly fallback interpreted the table as instructions
and exported `func_80169230` inside record 2's camera fields. This label is
**not unused**: `CampaignMap_UpdateAlternateLocation` declares and calls it.
The address is `table + 0x88`, or record 2 plus four bytes. The alternate
family already has unresolved raw callees; this overlapping call target is
not evidence for retargeting it to a live helper or claiming those camera
values are a verified callable function. It remains unresolved here.

The new C object defines the table's storage, not a function at that interior
address. Splat preserves `func_80169230 = 0x80169230` in the generated
undefined-function linker script, so the original `jal` target and every
table byte remain unchanged. The table is data-owned in the live view while
this independent alternate-path reference is explicitly retained.

At the original raw-tail boundary, Splat initially invented `func_801695C8` instead
of retaining the existing data name, which failed the link.
`overworld_symbols.txt` now records `D_801695C8` explicitly so the raw tail
exports the name its consumers use. This is an input label for Splat, not a
linker assignment over a C definition.

There is no linker alias for `gCampaignMap_aLocationTable` to retire. The
project-owned symbol map retains its reference address; the generated linker
script obtains its storage from the C object's `.data`. Clean image matches
and ELF/object ownership checks distinguish this from merely resolving the
old symbol at link time.

The separate alternate table is now also C-owned; see
[`alternate-location-data.md`](alternate-location-data.md). Its equal bytes
do not merge its address or type contract with the live table described here.

The following aligned 80-byte live-state prefix is now C-owned as well;
[`live-state-data.md`](live-state-data.md) documents its separate scalar
definitions and the nonzero boundary word retained at `0x80169618`.
That later mapping marks `D_801695C8` and the other state exports defined in
the project-owned build map rather than leaving them on the raw-tail path.
