# Battle side-state life view

After the resource-meter role is isolated, the battle controller's `view`
cursor has only two remaining roles: the opposing and active
`DuelSideState` records. It now uses that actual pointer type and the
existing `life_points.unsigned_value` union member at `+0x14`.

The side selector and opposing-side XOR, capture points, reused `result`
and unsigned-halfword `life` locals, and both arithmetic/store sequences
remain unchanged. The result is narrowed into `life` and stored before
the original `(s16)life < 0` test can store zero. This is not ordinary
unsigned underflow saturation: a positive result of 65536 leaves the
original low-halfword life bits, and a stored result of `0x8000` is cleared
by the signed test.

The signed rank-scorer view of the same union is unchanged. No side layout,
global type, bound, validation, API, battle result logic, call order or
profile changes. The meter's block-local resource cursor stays separate
from this side cursor and the earlier `effects` role.

The native witness executes only the actual two captured life-update
fragments, not their dispatch conditions, battle calculation, rendering or
engine. Full two-side/guard bytes are checked over every initial halfword
value and selected full-width positive/negative boundaries, preserving the
other side, displayed life, maximum and neighboring rank/status fields.
