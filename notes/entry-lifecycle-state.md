# Text/effect entry lifecycle state

## Three distinct working-state roles

| Original NA provider | Canonical view | Established role |
|---|---|---|
| `tent_DuelEffectEntries` (`0x800EB288`) | `gDuelEffect_aEntries` | Shared text/effect working-entry base |
| `tent_DuelEffectOccupancy` (`0x800EAF08`) | `gDuelEffect_aOccupancyValues` | Separate tagged occupancy-value table |
| `D_8009B330` | `gDuelEffect_nCompactionRequested` | Signed word requesting compaction after the current handler walk |

The former tentative table names are promoted on the connected range,
append/build, glyph/layout, marker and compaction evidence. No new meaning
is assigned to unknown entry fields. These names do not allocate storage,
introduce a new table format or merge the two tables.

## Regional record layouts remain different

The existing caller declaration remains
`DuelEffectEntry[DUEL_EFFECT_ENTRY_COUNT]`, with the inherited bound of
620. It is not a new statement that every regional physical table has
that count or element shape:

| Existing helper view | Record size | Existing iteration count |
|---|---:|---:|
| NA `DuelEffectEntry` | `0x1C` | 620 |
| `JapaneseDuelEffectEntry` | `0x18` | 300 |
| `EuropeanDuelEffectEntry` | `0x16` | 800 |

All casts, count macros, alignment assertions, field offsets, masks and
whole-record copies remain unchanged. The narrow canonical header follows
the original owner and repeats the exact old declaration views. It does
not fix, widen or reinterpret the inherited bound, invent a universal
record layout, or change marker/reset behavior. Japanese/PAL helpers keep
their own record views instead of being forced through the NA type.

## Occupancy values are tags, not Boolean entry flags

The separate occupancy declaration remains mutable `u8[240]`.
Rasterization records channel index plus one. Clearing by value compares
against the input plus one; reset clears the table and the existing scan
coordinates. The free-slot query searches for zero without reserving the
result. Display-object fade release clears the slot named by its existing
record field on the paths that already perform that operation.

This is not a 620-element parallel array or a Boolean conversion. All
byte truncation, scan ordering, repeated queries, grid behavior and
existing index assumptions remain. No bound check or general safety
claim for arbitrary indices/cursors is added.

The European exclusions remain exact: PAL compiles no runtime occupancy
import in this component. Retained declarations do not justify an
invented PAL provider or a copied North American address.

## Compaction is not a handler restart or queued next-frame action

`DuelEffect_ProcessEntries` clears the request word before walking the
current channel's active entries and invoking their existing handlers.
`DisplayObjectFade_ReleaseChannel` clears its occupancy/active state and
stores one into the request word.

When the word is nonzero after that handler walk, the processor scans
from the range base to the old end, copies active records forward in
their original order, publishes the new end and clears the next flags
byte. It neither restarts the handler walk nor clears every stale record
after the sentinel.

The request remains a plain signed `s32` tested for nonzero. A release
outside the walk is not guaranteed to queue a later compaction: the next
processor invocation still clears the word first. No new state machine,
reentrancy rule, cleanup/lifetime guarantee, callback signature or
allocation behavior is introduced.

## Provider and compatibility boundaries

The seven resident linker scripts retain every accepted byte as an exact
prefix. The sixteen active image/role bindings all use their original
tracked providers: entry bases and request words in all seven images,
occupancy values only in NA/Japan. Legacy names remain available; no
numeric address, field owner or storage definition is invented.

Actual compiler/CPP evidence covers nine roots, 31 source/profile
contexts and 113 function registrations. All 93 declaration views retain
the inherited table types/bounds and word width. Twenty-eight imports
are active (seventeen entry, five occupancy, six request); sixty-five
views remain unused. No selected data owner or transitive overlay C
consumer exists in the audited source closure.

The original declaration owner changes only the stale restart prose.
Its declarations and code, every regional type/header, source group,
compiler profile, wrapper, test and physical ownership control remain
unchanged. No new harness, fixture, probe or testcase is added.

## Configured ranges, entry start and append cursor

The channel's existing fields now spell entry_write_cursor (entry_end_20,
stored pointer +0x20), entry_begin (entry_head_24, stored pointer +0x24),
entry_range_start (range_start_5C) and entry_range_count (range_count_5E
in NA/PAL, range_count_5D in Japan). These are distinct roles, not a new
allocator, collection abstraction or enforced capacity.

| Existing channel/view | Configured start | Configured count |
| --- | --- | --- |
| Shared NA/PAL channel | u16 +0x5C | u16 +0x5E |
| Compact Japanese channel / narrow entry-range view | u8 +0x5C | u8 +0x5D |

The constructor retains consecutive boundary-word subtraction in NA/PAL.
Japan keeps its byte view of the SAME u16 boundary table, range[0] and
range[2] reads and byte stores: no high bytes are recovered or widened.
The two pointer fields gain no constructor initialization. First build
seeds BOTH pointers from the configured start only under the original
regional own-entries gate; the retained-entries path gains no reset.

Append uses entry_write_cursor, preserves all original pre-rejection
stores and rejection exits, writes the next sentinel and then publishes
the advanced pointer. There is still no range-count or capacity check.
Regional record strides, raw casts, upload behavior and tag stores stay.

The handler walk starts at entry_begin, stops at the first inactive flag
and keeps the original full-byte handler gate before masked dispatch.
It does NOT use entry_write_cursor or entry_range_count as a new bound.
When compaction is requested, scan and destination start independently at
the configured range base, scan to the OLD write cursor and copy whole
active records in order. Only the write cursor is republished; entry_begin
is not rewritten. The next flags byte is cleared without clearing stale
tails, resetting tags, restarting handlers or extending request lifetime.

Drawing and name-entry lookup likewise walk from entry_begin until the
existing inactive sentinel. Their regional strides, raw byte-cursor
biases, signed coordinate tests and display binding remain unchanged.
The configured count bounds only the existing control/query helpers,
not all consumers of the range.
