# Paired resource constructor return

`CardPreview_CreateResourceObjects`, formerly `func_800291E0`, returns its
allocated-second `DisplayObject`, which it also
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

At the stat-argument checkpoint, the old `x`/`y` parameter and scoped-local
names were misleading.
`Duel_CalcCardStats` returns effective attack in the low halfword and defense
in the high halfword. Both the battle and placement callers pass those
halves directly to `func_800291E0`, then position the returned display object
separately. In the default constructor path, the values go to resource
`attack/defense` at resource offsets `0x32/0x34`, not the screen-position members.

The independent renderer in `card_preview_draw_detail_panel.c` also reads
`attack + attack_adjustment` into `buf1` and draws those digits at its ATK
row; `defense + defense_adjustment` goes to `buf2` and its DEF row.
This corroborates the argument roles without changing the renderer's
scheduling-sensitive constructs.

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
existing presets. That checkpoint changed no public function name, API
type, member name, caller, pixel coordinate, profile or regional wrapper.

The retired `test_duel_resource_stat_arguments.py` compiled the two **actual extracted
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
They precede separate signed adjustments at `0x36/0x38`. No claim that
the first pair always contains unmodified base stats is made.

Only the typed constructor/placement writes and corresponding renderer reads
change member spelling. Placement's animation refresh takes the same
`Duel_CalcCardStats` low/high halves, independently confirming both stored
roles. Generic display-object coordinates with the same numeric
suffixes remain untouched, as do texture/object clients, layout, regional
bindings and all operations. Target compiler guards additionally fix both
offsets, two-byte widths and signedness without weakening the existing
stored-object type rejection controls. The existing actual-fragment
byte oracle, all 263,744 calls and three negative controls remain intact.

## Separate display adjustments

The two subsequent signed halfwords now have independently recovered names:

| Original member | Offset | Name | Type | Confidence |
| --- | --- | --- | --- | --- |
| `field_36` | `0x36` | `attack_adjustment` | `s16` | confirmed |
| `field_38` | `0x38` | `defense_adjustment` | `s16` | confirmed |

The constructor clears them on the default stat-bearing path, in the same
defense-then-attack store order as before. Battle's Guardian-Star display
path supplies the animated addition, and the renderer adds each to the
corresponding value and digit row. Other byte members remain offset-named.
The battle counter, flags, slot capture and explicit unsigned pointer-lvalue
stores are unchanged; see `battle-resource-meter-view.md` for their measured
compiler boundary and deliberately limited native witness. No positive-only
meaning, wider store, new clamp or underlying-card stat mutation is implied.
Target guards fix both signed two-byte members and their original offsets.

## Confirmed paired-object constructor name

The constructor is now `CardPreview_CreateResourceObjects`, declared with
the release helper in `card_preview_objects.h`. It is distinct from the
resource-transfer requester: it allocates and configures two display-pool
objects from an already-selected resource slot, without requesting artwork.

The first allocation receives `CardPreview_DrawDetailPanel` and is
published as `secondary_object`. The second receives
`CardPreview_UpdateVariant`, is published as `primary_object`, is linked
from the secondary object's `field_54` and is the actual returned object.
All card-type branches reach this same paired tail. The viewer, Library,
battle, placement, card-use, ritual and debug callers position or animate
that returned primary object. Password retains its explicit narrower view;
only the resident API identifier and header path change there.

All seven metadata graphs keep the original two-function G8 group, source
owners, sizes and definition order. No constructor statement, field type,
scalar argument, cast, callback representation, special-type preset,
narrowing/default-stat behavior or unchecked allocation assumption changes.
The raw `display_object.h` remains byte-identical.

Existing return/partial-view compiler probes, default-stat byte oracles,
object-link/release/clear fixtures, ritual events and regression negatives
retain their original logic; only symbol/header references change. Added
binding/source controls do not execute the complete constructor, pool,
renderer, GTE or GPU. Caller visibility controls check the canonical
declaration at every actual caller and detect losing its owning include.

All seven Password resident import tables also bind the canonical
constructor at its unchanged per-build address, alongside every existing
address-based alias. Each Password A/B manifest must select the checked
import script. Two-instruction MIPS relocation probes check the exact
absolute address with the same strict 32-bit/sign-extended `nm` guard as
the requester; removing only the constructor's named binding must fail.
These focused link probes do not build or execute cloud overlays.

## Card metadata display bytes

| Original member | Offset | Name | Type | Confidence |
| --- | --- | --- | --- | --- |
| `field_3A` | `0x3A` | `level` | `u8` | confirmed |
| `field_3B` | `0x3B` | `attribute_icon` | `u8` | confirmed |

The verified executable's 723-byte table at `0x801D5332` is indexed directly
by card ID, with entry zero reserved, not by `card_id - 1`. Rechecking all
722 cards against `card-catalog.csv` confirms the existing catalogue:
621 monsters have attributes 0..5 and levels 1..12; the 101 non-monsters
have level zero and high-nibble icon codes 6/7. One Magic-classified card
also uses code 7, so the name does not imply a type-to-icon rule or an
attribute matchup rule.

The constructor stores the high nibble first, then the low nibble.
The renderer uses `level` as its repeated-icon count and `attribute_icon`
as the tile selector, retaining the intermediate byte narrowing before
the palette-coordinate addition. Neither loop, packing, field width,
index adjustment nor any flag behavior changes. The former "guardian-star
tile" description was misleading: Guardian Stars occupy separate bits
in the packed stat word, not this metadata byte.

Target guards retain the exact one-byte unsigned fields and offsets.
Focused controls check the full verified catalogue and actual extracted
two-store producer for all packed bytes and valid card IDs; they do not
execute the renderer, GTE or engine.

The producer fixture runs 184,832 calls at both `-O0` and `-O2`, crossing
all 256 packed bytes with each valid one-based card ID. Independent raw
offset oracles check the complete guarded resource and input table.
Negative controls reject a zero-based index, swapped nibbles and a widened
level store. Consumer source controls retain the zero-count gate, casted
icon loop bound and intermediate tile-byte narrowing; those are not a
claim to execute the renderer. Existing stat-default, adjustment, life-point
and packet-record controls remain intact.

## Stored card ID versus full transfer argument

Resource `field_30` is now `card_id`, retaining its original `u16` at
`0x30`. The transfer helper's corresponding `s32` parameter has the same
name and unchanged ABI. The helper stores the low halfword, but computes
the disc position from the **original full argument**, not a reread of the
stored member. Its sector macro and callback/status publication order are
unchanged.

The constructor still takes `(s16)entry->card_id` for its one-based stat
lookup and direct metadata-byte index. That signed view does not justify
changing the stored unsigned declaration, widening the store, clamping IDs,
changing a table bias, or making the transfer calculation use a narrowed
value. Generic display-object `field_30` members are unrelated.

Target guards fix the stored offset, width and unsignedness. The focused
whole-helper witness stubs the transfer requester and callback; it does not
perform I/O, allocate real descriptors, invoke the callback, or execute
constructor lookups for injected non-card values. No null-safety or valid-ID
guarantee is added to the existing API.

The whole-helper fixture runs 1,310,720 cases at both `-O0` and `-O2`:
four valid slots, all low-halfword patterns, and five arithmetic-safe
high-word patterns. Those inputs deliberately include non-card values
without overflowing the original signed disc-position calculation.
Independent raw-offset/full-byte oracles cover the complete resource array,
descriptor and volatile status word; the transfer stub observes that the
ID store happened before the request and that the old status is still
visible then. It checks the original full-word position, exact arguments,
callback and returned descriptor. Negative controls reject using the stored
truncated ID for the disc position, omitting the primary-active bit, and a
widened ID store.

## Stat palette flags

Resource byte `field_3C` is now `stat_palette_flags`, retaining unsigned
one-byte storage at `0x3C`. The constructor clears it at the shared tail.
Battle ORs `0x40` into preview zero, and `0x80` or `0x40` into preview one
according to its existing mode branch. The renderer tests `0x80` for the
ATK icon/digits and `0x40` for DEF, selecting an alternate CLUT row.

The name describes palette selection, not unverified visual effects such
as dimming/highlighting or a gameplay stat mode. Numeric masks, unused-bit
preservation, compound ORs, zero-count and draw gates, and all palette
arithmetic remain unchanged. The first icon increments its current row;
the other three selections use the existing regional base-plus-one value.
Neither is replaced by the other. Generic display-object `field_3C` extents
and `attribute` flags are unrelated and untouched.

The focused fixture executes only the four actual writer statements and
four palette-selection conditionals with project record/packet types.
At both `-O0` and `-O2`, 139,264 cases cover all flag bytes, four boundary
rows, and every row halfword with the first icon's increment enabled and
disabled. Full guarded resource/packet byte oracles reject lost unused
bits, the wrong stat mask and a widened clear. It does not execute the
surrounding battle controller, draw gates, submissions, renderer, GPU,
regional variants or engine.
