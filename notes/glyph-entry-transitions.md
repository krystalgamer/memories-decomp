# Glyph-entry transition callbacks and dispatch views

This component connects the entry renderer's corner shading and rotation
modes, the per-entry callback pass and the overlapping text-command table.
Existing record types, callback signatures, region selectors and storage
remain intact; the historical display-object-fade source paths are kept.

## Renderer and callback evidence

The renderer in `text_box_draw_entries.c` interprets mode two's bytes +4..+7 as
four Gouraud corner shades and byte +0x14 as a width/skew term. Mode one
interprets bytes +8..+0xA as three rotation components, multiplied by16
before `RotMatrixZYX_gte`. The latter is not another color fade.

`DuelEffect_ProcessEntries` invokes an entry handler through the existing
masked control byte and passes its second context argument. The individual
callbacks keep their original one-argument declarations and measured
`DuelEffectChannel *` view. No callback ABI or record retyping is inferred
from the renderer's separate typed view.

## Paired glyph reveal and conceal

`DuelEffect_UpdateGlyphReveal` retains the one-time initialized latch,
mode-two selection and zero shade initialization. NA uses one signed word
clear and geometry values32/16; Europe keeps two halfword clears and16/8.
The geometry decrement and volatile frame-step reads remain distinct.

The first shade pair rises using the original signed-byte endpoint test;
its first corner clamps to0x80 before phase publication and the mirrored
corner copy. The second pair retains its original increment, control-byte
clear and final mirrored copy without a new clamp. Initialization still
progresses on the first invocation. No generic alpha or elapsed-time model
replaces those byte operations.

`DuelEffect_UpdateGlyphConceal` retains NA's0x80808080 word store versus
Europe's two0x8080 halfword stores. It lowers the first pair before
publishing the second phase, then lowers the second pair and releases the
entry occupancy claim. Release remains before the final zero stores, with
no new return or heap-free interpretation. NA keeps step16 and Europe24.

The shared `gcc_2_8_1_g0` profile, function order and exact extents remain:

| Target | Reveal | Conceal |
|---|---|---|
| North America | `0x80039AFC`, size `0xE4` | `0x80039BE0`, size `0xB4` |
| Japan | `0x80039170`, size `0xE4` | `0x80039254`, size `0xB4` |
| UK | `0x800398C4`, size `0xE4` | `0x800399A8`, size `0xC4` |
| France / Germany / Italy / Spain | `0x80039AC4`, size `0xE4` | `0x80039BA8`, size `0xC4` |

Only the two obsolete Japanese function macros retire. The frame-step alias
and European version-selector macros remain. All complete compiler/MASPSX
outputs and full C bodies preserve every other byte after those two
identities and exact macro removals. Header noncomment tokens retain all
declaration forms; all seven inventories change only the two reviewed
names, preserving notes, status, ownership, order, addresses and sizes.

## Delayed rotation and helper declarations

`DuelEffect_UpdateDelayedGlyphRotation` selects renderer mode one, clears
the three rotation bytes and seeds its byte delay from the original signed
coordinate shifts plus one. The seed narrows through the existing byte
store and is decremented in that same invocation. The phase change still
tests the narrowed result for zero; there is no new clamp or positive-delay
rule.

The rotation phase adds four per invocation, stores components in the
original +0xA, +9, +8 order and releases occupancy at the original
`v >= 0x40` test. It does not acquire the paired fades' frame-step factor,
new rounding or saturation.

| Target | Rotation callback | Size |
|---|---|---|
| North America | `0x80039C94` | `0xD0` |
| Japan | `0x80039308` | `0xD0` |
| UK | `0x80039A6C` | `0xD0` |
| France / Germany / Italy / Spain | `0x80039C6C` | `0xD0` |

The existing `DisplayObjectFade_ReleaseChannel` prototype moves exactly
from its numeric header into `display_object_fade.h`, beside the already
owned initialization helper and flag constants. Both implementation and
callback units already include that owner, so only the obsolete includes
and header retire. Its original struct-pointer spelling, helper bodies,
regional occupancy behavior and separate profiles remain.

The old description of rotation components as fade/color bytes is corrected
using the renderer. No new struct, signature, helper name or callback
argument is introduced. All 940 complete outputs match after only the
rotation identity; complete bodies otherwise retain the exact include
removals and all header tokens retain the moved declaration.

## Overlapping text and entry dispatch views

`gText_SecondaryCommandHandlers` is the initialized one-argument owner,
retaining its incomplete C array and every initializer/cast in the
47-entry NA definition. `Text_DispatchSecondaryCommand` still advances its
byte cursor and dispatches the raw byte without a new mask or bound.

`gDuelEffect_EntryHandlers` remains the separate incomplete
`void (*G32)(DuelEffectEntry *, DuelEffectChannel *)` view. It has no new
C definition or storage. The entry pass retains its original nonzero
control test, masked index, extra context argument and original `CALL32`.
Neither callback ABI is normalized to match the other.

| Target | Secondary owner | Entry view | Interior offset |
|---|---|---|---|
| North America | `0x80090EAC` | `0x80090F58` | `0xAC` |
| Japan | `0x80090D5C` | `0x80090E08` | `0xAC` |
| UK | `0x80091EA4` | `0x80091F58` | `0xB4` |
| France / Germany / Italy / Spain | `0x800922B8` | `0x8009236C` | `0xB4` |

NA's last four owner entries are conceal, reveal, conceal and rotation,
and remain in exactly that order. The European offset and existing
extensions are not replaced with an assumed NA length or a four-entry
bound. Numeric linker overrides keep each native interior address; the
initialized C owner does not gain a masking linker assignment.

Both view names replace their tentative names without changing any table
bytes or initialized-pointer representation. All 940 complete outputs and
four complete C files otherwise match after only these two identities.
All nine header noncomment-token sequences retain declaration/type/view
forms. The complete prior ledger gains exactly two rows; every inventory,
profile, manifest, test, trace consolidation and control remains.

## Component boundary

The paired and rotation/helper checkpoints join dispatch ownership before
one component PR. The 47-entry secondary command table and
its last-four-entry interior handler view are one storage relationship:
NA `0x80090F58` is `0x80090EAC + 0xAC`, not a separately allocated array.
Their different callback types, raw indices, table order and alias identities
must remain; no new bound or cast normalization is introduced.

European-only orbit/opacity extensions remain unchanged in this bundle,
including their RNG/store behavior and native source selectors. Accepted
trace consolidation and existing controls stay intact. No test or trace
harness is added, and regional/cloud validation stays PR-side.
