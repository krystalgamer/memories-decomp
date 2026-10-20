# Model camera orbit presets

## Direct and preset application

`Model_SetCameraOrbit` is the former `func_8005F3B8`. The existing live-view
and metric getters still run before the two-byte effect-state guard.
When permitted, a nonnegative slot selects the reference point through
`Model_CopySlotU16Values`; a negative slot retains the current point.
An optional `SVECTOR` offset keeps the original side-dependent signs.

The helper then writes distance, biased yaw and pitch into the existing
metric view, forces orbit recomputation with a raw full-turn argument,
captures the keyframe pose and requests camera move `-1`, in that order.
The `int` definition and compatible `s32` public declaration stay as they
were; no shared type, narrowing, remainder or guard policy is changed.

`Model_SetCameraOrbitFromPreset` is the former `func_8005F27C` (Japanese
`func_8004ECCC`). It selects a record before its ordered guard, takes the
base distance, conditionally applies the existing slot adjustment and
passes the record's yaw/pitch fields into the direct setter. The matching
parallel distance expressions are not extracted or normalized.

Both functions remain in the same grouped state source, at their original
native sizes `0x1AC` and `0x13C` and named `gcc_2_8_1_g8_split` profiles.
Only the obsolete Japanese preset-wrapper alias retires. The existing
intro-controller control file changes one identifier, preserving all
values, assertions, stubs and execution paths.
