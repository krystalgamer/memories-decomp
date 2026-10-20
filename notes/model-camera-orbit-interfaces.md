# Model camera orbit interfaces

## Live view adjustment

`Model_AdjustViewOrbit` is the former resident `func_80058434`. It updates
the cached view distance, yaw and pitch, then uses their sine/cosine values
to rebuild either the viewpoint around the reference point or the reverse.
The positive direction branch anchors the reference-point triple; the
other branch anchors the viewpoint. The original signed argument also
multiplies the displacement, so it is not normalized to a boolean or sign.

Keep every signed halfword view, division and remainder expression. A
distance change is accepted only when the sum is at least eleven.
Angle changes test the masked delta, but coordinate recomputation tests
the original delta: a raw full turn can force recomputation without
changing the cached angle. The all-zero path remains distinct.

The definition still reads four arguments. The semantic header retains
the literal `FUNC_80058434_CALL_WITH_UNUSED_ARG` selector and original
four/five-argument declarations so retail callers can keep their unused
fifth stack argument. All three existing camera/keyframe control files
change only the identifier and include path; their logic, values, stubs,
assertions and invalid-arity probes remain unchanged.

The numeric C filename remains for matching evidence. All seven native
sizes remain `0x1F0` under `gcc_2_8_1_g8_split`; no world-coordinate,
projection, clamp, normalized-direction or new movement policy is added.

## Requested orbit direction lock

`Model_LockOrbitDirection` is the former `func_8005A188`. It preserves the
ordered, short-circuit reads of the two existing effect-state getters. If
the guard permits the request, nonnegative input selects direction `+1`
(including zero), negative input selects `-1`, and the timer becomes
`0xFF`. The follow path does not decrement that sentinel, so the requested
direction remains locked rather than expiring as an ordinary countdown.

The interface retains its `int` parameter and the existing one-local
definition, with size `0x6C` and `gcc_2_8_1_g8` in every native image.
The guard bytes are not classified, combined, reordered or normalized.
The lock and adjustment headers remain separate contracts; the unused
fifth-argument selector belongs only to the adjustment API.
