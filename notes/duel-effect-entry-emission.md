# Entry emission and non-reserving occupancy lookup

The existing `DuelEffect_AppendEntry` API now names every C call and native
definition, rather than leaving the builder and commands on the
`func_80036C14` linker alias. Its grouped source/header paths remain
[`duel_effect_entries.c`](../src/game/duel_effect_entries.c) and
[`duel_effect_entries.h`](../src/game/duel_effect_entries.h).

The adjacent ownership contract names the NA/Japanese
`DuelEffect_FindFreeOccupancySlot` query and its two byte-valued start
coordinates. It does not create a European scanner or turn a query into an
allocator.

## One append name, regional return contracts

| Region | Entry stride | C return |
|---|---|---|
| NA | `0x1C` | `void` |
| Japan | `0x18` | `void` |
| Europe | `0x16` | `s32`, signed spacing adjustment |

The header retains these conditional return types. Europe returns an
adjustment that the text builder adds to the existing cell width; it is not
the complete glyph width or an append-success Boolean. The selected-card
and symbol wrappers still ignore it and keep their existing fixed pen
advance. No return value, cast, wrapper, status check or fallback is added.

The existing NA `func_80036C14` linker alias remains at `0x80036C14`.
Canonical C calls resolve the actual `DuelEffect_AppendEntry` definition.
Japan's obsolete definition macro is removed after binding the canonical
name at its native address. The unrelated record-lookup compatibility
declaration and its opaque payload contract remain unchanged.

## Partial writes and tagged values

All versions write entry header fields before some rejection paths.
Returning without advancing the tail does not mean no memory was written.
No preflight validation, rollback, buffer clearing or capacity check is
introduced.

NA/Japan retain the ordered flag branches:

- `0x80`: store the low tag byte in the icon field and publish flags
  `0xA0`; NA also keeps its extra channel byte.
- Otherwise `0x100`: extract tag bits 20..27, reject zero, then publish
  the existing selector/color/marker fields and flags `0xC0`.
- Otherwise mask with `0x8000FFFF`, reject zero and use the ordinary glyph
  path with flags `0x80`.

Japan queries occupancy before the ordinary path's zero-tag test, then
rejects a negative slot. Its rasterizer marks the chosen occupancy byte
with channel index plus one before glyph processing. The existing
`0x200` bitmap-buffer offset and `LoadImage`/`LoadImage2` selection stay
unchanged. NA retains its own buffer-offset and color behavior.

Europe instead treats `0x100` as the direct selector path, returning zero
adjustment after appending. Other values use tag bits 20..27 and the low
two channel mode bits. A zero glyph returns `-1` except in mode three,
which returns zero; neither zero-glyph case advances the tail.

| European mode | Existing position/spacing cases |
|---|---|
| 0 | Glyph 7 shifts X by -3 and returns -6. Glyphs `3F/42/45/0E/0C` shift by -1 and return -2. |
| 2 | Glyphs `28/42/45` shift X by -2 and return -4. Glyphs `3F/43/4D` shift by -1 and return -2. |
| 3 | Add two to X, retaining zero spacing adjustment. |

Other cases keep zero adjustment. The regional flag domains, word/byte
stores, coordinate wrapping, batching-marker clear, next-entry terminator
and final tail publication remain in their original order. No universal
glyph-mode enum or shared record stride is inferred.

## Retained palette and layout publication

The shared entry palette_index at +0x16 receives the original channel
color_index_54 only in the existing extracted-symbol and NA ordinary-glyph
paths. Direct icon entries do not gain a palette write, and their +0x17
wide-icon variant remains separate. JP rasterization and EU physical+0x14
array/color stores retain their existing representations and guards.
The NA layout_selector at +0x18 is configured over the channel's whole
entry range and retained by append, including partial/rejected writes;
no new append or initializer default is introduced. See the complete
producer/renderer distinctions in [text-glyph-renderer.md](text-glyph-renderer.md).

## Kind-dependent glyph indices and icon variants

The existing shared +0x10 glyph_index is a direct icon index, extracted
symbol index or Japanese raster occupancy slot according to the original
flags; ordinary NA glyphs retain their zero store and separate code word.
The +0x17 icon_variant is copied only by the existing NA direct-icon path.
No partial/rejected entry gains a write or new default. The callback-local
occupancy release and Japanese name-entry image-copy consumers keep their
original accesses and ordering. See [text-glyph-renderer.md](text-glyph-renderer.md)
for the full slot/variant chain and the PAL channel-field overlap.

## Occupancy start coordinates and lookup

`gDuelEffect_OccupancyStartColumn` and `gDuelEffect_OccupancyStartRow`
remain unsigned bytes. Only their declarations move from `unmatched.h`
into the existing occupancy header. Reset still clears the occupancy
table, then column and row in that order.

The query reads both bytes into its existing signed word-sized locals.
It follows the original tile-like row/column expressions with a
`0x100` probe budget, returns the first index whose table byte is zero,
or returns `-1`. It does not reserve a slot or publish its local cursors
back to the globals. The seed values and indexing remain unvalidated:
there is no new clamp, linear scan, bound change or altered wrap rule.

The query/reset/seed owner is absent from the European source branch.
The existing header declaration availability is not changed, but no
European inventory row or native seed binding is invented.

## Native identities and acceptance

| API/cell | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Append | `0x80036C14` | `0x8003648C` | `0x80036A78` | `0x80036C64` |
| Find free occupancy slot | `0x80035D10` | `0x80035A64` | absent | absent |
| Start column | `0x8009B324` | `0x8009B214` | no counterpart asserted | no counterpart asserted |
| Start row | `0x8009B325` | `0x8009B215` | no counterpart asserted | no counterpart asserted |

The accepted append semantic-map row is retained; only the query and two
start bytes gain new evidence/caveat rows. Every other inventory field,
address, size, profile, owner and row order stays fixed. Source paths,
grouping, record layouts, initializer order and legacy linker bindings
remain.

The ownership-only stage leaves all complete compiler/MASPSX outputs
raw-identical. The named stage compares every whole output after folding
the existing legacy append-call alias to its accepted canonical definition
in the baseline and reversing only three new identities. Historical
filenames, instructions, directives and relocations are not filtered.
Full clean retail matching remains mandatory. No tests, fixtures,
diagnostic controls or runtime captures change; regional/cloud validation
remains PR-side.

The initial whole-artifact proof precedes accepted #7618/#7619 integration.
The combined-tree comparison aligns the four accepted command-state names
in the saved baseline and the one accepted source-file move in its two
complete compiler/MASPSX outputs, then applies the same append-alias fold
and three owned identities. Nothing is dropped from the comparison; a
fresh clean gate covers the complete integrated tree.

A final pre-publish fetch also found accepted #7620. Its Build Deck
source/header/guard moves, native manifests and ledger path edit are kept
in full. The comparison additionally aligns that source filename in its
two whole outputs; no header or runtime logic is reconstructed from the
old paths.
