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
`attack/defense` at resource offsets `0x32/0x34`, not the screen-position members.

The independent renderer in `card_preview_draw_detail_panel.c` also reads
`attack + field_36` into `buf1` and draws those digits at its ATK row;
`defense + field_38` goes to `buf2` and its DEF row. This corroborates the
two argument roles without naming the remaining adjustment members or
changing the renderer's scheduling-sensitive constructs.

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

## Resource display-stat members

The same independent producer, packed-stat caller and digit-row evidence
confirms the resource's two stored display values, not just the arguments.

| Original member | Offset | Name | Type | Confidence |
| --- | --- | --- | --- | --- |
| `field_32` | `0x32` | `attack` | `s16` | confirmed |
| `field_34` | `0x34` | `defense` | `s16` | confirmed |

These values can be effective-stat overrides or decoded base stats.
They precede the separate `field_36/field_38` adjustments, whose offset
names and signed widths remain unchanged. No claim that the first pair
always contains unmodified base stats is made.

Only the typed constructor/placement writes and corresponding renderer reads
change member spelling. Placement's animation refresh takes the same
`Duel_CalcCardStats` low/high halves, independently confirming both stored
roles. Generic display-object coordinates with the same numeric
suffixes remain untouched, as do texture/object clients, layout, regional
bindings and all operations. Target compiler guards additionally fix both
offsets, two-byte widths and signedness without weakening the existing
stored-object type rejection controls. The existing actual-fragment
byte oracle, all 263,744 calls and three negative controls remain intact.
