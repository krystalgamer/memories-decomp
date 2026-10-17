# Active effect-keyframe update

`Model_UpdateEffectKeyframes`, originally `func_8005DBA4` at `0x8005DBA4`,
is the resident model-effect runtime invoked by `Model_UpdateScene`.
Its high-confidence name follows both the key-ring mutations and the two
camera endpoint channels it updates; it is not a skeletal-animation helper.

## Evidence and scope

The updater obtains the shared camera view and returns if the queued-key
count or current cursor is empty. When preparation is pending, it prepares
the queued keys through `func_8005E808`. It advances the active key's
progress using the frame-step helper and evaluates the requested channels
into the view's viewpoint and reference point.

The channel handlers include spline evaluation, slot-relative movement,
following and noise. A second pass handles relative orbits using the
captured distance/yaw/pitch pose. Completing a key captures the new
starting pose, advances the cursor, and either wraps a multi-key ring or
clears a completed one-key queue. A zero-magnitude successor can recurse
through this same updater. Open-ended completion and the final
persistent-channel post-processing branch remain distinct paths.

The plural name describes the keyframe runtime, not an unconditional
bulk evaluation of every queued key. No new meaning is assigned to the
adjacent address-based control flags or unrecognized endpoint kinds.

## Preserved execution contract

Only the function identifier changes in implementation and call sites.
The captured view/timing destinations, live state reloads after callbacks,
four-halfword slot-copy extents, arithmetic, narrowing, random calls,
recursion, and branch order are unchanged. The existing native witness
keeps its complete independent oracle and coverage; its forward declaration
and actual call use the new name.

The source remains `src/game/model_keyframe_update.c` with its existing
`gcc_2_8_1_g8_split_no_strength_reduce` profile and `0xC64` extent.
All seven inventories and symbol bindings preserve their addresses:

| Build | Address |
|---|---|
| North America | `0x8005DBA4` |
| Japan | `0x8004D5F4` |
| European base | `0x8004D9B0` |
| French, German, Italian, Spanish | `0x8004DBB4` |

No source move, data/layout change, overlay implementation, compiler
adjustment, or new input-validation policy is part of this naming change.
