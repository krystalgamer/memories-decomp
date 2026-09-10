# Live campaign-map record contracts

`gCampaignMap_aLocationTable` at overlay address `0x801691A8` is now declared
as `MapLocation[]`, not a byte array. Every live C consumer is in
`src/overlays/overworld/set_location.c`, compiled independently into both
overworld variants with `gcc_2_8_1_g0_split`. The declaration stays in
`campaign_map.h`; `MapLocation`, its nested `CampaignMapExit`, and the
existing `MapObject` view are defined only in `src/ygo_types.h`.

This is a union of observed field accesses, not a copy of the alternate
map's types and not a claim about the original source declarations.
The inventory's 66-byte location stride and four 12-byte exits at `+0x12`
exactly fit: `18 + 4 * 12 = 66`. Size and offset assertions pin the layout.
No table bound, data initializer, symbol address or compiler profile changes.

## One location

| Offset | Member | Type | Live evidence |
|---:|---|---|---|
| `0x00` | `confirm_gate` | `u16` | `CampaignMap_PickExit` tests it before querying the first exit's story flag. Confirm is ready when this gate is zero or that query succeeds. |
| `0x02` | `camera_field_04` | `s16` | Immediate setup stores it in `ViewState.field_04`; the tween subtracts the live signed value from it. |
| `0x04` | `camera_angle` | `s16` | Immediate setup writes `ViewState.angle`; the tween forms the wrapped heading difference from the same value. |
| `0x06` | `camera_field_00` | `s16` | Immediate setup writes `ViewState.field_00`; the tween computes its signed delta. No additional physical meaning is assigned to that channel. |
| `0x08` | `view_x` | `s16` | Both camera paths use it for `view.vrx`. |
| `0x0A` | `view_z` | `s16` | Both camera paths use it for `view.vrz`. |
| `0x0C` | `f12` | `s16` | Marker creation passes X to `func_800428A8`; the transition passes it to `func_8004318C` and eventually copies it into the marker's X halfword. |
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
