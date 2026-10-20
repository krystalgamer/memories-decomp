# Glyph-entry transition callbacks and dispatch views

This component connects the entry renderer's corner shading and rotation
modes, the per-entry callback pass and the overlapping text-command table.
Existing record types, callback signatures, region selectors and storage
remain intact; the historical display-object-fade source paths are kept.

## Renderer and callback evidence

The renderer in `func_80035E20.c` interprets mode two's bytes +4..+7 as
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

## Component boundary

Delayed glyph rotation and helper declaration ownership join the paired
handlers before one component PR. The 47-entry secondary command table and
its last-four-entry interior handler view are one storage relationship:
NA `0x80090F58` is `0x80090EAC + 0xAC`, not a separately allocated array.
Their different callback types, raw indices, table order and alias identities
must remain; no new bound or cast normalization is introduced.

European-only orbit/opacity extensions remain unchanged in this bundle,
including their RNG/store behavior and native source selectors. Accepted
trace consolidation and existing controls stay intact. No test or trace
harness is added, and regional/cloud validation stays PR-side.
