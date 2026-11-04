# Active scene-view state and alternate orbit

`gViewState_Current` is the view record at `0x800F2848` (formerly
`D_800F2848`, now its only name), not a new allocation or a replacement
layout. Duel setup, projection, view tweening, Library,
developer effect controls and result-heading updates use this record. The
overworld also imports its legacy name. It is distinct from `gModel_RView`
and `gModel_CameraMove`.

## Record evidence and boundaries

The recovered `ViewState` in `src/game/view_state.h` remains unchanged:
signed halfword prefix fields, heading at `+2`, projection at `+0xE`, and
the SDK `GsRVIEW2` at `+0x10`. `Duel_InitGroundProjectionReference` passes
the projection member to `SetGeomScreen`.
`ViewState_ApplyViewpointOffset` adds its three arguments to `vrx/vry/vrz`,
stores `vpx/vpy/vpz`, then passes the embedded view to `GsSetRefView2`.
The SDK `super` member at record `+0x2C` retains its stored-pointer contract.

`ViewState_ResetDefaults` establishes radial magnitude 600, heading `0x400`,
the other angular field `0x100`, and projection 300, clears the remaining
prefix/reference/view-control fields, and applies the default orbit.
Its second cursor over `view.vpx` and its store order are preserved.
Library's two sixteen-byte view copies remain separate cast-based views,
not evidence for a different member layout.

The layout describes observed accesses, not a proved allocation extent.
There is still no total-size or stride assertion. `field_06`, `field_08`,
`field_0A` and `field_0C` stay numeric. The alternate orbit swaps the roles
of the two angles, so `orbit_angle` is not called pitch.

## Named members: orbit distance and orbit angle

| Offset | Old | New | Evidence | Confidence | Uses |
| --- | --- | --- | --- | --- | --- |
| `0x00` | `field_00` | `distance` | Both orbit bodies negate it as the radius they scale by `rsin`/`rcos`. `Duel_StartViewTween` and the duel view-tween step treat it as the distance channel. `Library_UpdateCardView` loads it from `Model_AdjustCameraDistanceForSlot` and uses it as the radius of its own light orbit. The overworld location table (`MapLocation.camera_field_00`), its tween (`gCampaignMap_dwTweenDistance`), the debug L1/R1 zoom and the duel intro zoom-in all set or step it before reapplying the orbit. | high | 23 accesses, 9 files |
| `0x04` | `field_04` | `orbit_angle` | The second angle both orbit bodies pass to `rsin`/`rcos`. `Duel_StartViewTween` names it the orbit-angle channel, and every writer stores an angle: constants such as `0x100`, `0xC4` and `0x3FE`, the duel and overworld tween accumulators, pad-driven up/down steps, or `MapLocation.camera_field_04`. The overworld tween calls its copy pitch, but the alternate orbit uses it as the horizontal angle, so the name stays neutral. | high | 20 accesses, 8 files |

The renames are spelling only, and raw halfword views such as
`duel_phase_entry.c`'s `w[1]`/`w[2]` keep their existing index spelling.
`MapLocation.camera_field_00` and `camera_field_04` in `src/ygo_types.h`,
which `CampaignMap_SetCameraFromLocation` copies straight into `distance`
and `orbit_angle`, are left for a separate change: that header is included
by nearly every overlay image, so renaming them puts about 14,000 images in
scope for verification.

## Two orbit conventions

`ViewState_ApplyAlternateOrbit` names former `func_800135FC`. Both orbit
bodies negate signed-halfword `distance`, derive offsets with the existing
`rsin`/`rcos` fixed-point operations, and call
`ViewState_ApplyViewpointOffset`. Using `r = -distance`, `a = angle`,
`b = orbit_angle`, and `F = MODEL_FIXED_ONE`, their staged calculations are:

| Stage | Default `ViewState_ApplyOrbit` | Alternate |
| --- | --- | --- |
| First horizontal factor | `h = r * rcos(b) / F` | `h = r * rcos(a) / F` |
| First sine component | `s = r * rsin(b) / F` | `s = r * rsin(a) / F` |
| Second sine component | `t = h * rsin(a) / F` | `t = h * rsin(b + 0x400) / F` |
| Second cosine component | `u = h * rcos(a) / F` | `u = h * rcos(b + 0x400) / F` |
| Offset helper arguments | `(u, s, t)` | `(u, t, s)` |

These are staged integer operations, not permission to combine products or
change rounding, calls, signs, argument order or stores. “Alternate” describes
the algorithm relative to the established default. No named C caller or
gameplay purpose is asserted for the alternate operation; absence of such a
caller is not evidence of unreachability.

The offset helper, default orbit and alternate orbit retain their contiguous
source group, definition order and `gcc_2_8_1_g8_split` profile.

## Image bindings and compatibility

| Image | Alternate orbit | Current view |
| --- | --- | --- |
| North America | `0x800135FC` | `0x800F2848` |
| Japan | `0x800134E8` | `0x800EFF20` |
| SLES_039.47 | `0x80013514` | `0x800F0DC0` |
| Other PAL images | `0x80013518` | `0x800F11E0` |

All seven function records retain their game ownership, matching-C status,
`0xD8` extent, compiler profile and prior notes. Existing numeric record
providers remain; canonical aliases bind to those providers. Legacy function
aliases remain available. The French literal `D_800F2848_French` at
`0x800F2848` is not the logical French current-view record and is unchanged.

All pre-existing headers and overworld source, import maps and inventory
notes remain unchanged. Resident consumers use the isolated
`view_state_current.h` declarations. The two existing result-controller and
Library-target controls change only the record identifier; their assertions,
mutations, generators and case counts are preserved, with no added harness
or test case. Regional image and witness execution belongs to PR validation.
