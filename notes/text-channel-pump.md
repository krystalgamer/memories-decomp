# Shared text-channel pump and glyph-kind publication

`TextBox_UpdateChannels` (formerly `func_80039794`) is the four-channel
frame pump, called by the scene/menu/dialog runtimes and resident-linked
overlays. It is not a replacement for the single-channel
`TextBox_BuildStep` or either synchronous build wrapper.
The matching source/header paths remain
[`text_box_update_channels.c`](../src/game/text_box_update_channels.c) and
[`text_box_update_channels.h`](../src/game/text_box_update_channels.h).

## Two cursors, one record

The body retains two independently live cursors: the full channel base
passed to helpers, and a `ChoiceChannelCursor` parked on its `field_30`.
The latter sees the choice object at displacement zero and the channel
flags at displacement four. Folding it into the full record costs a
register and four instructions under the retained profile.

`ChoiceView.obj` now uses the existing `struct DisplayObject *G32`
type instead of `void *G32`: `Dialog_OpenChoice` returns that pointer,
and `DisplayObject_ReleaseIfPresent` consumes it. Its zero offset, the
flags' four-byte offset and eight-byte view size have target compile-time
contracts. No object or record storage is added, and the channel's shared
`field_30` is not globally renamed or restricted to choices.

## Pump and choice lifecycle

For each of four records, only an active channel enters the pump body.
An incomplete channel starts with the shared glyph index armed to `-1`
and counter `-1`, then calls `TextBox_BuildStep` before incrementing the
counter and re-reading flags. Completion can open a choice and exits the
loop. Otherwise a `0x1C00` batching flag resets the glyph index to `-1`
and repeats. Without that flag, the loop stops when its counter is at
least `gGraphics_bExtraFrameStep`.

The counter's initial value and inclusive comparison matter: with a zero
extra-step limit the loop still calls BuildStep once. There is no new
iteration bound, timeout, yield or fallback for repeatedly batched work.

A channel already complete may close its flagged choice on the regional
confirmation input, clear the regional choice flag, release/null the
object and play sound `0xB`. The incomplete-to-complete opening path and
already-complete closing path remain separate.

After pumping or closing, the channel derives the menu-record argument,
calls `DisplayEffect_ProcessMenuRecords` and then
`DuelEffect_ProcessEntries`. Inactive channels still advance both
cursors but perform none of these per-channel operations.

## Shared publication cells

| Native NA cell | Name | Storage and interpretation |
|---|---|---|
| `0x8009B35A` | `gText_PumpedGlyphIndex` | Signed halfword, transient shared glyph-table index or sentinel |
| `0x8009B356` | `gText_PreviousGlyphKind` | Unsigned byte carrying a previous non-four glyph kind |

`TextBox_BuildStep` publishes its script word after command dispatch and
line-wrap handling, before appending a glyph. Only nonnegative indices
are read through `tent_GlyphLookupTable[idx].halves.upper & 7`.
The pump first stores zero for every active channel, then stores `-1`
before pumping an incomplete channel and on batching resets. Consequently
this is neither a choice index nor a universal last-emitted-glyph value.
An already-complete active channel retains the zero publication; that
behavior is not replaced by a new sentinel.

The menu-record argument starts at `-1`. When there is a nonnegative
index, kind four reuses `gText_PreviousGlyphKind`; other kinds both
replace that byte and become the argument. Neither the byte nor its
carryover is reset here, so reuse can cross channel and frame boundaries.
There is no new enum, per-channel cache, initialization or signedness
normalization.

## Regional identities and controls

| Region | Pump | Pumped glyph index | Previous kind |
|---|---|---|---|
| NA | `0x80039794` | `0x8009B35A` | `0x8009B356` |
| Japan | `0x80038E08` | `0x8009B24A` | `0x8009B246` |
| UK | `0x80039570` | `0x8009C2B6` | `0x8009C2B9` |
| France / Germany / Italy / Spain | `0x80039770` | `0x8009C6D6` | `0x8009C6D9` |

The UK/continental cell order is not inferred from the NA order. Their
native symbol/linker bindings are the evidence. Historical French
address-suffixed aliases remain untouched; they are not native identities
for these newly named cells.

Japan retains its explicit `0x60` cursor advance and Circle/Square input
mask. The default body uses its existing shared-record `sizeof` and
confirmation mask. Europe retains choice flag `0x10`; the default is
eight. Only three obsolete Japanese staging-cell macros are removed,
after binding the canonical names to their actual native addresses.
The other wrapper macros, GP/absolute declaration arms, profiles, file
paths and grouped source ownership remain.

All resident/overlay callers and their resident linker imports use the
same pump API. The NA Free Duel and Password modules require explicit
`TextBox_UpdateChannels = 0x80039794` imports: their generated
address-based fallbacks do not export the new name, and the resident
symbol inventory alone does not supply an overlay import. Other regions'
existing explicit imports are renamed at their unchanged native addresses.
The two retained controller witnesses change only their
stub identifier: events, numeric expectations, mutations, scenarios and
positive/negative controls remain byte-for-byte unchanged.

Acceptance compares complete compiler and MASPSX outputs after reversing
only the three new symbol names, without instruction/directive/relocation
or filename filtering. Native inventories preserve all other fields and
row order, and the complete accepted semantic ledger gains exactly three
evidence/caveat rows. Full clean executable matching is mandatory.
No synthetic/generated-C/source-string harness or new runtime capture is
added; cloud and regional validation remain on the PR.

The initial whole-artifact proof precedes accepted display-family
integration (#7613). The combined-tree comparison aligns that accepted
family's three independently verified names in the saved baseline, then
reverses only this component's three names. A subsequent integration also
preserves #7614's accepted `DebugMenu_UpdateMapEntry` name and complete
ordered ledger update, aligning that additional identity in the baseline.
No code or directive filtering is introduced; the combined tree also
requires a fresh clean retail match.
