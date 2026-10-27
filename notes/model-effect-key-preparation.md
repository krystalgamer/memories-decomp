# Resident effect-key preparation

`Model_PrepareEffectKey`, originally `func_8005E808` at `0x8005E808`,
prepares one effect key for the resident keyframe runtime. The
high-confidence name covers more than duration calculation.

## Ordered effects and readiness

The function first derives its magnitude scale, refreshes slot-derived
resolved endpoints through `func_8005FB30`, and resets the input key's
progress. It then tests **the global current key's readiness**, not the
input key's readiness. Those keys are separate source expressions and
must not be conflated.

If that guard permits further work, the function marks the input ready
and initializes its magnitude-based duration, retaining the `0x4000`
threshold. Negative magnitudes additionally extend timing from the
relative-position, sampled-spline or orbit-distance bounds. Both
intrinsic absolute values, unsigned arithmetic, case-local temporaries,
bound checks and original narrowing remain unchanged.

Queue setup invokes the preparer while walking queued keys. The active
updater invokes it during pending preparation and after advancing the
current cursor. This lifecycle supports `PrepareEffectKey`; a
duration-only name would omit the earlier endpoint/progress effects and
readiness behavior. The name does not imply that every call reaches all
initialization stores or that the global cursor equals the input.

## Preparation, spline and pose-capture local views

The three contiguous preparation/evaluation/capture definitions now
name only their existing local views. signed_magnitude, magnitude,
positive_magnitude and duration_scale preserve the nested scope, intrinsic
absolute value, <=0 fallback to1 and multiplication by2. Endpoint refresh
and progress reset STILL precede the separate GLOBAL current-key guard.
Input readiness, the magnitude0x4000 threshold and later SIGNED input
test retain their original order, reads and halfword stores.

endpoint_view remains the aligned Coeff view of the two eight-byte
requests, not a new packed-member access. pose_channel/slot_index and
slot_position retain readiness gating, the FOUR-halfword copier extent,
three narrowed coordinate additions and the case-local dx/dy/dz shadowing.
Case1 keeps TEN spline samples at scales0..9 with denominator30, then
NINE adjacent differences; sampled_distance is not a promised complete
path length. duration_bound stays u32 with the original mixed signed/
unsigned products, divisions, comparisons and u16 stores. Keep both
intrinsic absolute values and all overflow/truncation behavior.

radius_halfword preserves the original unsigned-halfword selection and
SIGNED reinterpretation before multiplication6284/division1000.
endpoint_y_magnitude is only abs(endpoint_view->y), not an inferred
angle unit or complete orbit-kind schema. Preserve the separate bound
multiply, unsigned multiply/divide4096000 and per-case bound check.
No new nonnegative-radius guarantee, wide intermediate or time unit.

The evaluator's control_points/axis_coefficients retain all four point
pointers and THREE Coeff records. Coeff is EIGHT bytes with s16 x/y/z
and u16 w; coefficient stores and output remain narrowed, not widened.
Keep the literal16-byte memset, pointer arithmetic, channel stride8,
three SEPARATE live ring-count reads and original modulo expressions.
The future-key scan continues while kind==1, stops at the FIRST !=1
and pads that tail from the preceding key. tail_source_index names only
that i-1 selection; it adds no new key-validity or termination policy.

previous_key/previous_channel retain the separate live count reads.
Only nonzero den permits kind1 of the previous channel to replace the
pose control point. Zero den instead sums durations ONLY when adjacent
selected key pointers differ. No new count/divisor guard or duration
deduplication. point_or_axis_index remains ONE reused binding, including
the redundant branch-local zero assignments and final axis loop.
polynomial_value keeps all THREE ordered multiply/divide stages,
halfword coefficients and final constant addition. No curve-family,
floating-point, normalized-time or endpoint-clamping claim.

Pose capture obtains view THEN metrics, and always publishes all THREE
xyz channels without their pad halfwords. Only the optional endpoint
refresh is gated. Its key is captured ONCE; requested_cursor remains a
BYTE cursor with signed kind/slot loads and two eight-byte advances.
resolved_byte_offset starts at16 and advances by8, preserving FOUR
copied halfwords at each qualifying kind2/3 request. Keep callback order,
captured destination key and live next-request reads; no cursor reload,
pad reset, queue-count guard or input/output ownership change.

Only scoped bare locals in these THREE complete definitions change.
Arguments, member tokens, comments, types, expressions, scopes, helpers
and order remain. The existing extent witness changes ONLY pos to
slot_position inside THREE existing count/mutation string literals;
all 4/3 extents, assertions, controls, scenarios, flags and oracles stay.
Headers, callers and other fixtures stay exact. No new harness or local
cloud-witness execution.

## Source, adapters and contracts

The function opens the grouped `model_effect_state.c` unit, directly
before `Model_EvaluateKeyframeSpline`; it previously had its own
`model_prepare_effect_key.c` source and European and Japanese include
adapters. The unit's adapters already alias `gModel_pCurrentEffectKey`
to the same regional targets. All seven matching manifests and tracked
split paths follow the source without changing profiles, function order
or layout. The profile remains `gcc_2_8_1_g8_split`, and the extent remains
`0x3EC`:

| Build | Address |
|---|---|
| North America | `0x8005E808` |
| Japan | `0x8004E258` |
| European base | `0x8004E614` |
| French, German, Italian, Spanish | `0x8004E818` |

The existing `Key *` interface stays in `model_keyframe_spline.h`.
Implementation, callers and native witnesses change only the symbol and
necessary source locator/include paths. Complete fixture oracles, event
data, four-halfword coordinate buffers, callback state and ownership
controls are retained. Historical attempt paths remain historical.
No SDK/overlay implementation, storage, compiler flag, selector meaning
or runtime validation policy changes.


## Later spline/consumer argument and cursor labels

The later complete spline/frame-consumer component extends the evaluator's
definition labels and segment-key cursor without changing this preparation
routine or the earlier control-point/coefficient/tail views. The public
prototype still has its original argument labels and exact signature.
See [the complete spline/consumer contract](model-effect-keyframe-update.md#complete-spline-and-frame-consumer-views).
Earlier fixture statements above record historical campaign evidence; no
retired gameplay witness is restored or executed by this later component.
