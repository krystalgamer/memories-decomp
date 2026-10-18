# Paired effect-endpoint sequences

`Model_QueueEffectEndpointSequences`, originally `func_8005F828` at
`0x8005F828`, queues pairs from two nullable `ModelEffectEndpoint`
streams. No static production caller is observed in the current sources;
the high-confidence role follows the complete body and its existing
typed queue-command boundary, not an assumed observed scene.

## Counter, count and termination boundaries

The shared two-flag gate remains first. The signed counter then selects
begin or append for the first pair; a nonnegative counter is incremented
before the zero-count return. Count zero is therefore not a general
no-side-effect guarantee.

For every issued pair, nonnull stream pointers advance by one eight-byte
record. The helper reads the **next** kind before decrementing a positive
count, including the last bounded iteration. Those lookahead reads are
not moved behind the count test or replaced by a conventional pre-check.
The existing valid halfword-alignment requirement remains in place.

A nonzero next kind in either available stream permits continuation.
If the first stream cannot continue, a null second stream or zero second
kind ends the call. Positive counts are decremented; negative counts
retain sentinel-driven iteration. Every subsequent queue command uses
append mode. Capacity remains the queue command's responsibility; cursor
walking is not redesigned when that command declines an append.

## Preserved source and regional identity

Only the identifier changes. The function retains its nullable-pointer
ABI, byte/halfword widths, branch order, pointer advancement, callback
sequence and signed counter behavior. The existing owner controls still
accept both typed streams and reject independently reverting either one
to an opaque pointer contract.

The same 23-function grouped source, profile and `0xF4` extent remain:

| Build | Address |
|---|---|
| North America | `0x8005F828` |
| Japan | `0x8004F278` |
| European base | `0x8004F634` |
| French, German, Italian, Spanish | `0x8004F838` |

Japan keeps its existing `func_8004F278` local export, inventory name and
binding. Only the include adapter's shared macro name changes. Other
regions rename their existing common binding at the same address.

No source move, stored layout, selector enum, guest-pointer cast, compiler
setting, overlay implementation or runtime validation policy changes.
Historical attempt/reference records and generated usage reports remain
historical/generated evidence.
