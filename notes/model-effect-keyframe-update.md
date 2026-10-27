# Active effect-keyframe update

`Model_UpdateEffectKeyframes`, originally `func_8005DBA4` at `0x8005DBA4`,
is the resident model-effect runtime invoked by `Model_UpdateScene`.
Its high-confidence name follows both the key-ring mutations and the two
camera endpoint channels it updates; it is not a skeletal-animation helper.

## Evidence and scope

The updater obtains the shared camera view and returns if the queued-key
count or current cursor is empty. When preparation is pending, it prepares
the queued keys through `Model_PrepareEffectKey`. It advances the active key's
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

The original identity change touched only the function identifier in
implementation and call sites. Its then-existing native witness retained
its independent oracle and coverage with identifier-only updates; that is
historical evidence, not a request to restore retired gameplay fixtures.
The later local-view component below still preserves captured view/timing
destinations, live state reads after callbacks, four-halfword slot copies,
arithmetic, narrowing, random calls, recursion and branch order.

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


## Complete spline and frame-consumer views

The COMPLETE spline definition distinguishes current_key, channel_index,
time_numerator, mutable time_denominator, output_xyz and segment_keys.
Only definition argument labels change; public prototype labels and all
types/signatures remain byte-identical. The existing comment adopts those
same labels. These time names do not establish units, normalized fractions,
nonzero denominators or a full-path sampling claim.

Keep all three separate initial ring-count reads, modulo expressions,
future-kind scan, repeated tail pointer, previous-key selection and adjacent-
distinct duration sum. The previous channel replaces the pose point ONLY
under the original nonzero-denominator/kind1 gates. The ONE point-or-axis
binding, literal 16-byte memset, three eight-byte Coeff records, coefficient
call order and THREE signed multiply/divide stages stay. No sum/divisor
guard, wider intermediate, alternate curve or output saturation is added.

The COMPLETE updater's preparation_key remains a Key*, separate from both
u8* channel_key_bytes bindings in the channel/orbit loops. preparation_index
walks the original live-count preparation loop. progress_write_key captures
the timing STORE destination BEFORE the frame-step callback; subsequent
global key reads and the optional second frame-step call stay live at their
original source sites. No key alias merge, forced reload or callback policy.

output_channels captures the view pointer BEFORE the empty-queue guard.
Its two endpoints, output_cursor, pose_halfwords and spline_result_cursor
retain original pointer types, strides, extents and lifetimes. slot_position,
follow_target_position, spline_result and follow_delta each remain FOUR
halfwords, even where only three components are consumed. active_channel_count
still counts all nonzero kinds, including unhandled ones and deferred orbit
kind4; it does not count only successful evaluations. persistent_channel_count
still increments for kinds2/3 BEFORE readiness and for kind5 BEFORE completion.

The separate slot_offset_halfwords, follow_reference_halfwords and
committed_reference_halfwords are live-global-derived views at their original
read/write sites, not aliases collapsed into the captured channel record.
follow_amount remains one kind2/3 binding with the distinct defaults 10/2000;
kind5's noise_amplitude is a DIFFERENT lexical binding. Preserve all signed/
unsigned halfword conversions, mode reread, deadzone publication/narrowing,
middle-axis helper call, 60 limit, three additions and later reference writes.
Noise keeps all FOUR ordered rand calls, signed shifts/remainders, X/Z-only
writes and completed-duration branch; no positivity or divisor guard.

The spline copy retains axis_index and separate component/output cursors.
The orbit pass keeps ONE reused channel index, its distinct key/halfword
views, early elapsed assignment, duration/remaining captures, radius gate,
Q12 projections and opposite-output pointer increment. No trigonometric
reordering, orbit-center recapture, finite-angle or radius guarantee.
Completion still handles inactive/open-ended clearing, pose capture, ring
advance/wrap and single-key clearing, preparation, zero-magnitude recursion
and final persistent==2 orbit-step versus metrics update in that order.

Only scoped bare identifiers in these TWO definitions and matching words in
TWO existing comments change. All other functions, members, operators,
types, declarations/order/lifetimes, casts, constants, labels, calls, macros,
comments, public headers, profiles, private data and storage remain.
Two existing CSV NOTES cells are appended; identities/evidence/confidence
and every other row stay. No fixture/control/oracle/mutation changes, new
synthetic/generated-C/source-string behavioral harness, retired fixture
revival or local cloud execution.
