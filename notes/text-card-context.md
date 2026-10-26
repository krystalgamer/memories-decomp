# Shared selected-card text context and selector jumps

This contract covers the matching regional bodies in
[`duel_effect_command.c`](../src/game/duel_effect_command.c), the two indexed
jumps in
[`duel_effect_object_commands.c`](../src/game/duel_effect_object_commands.c)
and their
[`secondary dispatch table`](../src/game/duel_effect_command_table.c).
It follows actual producers and consumers; it does not assign every shared
text cell a universal duel-only meaning.

## Four staging bytes, not one new structure

| NA address | Identity | Proven use |
|---|---|---|
| `0x8009B320` | `gText_SelectedCardColorIndex` | Field info stages guardian comparison color `4/1/6`; Library stages the selected channel's effective color or neutral `4`. Selected-card modifier bit `0x10` copies it to channel `color_index_54`. |
| `0x8009B344` | `gText_GuardianGlyphCode` | Field info selects a guardian star and adds `0x17`; the guardian wrapper emits that encoded symbol. Not a raw star ordinal or stream pointer. |
| `0x8009B34E` | `gText_BranchSelectorA` | Independent u16-table index consumed by selector jump A. |
| `0x8009B355` | `gText_BranchSelectorB` | Independent u16-table index consumed by selector jump B. |

Field info gives selector A card-visibility states and selector B its
Swords/comparison presence bits. Result presentation instead writes
`A = multiplayer` and `B = winner + 2 * multiplayer`; the trade prompt writes
its mode argument into A. The two selectors are therefore deliberately
generic, not a single card-state enum or globally named Swords flags.

All four remain unsigned scalar bytes at their original scattered addresses.
Existing `.data` declaration arms retain the writers' absolute addressing;
plain reader declarations retain GP-relative accesses. The address-based
`D_8009B..._IN_DATA` selectors remain code-generation contracts, not new
runtime modes. No cell gains an initializer, bound, validity check, merged
state record or lifetime guarantee. The already named selected-card ID and
all unrelated context fields retain their existing declarations.

## Secondary commands and symbol emission

| NA address | API | Secondary index |
|---|---|---|
| `0x80037DA4` | `Text_ExpandSelectedCardInfo` | `0x00` |
| `0x80038024` | `Text_EmitSymbolGlyph` | Helper only; not a table entry |
| `0x80038070` | `Text_EmitGuardianStarGlyph` | `0x08` |
| `0x80038094` | `Text_EmitNextSymbolGlyph` | `0x0B` |
| `0x800389D8` | `Text_JumpBySelectorA` | `0x17` |
| `0x80038A44` | `Text_JumpBySelectorB` | `0x18` |

Selected-card expansion consumes one modifier byte. Bit `0x10` applies the
staged color and returns. Bits `0x20`/`0x40` choose the selected card's name
or description stream; low-nibble selectors choose type or guardian
attributes, with bit `0x80` selecting a string expansion instead of the plain
glyph path. It does not read a new card ID operand or format numeric ATK/DEF.

Preserve every regional alternative: the North American `field_62` handling,
absent-second-star glyph suppression and three-bank arithmetic; Japan's
two-bank lookup and plain glyph behavior; and Europe's extra type selector,
bank bases and different symbol flag. Stream depth, raw operand priority,
masked address operations and cursor publication order remain unchanged.

The signed stream index first lives in the byte-pointer local, becomes its
four-byte slot offset, and is coalesced back through a scoped pointer. Those
lifetimes are not rewritten. The three variants' stored pointer load/store
casts now spell `u8 *G32 *`, without changing the accesses or substituting a
new channel layout.

The symbol helper preserves the measured low-byte flag self-assignment,
raises `DUEL_EFFECT_SELECTOR_FLAG`, calls the existing glyph append routine,
clears the flag and advances the pen by exactly `0x10`, ignoring the append
result. It does not save and restore the old flag value. Europe retains
`0x100`; NA/Japan retain `0x80`. The guardian wrapper consumes no operand;
the next-symbol wrapper publishes its one-byte post-increment before calling
the helper. No resource bitmap name, glyph bound or allocator guarantee is
inferred.

## Indexed jumps preserve the advanced cursor's bank

Each jump uses its own selector to advance the current stored stream pointer
by `selector * 2`, then calls `TextStream_ReadU16LE` on the selected entry.
The final pointer combines that u16 with the **same advanced post-read
cursor's** masked high bits using OR. It does not derive a bank from the
original table base, add the target arithmetically, push a stream or return
through a call stack.

The regional high mask remains exact, including Japan's `0x801F0000`, rather
than being normalized to a presumed universal `0xFFFF0000`. This also retains
the old behavior at bank boundaries. Selectors are not clamped and no table
size is invented.

## Native identities and acceptance

| API | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Selected-card expansion | `0x80037DA4` | `0x8003755C` | `0x80037B3C` | `0x80037D3C` |
| Symbol helper | `0x80038024` | `0x8003774C` | `0x80037D94` | `0x80037F94` |
| Guardian symbol | `0x80038070` | `0x80037798` | `0x80037DE0` | `0x80037FE0` |
| Next symbol | `0x80038094` | `0x800377BC` | `0x80037E04` | `0x80038004` |
| Selector A jump | `0x800389D8` | `0x80038094` | `0x800387AC` | `0x800389AC` |
| Selector B jump | `0x80038A44` | `0x80038100` | `0x80038818` | `0x80038A18` |

Japan's native `0x80038094` is selector A's jump, not the next-symbol wrapper
whose NA address has that spelling. The six obsolete wrapper aliases are
removed only after binding each semantic name to its actual native address.
All inventories retain sizes, row order, status, ownership and notes; source
groups, profiles and regional bodies remain unchanged.

| Context cell | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Selected-card color | `0x8009B320` | `0x8009B210` | `0x8009C2C1` | `0x8009C6E1` |
| Guardian glyph | `0x8009B344` | `0x8009B234` | `0x8009C2A0` | `0x8009C6C0` |
| Selector A | `0x8009B34E` | `0x8009B23E` | `0x8009C2B1` | `0x8009C6D1` |
| Selector B | `0x8009B355` | `0x8009B245` | `0x8009C2B8` | `0x8009C6D8` |

Similarly numbered French SDK labels are unrelated and remain untouched.
The stored-pointer-only stage reproduces fourteen complete
compiler/MASPSX artifacts raw-identically. The named stage compares the
complete six producer/handler units and full initialized dispatch table after
reversing only six function and four global spellings. No instructions,
directives, table positions or relocation structure are filtered.

The existing result-controller fixture changes only its two staging-global
declaration/reference names. Its winner/multiplayer oracle, sentinel values,
cases, diagnostics and controls remain unchanged. No new test, fixture,
synthetic source, scenario or runtime capture is added, and cloud tests stay
on the PR. Full clean executable matching remains mandatory.

## North American wide-icon staging is not a universal channel field

The first-guardian case of Text_ExpandSelectedCardInfo stages channel
field_62 from card type only in the original non-monster range 20..23.
The direct-icon append path copies that byte into entry icon_variant;
the renderer combines it with glyph_index to select its wide-icon atlas,
geometry and palettes. The command's initial zero, color-only early
return, opcode priority and absent-second-star behavior remain unchanged.
Text_CloseChoice and Text_CompletePageAdvance retain their NA-only clears.
Generic symbol emission gains no staging reset.

Do not rename the shared channel field_62 to icon_variant: Europe's
Text_SetOutputLimit and TextBox_BuildStep use the u16 at +0x62/+0x63 as
an output limit. Japan's compact channel has neither field. This scope
keeps those halfword casts, pad_63, regional guards and all numeric values;
only the shared entry and its established callback-prefix index acquire
semantic names. See [text-glyph-renderer.md](text-glyph-renderer.md).
