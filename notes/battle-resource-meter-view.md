# Battle resource-meter view

The state-4 meter block in `DuelScene_UpdateBattle` captures
`D_800EA0E8[D_8009B1B9]` after the existing meter increment/cap. Only that
resource role now uses a block-local `DuelEffectResourceRecord *`; the
function's `view` byte cursor still serves its unrelated side-state views.
The original index, capture point, `arg` scratch assignment, branch tests
and two store positions remain unchanged.

The canonical `field_36` and `field_38` are signed halfwords used by the stat
renderer. This block deliberately retains unsigned `u16` lvalue views,
matching the old `H(view, 0x36/0x38)` stores and every narrowing bit pattern.
The casts also preserve the non-struct store shape. No wider store, new
clamp, new field meaning, pointer-slot change or API is introduced.

A first candidate reused the existing `effects` local. Supported GCC 2.8.1
then moved state 2's argument setup below its resource-base construction and
selected `$19` instead of `$3` for the later record address. The image kept
its size but no longer matched. Isolating the new role in its actual block
preserves the old single-assignment role and exact instructions without
flags, volatile accesses, register bindings or fake storage.

The focused native witness executes only the actual capture/argument/two
stores, with test-bound record storage. It checks full record/guard bytes
for four valid slots and every halfword value; it does not execute the
meter ramp/cap, the complete controller, rendering or the game engine.
