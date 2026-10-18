# Paired resource constructor return

`func_800291E0` returns its allocated-second `DisplayObject`, which it also
publishes as the resource record's primary object and links from the first
object. The local already has that actual type. Its public return now keeps
`struct DisplayObject *` through a forward declaration instead of discarding
the type through `u8 *`; neither the pool API nor the three scalar arguments
changes.

All observed game callers already cast to `DisplayObject *`. Password keeps
its existing explicit `PasswordCardPreviewView *` cast. No caller or overlay
implementation needs changing, and deliberate narrower/configuration views
are not replaced. No function-pointer consumers were found.

The grouped constructor changes only its declared return and final
redundant byte cast. The selected object, returned address, publication
order, signed-card/default-stat logic, exact compiler profile and
all matching instructions remain unchanged; this adds no allocation or
null-safety guarantee.

The ritual native fixture's stub uses the corrected declaration while
returning the exact same object address. Its events, expectations and
regression mutants stay unchanged. Standalone supported-compiler probes
accept the exact function-pointer return, reject byte/opaque returns,
and retain explicit partial/byte and ordinary generic caller views.

## Stat override arguments, not coordinates

The old `x`/`y` parameter and scoped-local names were misleading.
`Duel_CalcCardStats` returns effective attack in the low halfword and defense
in the high halfword. Both the battle and placement callers pass those
halves directly to `func_800291E0`, then position the returned display object
separately. In the default constructor path, the values go to resource
`field_32/field_34`, not the screen-position members.

The two packed fallbacks independently corroborate the roles:
`DuelCard_Setup` populates `attack` from bits 0..8 and `defense` from bits
9..17, scaled by ten, exactly the fields decoded here. The constructor
parameters are now `attack` and `defense`, with corresponding scoped
`attack_value`/`defense_value` names. They are display-stat overrides, not
necessarily unmodified base stats. The source's shared `setup` local stays
unchanged because the special-type branches reuse it for sprite setup.

Narrowing still happens **before** testing for a negative override. For
example, `0x00008000` takes the fallback even though the full argument is
positive, while `0xFFFF0000` becomes zero and does not. Do not replace the
`s16` tests with full-word sign tests, clamping or an unsigned comparison.
Special card types still bypass both default-stat blocks and use their
existing presets. No public function name, API type, member name, caller,
pixel coordinate, profile or regional wrapper changes.

`test_duel_resource_stat_arguments.py` compiles the two **actual extracted
default-stat blocks** with the real resource/card types, not the complete
constructor or renderer. Each `-O0`/`-O2` run checks 263,744 calls: all low
halfword patterns with four distinct high-word patterns, crossed sign/zero
boundaries and packed-stat extremes. The independent byte oracle checks the
entire resource record and guarded stat table. Negative controls reject
full-word sign tests, swapped ATK/DEF fields and an incorrect default scale.
The surrounding special-type branches and every other producer/consumer
are separately preserved by the token-only source audit and exact complete
executable matching.
