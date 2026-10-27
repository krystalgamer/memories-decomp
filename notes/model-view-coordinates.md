# Paired model coordinate helpers

This component names two existing game-owned matching-C helpers and all
fourteen corresponding native function records across seven images.
It introduces no new geometry implementation, type, validation, allocation,
compiler profile or source-file movement.

## View-relative coordinate builder

`Model_BuildViewRelativeCoordinateUnit`, formerly `func_800580D4`, retains
`void(s32, s32, SVECTOR *, struct _GsCOORDUNIT *)`.

The first argument selects `gModel_aSlots[index]`. The second requests an
80-byte coordinate entry. The original comparison remains strictly
`entry_count < requested`: that case substitutes `root_coordinate_index`. Equality,
negative requests and caller validation are not changed, and no anatomical
meaning is assigned to entry fifteen or the fallback.

The builder obtains that entry's local-world matrix, installs it in GTE,
and transforms the input vector. A translation parent starts with identity
rotation and the resulting point. The output combines cached view yaw/pitch
with the original negated slot-zero rotation and clears its local translation.
Translation uses the selected slot, but the orientation's repeated volatile
reads still come from slot zero, not that selected slot.

This describes a view-relative construction, not a guarantee of normalized
billboarding. All inline-helper structure, casts, full-turn constants,
matrix/GTE call order, flags and stack/store layout remain.

The legacy `super` assignment points to the builder's automatic parent
workspace. This lifetime-sensitive behavior is intentionally preserved,
not repaired or wrapped. The canonical name supplies no new stable-storage
or ownership guarantee.

## Cached-view, translation-parent and orbit local views

The existing metrics, view-relative builder and orbit-step definitions
now distinguish their local views without changing arguments or helpers.
cached_view and selected_view retain BOTH pointer bindings, the null-input
selection and the original nonnull/different-pointer copy condition.
Metrics still use the selected INPUT record after the optional copy,
not a replacement cached-view reread. Every scalar member-address cast,
repeated delta expression and intermediate store remains.

The distance halfword first receives the XZ-plane SquareRoot0 result;
pitch uses its SIGNED reload before full XYZ distance overwrites it.
Yaw and pitch retain their signed-halfword reloads and signed remainder
normalization. No widened distance, shared delta temporary, atan argument
swap, singularity handling or new normalization guarantee is added.

The builder's entry_world_matrix/world_position name the original
GsGetLwUnit, GsSetLsMatrix and RotTransSV path. transform_flags remains
an EIGHT-BYTE byte array, cast ONLY at the SDK boundary; no typed output,
initialization or interpretation of those status bits is added.
translation_parent keeps zero rotation, identity-matrix construction,
flag1, narrowed transformed translation, work-matrix copy and null parent.
view_rotation/view_rotation_copy/view_work_matrix retain separate stack
objects, memset extent and inline setup/build helpers. The helpers'
parameters and repeated VOLATILE slot-zero root-coordinate reads stay.
Output translation clears in Z/Y/X order before super takes the address
of the AUTOMATIC translation_parent. Its lifetime-sensitive legacy
behavior is NOT repaired, wrapped or described as stable ownership.
The STRICT entry_count<request fallback, equality/negative behavior,
selected-slot translation and slot-ZERO orientation remain distinct.

pivot_position/orbit_position retain the two original PSXLONG pointer
triples. Positive direction anchors the reference triple and moves the
viewpoint; otherwise it anchors the viewpoint and moves the reference.
The exact direction value STILL multiplies all offsets, including zero
or a magnitude other than one; these local names add no sign-only policy.
Keep the reference-point INTERIOR symbol and +/-3 pointer arithmetic.

The signed distance pretest >=0xB still precedes the unsigned-halfword
reload, addition and narrowed store. No nonwrapping clamp is promised.
Masked yaw/pitch updates and raw nonzero recomputation tests stay
DIFFERENT: a full-turn delta can skip an angle update but still rebuild
the view. angle_sum, cosine and sine deliberately remain reused across
axes. horizontal_or_x remains ONE binding for the horizontal projection
then X offset; vertical_offset/z_offset stay separate. Preserve every
signed multiply/divide, denominator, halfword reload and X/Y/Z store.
No trigonometric caching, wide intermediates or spherical rewrite.

Only scoped bare locals in these THREE complete definitions and FOUR
matching identifier words in ONE existing orbit comment change.
All arguments, member tokens, types, constants, expressions, scopes,
inline helpers, labels and order remain. All other grouped definitions,
headers, SDK declarations, callers, fixtures and controls stay exact.

## Slot-zero offset world position

`Model_GetSlot0OffsetWorldPosition`, formerly `func_80059B90`, retains
`void(s16, s16 *)`, not a retyped vector argument.

It zeros one `SVECTOR`, puts the signed input in its local X component,
and invokes the builder with slot zero and entry request fifteen. It then
asks `GsGetLwUnit` for the result and extracts low halfwords of the three
translation words. The single byte-backed SVECTOR/GsCOORDUNIT/MATRIX scratch
block, volatile halfword reads, narrowing and output extent stay intact.
The store `out[3] = 0` still precedes the final `out[2]` store.

Exodia's second-entry logic consumes this result as a world-space origin
at local X offsets 480 or zero before forming its direction. It includes
the coordinate entry's translation; it is not a projected screen coordinate,
camera position or view-dependent offset vector. No hand, bone or body-part
identity is inferred from the fixed entry request.

The SDK setter was checked independently: resident `GsSetLsMatrix` at
`0x800855D0` calls `SetRotMatrix` then `SetTransMatrix`. It is not the separate
`GsSetLightMatrix`. Reading that SDK evidence does not make it a game naming
candidate; SDK-owned `func_80058F10` likewise remains outside this component.

## Native and source contracts

| Image | Builder | Position helper |
| --- | --- | --- |
| NA | `0x800580D4` | `0x80059B90` |
| Japan | `0x8005AF04` | `0x8005C9C0` |
| SLES-03947 | `0x8005C614` | `0x8005E054` |
| Other PAL images | `0x8005B258` | `0x8005CC98` |

Every builder remains `0x1EC` bytes and every position helper `0x88`.
All retain game ownership, matching-C status and historical notes. The
builder stays grouped after the contiguous `0x19C`-byte
`Model_UpdateViewMetrics` in the same source definition order and named
`gcc_2_8_1_g8_split` profile. The position helper keeps its source filename
and `gcc_2_8_1_g0_split` registration. Address/source/profile manifests are
unchanged; no name-field migration is needed there.

The isolated canonical header preserves the old SDK/forward-tag declaration
forms. Every pre-existing header remains frozen, including the old numeric
builder prototype and the Exodia position prototype. Root compatibility
aliases retain those APIs, and Exodia C/import maps keep their numeric calls,
addresses and cardinalities. Existing ABI and French/Spanish import controls
remain byte-identical.

The resident source's obsolete candidate-status comment is updated to the
current matching-C interpretation. Frozen historical header comments are
not authoritative status inventories; `functions.csv` is.

No local cloud/full regional/overlay/witness suite, synthetic C fixture,
source-string harness or probe is added. Whole compiler/MASPSX, filtered
intermediate and complete fallback comparisons reverse only the two
identities, without instruction, directive, register or filename filtering.
Final clean resident matching, policy/ownership checks and fresh applicable
PR gates remain required.


## View separation and pitch/distance correction local views

The complete Model_SeparateViewFromSlots, Model_CorrectViewPitch and
Model_CorrectViewDistance definitions now distinguish existing locals and
three definition arguments. Public prototypes, layouts and profiles stay.
The pre-match structural decode remains historical; its guesses and probes
are not a replacement declaration or new behavioral evidence.

Separation's apply_request retains the full-width nonzero test, and
recursion_depth is incremented at its original site before the two-slot
loop. separation_extent is the first accepted limit and the return value,
NOT the actual displacement or a Boolean: it is still set when apply_request
is zero. extra_overlap_count counts later accepted overlaps after that
first extent; the original retry conditions and depth-three cutoff remain.

probe_offset_x/probe_offset_z and probe_x/probe_z distinguish the yaw-relative
probe from the eye position that is later modified. The initial dead X read
and later Y use remain ONE view_work binding. extent_work and slot1_extent
retain signed-halfword division by two and the exact field order 3/0/1/2.
All nine ModelSeparationPair objects, declaration order, aggregate copy-downs
and do-while-zero scheduling barriers stay intact; they are not assigned
false universal axis meanings or replaced by new arrays/structs.

The loop's vertical_pointer and height_pointer STILL begin at e3.values-4,
step separately by slot_index and then one, and read offsets 13 and 7.
No typed-array substitution or cross-object pointer repair is introduced.
vertical_work retains signed delta THEN absolute value; height_gate_work
retains the half-extent read THEN comparison result in the SAME binding.
separation_limit, radial_distance and all disabled-slot/tint guards retain
their casts, comparisons and read sites. Root extent_work and loop-local
vertical_work remain distinct despite their former shared v spelling.

penetration_scale still divides by the limit; the signed extent products,
not the radial deltas, form push_x/push_z, with the original <=0 sign tests.
target_x/target_z and eye_x/eye_z retain their four independent live reads.
normal_x/normal_z, pushed_eye_x/pushed_eye_z and line_constant keep their
original order around SquareRoot0. side_distance remains the initialized,
truncated sign discriminator, with its do-while-zero division and zero-length
path. No wider arithmetic, exact real-valued distance or overflow guarantee
is inferred. Timer-zero selects direction; the 1..FE range decrements and
FF retains its original behavior. Only the eye X/Z stores are changed by
the algorithm; no slot-record displacement is asserted.

Pitch distinguishes target_y, height_weight, pitch_target, lower_pitch_limit,
pitch_sample and mixed pitch_work. The latter is first an upper band then
a step, not a pure delta. Keep the signed 0x801 split, target-Y interval,
3755/3968 weighting, +/-0x10 band, separate 0xEAB floor and asymmetric -1/+4
caps. The do-while-zero capture and unconditional low-pitch -4 arm remain.

Distance distinguishes target_slot, lower_distance/upper_distance,
separation_move and relaxation_move. The lower adjustment still precedes
the conditional upper adjustment. Separation still runs before the first
pitch call, then distance_sample captures the signed distance. zero_yaw
stays a separate original binding. distance_work intentionally remains
step THEN current-distance THEN further step scratch; repeated distance
loads and the last label are not replaced with the earlier sample.
pull_in_request retains a raw nonzero test for the optional lower-limit
pull, not a Boolean enum or unconditional correction policy.

Keep all 150/300/700/1000 boundaries and 10/5/-10/-20 step limits, both pitch
calls, and the final independently reread upper-distance check. Separation
still clears eye flag bit zero. NA's old-step threshold/increment/decrement
and Europe's two-unit changes remain distinct. The European baseline_step
and narrowed u16 step_candidate retain subtraction/store/compare order.
Japan still accesses neither adaptive baseline nor step update.

Only scoped bare locals/definition arguments and existing separation-comment
identifier references change. All other C comments, types, declarations,
lifetimes, qualified members, casts/operators/constants/labels/calls, other
grouped functions, regional macros, public headers, compiler profiles and
storage remain exact. The entire semantic ledger stays byte-identical:
no function/global/member identity promotion or NOTES churn. No fixture/
control/oracle/mutation edits, retired-fixture revival, local cloud tests
or new behavioral harness accompany these views.
